#ifndef CreateDatabaseStatement_hpp
#define CreateDatabaseStatement_hpp

#include "keywords.hpp"
#include "DatabaseStatement.hpp"
#include <iostream>
#include "Tokenizer.hpp"
#include "CreateDatabaseView.hpp"

namespace ECE141 {
  
  class CreateDatabaseStatement : public DatabaseStatement {
    public:
    CreateDatabaseStatement(Application* anApplication, Keywords aStatementType=Keywords::create_kw) : DatabaseStatement(anApplication, aStatementType) {}
    CreateDatabaseStatement(const DatabaseStatement &aCopy) : DatabaseStatement(aCopy) {}

    StatusResult run(std::ostream& aStream) override {
          Timer theTimer;
          StatusResult theResult = application->createDatabase(dbName);
          double theTime = theTimer.elapsed();

          CreateDatabaseView theView{dbName, theResult, theTime};
          theView.show(aStream);

          return theResult;
    }

    const char* getStatementName() const { return "create_database"; }
  };
  
}
#endif
