#include "ontoloGenius/arguer/plugins/ArguerChain.h"
#include <pluginlib/class_list_macros.h>

#include <iostream>

void ArguerChain::preReason()
{

}

void ArguerChain::postReason()
{
  std::vector<IndividualBranch_t*> indiv = ontology_->individuals_.get();
  for(size_t indiv_i = 0; indiv_i < indiv.size(); indiv_i++)
    for(size_t prop_i = 0; prop_i < indiv[indiv_i]->properties_name_.size(); prop_i++)
      for(size_t chain_i = 0; chain_i < indiv[indiv_i]->properties_name_[prop_i]->chains_.size(); chain_i++)
        resolveChain(indiv[indiv_i]->properties_name_[prop_i]->chains_[chain_i], indiv[indiv_i]->properties_on_[prop_i], indiv[indiv_i]);
}

void ArguerChain::resolveChain(std::vector<PropertyClassBranch_t*> chain, IndividualBranch_t* indiv, IndividualBranch_t* on)
{
  bool complete_chaine = true;
  std::vector<IndividualBranch_t*> indivs;
  indivs.push_back(indiv);
  for(size_t link_i = 0; link_i < chain.size() - 1; link_i++)
  {
    resolveLink(chain[link_i], indivs);
  }

  if((chain.size() != 0) && (indivs.size() != 0))
    for(size_t i = 0; i < indivs.size(); i++)
    {
      on->properties_name_.push_back(chain[chain.size() - 1]);
      on->properties_on_.push_back(indivs[i]);
    }
}

void ArguerChain::resolveLink(PropertyClassBranch_t* chain_property, std::vector<IndividualBranch_t*>& indivs)
{
  std::vector<IndividualBranch_t*> tmp;

  for(size_t indiv_i = 0; indiv_i < indivs.size(); indiv_i++)
    for(size_t prop_i = 0; prop_i < indivs[indiv_i]->properties_name_.size(); prop_i++)
      if(indivs[indiv_i]->properties_name_[prop_i]->value_ == chain_property->value_)
        tmp.push_back(indivs[indiv_i]->properties_on_[prop_i]);

  indivs = tmp;
}

std::string ArguerChain::getName()
{
  return "arguer chain";
}

std::string ArguerChain::getDesciption()
{
  return "This arguer resolve the properties chains axioms.";
}

PLUGINLIB_EXPORT_CLASS(ArguerChain, ArguerInterface)