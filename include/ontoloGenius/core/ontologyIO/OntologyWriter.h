#ifndef ONTOLOGYWRITER_H
#define ONTOLOGYWRITER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/ClassGraph.h"
#include "ontoloGenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Graphs/IndividualGraph.h"

#include <string>

class Ontology;

class OntologyWriter
{
public:
  OntologyWriter(ClassGraph* class_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph, IndividualGraph* individual_graph);
  explicit OntologyWriter(Ontology& onto);
  ~OntologyWriter() {}

  void setFileName(const std::string& name) {file_name_ = name; }
  void write(const std::string& file_name = "none");

private:
  ClassGraph* class_graph_;
  ObjectPropertyGraph* object_property_graph_;
  DataPropertyGraph* data_property_graph_;
  IndividualGraph* individual_graph_;

  std::string file_name_;
  FILE* file_;

  void writeStart();
  void writeEnd();
  void writeBanner(const std::string& name);
  void writeString(const std::string& text);
};

#endif
