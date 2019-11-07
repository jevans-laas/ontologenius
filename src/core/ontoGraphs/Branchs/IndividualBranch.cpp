#include "ontoloGenius/core/ontoGraphs/Branchs/IndividualBranch.h"

#include <algorithm>

namespace ontologenius {

void IndividualBranch_t::setFullSteady()
{
  steady_.is_a_.clear();
  for(size_t i = 0; i < is_a_.size(); i++)
    steady_.is_a_.push_back(is_a_[i]);

  steady_.object_relations_.clear();
  for(size_t i = 0; i < object_relations_.size(); i++)
    steady_.object_relations_.push_back(object_relations_[i]);

  steady_.data_relations_.clear();
  for(size_t i = 0; i < data_relations_.size(); i++)
    steady_.data_relations_.push_back(data_relations_[i]);

  steady_.same_as_.clear();
  for(size_t i = 0; i < same_as_.size(); i++)
    steady_.same_as_.push_back(same_as_[i]);

  steady_.distinct_.clear();
  for(size_t i = 0; i < distinct_.size(); i++)
    steady_.distinct_.push_back(distinct_[i]);
}

void IndividualBranch_t::setSteady_is_a(const ClassElement_t& is_a)
{
  conditionalPushBack(steady_.is_a_, is_a);
  conditionalPushBack(is_a_, is_a);
}

void IndividualBranch_t::setSteady_objectRelation(const IndivObjectRelationElement_t& object_relation)
{
  steady_.object_relations_.push_back(object_relation);
  object_relations_.push_back(object_relation);
}

void IndividualBranch_t::setSteady_dataRelation(const IndivDataRelationElement_t& data_relation)
{
  steady_.data_relations_.push_back(data_relation);
  data_relations_.push_back(data_relation);
}

void IndividualBranch_t::setSteady_same_as(IndividualBranch_t* same_as)
{
  conditionalPushBack(steady_.same_as_, same_as);
  conditionalPushBack(same_as_, same_as);
}

void IndividualBranch_t::setSteady_distinct(IndividualBranch_t* distinct)
{
  conditionalPushBack(steady_.distinct_, distinct);
  conditionalPushBack(distinct_, distinct);
}

void IndividualBranch_t::setSteady_dictionary(std::string lang, std::string word)
{
  conditionalPushBack(steady_.dictionary_[lang], word);
  conditionalPushBack(dictionary_[lang], word);
}

void IndividualBranch_t::setSteady_muted_dictionary(std::string lang, std::string word)
{
  conditionalPushBack(steady_.muted_dictionary_[lang], word);
  conditionalPushBack(muted_dictionary_[lang], word);
}

void IndividualBranch_t::setSteady_dictionary(std::map<std::string, std::vector<std::string>> dictionary)
{
  for(auto it : dictionary)
  {
    if(steady_.dictionary_.find(it.first) == steady_.dictionary_.end())
      steady_.dictionary_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(steady_.dictionary_[it.first], name);
    }

    if(dictionary_.find(it.first) == dictionary_.end())
      dictionary_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(dictionary_[it.first], name);
    }
  }
}

void IndividualBranch_t::setSteady_muted_dictionary(std::map<std::string, std::vector<std::string>> dictionary)
{
  for(auto it : dictionary)
  {
    if(steady_.muted_dictionary_.find(it.first) == steady_.muted_dictionary_.end())
      steady_.muted_dictionary_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(steady_.muted_dictionary_[it.first], name);
    }

    if(muted_dictionary_.find(it.first) == muted_dictionary_.end())
      muted_dictionary_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(muted_dictionary_[it.first], name);
    }
  }
}

int IndividualBranch_t::ObjectPropertyExistSteady(ObjectPropertyBranch_t* property, IndividualBranch_t* individual)
{
  int res = -1;
  for(size_t i = 0; i < steady_.object_relations_.size(); i++)
  {
    if(steady_.object_relations_[i].first == property)
      if(steady_.object_relations_[i].second == individual)
      {
        res = i;
        break;
      }
  }

  return res;
}

int IndividualBranch_t::ObjectPropertyExist(ObjectPropertyBranch_t* property, IndividualBranch_t* individual)
{
  int res = -1;
  for(size_t i = 0; i < object_relations_.size(); i++)
  {
    if(object_relations_[i].first == property)
      if(object_relations_[i].second == individual)
      {
        res = i;
        break;
      }
  }

  return res;
}

} // namespace ontologenius
