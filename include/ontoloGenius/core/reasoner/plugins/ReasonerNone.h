#ifndef REASONERNONE_H
#define REASONERNONE_H

#include "ontoloGenius/core/reasoner/plugins/ReasonerInterface.h"

class ReasonerNone : public ReasonerInterface
{
public:
  ReasonerNone() {}
  ~ReasonerNone() {}

  virtual void preReason();
  virtual void postReason();
  virtual void periodicReason();

  virtual std::string getName();
  virtual std::string getDesciption();
private:
};

#endif