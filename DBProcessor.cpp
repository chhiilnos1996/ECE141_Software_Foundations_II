//
//  DBProcessor.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "DBProcessor.hpp"

#include <map>
#include <fstream>
#include <filesystem>
#include "Config.hpp"
#include "DBProcessor.hpp"
#include "Database.hpp"
#include "Helpers.hpp"

#include "CreateDatabaseStatement.hpp"
#include "UseDatabaseStatement.hpp"
#include "DropDatabaseStatement.hpp"
#include "ShowDatabasesStatement.hpp"
#include "DumpDatabaseStatement.hpp"

namespace ECE141 {

  DBProcessor::DBProcessor(Application* anApplication, std::ostream &anOutput) : application(anApplication), sqlProcessor(application->getDatabaseInUse(), anOutput), CmdProcessor(anOutput) {}
  
  DBProcessor::~DBProcessor() {
  }
 
  bool DBProcessor::isKnown(Keywords aKeyword) {
    return (Keywords::create_kw == aKeyword
    || Keywords::drop_kw == aKeyword
    || Keywords::use_kw == aKeyword
    || Keywords::show_kw == aKeyword
    || Keywords::dump_kw == aKeyword);
  }
  
  //CmdProcessor interface ...
  CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
    if (isKnown(theKeyword)) return this;

    return sqlProcessor.recognizes(aTokenizer);
  }

  StatusResult DBProcessor::run(Statement *aStmt) {
    StatusResult theResult = aStmt->run(output);
    //std::cout<<theResult.error<<std::endl;
    return theResult;
  }

  // USE: retrieve a statement based on given text input...
  Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer, StatusResult &aResult) {
    std::map<Keywords, StmtFactory> statementFactories = {
      {Keywords::create_kw, [&](){ return new CreateDatabaseStatement(application, Keywords::create_kw); }},
      {Keywords::use_kw, [&](){ return new UseDatabaseStatement(application, Keywords::use_kw); }},
      {Keywords::drop_kw, [&](){ return new DropDatabaseStatement(application, Keywords::drop_kw); }},
      {Keywords::show_kw, [&](){ return new ShowDatabasesStatement(application, Keywords::show_kw); }},
      {Keywords::dump_kw, [&](){ return new DumpDatabaseStatement(application, Keywords::dump_kw); }},
    };

    Token theToken = aTokenizer.current();

    if (!statementFactories.count(theToken.keyword)) return nullptr;

    Statement* theStatement = statementFactories[theToken.keyword]();

    StatusResult theStatus = theStatement->parse(aTokenizer);
    aResult = theStatus;
    if (Errors::noError == theStatus.error) return theStatement;

    delete theStatement;

    aTokenizer.restart();
    return sqlProcessor.makeStatement(aTokenizer, aResult);
  }
}
