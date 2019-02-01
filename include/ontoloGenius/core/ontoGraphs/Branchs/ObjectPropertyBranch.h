#ifndef OBJETCPROPERTYBRANCH_H
#define OBJETCPROPERTYBRANCH_H

#include <string>
#include <vector>

#include "ontoloGenius/core/ontoGraphs/Branchs/PropertyBranch.h"

//for branch type usage
class ClassBranch_t;

template <typename T>
class ObjectPropertyBranchData_t : public PropertyBranchData_t<T>
{
public:
  std::vector<T*> inverses_;
  std::vector<ClassBranch_t*> domains_;
  std::vector<ClassBranch_t*> ranges_;
  std::vector<std::vector<T*>> chains_;
  std::vector<std::vector<std::string>> str_chains_;
};

//for template usage
class ObjectPropertyBranch_t;
class ObjectPropertySteady_t :  public BranchSteady_t<ObjectPropertyBranch_t>,
                                public ObjectPropertyBranchData_t<ObjectPropertyBranch_t>
{
public:
  ObjectPropertySteady_t() {}
};

class ObjectPropertyBranch_t :  public Branch_t<ObjectPropertyBranch_t>,
                                public ObjectPropertyBranchData_t<ObjectPropertyBranch_t>
{
public:
  ObjectPropertySteady_t steady_;

  explicit ObjectPropertyBranch_t(std::string value = "") : Branch_t(value) {}

  void setFullSteady() noexcept;
  void setSteady_disjoint(ObjectPropertyBranch_t* disjoint) noexcept;
  void setSteady_properties(Properties_t properties) noexcept;
  void setSteady_inverse(ObjectPropertyBranch_t* inverse) noexcept;
  void setSteady_domain(ClassBranch_t* domain) noexcept;
  void setSteady_range(ClassBranch_t* range) noexcept;
  void set_chain(std::vector<ObjectPropertyBranch_t*> chain) noexcept;
  void setSteady_chain(std::vector<std::string> chain) noexcept;
  void setSteady_child(ObjectPropertyBranch_t* child) noexcept;
  void setSteady_mother(ObjectPropertyBranch_t* mother) noexcept;
  void setSteady_dictionary(std::string lang, std::string word) noexcept;
  void setSteady_dictionary(std::map<std::string, std::vector<std::string>> dictionary) noexcept;
};

#endif
