//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <fstream>

namespace fs =  std::filesystem;

namespace ECE141 {
  
  using FileVisitor = std::function<bool(const std::string&)>;

  class FolderReader {
  public:
            FolderReader(const char *aPath) : path(aPath) {}
    virtual ~FolderReader() {}
    
    virtual bool exists(const std::string &aFilename) {
      return std::filesystem::exists(aFilename);
    }
    
    virtual void each(const std::string &anExt, const FileVisitor &aVisitor) const {      
      fs::path theFolder{path};
      for (auto const& theDirEntry : fs::directory_iterator{theFolder}) {
        if (anExt != fs::path(theDirEntry).extension()) continue;
        aVisitor(theDirEntry.path().string());
      }
    };
    
    std::string path;
  };
  
}

#endif /* FolderReader_h */
