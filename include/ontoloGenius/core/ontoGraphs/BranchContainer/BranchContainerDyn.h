#ifndef BRANCHCONTAINERDYN_H
#define BRANCHCONTAINERDYN_H

#include <map>
#include <iostream>
#include <math.h>

#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerBase.h"

template <typename T>
struct BranchNode_t
{
  BranchNode_t<T>* next;
  std::string id;
  T* branch;

  BranchNode_t() {next = nullptr; id = ""; branch = nullptr; }
  ~BranchNode_t()
  {
    if(next != nullptr)
    {
      delete next;
      next = nullptr;
    }
  }
};

template <typename B>
class BranchContainerDyn : public BranchContainerBase<B>
{
public:
  BranchContainerDyn()
  {
    nodes_ = new BranchNode_t<B>;
    nodes_end_ = nodes_;
    nb_elem_ = 0;
  }

  virtual ~BranchContainerDyn()
  {
    delete nodes_;
  }

  virtual B* find(const std::string& word);
  virtual std::vector<B*> find(bool (*comp)(B*, std::string, std::string), const std::string& word, const std::string& lang);
  virtual void load(std::vector<B*>& vect);
private:
  BranchNode_t<B>* nodes_;
  BranchNode_t<B>* nodes_end_;
  size_t buffer_size_;
  size_t nb_elem_;

  void insertEnd(std::string id, B* branch);
  void reconf(BranchNode_t<B>* node);
};

template <typename B>
B* BranchContainerDyn<B>::find(const std::string& word)
{
  B* tmp = nullptr;
  size_t i = 0;
  for(BranchNode_t<B>* node = nodes_; node->next != nullptr; node = node->next)
  {
    if(node->next->id == word)
    {
      tmp = node->next->branch;
      if(i > 5)
        reconf(node);
      break;
    }
    i++;
  }

  return tmp;
}

template <typename B>
std::vector<B*> BranchContainerDyn<B>::find(bool (*comp)(B*, std::string, std::string), const std::string& word, const std::string& lang)
{
  std::vector<B*> res;

  for(BranchNode_t<B>* node = nodes_; node->next != nullptr; node = node->next)
    if(comp( node->next->branch, word, lang))
      res.push_back(node->next->branch);

  return res;
}

template <typename B>
void BranchContainerDyn<B>::load(std::vector<B*>& vect)
{
  for(size_t i = 0; i < vect.size(); i++)
    insertEnd(vect[i]->value(), vect[i]);

  nb_elem_ += vect.size();
  buffer_size_ = log2(nb_elem_);
}

template <typename B>
void BranchContainerDyn<B>::insertEnd(std::string id, B* branch)
{
  BranchNode_t<B>* tmp = new BranchNode_t<B>;
  tmp->id = id;
  tmp->branch = branch;
  nodes_end_->next = tmp;
  nodes_end_ = tmp;
}

template <typename B>
void BranchContainerDyn<B>::reconf(BranchNode_t<B>* node)
{
 BranchNode_t<B>* tmp = node->next;
 node->next = tmp->next;
 tmp->next = nodes_->next;
 nodes_->next = tmp;
}

#endif
