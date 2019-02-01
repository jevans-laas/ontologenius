#include "ontoloGenius/interpreter/codeDescription/Namespace.h"

Namespace::Namespace(const std::string& name)
{
  name_ = name;
}

bool Namespace::isThisNamespace(const std::string& ns)
{
  if(ns == name_)
    return true;
  else
    return false;
}
