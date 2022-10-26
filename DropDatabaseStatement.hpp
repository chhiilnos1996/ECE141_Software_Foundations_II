#include "keywords.hpp"
#include "DatabaseStatement.hpp"
#include <iostream>
#include "DropDatabaseView.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  class DropDatabaseStatement : public DatabaseStatement {
    public:
    DropDatabaseStatement(Application* anApplication, Keywords aStatementType=Keywords::drop_kw) : DatabaseStatement(anApplication, aStatementType) {}
    DropDatabaseStatement(const DatabaseStatement &aCopy) : DatabaseStatement(aCopy) {}

    StatusResult run(std::ostream& aStream)  override {
          Timer theTimer;
          StatusResult theResult = application->dropDatabase(dbName);
          double theTime = theTimer.elapsed();

          DropDatabaseView theView{dbName, theResult, theTime};
          theView.show(aStream);

          return theResult;
    }

    const char* getStatementName() const { return "drop_database"; }
  };
  
}
