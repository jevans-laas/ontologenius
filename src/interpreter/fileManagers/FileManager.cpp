#include "ontoloGenius/interpreter/fileManagers/FileManager.h"

#include <ros/package.h>
#include <iostream>

FileManager::FileManager()
{
  file_ = NULL;
}

FileManager::~FileManager()
{
  if(file_ != NULL)
    fclose(file_);
}

void FileManager::init(const std::string& file_name, const std::string& option)
{
  std::string path = ros::package::getPath("ontologenius");
  path+= "/build/" + file_name + ".ont";

  file_ = fopen(path.c_str(), option.c_str());
  if(file_ == NULL)
    std::cout << "Fail to open file " << file_name << " with option '" << option << "'" << std::endl;
}

void FileManager::reset(const std::string& file_name)
{
  std::string path = ros::package::getPath("ontologenius");
  path+= "/build/" + file_name + ".ont";

  file_ = fopen(path.c_str(), "w");
  if(file_ == NULL)
    std::cout << "Fail to reset file " << file_name << std::endl;
  else
    fclose(file_);
  file_ = NULL;
}
