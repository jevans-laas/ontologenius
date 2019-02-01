#ifndef OBJECTROPERTYGRAPH_H
#define OBJECTROPERTYGRAPH_H

#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <stdint.h>

#include "ontoloGenius/core/ontoGraphs/Graphs/OntoGraph.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/ObjectPropertyBranch.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/ClassBranch.h"

struct ObjectPropertyVectors_t
{
   std::vector<std::string> mothers_;
   std::vector<std::string> disjoints_;
   std::vector<std::string> inverses_;
   std::vector<std::string> domains_;
   std::vector<std::string> ranges_;
   std::vector<std::vector<std::string>> chains_;
   Properties_t properties_;
   std::map<std::string, std::vector<std::string>> dictionary_;
};

//for friend
class ObjectPropertyDrawer;
class IndividualGraph;

//for graphs usage
class ClassGraph;

class ObjectPropertyGraph : public OntoGraph<ObjectPropertyBranch_t>
{
  friend ObjectPropertyDrawer;
  friend IndividualGraph;
  friend ClassGraph;
public:
  explicit ObjectPropertyGraph(ClassGraph* class_graph) {class_graph_ = class_graph; }
  ~ObjectPropertyGraph() {}

  void linkGraph(ClassGraph* class_graph) noexcept {class_graph_ = class_graph; }

  void add(const std::string& value, ObjectPropertyVectors_t& property_vectors) noexcept;
  void add(std::vector<std::string>& disjoints) noexcept;

  std::unordered_set<std::string> getDisjoint(const std::string& value) noexcept;
  std::unordered_set<std::string> getInverse(const std::string& value) noexcept;
  std::unordered_set<std::string> getDomain(const std::string& value) noexcept;
  void getDomainPtr(ObjectPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth = -1) noexcept;
  std::unordered_set<std::string> getRange(const std::string& value) noexcept;
  void getRangePtr(ObjectPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth = -1) noexcept;
  std::unordered_set<std::string> select(std::unordered_set<std::string>& on, const std::string& selector) noexcept;

  void getDisjoint(ObjectPropertyBranch_t* branch, std::unordered_set<ObjectPropertyBranch_t*>& res) noexcept;

  bool add(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;
  bool addInvert(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;
  bool remove(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;

private:
  ClassGraph* class_graph_;

  void isMyDisjoint(ObjectPropertyBranch_t* me, std::string& disjoint, std::map<std::string, ObjectPropertyBranch_t*>& vect, bool& find, bool all = true) noexcept
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

  void isMyInverse(ObjectPropertyBranch_t* me, std::string& inverse, std::map<std::string, ObjectPropertyBranch_t*>& vect, bool& find, bool all = true) noexcept
  {
    if(find)
      return;

    auto it = vect.find(inverse);
    if(it != vect.end())
    {
      me->setSteady_inverse(it->second);
      if(all)
        it->second->inverses_.push_back(me);
      find = true;
    }
  }

  void isMyDomain(ObjectPropertyBranch_t* me, std::string& domain, std::map<std::string, ClassBranch_t*>& vect, bool& find) noexcept
  {
    if(find)
      return;

    auto it = vect.find(domain);
    if(it != vect.end())
    {
      me->setSteady_domain(it->second);
      find = true;
    }
  }

  void isMyRange(ObjectPropertyBranch_t* me, std::string& range, std::map<std::string, ClassBranch_t*>& vect, bool& find) noexcept
  {
    if(find)
      return;

    auto it = vect.find(range);
    if(it != vect.end())
    {
      me->setSteady_range(it->second);
      find = true;
    }
  }

  void getNextChainLink(ObjectPropertyBranch_t** next, std::string& next_link, std::map<std::string, ObjectPropertyBranch_t*>& vect) noexcept
  {
    if(*next == nullptr)
    {
      auto it = vect.find(next_link);
      if(it != vect.end())
        *next = it->second;
    }
  }
};

#endif /* OBJECTROPERTYGRAPH_H */
