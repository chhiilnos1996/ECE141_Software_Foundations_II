#ifndef DatabaseStatement_hpp
#define DatabaseStatement_hpp

#include "keywords.hpp"
#include "Statement.hpp"
#include <iostream>

#include "Tokenizer.hpp"

#include "Application.hpp"

namespace ECE141 {
  
  class DatabaseStatement : public Statement {
    protected:
    std::string dbName;

    Application* application;

    public:
    DatabaseStatement(Application* anApplication, Keywords aStatementType=Keywords::unknown_kw) : application(anApplication), Statement(aStatementType) {}
    DatabaseStatement(const DatabaseStatement &aCopy) : application(aCopy.application), Statement(aCopy) {}
    virtual StatusResult run(std::ostream &aStream) override {
        return StatusResult{Errors::noError};
    }

    virtual StatusResult parse(Tokenizer& aTokenizer) override {
      if (!aTokenizer.next() || Keywords::database_kw != aTokenizer.current().keyword) return StatusResult{Errors::unexpectedKeyword};

      if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{Errors::identifierExpected};

      dbName = aTokenizer.current().data;

      return StatusResult{Errors::noError};
    }

    std::string getDBName() { return dbName; }
  };
}

#endif
