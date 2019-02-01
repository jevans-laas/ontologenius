#ifndef INDIVIDUALGRAPH_H
#define INDIVIDUALGRAPH_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <stdint.h>

#include "ontoloGenius/core/ontoGraphs/Graphs/Graph.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/IndividualBranch.h"

struct IndividualVectors_t
{
   std::vector<std::string> is_a_;

   std::vector<std::string> object_properties_name_;
   std::vector<std::string> object_properties_on_;
   std::vector<bool> object_properties_deduced_;

   std::vector<std::string> data_properties_name_;
   std::vector<std::string> data_properties_type_;
   std::vector<std::string> data_properties_value_;
   std::vector<bool> data_properties_deduced_;

   std::vector<std::string> same_as_;
   std::map<std::string, std::vector<std::string>> dictionary_;
};

//for friend
class IndividualChecker;

//for graphs usage
class ClassGraph;
class ObjectPropertyGraph;
class DataPropertyGraph;

class IndividualGraph : public Graph<IndividualBranch_t>
{
  friend IndividualChecker;
public:
  IndividualGraph(ClassGraph* class_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph);
  ~IndividualGraph();

  void linkGraph(ClassGraph* class_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph) noexcept;

  void close() noexcept;
  std::vector<IndividualBranch_t*> get() noexcept {return individuals_; }

  std::vector<IndividualBranch_t*> getSafe() noexcept
  {
    std::shared_lock<std::shared_timed_mutex> lock(mutex_);

    return individuals_;
  }

  void add(const std::string& value, IndividualVectors_t& individual_vector) noexcept;
  void add(std::vector<std::string>& distinct_) noexcept;

  std::unordered_set<std::string> getSame(const std::string& individual) noexcept;          //C1
  std::unordered_set<std::string> getDistincts(const std::string& individual) noexcept;     //C2
  std::unordered_set<std::string> getRelationFrom(const std::string& individual, int depth = -1) noexcept;  //C3
  std::unordered_set<std::string> getRelatedFrom(const std::string& property) noexcept;     //C3
  std::unordered_set<std::string> getRelationOn(const std::string& individual, int depth = -1) noexcept;    //C4
  std::unordered_set<std::string> getRelatedOn(const std::string& property) noexcept;       //C3
  std::unordered_set<std::string> getRelationWith(const std::string& individual) noexcept;  //C3
  std::unordered_set<std::string> getRelatedWith(const std::string& individual) noexcept;   //C3
  std::unordered_set<std::string> getFrom(const std::string& param) noexcept;
  std::unordered_set<std::string> getFrom(const std::string& individual, const std::string& property) noexcept;
  std::unordered_set<std::string> getOn(const std::string& param) noexcept;
  std::unordered_set<std::string> getOn(const std::string& individual, const std::string& property) noexcept;
  std::unordered_set<std::string> getWith(const std::string& param, int depth = -1) noexcept;
  std::unordered_set<std::string> getWith(const std::string& first_individual, const std::string& second_individual, int depth = -1) noexcept;
  std::unordered_set<std::string> getUp(IndividualBranch_t* indiv, int depth = -1, unsigned int current_depth = 0) noexcept;
  std::unordered_set<std::string> getUp(const std::string& individual, int depth = -1) noexcept;            //C3
  std::unordered_set<std::string> select(std::unordered_set<std::string>& on, const std::string& class_selector) noexcept;
  std::string getName(const std::string& value) noexcept;
  std::vector<std::string> getNames(const std::string& value) noexcept;
  std::unordered_set<std::string> find(const std::string& value) noexcept;
  std::unordered_set<std::string> getType(const std::string& class_selector) noexcept;

  ClassBranch_t* upgradeToBranch(IndividualBranch_t* indiv) noexcept;
  void createIndividual(std::string& name) noexcept;
  void deleteIndividual(IndividualBranch_t* indiv) noexcept;
  void redirectDeleteIndividual(IndividualBranch_t* indiv, ClassBranch_t* _class) noexcept;
  void addLang(std::string& indiv, std::string& lang, std::string& name) noexcept;
  void addInheritage(std::string& indiv, std::string& class_inherited) noexcept;
  void addInheritageInvert(std::string& indiv, std::string& class_inherited) noexcept;
  void addInheritageInvertUpgrade(std::string& indiv, std::string& class_inherited) noexcept;
  bool addProperty(std::string& indiv_from, std::string& property, std::string& indiv_on) noexcept;
  bool addProperty(std::string& indiv_from, std::string& property, std::string& type, std::string& data) noexcept;
  bool addPropertyInvert(std::string& indiv_from, std::string& property, std::string& indiv_on) noexcept;
  void removeLang(std::string& indiv, std::string& lang, std::string& name) noexcept;
  void removeInheritage(std::string& indiv, std::string& class_inherited) noexcept;
  bool removeProperty(IndividualBranch_t* branch_from, ObjectPropertyBranch_t* property, IndividualBranch_t* branch_on) noexcept;
  bool removeProperty(std::string& indiv_from, std::string& property, std::string& indiv_on) noexcept;
  bool removeProperty(std::string& indiv_from, std::string& property, std::string& type, std::string& data) noexcept;
  void removePropertyInverse(IndividualBranch_t* indiv_from, ObjectPropertyBranch_t* property, IndividualBranch_t* indiv_on) noexcept;
  void removePropertySymetric(IndividualBranch_t* indiv_from, ObjectPropertyBranch_t* property, IndividualBranch_t* indiv_on) noexcept;
  void removePropertyChain(IndividualBranch_t* indiv_from, ObjectPropertyBranch_t* property, IndividualBranch_t* indiv_on) noexcept;
  std::vector<IndividualBranch_t*> resolveLink(std::vector<ObjectPropertyBranch_t*>& chain, IndividualBranch_t* indiv_on, size_t index) noexcept;

  void getUpPtr(IndividualBranch_t* indiv, std::unordered_set<ClassBranch_t*>& res, int depth = -1, unsigned int current_depth = 0) noexcept;

private:
  ClassGraph* class_graph_;
  ObjectPropertyGraph* object_property_graph_;
  DataPropertyGraph* data_property_graph_;

  std::vector<IndividualBranch_t*> individuals_;

  void addObjectPropertyName(IndividualBranch_t* me, std::string& name, bool deduced) noexcept;
  void addObjectPropertyOn(IndividualBranch_t* me, std::string& name, bool deduced) noexcept;
  void addDataPropertyName(IndividualBranch_t* me, std::string& name, bool deduced) noexcept;
  void addDataPropertyData(IndividualBranch_t* me, data_t& data, bool deduced) noexcept;
  void setObjectPropertiesUpdated(std::vector<IndividualBranch_t*> branchs) noexcept;
  void getRelationFrom(ClassBranch_t* class_branch, std::unordered_set<std::string>& res, int depth = -1) noexcept;
  bool getRelatedWith(ClassBranch_t* class_branch, const std::string& data, std::unordered_set<ClassBranch_t*>& next_step, std::unordered_set<uint32_t>& took) noexcept;
  bool getFrom(ClassBranch_t* class_branch, std::unordered_set<uint32_t>& object_properties, std::unordered_set<uint32_t>& data_properties, const std::string& data, std::unordered_set<uint32_t>& down_classes, std::unordered_set<ClassBranch_t*>& next_step, std::unordered_set<uint32_t>& doNotTake) noexcept;

  std::unordered_set<uint32_t> getSameId(const std::string& individual) noexcept;
  void getSame(IndividualBranch_t* individual, std::unordered_set<IndividualBranch_t*>& res) noexcept;
  std::unordered_set<std::string> getSameAndClean(IndividualBranch_t* individual) noexcept;
  std::unordered_set<uint32_t> getSameIdAndClean(IndividualBranch_t* individual) noexcept;
  void cleanMarks(std::unordered_set<IndividualBranch_t*>& indSet) noexcept;
  std::unordered_set<std::string> set2set(std::unordered_set<IndividualBranch_t*>& indSet, bool clean = true) noexcept;

  bool checkRangeAndDomain(IndividualBranch_t* from, ObjectPropertyBranch_t* prop, IndividualBranch_t* on) noexcept;
  bool checkRangeAndDomain(IndividualBranch_t* from, DataPropertyBranch_t* prop, data_t& data) noexcept;
};

#endif /* INDIVIDUALGRAPH_H */
