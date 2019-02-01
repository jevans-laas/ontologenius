#ifndef FEEDER_H
#define FEEDER_H

#include "ontoloGenius/core/feeder/FeedStorage.h"

class Ontology;

class Feeder
{
public:
  explicit Feeder(Ontology* onto) {onto_ = onto; }

  void store(std::string feed) noexcept { feed_storage_.add(feed); }
  bool run() noexcept;
  void link(Ontology* onto) noexcept {onto_ = onto; }

  std::vector<std::string> getNotifications() noexcept
  {
    std::vector<std::string> tmp = notifications_;
    notifications_.clear();
    return tmp;
  }

private:
  FeedStorage feed_storage_;
  Ontology* onto_;

  std::vector<std::string> notifications_;
  std::string current_str_feed_;

  void addDelClass(action_t& action, std::string& name) noexcept;
  void addDelIndiv(action_t& action, std::string& name) noexcept;

  void addInheritage(feed_t& feed) noexcept;
  void modifyDataPropertyInheritance(feed_t& feed) noexcept;
  void modifyDataPropertyInheritanceInvert(feed_t& feed) noexcept;
  void modifyObjectPropertyInheritance(feed_t& feed) noexcept;
  void modifyObjectPropertyInheritanceInvert(feed_t& feed) noexcept;
  void classIndividualIsA(feed_t& feed) noexcept;

  void classIndividualLangage(feed_t& feed) noexcept;
  void applyProperty(feed_t& feed) noexcept;
};

#endif
