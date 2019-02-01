#ifndef CLASSGRAPH_H
#define CLASSGRAPH_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <stdint.h>

#include "ontoloGenius/core/ontoGraphs/Graphs/OntoGraph.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/ClassBranch.h"

struct ObjectVectors_t
{
   std::vector<std::string> mothers_;
   std::vector<std::string> disjoints_;
   std::map<std::string, std::vector<std::string>> dictionary_;

   std::vector<std::string> object_properties_name_;
   std::vector<std::string> object_properties_on_;
   std::vector<bool> object_properties_deduced_;

   std::vector<std::string> data_properties_name_;
   std::vector<std::string> data_properties_type_;
   std::vector<std::string> data_properties_value_;
   std::vector<bool> data_properties_deduced_;
};

//for friend
class ClassDrawer;
class ObjectPropertyGraph;
class DataPropertyGraph;
class IndividualGraph;
class ClassChecker;

class ClassGraph : public OntoGraph<ClassBranch_t>
{
  friend ClassDrawer;
  friend ObjectPropertyGraph;
  friend DataPropertyGraph;
  friend IndividualGraph;

  friend ClassChecker;
public:
  ClassGraph(IndividualGraph* individual_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph);
  ~ClassGraph() {}

  void add(const std::string& value, ObjectVectors_t& object_vector) noexcept;
  void add(std::vector<std::string>& disjoints) noexcept;

  std::unordered_set<std::string> getDisjoint(const std::string& value) noexcept;
  void getDisjoint(ClassBranch_t* branch, std::unordered_set<ClassBranch_t*>& res) noexcept;
  std::unordered_set<std::string> select(std::unordered_set<std::string>& on, const std::string& class_selector) noexcept;

  std::unordered_set<std::string> getRelationFrom(const std::string& _class, int depth = -1) noexcept;  //C3
  std::unordered_set<std::string> getRelatedFrom(const std::string& property) noexcept;     //C3
  std::unordered_set<std::string> getRelationOn(const std::string& _class, int depth = -1) noexcept;    //C4
  std::unordered_set<std::string> getRelatedOn(const std::string& property) noexcept;       //C3
  std::unordered_set<std::string> getRelationWith(const std::string& _class) noexcept;  //C3
  std::unordered_set<std::string> getRelatedWith(const std::string& _class) noexcept;   //C3
  std::unordered_set<std::string> getFrom(const std::string& param) noexcept;
  std::unordered_set<std::string> getFrom(const std::string& _class, const std::string& property) noexcept;
  std::unordered_set<std::string> getOn(const std::string& param) noexcept;
  std::unordered_set<std::string> getOn(const std::string& _class, const std::string& property) noexcept;
  std::unordered_set<std::string> getWith(const std::string& param, int depth = -1) noexcept;
  std::unordered_set<std::string> getWith(const std::string& first_class, const std::string& second_class, int depth = -1) noexcept;

  std::unordered_set<std::string> getDownIndividual(ClassBranch_t* branch) noexcept;
  void getDownIndividual(ClassBranch_t* branch, std::unordered_set<std::string>& res) noexcept;
  std::unordered_set<IndividualBranch_t*> getDownIndividualPtrSafe(ClassBranch_t* branch) noexcept;
  void getDownIndividualPtrSafe(ClassBranch_t* branch, std::unordered_set<IndividualBranch_t*>& res) noexcept;

  void deleteClass(ClassBranch_t* _class) noexcept;
  int deletePropertiesOnClass(ClassBranch_t* _class, std::vector<ClassBranch_t*> vect) noexcept;
  void addLang(std::string& _class, std::string& lang, std::string& name) noexcept;
  void addInheritage(std::string& class_base, std::string& class_inherited) noexcept;
  bool addProperty(std::string& class_from, std::string& property, std::string& class_on) noexcept;
  bool addProperty(std::string& class_from, std::string& property, std::string& type, std::string& data) noexcept;
  bool addPropertyInvert(std::string& class_from, std::string& property, std::string& class_on) noexcept;
  void removeLang(std::string& indiv, std::string& lang, std::string& name) noexcept;
  void removeInheritage(std::string& class_base, std::string& class_inherited) noexcept;
  bool removeProperty(std::string& class_from, std::string& property, std::string& class_on) noexcept;
  bool removeProperty(std::string& class_from, std::string& property, std::string& type, std::string& data) noexcept;

private:
  ObjectPropertyGraph* object_property_graph_;
  DataPropertyGraph* data_property_graph_;
  IndividualGraph* individual_graph_;

  void addObjectPropertyName(ClassBranch_t* me, std::string& name, bool deduced) noexcept;
  void addObjectPropertyOn(ClassBranch_t* me, std::string& name, bool deduced) noexcept;
  void addDataPropertyName(ClassBranch_t* me, std::string& name, bool deduced) noexcept;
  void addDataPropertyData(ClassBranch_t* me, data_t& data, bool deduced) noexcept;

  void setSteadyObjectProperty(ClassBranch_t* branch_from, ObjectPropertyBranch_t* branch_prop, ClassBranch_t* branch_on) noexcept;
  void setSteadyDataProperty(ClassBranch_t* branch_from, DataPropertyBranch_t* branch_prop, data_t& data) noexcept;

  void getRelationFrom(ClassBranch_t* class_branch, std::unordered_set<std::string>& res, int depth) noexcept;
  void getRelatedFrom(std::unordered_set<uint32_t>& object_properties, std::unordered_set<uint32_t>& data_properties, std::unordered_set<std::string>& res) noexcept;
  void getRelationOnDataProperties(const std::string& _class, std::unordered_set<std::string>& res, int depth) noexcept;
  void getRelatedOnDataProperties(const std::string& property, std::unordered_set<std::string>& res) noexcept;
  void getRelationWith(ClassBranch_t* class_branch, std::map<std::string, int>& properties, std::vector<int>& depths, std::vector<std::string>& res, int depth) noexcept;
  void dataGetRelatedWith(ClassBranch_t* class_branch, const std::string& property, const std::string& _class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake) noexcept;
  void objectGetRelatedWith(ClassBranch_t* class_branch, const std::string& property, const std::string& _class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake) noexcept;
  void getOn(ClassBranch_t* class_branch, std::unordered_set<uint32_t>& object_properties, std::unordered_set<uint32_t>& data_properties, std::unordered_set<std::string>& res, uint32_t current_depth, int& found_depth) noexcept;
  void getWith(ClassBranch_t* first_class, const std::string& second_class, std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& doNotTake, uint32_t current_depth, int& found_depth, int depth_prop, std::unordered_set<ClassBranch_t*>& next_step) noexcept;

  bool checkRangeAndDomain(ClassBranch_t* from, ObjectPropertyBranch_t* prop, ClassBranch_t* on) noexcept;
  bool checkRangeAndDomain(ClassBranch_t* from, DataPropertyBranch_t* prop, data_t& data) noexcept;

  void isMyDisjoint(ClassBranch_t* me, const std::string& disjoint, std::map<std::string, ClassBranch_t*>& vect, bool& find, bool all = true) noexcept
  {
    if(find)
      return;

    auto it = vect.find(disjoint);
    if(it != vect.end())
    {
      me->setSteady_disjoint(it->second);
      if(all)
        it->second->disjoints_.push_back(me);
      find = true;
    }
  }

  void isMyObjectPropertiesOn(ClassBranch_t* me, const std::string& propertyOn, std::map<std::string, ClassBranch_t*>& vect, bool& find, bool deduced) noexcept
  {
    if(find)
      return;

    auto it = vect.find(propertyOn);
    if(it != vect.end())
    {
      if(deduced == false)
        me->setSteady_object_properties_on(it->second);
      else
        me->object_properties_on_.push_back(it->second);

      find = true;
    }
  }

  ClassBranch_t* findIntersection(std::unordered_set<ClassBranch_t*>& base, std::unordered_set<ClassBranch_t*>& comp) noexcept
  {
    for (ClassBranch_t* it : comp)
    {
      if(base.find(it) != base.end())
        return it;
    }
    return nullptr;
  }
};

#endif /* CLASSGRAPH_H */
