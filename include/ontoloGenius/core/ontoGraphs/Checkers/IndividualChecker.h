#ifndef INDIVIDUALCHECKER_H
#define INDIVIDUALCHECKER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/IndividualGraph.h"
#include "ontoloGenius/core/ontoGraphs/Checkers/ValidityChecker.h"

class IndividualChecker : public ValidityChecker<IndividualBranch_t>
{
public:
  explicit IndividualChecker(IndividualGraph* graph) : ValidityChecker(graph) {individual_graph_ = graph;}
  ~IndividualChecker() {}

  size_t check() noexcept;
  void printStatus() noexcept {ValidityChecker<IndividualBranch_t>::printStatus(std::string("individual"), std::string("individuals"), graph_vect_.size());}
private:
  IndividualGraph* individual_graph_;

  void checkSame() noexcept;
  void checkReflexive() noexcept;
  void checkObectPropertyDomain() noexcept;
  void checkObectPropertyRange() noexcept;

  void checkDataPropertyDomain() noexcept;
  void checkDataPropertyRange() noexcept;

  void checkAssymetric() noexcept;

  bool symetricExist(IndividualBranch_t* indiv_on, ObjectPropertyBranch_t* sym_prop, IndividualBranch_t* sym_indiv) noexcept;
};

#endif
