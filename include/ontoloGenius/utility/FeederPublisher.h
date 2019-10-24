#ifndef ONTOLOGENIUS_FEEDERPUBLISHER_H
#define ONTOLOGENIUS_FEEDERPUBLISHER_H

#include <ros/ros.h>
#include "std_msgs/String.h"

class FeederPublisher
{
public:
  FeederPublisher(ros::NodeHandle* n, const std::string& name) :
                  pub_(n->advertise<std_msgs::String>((name == "") ? "ontologenius/insert" : "ontologenius/insert/" + name, 1000))
  {
    n_ = n;
  }

  void addProperty(const std::string& from, const std::string& property, const std::string& on);
  void addProperty(const std::string& from, const std::string& property, const std::string& type, const std::string& value);
  void addInheritage(const std::string& from, const std::string& on);
  void addLanguage(const std::string& from, const std::string& lang, const std::string& name);
  void addConcept(const std::string& from);

  void removeProperty(const std::string& from, const std::string& property, const std::string& on);
  void removeProperty(const std::string& from, const std::string& property, const std::string& type, const std::string& value);
  void removeInheritage(const std::string& from, const std::string& on);
  void removeLanguage(const std::string& from, const std::string& lang, const std::string& name);
  void removeConcept(const std::string& from);

  size_t getNumSubscribers() { return pub_.getNumSubscribers(); }

private:
  ros::NodeHandle* n_;
  ros::Publisher pub_;

  void publish(std::string& str);
};

#endif // ONTOLOGENIUS_FEEDERPUBLISHER_H
