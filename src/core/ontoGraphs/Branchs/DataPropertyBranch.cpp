#include "ontologenius/core/ontoGraphs/Branchs/DataPropertyBranch.h"

#include <algorithm>

namespace ontologenius {

void DataPropertyBranch_t::setSteady_dictionary(const std::string& lang, const std::string& word)
{
  conditionalPushBack(dictionary_.spoken_[lang], word);
  conditionalPushBack(steady_dictionary_.spoken_[lang], word);
}

void DataPropertyBranch_t::setSteady_muted_dictionary(const std::string& lang, const std::string& word)
{
  conditionalPushBack(dictionary_.muted_[lang], word);
  conditionalPushBack(steady_dictionary_.muted_[lang], word);
}

void DataPropertyBranch_t::setSteady_dictionary(const std::map<std::string, std::vector<std::string>>& dictionary)
{
  for(auto it : dictionary)
  {
    if(dictionary_.spoken_.find(it.first) == dictionary_.spoken_.end())
      dictionary_.spoken_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(dictionary_.spoken_[it.first], name);
    }

    if(steady_dictionary_.spoken_.find(it.first) == steady_dictionary_.spoken_.end())
      steady_dictionary_.spoken_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(steady_dictionary_.spoken_[it.first], name);
    }
  }
}

void DataPropertyBranch_t::setSteady_muted_dictionary(const std::map<std::string, std::vector<std::string>>& dictionary)
{
  for(auto it : dictionary)
  {
    if(dictionary_.muted_.find(it.first) == dictionary_.muted_.end())
      dictionary_.muted_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(dictionary_.muted_[it.first], name);
    }

    if(steady_dictionary_.muted_.find(it.first) == steady_dictionary_.muted_.end())
      steady_dictionary_.muted_[it.first] = it.second;
    else
    {
      for(const auto& name : it.second)
        conditionalPushBack(steady_dictionary_.muted_[it.first], name);
    }
  }
}

} // namespace ontologenius
