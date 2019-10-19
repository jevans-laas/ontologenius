#include "ontoloGenius/core/ontologyIO/OntologyWriter.h"

#include <iostream>

#include "ontoloGenius/core/ontologyIO/writers/ClassWriter.h"
#include "ontoloGenius/core/ontologyIO/writers/ObjectPropertiesWriter.h"
#include "ontoloGenius/core/ontologyIO/writers/DataPropertiesWriter.h"
#include "ontoloGenius/core/ontologyIO/writers/IndividualWriter.h"

#include "ontoloGenius/core/ontoGraphs/Ontology.h"
#include "ontoloGenius/core/utility/color.h"

namespace ontologenius {

OntologyWriter::OntologyWriter(ClassGraph* class_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph, IndividualGraph* individual_graph)
{
  class_graph_ = class_graph;
  object_property_graph_ = object_property_graph;
  data_property_graph_ = data_property_graph;
  individual_graph_ = individual_graph;
}

OntologyWriter::OntologyWriter(Ontology& onto)
{
  class_graph_ = &onto.class_graph_;
  object_property_graph_ = &onto.object_property_graph_;
  individual_graph_ = &onto.individual_graph_;
  data_property_graph_ = &onto.data_property_graph_;
}

void OntologyWriter::write(std::string file_name)
{
  if(file_name != "none")
    file_name_ = file_name;

  if(file_name_ == "none")
    return;

  file_ = fopen(file_name_.c_str(), "w");
  if(file_ == NULL)
  {
    std::cout << "Fail to open file " << file_name_ << std::endl;
    return;
  }

  writeStart();

  writeBanner("Classes");
  ClassWriter classes(class_graph_);
  classes.write(file_);

  writeBanner("Object Properties");
  ObjectPropertiesWriter object_properties(object_property_graph_);
  object_properties.write(file_);

  writeBanner("Data properties");
  DataPropertiesWriter data_properties(data_property_graph_);
  data_properties.write(file_);

  writeBanner("Individuals");
  IndividualWriter individuals(individual_graph_);
  individuals.write(file_);

  writeBanner("General axioms");
  classes.writeGeneralAxioms(file_);
  individuals.writeGeneralAxioms(file_);

  writeEnd();

  std::cout << COLOR_GREEN << "ontology loaded in : " << COLOR_OFF << file_name_ << std::endl;

  if(file_ != NULL)
    fclose(file_);
}

void OntologyWriter::writeStart()
{
  std::string tmp = "<?xml version=\"1.0\"?> \n\
<rdf:RDF xmlns=\"ontologenius#\" \n\
     xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" \n\
     xmlns:owl=\"http://www.w3.org/2002/07/owl#\" \n\
     xmlns:xml=\"http://www.w3.org/XML/1998/namespace\" \n\
     xmlns:xsd=\"http://www.w3.org/2001/XMLSchema#\" \n\
     xmlns:rdfs=\"http://www.w3.org/2000/01/rdf-schema#\"> \n\
    <owl:Ontology rdf:about=\"ontologenius\"/>\n\n\n\n";
  writeString(tmp);
}

void OntologyWriter::writeEnd()
{
  std::string tmp = "</rdf:RDF> \n\n\n\n\
<!-- Generated by the ontologenius https://sarthou.github.io/ontologenius/ -->\n";
  writeString(tmp);
}

void OntologyWriter::writeBanner(std::string name)
{
  std::string tmp = "    <!--\n\
    ///////////////////////////////////////////////////////////////////////////////////////\n\
    //\n\
    // " + name + "\n\
    //\n\
    ///////////////////////////////////////////////////////////////////////////////////////\n\
     -->\n\n\n\n\n";
  writeString(tmp);
}

void OntologyWriter::writeString(std::string text)
{
  if(file_ != NULL)
    fwrite(text.c_str(), sizeof(char), text.size(), file_);
}

} // namespace ontologenius
