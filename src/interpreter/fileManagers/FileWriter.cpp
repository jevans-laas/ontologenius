#include "ontoloGenius/interpreter/fileManagers/FileWriter.h"

void FileWriter::write(const std::string& text)
{
  if(file_ != NULL)
    fwrite(text.c_str(), sizeof(char), text.size(), file_);
}
