#ifndef FUNCTIONCONTAINER_H
#define FUNCTIONCONTAINER_H

#include <vector>

#include "ontoloGenius/interpreter/codeDescription/Functions/FunctionDescriptor.h"

class FunctionContainer
{
public:
  FunctionContainer() {}
  ~FunctionContainer() {}

  bool functionExist(const std::string& name);
  FunctionDescriptor* findFunction(const std::string& name);

protected:
  std::vector<FunctionDescriptor> functions_;
};

#endif
