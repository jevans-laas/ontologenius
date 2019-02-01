#ifndef BRANCHCONTAINERMAP_H
#define BRANCHCONTAINERMAP_H

#include <map>

#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerBase.h"

template <typename B>
class BranchContainerMap : public BranchContainerBase<B>
{
public:
  BranchContainerMap() {}
  BranchContainerMap(const BranchContainerMap& base);
  virtual ~BranchContainerMap() {} //B* is destructed by ontograph

  virtual B* find(const std::string& word) noexcept;
  virtual std::vector<B*> find(bool (*comp)(B*, std::string, std::string), const std::string& word, const std::string& lang) noexcept;
  virtual void load(std::vector<B*>& vect) noexcept;
  virtual void insert(B* branch) noexcept;
  virtual void erase(B* branch) noexcept;
private:
  std::map<std::string, B*> nodes_;
};

template <typename B>
BranchContainerMap<B>::BranchContainerMap(const BranchContainerMap& base)
{
  for(auto& it : base.nodes_)
  {
    B* tmp = new B();
    *tmp = *(it.second);
    nodes_[it.first] = tmp;
  }
}

template <typename B>
B* BranchContainerMap<B>::find(const std::string& word) noexcept
{
  typename std::map<std::string, B*>::iterator it = nodes_.find(word);
  if(it == nodes_.end())
    return nullptr;
  else
    return it->second;
}

template <typename B>
std::vector<B*> BranchContainerMap<B>::find(bool (*comp)(B*, std::string, std::string), const std::string& word, const std::string& lang) noexcept
{
  std::vector<B*> res;

  for(auto& it : nodes_)
    if(comp(it.second, word, lang))
      res.push_back(it.second);
  return res;
}

template <typename B>
void BranchContainerMap<B>::load(std::vector<B*>& vect) noexcept
{
  for(size_t i = 0; i < vect.size(); i++)
    nodes_[vect[i]->value()] = vect[i];
}

template <typename B>
void BranchContainerMap<B>::insert(B* branch) noexcept
{
  nodes_[branch->value()] = branch;
}

template <typename B>
void BranchContainerMap<B>::erase(B* branch) noexcept
{
  nodes_.erase(branch->value());
}

#endif
