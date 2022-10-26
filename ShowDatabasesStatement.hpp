#include "keywords.hpp"
#include "Statement.hpp"
#include <iostream>
#include "Tokenizer.hpp"
#include "ShowDatabasesView.hpp"

namespace ECE141 {
  
  class ShowDatabasesStatement : public DatabaseStatement {
    public:
    ShowDatabasesStatement(Application* anApplication, Keywords aStatementType=Keywords::show_kw) : DatabaseStatement(anApplication, aStatementType) {}
    ShowDatabasesStatement(const DatabaseStatement &aCopy) : DatabaseStatement(aCopy) {}

    StatusResult parse(Tokenizer& aTokenizer) override {
      if (!aTokenizer.next() || Keywords::databases_kw != aTokenizer.current().keyword) return StatusResult{Errors::unexpectedKeyword};

      return StatusResult{Errors::noError};
    }

    StatusResult run(std::ostream& aStream) override {
          Timer theTimer;
          StatusResult theResult = application->showDatabases();
          double theTime = theTimer.elapsed();
          aStream << theTime << " secs)\n";
          ShowDatabasesView theView{dbName, theResult, theTime};
          theView.show(aStream);

          return theResult;
    }

    const char* getStatementName() const { return "show_databases"; }
  };
  
}
