#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>
#include <map>
#include <unordered_set>

#include <stdint.h>

#include "ontoloGenius/interpreter/codeDescription/Namespace.h"
#include "ontoloGenius/interpreter/codeDescription/Functions/FunctionContainer.h"

struct Variable_t
{
  std::string name;
  std::unordered_set<std::string> values;
};

class VariablesType : public Namespace, public FunctionContainer
{
public:
  VariablesType();
  ~VariablesType() {}

  std::string add(const std::string& name);

  std::string name(const std::string& id);
  std::unordered_set<std::string> get(const std::string& id);
  std::string toString(const std::string& id);

  bool set(const std::string& id, const std::string& value);
  bool set(const std::string& id, std::unordered_set<std::string>& value);

  bool insert(const std::string& id, const std::string& value);
  bool insert(const std::string& id, std::unordered_set<std::string>& value);

  bool remove(const std::string& id);
  bool remove(const std::string& id, const std::string& value);
  bool remove(const std::string& id, std::unordered_set<std::string>& value);

  size_t size(const std::string& id);

private:
  uint16_t nb_variables_;

  std::map <std::string, Variable_t> var_;
};

#endif
