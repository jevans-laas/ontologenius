#include "ontoloGenius/core/reasoner/plugins/ReasonerInverseOf.h"
#include <pluginlib/class_list_macros.h>

namespace ontologenius {

void ReasonerInverseOf::preReason()
{

}

void ReasonerInverseOf::postReason()
{
  std::lock_guard<std::shared_timed_mutex> lock(ontology_->individual_graph_.mutex_);
  size_t indiv_i, prop_i, inv_i = 0;
  std::vector<IndividualBranch_t*> indiv = ontology_->individual_graph_.get();
  size_t indiv_size = indiv.size();
  for(indiv_i = 0; indiv_i < indiv_size; indiv_i++)
    if(indiv[indiv_i]->updated_ == true)
    {
      for(prop_i = 0; prop_i < indiv[indiv_i]->object_properties_name_.size(); prop_i++)
      {
        for(inv_i = 0; inv_i < indiv[indiv_i]->object_properties_name_[prop_i]->inverses_.size(); inv_i++)
        {
          IndividualBranch_t* sub_indiv = indiv[indiv_i]->object_properties_on_[prop_i];
          insetInverse(sub_indiv,
                      indiv[indiv_i]->object_properties_name_[prop_i]->inverses_[inv_i],
                      indiv[indiv_i]);
        }
      }
    }
}

void ReasonerInverseOf::insetInverse(IndividualBranch_t* indiv_on, ObjectPropertyBranch_t* inv_prop, IndividualBranch_t* inv_indiv)
{
  size_t properties_size = indiv_on->object_properties_name_.size();
  for(size_t i = 0; i < properties_size; i++)
  {
    if(indiv_on->object_properties_name_[i] == inv_prop)
      if(indiv_on->object_properties_on_[i] == inv_indiv)
        return;
  }

  indiv_on->object_properties_name_.push_back(inv_prop);
  indiv_on->object_properties_on_.push_back(inv_indiv);
  indiv_on->object_properties_deduced_.push_back(false);
  indiv_on->object_properties_has_induced_.push_back(Triplet());
  indiv_on->nb_updates_++;
  nb_update_++;
}

std::string ReasonerInverseOf::getName()
{
  return "reasoner inverse of";
}

std::string ReasonerInverseOf::getDesciption()
{
  return "This reasoner creates the inverse properties for each individual.";
}

PLUGINLIB_EXPORT_CLASS(ReasonerInverseOf, ReasonerInterface)

} // namespace ontologenius
