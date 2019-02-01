#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

class FileManager
{
public:
  FileManager();
  ~FileManager();

  void init(const std::string& file_name, const std::string& option);
  void reset(const std::string& file_name);
protected:
  FILE* file_;
};

#endif
