//
//  UseDatabaseStatement.hpp
//  Assignment2
//
//  Created by chhiilnos on 4/10/22.
//

#ifndef UseDatabaseStatement_h
#define UseDatabaseStatement_h

#include "keywords.hpp"
#include "DatabaseStatement.hpp"
#include <iostream>
#include "Tokenizer.hpp"
#include "UseDatabaseView.hpp"

namespace ECE141 {
  
  class UseDatabaseStatement : public DatabaseStatement {
    public:
    UseDatabaseStatement(Application* anApplication, Keywords aStatementType=Keywords::use_kw) : DatabaseStatement(anApplication, aStatementType) {}
    UseDatabaseStatement(const DatabaseStatement &aCopy) : DatabaseStatement(aCopy) {}

    StatusResult run(std::ostream& aStream) override {
        Timer theTimer;
        StatusResult theResult = application->useDatabase(dbName);
        double theTime = theTimer.elapsed();
        //std::cout<<"dbName = "<<dbName<<std::endl;
        UseDatabaseView theView{dbName, theResult, theTime};
        theView.show(aStream);
        return theResult;
    }

    StatusResult parse(Tokenizer& aTokenizer) override {
        if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{Errors::identifierExpected};
        dbName = aTokenizer.current().data;
        //std::cout<<"use database"<<dbName<<std::endl;
        return StatusResult{Errors::noError};
    }
    const char* getStatementName() const { return "use_database"; }
  };
  
}

#endif /* UseDatabaseStatement_h */
