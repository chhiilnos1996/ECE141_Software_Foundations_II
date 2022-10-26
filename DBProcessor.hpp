//
//  DBProcessor.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"

#include <memory>
#include <functional>
#include "SQLProcessor.hpp"

namespace ECE141 {
  
  //class DBStatement;
  using StmtFactory = std::function<Statement*()>;

  class Application;

  class DBProcessor : public CmdProcessor {
  public:
    
    DBProcessor(Application* anApplication, std::ostream &anOutput);
    ~DBProcessor();
    
      //cmd processor interface...
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;
    StatusResult  run(Statement *aStmt) override;
    bool          isKnown(Keywords aKeyword);

    StatusResult setDB(Database* aDatabase) { return sqlProcessor.setDB(aDatabase); }

  protected:
    Application* application;
    SQLProcessor sqlProcessor;
  };

}
#endif /* DBProcessor_hpp */
