#include "ontoloGenius/interpreter/codeDescription/Functions/FunctionContainer.h"

bool FunctionContainer::functionExist(const std::string& name)
{
  for(size_t i = 0; i < functions_.size(); i++)
  {
    if(functions_[i].getName() == name)
      return true;
  }
  return false;
}

FunctionDescriptor* FunctionContainer::findFunction(const std::string& name)
{
  for(size_t i = 0; i < functions_.size(); i++)
  {
    if(functions_[i].getName() == name)
      return &functions_[i];
  }
  return nullptr;
}
