//
//  AppProcessor.hpp
//  Database1
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Database.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {

  class Application : public CmdProcessor {
  protected:
    DBProcessor dbProcessor;
    std::unique_ptr<Database> database;

  public:
    
    Application(std::ostream &anOutput);
    virtual ~Application();

      //app api...
            std::string   getVersion() {return "10";}
    
    virtual StatusResult  handleInput(std::istream &anInput);
            CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
            Statement*    makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) override;
            StatusResult  run(Statement *aStmt) override;

    StatusResult  createDatabase(const std::string &aName);
    StatusResult  dumpDatabase(const std::string &aName);
    StatusResult  dropDatabase(const std::string &aName);
    StatusResult  showDatabases() const;
    StatusResult  useDatabase(const std::string &aName);
    Database* getDatabaseInUse();

    bool          dbExists(const std::string &aDBName);
  };
  
}

#endif /* Application_hpp */
