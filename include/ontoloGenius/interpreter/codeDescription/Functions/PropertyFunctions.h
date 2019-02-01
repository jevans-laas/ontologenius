#ifndef PROPERTYFUNCTIONS_H
#define PROPERTYFUNCTIONS_H

#include <string>

#include "ontoloGenius/interpreter/codeDescription/Functions/FunctionContainer.h"

class PropertyFunctions: public FunctionContainer
{
public:
  PropertyFunctions() {}
  ~PropertyFunctions() {}

  bool functionExist(const std::string& name);
  FunctionDescriptor* findFunction(const std::string& name);

private:
};

#endif
