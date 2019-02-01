#ifndef CLASSBRANCH_H
#define CLASSBRANCH_H

#include <string>
#include <vector>

#include "ontoloGenius/core/ontoGraphs/Branchs/Branch.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/Data.h"

//for branch type usage
class ObjectPropertyBranch_t;
class DataPropertyBranch_t;
class IndividualBranch_t;

template <typename T>
class ClassBranchData_t
{
public:
  std::vector<IndividualBranch_t*> individual_childs_;

  std::vector<T*> disjoints_;

  std::vector<ObjectPropertyBranch_t*> object_properties_name_;
  std::vector<T*> object_properties_on_;
  std::vector<bool> object_properties_deduced_;

  std::vector<DataPropertyBranch_t*> data_properties_name_;
  std::vector<data_t> data_properties_data_;
  std::vector<bool> data_properties_deduced_;
};

//for template usage
class ClassBranch_t;
class ClassSteady_t : public BranchSteady_t<ClassBranch_t>, public ClassBranchData_t<ClassBranch_t>
{
public:
  ClassSteady_t() {}
};

class ClassBranch_t : public Branch_t<ClassBranch_t>, public ClassBranchData_t<ClassBranch_t>
{
public:
  ClassSteady_t steady_;

  explicit ClassBranch_t(std::string value = "") : Branch_t(value) {}

  void setFullSteady() noexcept;
  void setSteady_disjoint(ClassBranch_t* disjoint) noexcept;
  void setSteady_child(ClassBranch_t* child) noexcept;
  void setSteady_individual_child(IndividualBranch_t* child) noexcept;
  void setSteady_mother(ClassBranch_t* mother) noexcept;
  void setSteady_dictionary(std::string lang, std::string word) noexcept;
  void setSteady_dictionary(std::map<std::string, std::vector<std::string>> dictionary) noexcept;
  void setSteady_object_properties_name(ObjectPropertyBranch_t* object_properties_name) noexcept;
  void setSteady_object_properties_on(ClassBranch_t* object_properties_on) noexcept;
  void setSteady_data_properties_name(DataPropertyBranch_t* data_properties_name) noexcept;
  void setSteady_data_properties_data(data_t data_properties_data) noexcept;
};

#endif
