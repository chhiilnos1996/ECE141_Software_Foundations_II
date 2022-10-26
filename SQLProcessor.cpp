//
//  SQLProcessor.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include "SQLProcessor.hpp"
#include "SQLStatement.hpp"
#include "SelectStatement.hpp"

namespace ECE141 {

  SQLProcessor::~SQLProcessor() {}

  bool SQLProcessor::isKnown(Keywords aKeyword) {
    return (
      Keywords::create_kw == aKeyword
      || Keywords::describe_kw == aKeyword
      || Keywords::drop_kw == aKeyword
      || Keywords::show_kw == aKeyword
      || Keywords::insert_kw == aKeyword
      || Keywords::select_kw == aKeyword
      || Keywords::update_kw == aKeyword
      || Keywords::delete_kw == aKeyword
    );
  }

  CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
    if (isKnown(theKeyword)) return this;
    else return nullptr;
  }

  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer, StatusResult &aResult) {
    //std::cout << "SQLProcessor::makeStatement" << std::endl;
    std::map<Keywords, std::vector<StmtFactory>> statementFactories = {
      {Keywords::create_kw, { [&](){ return new CreateTableStatement(database); }}},
      {Keywords::describe_kw, { [&](){ return new DescribeTableStatement(database); }}},
      {Keywords::drop_kw, { [&](){ return new DropTableStatement(database); }}},
      {Keywords::show_kw, { [&](){ return new ShowTablesStatement(database); },
      [&](){ return new ShowIndexesStatement(database); },
      [&](){ return new ShowIndexStatement(database); }}},
      {Keywords::insert_kw, { [&](){ return new InsertTableStatement(database); }}},
      {Keywords::select_kw, { [&](){ return new SelectStatement(database); }}},
      {Keywords::update_kw, { [&](){ return new UpdateStatement(database); }}},
      {Keywords::delete_kw, { [&](){ return new DeleteStatement(database); }}},
    };

    Token theToken = aTokenizer.current();

    if (!statementFactories.count(theToken.keyword)) return nullptr;

    Statement* theStatement;
    StatusResult theStatus;

    for (auto theFactory : statementFactories[theToken.keyword]) {
      std::unique_ptr<Statement> theTempStatement = std::unique_ptr<Statement>(theFactory());
      theStatus = theTempStatement->parse(aTokenizer);
      if (theStatus) {
        theStatement = theTempStatement.release();
        break;
      }
    }

    aResult = theStatus;
    if (Errors::noError == theStatus.error) return theStatement;

    if (!theStatement) delete theStatement;
    return nullptr;
  }

  StatusResult  SQLProcessor::run(Statement *aStmt) {
    return aStmt->run(output);
  }

}
