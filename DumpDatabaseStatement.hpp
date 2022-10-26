#include "keywords.hpp"
#include "DatabaseStatement.hpp"
#include <iostream>

#include "Tokenizer.hpp"

#include "CreateDatabaseView.hpp"
#include "Timer.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {
  
  class DumpDatabaseStatement : public DatabaseStatement {
    public:
    DumpDatabaseStatement(Application* anApplication, Keywords aStatementType=Keywords::drop_kw) : DatabaseStatement(anApplication, aStatementType) {}
    DumpDatabaseStatement(const DatabaseStatement &aCopy) : DatabaseStatement(aCopy) {}

    StatusResult run(std::ostream& aStream) override {
          return application->dumpDatabase(dbName);
    }

    const char* getStatementName() const { return "dump_database"; }
  };
  
}
