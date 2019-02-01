#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <string>

class Namespace
{
public:
  explicit Namespace(const std::string& name);
  ~Namespace() {}

  bool isThisNamespace(const std::string& ns);
  std::string ns() {return name_; }

private:
  std::string name_;
};

#endif
