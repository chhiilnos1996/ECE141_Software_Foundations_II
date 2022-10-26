//
//  SQLProcessor.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include <functional>
#include "Database.hpp"

namespace ECE141 {

  class Statement;
  //class DBProcessor; //define this later...
  class Entity;
  //class Database;

  //Should this processor have it's own Statement types?
  using StmtFactory = std::function<Statement*()>;

  class SQLProcessor : public CmdProcessor {
  public:
    
    SQLProcessor(Database* aDatabase, std::ostream &anOutput) : database(aDatabase), CmdProcessor(anOutput) {}
    virtual ~SQLProcessor();
    
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;
    
    StatusResult  run(Statement *aStmt) override;
    bool  isKnown(Keywords aKeyword);
    
    //We don't DO the work here, but we can help route cmds...
    /*
    StatusResult  createTable(Entity *anEntity);
    StatusResult  describeTable(const std::string &aName);
    StatusResult  dropTable(const std::string &aName);
    StatusResult  showTables();*/

    StatusResult setDB(Database* aDatabase) {
      if (!aDatabase) return StatusResult{noDatabaseSpecified};
      database = aDatabase;

      return StatusResult{noError};
    }
    
      
  protected:
    Database* database;
  };

}
#endif /* SQLProcessor_hpp */
