#include "ontoloGenius/core/ontoGraphs/Graphs/ClassGraph.h"

#include <iostream>
#include <algorithm>

#include "ontoloGenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Graphs/IndividualGraph.h"

namespace ontologenius {

ClassGraph::ClassGraph(IndividualGraph* individual_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph)
{
  individual_graph_ = individual_graph;
  object_property_graph_ = object_property_graph;
  data_property_graph_ = data_property_graph;
}

void ClassGraph::add(const std::string& value, ObjectVectors_t& object_vector)
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);
  ClassBranch_t* me = nullptr;
  //am I a created mother ?
  amIA(&me, tmp_mothers_, value);

  //am I a created branch ?
  amIA(&me, branchs_, value);

  //am I a created root ?
  amIA(&me, roots_, value);

  //am I created ?
  if(me == nullptr)
    me = new ClassBranch_t(value);

  me->nb_mothers_ += object_vector.mothers_.size();

  //am I a root ?
  if(me->nb_mothers_ == 0)
    roots_[value] = me;
  else
  {
    /**********************
    ** Class assertion
    **********************/
    //for all my mothers
    for(auto& mother : object_vector.mothers_)
    {
      ClassBranch_t* mother_branch = nullptr;
      getInMap(&mother_branch, mother.elem, roots_);
      getInMap(&mother_branch, mother.elem, branchs_);
      getInMap(&mother_branch, mother.elem, tmp_mothers_);
      if(mother_branch == nullptr)
      {
        mother_branch = new struct ClassBranch_t(mother.elem);
        tmp_mothers_[mother_branch->value()] = mother_branch;
      }

      conditionalPushBack(mother_branch->childs_, ClassElement_t(me, mother.probability, true));
      conditionalPushBack(me->mothers_, ClassElement_t(mother_branch, mother.probability));
    }
    //but i am also a branch
    branchs_[me->value()] = me;
  }

  /**********************
  ** Disjoint assertion
  **********************/
  //for all my disjoints
  for(size_t disjoints_i = 0; disjoints_i < object_vector.disjoints_.size(); disjoints_i++)
  {
    bool i_find_my_disjoint = false;

    //is a root my disjoint ?
    isMyDisjoint(me, object_vector.disjoints_[disjoints_i], roots_, i_find_my_disjoint);

    //is a branch my disjoint ?
    isMyDisjoint(me, object_vector.disjoints_[disjoints_i], branchs_, i_find_my_disjoint);

    //is a tmp mother is my disjoint ?
    isMyDisjoint(me, object_vector.disjoints_[disjoints_i], tmp_mothers_, i_find_my_disjoint);

    //I create my disjoint
    if(!i_find_my_disjoint)
    {
      ClassBranch_t* my_disjoint = new struct ClassBranch_t(object_vector.disjoints_[disjoints_i]);
      me->disjoints_.push_back(my_disjoint);
      my_disjoint->disjoints_.push_back(me); // TODO do not save
      tmp_mothers_[my_disjoint->value()] = my_disjoint; //I put my disjoint as tmp_mother
    }
  }

  /**********************
  ** Object Property assertion
  **********************/
  for(auto& object_relation : object_vector.object_relations_)
    addObjectProperty(me, object_relation);

  /**********************
  ** Data Property assertion
  **********************/
  //for all my properties
  for(auto& data_relation : object_vector.data_relations_)
    addDataProperty(me, data_relation);

  me->setSteady_dictionary(object_vector.dictionary_);
  if(me->dictionary_.spoken_.find("en") == me->dictionary_.spoken_.end())
    me->dictionary_.spoken_["en"].push_back(me->value());
  me->setSteady_muted_dictionary(object_vector.muted_dictionary_);

  mitigate(me);
}

void ClassGraph::add(std::vector<std::string>& disjoints)
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(size_t disjoints_i = 0; disjoints_i < disjoints.size(); disjoints_i++)
  {
    //I need to find myself
    ClassBranch_t* me = nullptr;
    //Am I a root ?
    amIA(&me, roots_, disjoints[disjoints_i], false);

    //Am I a branch ?
    amIA(&me, branchs_, disjoints[disjoints_i], false);

    //Am I a tmp_mother ?
    amIA(&me, tmp_mothers_, disjoints[disjoints_i], false);

    // I don't exist ? so I will be a tmp_mother
    if(me == nullptr)
    {
      me = new struct ClassBranch_t(disjoints[disjoints_i]);
      tmp_mothers_[me->value()] = me;
    }

    //for all my disjoints ...
    for(size_t disjoints_j = 0; disjoints_j < disjoints.size(); disjoints_j++)
    {
      //... excepted me
      if(disjoints_i != disjoints_j)
      {
        bool i_find_my_disjoint = false;

        //is a root my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], roots_, i_find_my_disjoint, false);

        //is a branch my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], branchs_, i_find_my_disjoint, false);

        //is a tmp mother is my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], tmp_mothers_, i_find_my_disjoint, false);

        //I create my disjoint
        if(!i_find_my_disjoint)
        {
          ClassBranch_t* my_disjoint = new struct ClassBranch_t(disjoints[disjoints_j]);
          me->disjoints_.push_back(my_disjoint);
          my_disjoint->disjoints_.push_back(me); // TODO do not save
          tmp_mothers_[my_disjoint->value()] = my_disjoint; //I put my disjoint as tmp_mother
        }
      }
    }
  }
}

/*********
*
* add functions
*
*********/

void ClassGraph::addObjectProperty(ClassBranch_t* me, Pair_t<std::string, std::string>& relation)
{
  ObjectPropertyBranch_t* property_branch = nullptr;
  getInMap(&property_branch, relation.first, object_property_graph_->roots_);
  getInMap(&property_branch, relation.first, object_property_graph_->branchs_);
  getInMap(&property_branch, relation.first, object_property_graph_->tmp_mothers_);
  if(property_branch == nullptr)
  {
    ObjectPropertyVectors_t empty_vectors;
    object_property_graph_->add(relation.first, empty_vectors);
    getInMap(&property_branch, relation.first, object_property_graph_->roots_);
  }

  ClassBranch_t* class_branch = nullptr;
  getInMap(&class_branch, relation.second, roots_);
  getInMap(&class_branch, relation.second, branchs_);
  getInMap(&class_branch, relation.second, tmp_mothers_);
  if(class_branch == nullptr)
  {
    class_branch = new ClassBranch_t(relation.second);
    tmp_mothers_[class_branch->value()] = class_branch;
  }

  me->object_relations_.push_back(ClassObjectRelationElement_t(property_branch, class_branch, relation.probability));
}

void ClassGraph::addDataProperty(ClassBranch_t* me, Pair_t<std::string, data_t>& relation)
{
  DataPropertyBranch_t* property_branch = nullptr;
  getInMap(&property_branch, relation.first, data_property_graph_->roots_);
  getInMap(&property_branch, relation.first, data_property_graph_->branchs_);
  getInMap(&property_branch, relation.first, data_property_graph_->tmp_mothers_);
  if(property_branch == nullptr)
  {
    DataPropertyVectors_t empty_vectors;
    data_property_graph_->add(relation.first, empty_vectors);
    getInMap(&property_branch, relation.first, data_property_graph_->roots_);
  }

  me->data_relations_.push_back(ClassDataRelationElement_t(property_branch, relation.second, relation.probability));
}

/*********
*
* get functions
*
*********/

std::unordered_set<std::string> ClassGraph::getDisjoint(const std::string& value)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  ClassBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(size_t disjoint_i = 0; disjoint_i < branch->disjoints_.size(); disjoint_i++)
      getDown(branch->disjoints_[disjoint_i], res);

  return res;
}

void ClassGraph::getDisjoint(ClassBranch_t* branch, std::unordered_set<ClassBranch_t*>& res)
{
  for(ClassBranch_t* it : branch->disjoints_)
    getDownPtr(it, res);
}

std::unordered_set<std::string> ClassGraph::select(std::unordered_set<std::string>& on, const std::string& class_selector)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(const std::string& it : on)
  {
    std::unordered_set<std::string> tmp = getUp(it);
    if(tmp.find(class_selector) != tmp.end())
      res.insert(it);
  }
  return res;
}

std::unordered_set<std::string> ClassGraph::getRelationFrom(const std::string& _class, int depth)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  ClassBranch_t* class_branch = container_.find(_class);
  if(class_branch != nullptr)
  {
    std::unordered_set<ClassBranch_t*> up_classes = getUpPtrSafe(class_branch);
    for(ClassBranch_t* it : up_classes)
      getRelationFrom(it, res, depth);
  }

  return res;
}

void ClassGraph::getRelationFrom(ClassBranch_t* class_branch, std::unordered_set<std::string>& res, int depth)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);
  if(class_branch != nullptr)
  {
    for(ClassObjectRelationElement_t& relation : class_branch->object_relations_)
      object_property_graph_->getUp(relation.first, res, depth);

    for(ClassDataRelationElement_t& relation : class_branch->data_relations_)
      data_property_graph_->getUp(relation.first, res, depth);
  }
}

std::unordered_set<std::string> ClassGraph::getRelatedFrom(const std::string& property)
{
  std::unordered_set<uint32_t> object_properties = object_property_graph_->getDownIdSafe(property);
  std::unordered_set<uint32_t> data_properties = data_property_graph_->getDownIdSafe(property);

  std::unordered_set<std::string> res;
  getRelatedFrom(object_properties, data_properties, res);

  return res;
}

void ClassGraph::getRelatedFrom(std::unordered_set<uint32_t>& object_properties, std::unordered_set<uint32_t>& data_properties, std::unordered_set<std::string>& res)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);
  for(size_t i = 0; i < all_branchs_.size(); i++)
  {
    for(ClassObjectRelationElement_t& relation : all_branchs_[i]->object_relations_)
      for (uint32_t id : object_properties)
        if(relation.first->get() == id)
        {
          std::unordered_set<ClassBranch_t*> tmp = getDownPtrSafe(all_branchs_[i]);
          for(auto tmp_i : tmp)
            res.insert(tmp_i->value());
        }

    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      for (uint32_t id : data_properties)
        if(relation.first->get() == id)
        {
          std::unordered_set<ClassBranch_t*> tmp = getDownPtrSafe(all_branchs_[i]);
          for(auto tmp_i : tmp)
            res.insert(tmp_i->value());
        }
  }
}

std::unordered_set<std::string> ClassGraph::getRelationOn(const std::string& _class, int depth)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  ClassBranch_t* class_branch = container_.find(_class);
  if(class_branch != nullptr)
  {
    uint32_t id = class_branch->get();

    for(size_t i = 0; i < all_branchs_.size(); i++)
      for(ClassObjectRelationElement_t& relation : all_branchs_[i]->object_relations_)
        if(relation.second->get() == id)
          object_property_graph_->getUp(relation.first, res, depth);
  }

  if(res.size() == 0)
    getRelationOnDataProperties(_class, res, depth);

  return res;
}

void ClassGraph::getRelationOnDataProperties(const std::string& _class, std::unordered_set<std::string>& res, int depth)
{
  for(size_t i = 0; i < all_branchs_.size(); i++)
    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      if(relation.second.value_ == _class)
        data_property_graph_->getUp(relation.first, res, depth);
}

std::unordered_set<std::string> ClassGraph::getRelatedOn(const std::string& property)
{
  std::unordered_set<uint32_t> object_properties = object_property_graph_->getDownIdSafe(property);
  std::unordered_set<uint32_t> data_properties = data_property_graph_->getDownIdSafe(property);

  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(size_t i = 0; i < all_branchs_.size(); i++)
  {
    for(ClassObjectRelationElement_t& relation : all_branchs_[i]->object_relations_)
      for (uint32_t id : object_properties)
        if(relation.first->get() == id)
          res.insert(relation.second->value());

    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      for (uint32_t id : data_properties)
        if(relation.first->get() == id)
          res.insert(relation.second.toString());
  }

  return res;
}

void ClassGraph::getRelatedOnDataProperties(const std::string& property, std::unordered_set<std::string>& res)
{
  std::unordered_set<uint32_t> data_properties = data_property_graph_->getDownIdSafe(property);

  for(size_t i = 0; i < all_branchs_.size(); i++)
  {
    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      for (uint32_t id : data_properties)
        if(relation.first->get() == id)
          res.insert(relation.second.toString());
  }
}

std::unordered_set<std::string> ClassGraph::getRelationWith(const std::string& _class)
{
  std::unordered_set<std::string> res;
  ClassBranch_t* class_branch = container_.find(_class);
  if(class_branch != nullptr)
  {
    std::map<std::string, int> properties;
    std::vector<int> depths;
    std::vector<std::string> tmp_res;
    getRelationWith(class_branch, properties, depths, tmp_res, 0);
    for(auto it : tmp_res)
      res.insert(it);
  }
  return res;
}

void ClassGraph::getRelationWith(ClassBranch_t* class_branch, std::map<std::string, int>& properties, std::vector<int>& depths, std::vector<std::string>& res, int depth)
{
  depth++;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  if(class_branch != nullptr)
  {
    for(ClassObjectRelationElement_t& relation : class_branch->object_relations_)
    {
      auto it = properties.find(relation.first->value());
      if(it != properties.end())
      {
        int index = properties[relation.first->value()];
        if(depths[index] > depth)
        {
          depths[index] = depth;
          res[index] = relation.second->value();
        }
      }
      else
      {
        properties[relation.first->value()] = res.size();
        depths.push_back(depth);
        res.push_back(relation.second->value());
      }
    }

    for(ClassDataRelationElement_t& relation : class_branch->data_relations_)
    {
      auto it = properties.find(relation.first->value());
      if(it != properties.end())
      {
        int index = properties[relation.first->value()];
        if(depths[index] > depth)
        {
          depths[index] = depth;
          res[index] = relation.second.toString();
        }
      }
      else
      {
        properties[relation.first->value()] = res.size();
        depths.push_back(depth);
        res.push_back(relation.second.toString());
      }
    }

    std::unordered_set<ClassBranch_t*> up_set = getUpPtrSafe(class_branch, 1);
    for(ClassBranch_t* up : up_set)
      if(up != class_branch)
        getRelationWith(up, properties, depths, res, depth);
  }
}

std::unordered_set<std::string> ClassGraph::getRelatedWith(const std::string& _class)
{
  std::unordered_set<std::string> res;
  std::unordered_set<uint32_t> doNotTake;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(size_t i = 0; i < all_branchs_.size(); i++)
  {
    for(ClassObjectRelationElement_t& relation : all_branchs_[i]->object_relations_)
      if(relation.second->value() == _class)
        objectGetRelatedWith(all_branchs_[i], relation.first->value(), _class, res, doNotTake);

    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      if(relation.second.value_ == _class)
        dataGetRelatedWith(all_branchs_[i], relation.first->value(), _class, res, doNotTake);
  }

  for(auto i : doNotTake)
    if(res.find(ValuedNode::table_[i]) != res.end())
      res.erase(ValuedNode::table_[i]);

  return res;
}

void ClassGraph::dataGetRelatedWith(ClassBranch_t* class_branch, const std::string& property, const std::string& _class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake)
{
  if(doNotTake.find(class_branch->get()) != doNotTake.end())
    return;

  if(class_branch != nullptr)
  {
    res.insert(class_branch->value());

    std::unordered_set<ClassBranch_t*> down_set = getDownPtrSafe(class_branch, 1);
    for(ClassBranch_t* down : down_set)
      if(down != class_branch)
      {
        bool found = false;

        for(ClassDataRelationElement_t& relation : down->data_relations_)
          if(relation.first->value() == property)
            if(relation.second.value_ != _class)
            {
              found = true;
              getDownIdSafe(down, doNotTake);
            }

        if(found == false)
          dataGetRelatedWith(down, property, _class, res, doNotTake);
      }
  }
}

void ClassGraph::objectGetRelatedWith(ClassBranch_t* class_branch, const std::string& property, const std::string& _class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake)
{
  if(doNotTake.find(class_branch->get()) != doNotTake.end())
    return;

  if(class_branch != nullptr)
  {
    res.insert(class_branch->value());

    std::unordered_set<ClassBranch_t*> down_set = getDownPtrSafe(class_branch, 1);
    for(ClassBranch_t* down : down_set)
      if(down != class_branch)
      {
        bool found = false;
        for(ClassObjectRelationElement_t& relation : down->object_relations_)
          if(relation.first->value() == property)
            if(relation.second->value() != _class)
            {
              found = true;
              getDownIdSafe(down, doNotTake);
            }

        if(found == false)
          objectGetRelatedWith(down, property, _class, res, doNotTake);
      }
  }
}

std::unordered_set<std::string> ClassGraph::getFrom(const std::string& param)
{
  std::unordered_set<std::string> res;
  std::string _class;
  std::string property;
  size_t pose = param.find(":");
  if(pose != std::string::npos)
  {
    _class = param.substr(0, pose);
    property = param.substr(pose+1);
    return getFrom(_class, property);
  }
  return res;
}

std::unordered_set<std::string> ClassGraph::getFrom(const std::string& _class, const std::string& property)
{
  std::unordered_set<uint32_t> object_properties = object_property_graph_->getDownIdSafe(property);
  std::unordered_set<uint32_t> data_properties = data_property_graph_->getDownIdSafe(property);
  std::unordered_set<uint32_t> down_classes = getDownIdSafe(_class);

  std::unordered_set<std::string> res;
  std::unordered_set<uint32_t> doNotTake;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(size_t i = 0; i < all_branchs_.size(); i++)
  {
    for(ClassObjectRelationElement_t& relation : all_branchs_[i]->object_relations_)
      for(uint32_t class_id : down_classes)
        if(relation.second->get() == class_id)
          for (uint32_t id : object_properties)
            if(relation.first->get() == id)
              objectGetRelatedWith(all_branchs_[i], relation.first->value(), ValuedNode::table_[class_id], res, doNotTake);

    for(ClassDataRelationElement_t& relation : all_branchs_[i]->data_relations_)
      if(relation.second.value_ == _class)
        for (uint32_t id : data_properties)
          if(relation.first->get() == id)
            dataGetRelatedWith(all_branchs_[i], relation.first->value(), _class, res, doNotTake);
  }

  for(auto i : doNotTake)
    if(res.find(ValuedNode::table_[i]) != res.end())
      res.erase(ValuedNode::table_[i]);

  return res;
}

std::unordered_set<std::string> ClassGraph::getOn(const std::string& param)
{
  std::unordered_set<std::string> res;
  std::string _class;
  std::string property;
  size_t pose = param.find(":");
  if(pose != std::string::npos)
  {
    _class = param.substr(0, pose);
    property = param.substr(pose+1);
    return getOn(_class, property);
  }
  return res;
}

std::unordered_set<std::string> ClassGraph::getOn(const std::string& _class, const std::string& property)
{
  std::unordered_set<uint32_t> object_properties = object_property_graph_->getDownIdSafe(property);
  std::unordered_set<uint32_t> data_properties = data_property_graph_->getDownIdSafe(property);

  int found_depth = -1;
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  ClassBranch_t* class_branch = container_.find(_class);
  getOn(class_branch, object_properties, data_properties, res, 0, found_depth);

  return res;
}

void ClassGraph::getOn(ClassBranch_t* class_branch, std::unordered_set<uint32_t>& object_properties, std::unordered_set<uint32_t>& data_properties, std::unordered_set<std::string>& res, uint32_t current_depth, int& found_depth)
{
  if(class_branch != nullptr)
  {
    std::unordered_set<std::string> tmp_res;

    for(ClassObjectRelationElement_t& relation : class_branch->object_relations_)
      for (uint32_t id : object_properties)
        if(relation.first->get() == id)
          tmp_res.insert(relation.second->value());

    if(tmp_res.size() == 0)
      for(ClassDataRelationElement_t& relation : class_branch->data_relations_)
        for (uint32_t id : data_properties)
          if(relation.first->get() == id)
            tmp_res.insert(relation.second.toString());

    if(tmp_res.size() != 0)
      if(current_depth < (uint32_t)found_depth)
      {
        if(data_properties.size())
        {
          res = tmp_res;
          found_depth = current_depth;
          return;
        }
        else
        res.insert(tmp_res.begin(), res.end());
      }

    current_depth++;
    std::unordered_set<ClassBranch_t*> up_set = getUpPtrSafe(class_branch, 1);
    for(ClassBranch_t* up : up_set)
      if(up != class_branch)
        getOn(up, object_properties, data_properties, res, current_depth, found_depth);
  }
}

std::unordered_set<std::string> ClassGraph::getWith(const std::string& param, int depth)
{
  std::unordered_set<std::string> res;
  size_t pose = param.find(":");
  if(pose != std::string::npos)
  {
    std::string first_class = param.substr(0, pose);
    std::string second_class = param.substr(pose+1);
    return getWith(first_class, second_class, depth);
  }
  return res;
}

std::unordered_set<std::string> ClassGraph::getWith(const std::string& first_class, const std::string& second_class, int depth)
{
  std::unordered_set<std::string> res;

  int found_depth = -1;
  uint32_t current_depth = 0;
  std::unordered_set<uint32_t> doNotTake;
  std::unordered_set<ClassBranch_t*> up_set;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  up_set.insert(container_.find(first_class));
  while(up_set.size() > 0)
  {
    std::unordered_set<ClassBranch_t*> next_step;
    for(auto up : up_set)
      getWith(up, second_class, res, doNotTake, current_depth, found_depth, depth, next_step);

    up_set = next_step;
    current_depth++;
  }

  return res;
}

void ClassGraph::getWith(ClassBranch_t* first_class, const std::string& second_class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake, uint32_t current_depth, int& found_depth, int depth_prop, std::unordered_set<ClassBranch_t*>& next_step)
{
  if(first_class != nullptr)
  {
    std::unordered_set<std::string> tmp_res;
    std::unordered_set<uint32_t> doNotTake_tmp;

    for(ClassObjectRelationElement_t& relation : first_class->object_relations_)
    {
      doNotTake_tmp.insert(relation.first->get());
      if(relation.second->value() == second_class)
        if(doNotTake.find(relation.first->get()) == doNotTake.end())
          object_property_graph_->getUp(relation.first, tmp_res, depth_prop);
    }

    for(ClassDataRelationElement_t& relation : first_class->data_relations_)
    {
      doNotTake_tmp.insert(relation.first->get());
      if(relation.second.value_ == second_class)
        if(doNotTake.find(relation.first->get()) == doNotTake.end())
          data_property_graph_->getUp(relation.first, tmp_res, depth_prop);
    }

    doNotTake.insert(doNotTake_tmp.begin(), doNotTake_tmp.end());

    if(tmp_res.size() != 0)
      if(current_depth < (uint32_t)found_depth)
      {
        res = tmp_res;
        found_depth = current_depth;
        return;
      }

    current_depth++;
    getUpPtr(first_class, next_step, 1);
    next_step.erase(first_class);
  }
}

std::unordered_set<std::string> ClassGraph::getDownIndividual(ClassBranch_t* branch)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(auto indiv : branch->individual_childs_)
    res.insert(indiv.elem->value());

  return res;
}

void ClassGraph::getDownIndividual(ClassBranch_t* branch, std::unordered_set<std::string>& res)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);
  for(auto indiv : branch->individual_childs_)
    res.insert(indiv.elem->value());
}

std::unordered_set<IndividualBranch_t*> ClassGraph::getDownIndividualPtrSafe(ClassBranch_t* branch)
{
  std::unordered_set<IndividualBranch_t*> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);

  for(auto indiv : branch->individual_childs_)
    res.insert(indiv.elem);

  return res;
}

void ClassGraph::getDownIndividualPtrSafe(ClassBranch_t* branch, std::unordered_set<IndividualBranch_t*>& res)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ClassBranch_t>::mutex_);
  for(auto indiv : branch->individual_childs_)
    res.insert(indiv.elem);
}

void ClassGraph::deleteClass(ClassBranch_t* _class)
{
  if(_class != nullptr)
  {
    //std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);

    // erase indiv from parents
    std::unordered_set<ClassBranch_t*> up_set;
    getUpPtr(_class, up_set, 1);
    for(auto up : up_set)
    {
      for(size_t i = 0; i < up->childs_.size();)
      {
        if(up->childs_[i].elem == _class)
          up->childs_.erase(up->childs_.begin() + i);
        else
          i++;
      }
    }

    std::unordered_set<ClassBranch_t*> down_set;
    getDownPtr(_class, down_set, 1);
    for(auto down : down_set)
    {
      for(size_t i = 0; i < down->mothers_.size();)
      {
        if(down->mothers_[i].elem == _class)
          down->mothers_.erase(down->mothers_.begin() + i);
        else
          i++;
      }
    }

    IndividualBranch_t* elem = nullptr;
    for(auto indiv : _class->individual_childs_)
    {
      elem = indiv.elem;
      for(size_t i = 0; i < elem->is_a_.size();)
      {
        if(elem->is_a_[i].elem == _class)
          elem->is_a_.erase(elem->is_a_.begin() + i);
        else
          i++;
      }
    }

    //erase properties applied to _class
    int index = deletePropertiesOnClass(_class, all_branchs_);

    //delete indiv
    if(index > 0)
      all_branchs_.erase(all_branchs_.begin() + index);

    container_.erase(_class);
    delete _class;
  }
}

int ClassGraph::deletePropertiesOnClass(ClassBranch_t* _class, std::vector<ClassBranch_t*> vect)
{
  int class_index = -1;
  for(size_t class_i = 0; class_i < vect.size(); class_i++)
  {
    if(vect[class_i] == _class)
      class_index = class_i;

    for(size_t i = 0; i < vect[class_i]->object_relations_.size();)
      if(vect[class_i]->object_relations_[i].second == _class)
        vect[class_i]->object_relations_.erase(vect[class_i]->object_relations_.begin() + i);
      else
        i++;
  }
  return class_index;
}

void ClassGraph::addLang(std::string& _class, std::string& lang, std::string& name)
{
  ClassBranch_t* branch = findBranch(_class);
  if(branch != nullptr)
  {
    lang = lang.substr(1);
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    branch->setSteady_dictionary(lang, name);
    branch->updated_ = true;
  }
}

void ClassGraph::addInheritage(std::string& class_base, std::string& class_inherited)
{
  ClassBranch_t* branch = findBranch(class_base);
  if(branch != nullptr)
  {
    ClassBranch_t* inherited = findBranch(class_inherited);
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    if(inherited == nullptr)
    {
      IndividualBranch_t* tmp = individual_graph_->findBranch(class_inherited);
      if(tmp != nullptr)
        inherited = individual_graph_->upgradeToBranch(tmp);
      else
      {
        inherited = new ClassBranch_t(class_inherited);
        container_.insert(inherited);
        all_branchs_.push_back(inherited);
      }
    }
    conditionalPushBack(branch->mothers_, ClassElement_t(inherited));
    conditionalPushBack(inherited->childs_, ClassElement_t(branch));
    branch->updated_ = true;
    inherited->updated_ = true;
    mitigate(branch);
  }
}

bool ClassGraph::addProperty(std::string& class_from, std::string& property, std::string& class_on)
{
  ClassBranch_t* branch_from = findBranch(class_from);
  if(branch_from != nullptr)
  {
    ClassBranch_t* branch_on = findBranch(class_on);
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    if(branch_on == nullptr)
    {
      IndividualBranch_t* test = individual_graph_->findBranch(class_on);
      if(test != nullptr)
        return false; // TODO ERR

      branch_on = new ClassBranch_t(class_on);
      container_.insert(branch_on);
      all_branchs_.push_back(branch_on);
    }

    ObjectPropertyBranch_t* branch_prop = object_property_graph_->findBranch(property);
    if(branch_prop == nullptr)
    {
      DataPropertyBranch_t* test = data_property_graph_->findBranch(property);
      if(test != nullptr)
        return false; // TODO ERR

      std::lock_guard<std::shared_timed_mutex> lock_property(object_property_graph_->mutex_);
      branch_prop = new ObjectPropertyBranch_t(property);
      object_property_graph_->container_.insert(branch_prop);
      object_property_graph_->all_branchs_.push_back(branch_prop);
    }

    if(checkRangeAndDomain(branch_from, branch_prop, branch_on))
    {
      conditionalPushBack(branch_from->object_relations_, ClassObjectRelationElement_t(branch_prop, branch_on));
      return true;
    }
    else
        return false;
  }
  return false;
}

bool ClassGraph::addProperty(std::string& class_from, std::string& property, std::string& type, std::string& data)
{
  ClassBranch_t* branch_from = findBranch(class_from);
  if(branch_from != nullptr)
  {
    data_t data_branch;
    data_branch.value_ = data;
    data_branch.type_ = type;

    DataPropertyBranch_t* branch_prop = data_property_graph_->findBranch(property);
    if(branch_prop == nullptr)
    {
      ObjectPropertyBranch_t* test = object_property_graph_->findBranch(property);
      if(test != nullptr)
        return false; // TODO ERR

      std::lock_guard<std::shared_timed_mutex> lock_property(data_property_graph_->mutex_);
      branch_prop = new DataPropertyBranch_t(property);
      data_property_graph_->container_.insert(branch_prop);
      data_property_graph_->all_branchs_.push_back(branch_prop);
    }

    if(checkRangeAndDomain(branch_from, branch_prop, data_branch))
    {
      conditionalPushBack(branch_from->data_relations_, ClassDataRelationElement_t(branch_prop, data_branch));
      return true;
    }
    else
      return false;
  }
  return false;
}

bool ClassGraph::addPropertyInvert(std::string& class_from, std::string& property, std::string& class_on)
{
  ClassBranch_t* branch_on = findBranch(class_on);
  if(branch_on != nullptr)
  {
    ClassBranch_t* branch_from = findBranch(class_from);
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    if(branch_from == nullptr)
    {
      IndividualBranch_t* test = individual_graph_->findBranch(class_from);
      if(test != nullptr)
        return false; // TODO ERR

      branch_from = new ClassBranch_t(class_from);
      container_.insert(branch_from);
      all_branchs_.push_back(branch_from);
    }

    ObjectPropertyBranch_t* branch_prop = object_property_graph_->findBranch(property);
    if(branch_prop == nullptr)
    {
      DataPropertyBranch_t* test = data_property_graph_->findBranch(property);
      if(test != nullptr)
        return false; // TODO ERR

      std::lock_guard<std::shared_timed_mutex> lock_property(object_property_graph_->mutex_);
      branch_prop = new ObjectPropertyBranch_t(property);
      object_property_graph_->container_.insert(branch_prop);
      object_property_graph_->all_branchs_.push_back(branch_prop);
    }

    if(checkRangeAndDomain(branch_from, branch_prop, branch_on))
    {
      conditionalPushBack(branch_from->object_relations_, ClassObjectRelationElement_t(branch_prop, branch_on));
      return true;
    }
    else
      return false;
  }
  return false;
}

void ClassGraph::removeLang(std::string& indiv, std::string& lang, std::string& name)
{
  ClassBranch_t* branch = findBranch(indiv);

  std::lock_guard<std::shared_timed_mutex> lock(mutex_);

  lang = lang.substr(1);
  removeFromDictionary(branch->dictionary_.spoken_, lang, name);
  removeFromDictionary(branch->dictionary_.muted_, lang, name);
  removeFromDictionary(branch->steady_dictionary_.spoken_, lang, name);
  removeFromDictionary(branch->steady_dictionary_.muted_, lang, name);
}

void ClassGraph::removeInheritage(std::string& class_base, std::string& class_inherited)
{
  ClassBranch_t* branch_base = findBranch(class_base);
  ClassBranch_t* branch_inherited = findBranch(class_inherited);

  if(branch_base == nullptr)
    return;
  if(branch_inherited == nullptr)
    return;

  std::lock_guard<std::shared_timed_mutex> lock(mutex_);

  removeFromElemVect(branch_base->mothers_, branch_inherited);
  removeFromElemVect(branch_inherited->childs_, branch_base);

  branch_base->updated_ = true;
  branch_inherited->updated_ = true;
}

bool ClassGraph::removeProperty(std::string& class_from, std::string& property, std::string& class_on)
{
  ClassBranch_t* branch_from = findBranch(class_from);
  if(branch_from != nullptr)
  {
    for(size_t i = 0; i < branch_from->object_relations_.size();)
    {
      if(branch_from->object_relations_[i].first->value() == property)
      {
        if(branch_from->object_relations_[i].second->value() == class_on)
        {
          branch_from->object_relations_[i].second->updated_ = true;
          branch_from->object_relations_.erase(branch_from->object_relations_.begin() + i);
        }
        else
          i++;
      }
      else
        i++;
    }

    return true;
  }
  return false;
}

bool ClassGraph::removeProperty(std::string& class_from, std::string& property, std::string& type, std::string& data)
{
  ClassBranch_t* branch_from = findBranch(class_from);
  if(branch_from != nullptr)
  {
    for(size_t i = 0; i < branch_from->data_relations_.size();)
    {
      if(branch_from->data_relations_[i].first->value() == property)
      {
        if((branch_from->data_relations_[i].second.type_ == type) &&
          (branch_from->data_relations_[i].second.value_ == data))
        {
          branch_from->data_relations_.erase(branch_from->data_relations_.begin() + i);
        }
        else
          i++;
      }
      else
        i++;
    }

    return true;
  }
  return false;
}

bool ClassGraph::checkRangeAndDomain(ClassBranch_t* from, ObjectPropertyBranch_t* prop, ClassBranch_t* on)
{
  std::unordered_set<ClassBranch_t*> up_from;
  getUpPtr(from, up_from);

  std::unordered_set<ObjectPropertyBranch_t*> prop_up;
  object_property_graph_->getUpPtr(prop, prop_up);

  //DOMAIN
  std::unordered_set<ClassBranch_t*> domain;
  for(auto prop : prop_up)
    object_property_graph_->getDomainPtr(prop, domain);

  if(domain.size() != 0)
  {
    ClassBranch_t* intersection = findIntersection(up_from, domain);
    if(intersection == nullptr)
    {
      std::unordered_set<ClassBranch_t*> disjoints;
      for(auto dom : domain)
        getDisjoint(dom, disjoints);
      intersection = findIntersection(up_from, disjoints);

      if(intersection == nullptr)
        from->flags_["domain"].push_back(prop->value());
      else
        return false;
    }
  }

  //RANGE
  std::unordered_set<ClassBranch_t*> up_on;
  getUpPtr(on, up_on);

  std::unordered_set<ClassBranch_t*> range;
  for(auto prop : prop_up)
    object_property_graph_->getRangePtr(prop, range);

  if(range.size() != 0)
  {
    ClassBranch_t* intersection = findIntersection(up_on, range);
    if(intersection == nullptr)
    {
      std::unordered_set<ClassBranch_t*> disjoints;
      for(auto ran : range)
        getDisjoint(ran, disjoints);
      intersection = findIntersection(up_on, disjoints);

      if(intersection == nullptr)
        from->flags_["range"].push_back(prop->value());
      else
        return false;
    }
  }

  return true;
}

bool ClassGraph::checkRangeAndDomain(ClassBranch_t* from, DataPropertyBranch_t* prop, data_t& data)
{
  std::unordered_set<ClassBranch_t*> up_from;
  getUpPtr(from, up_from);

  std::unordered_set<DataPropertyBranch_t*> prop_up;
  data_property_graph_->getUpPtr(prop, prop_up);

  //DOMAIN
  std::unordered_set<ClassBranch_t*> domain;
  for(auto prop : prop_up)
    data_property_graph_->getDomainPtr(prop, domain);

  if(domain.size() != 0)
  {
    ClassBranch_t* intersection = findIntersection(up_from, domain);
    if(intersection == nullptr)
    {
      std::unordered_set<ClassBranch_t*> disjoints;
      for(auto dom : domain)
        getDisjoint(dom, disjoints);
      intersection = findIntersection(up_from, disjoints);

      if(intersection == nullptr)
        from->flags_["range"].push_back(prop->value());
      else
        return false;
    }
  }

  //RANGE
  std::unordered_set<std::string> range = data_property_graph_->getRange(prop->value());
  if(range.size() != 0)
  {
    std::unordered_set<std::string>::iterator intersection = std::find(range.begin(), range.end(), data.type_);
    if(intersection == range.end())
      return false;
  }

  return true;
}

} // namespace ontologenius
