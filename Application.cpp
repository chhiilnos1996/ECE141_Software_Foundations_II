//
//  CommandProcessor.cpp
//
//  Created by rick gessner on 3/30/20
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "Application.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <algorithm>
#include <vector>
#include "Config.hpp"

#include "FolderView.hpp"
#include "FolderReader.hpp"

namespace ECE141 {
  size_t Config::cacheSize[]={0,0,0};
  
  Application::Application(std::ostream &anOutput) : CmdProcessor(anOutput), dbProcessor(this, anOutput) {}
  
  Application::~Application() {}
  
  // USE: -----------------------------------------------------
  
  bool isKnown(Keywords aKeyword) {
    static Keywords theKnown[]=
      {Keywords::quit_kw,Keywords::version_kw, Keywords::help_kw};
    auto theIt = std::find(std::begin(theKnown),
                           std::end(theKnown), aKeyword);
    return theIt!=std::end(theKnown);
  }

  CmdProcessor* Application::recognizes(Tokenizer &aTokenizer) {
    if(isKnown(aTokenizer.current().keyword)) {
      return this;
    }

    return dbProcessor.recognizes(aTokenizer);
  }

  StatusResult Application::run(Statement* aStatement) {
    switch(aStatement->getType()) {
      case Keywords::quit_kw:
        output << "DB::141 is shutting down\n";
        return StatusResult(ECE141::userTerminated);
        break;
      case Keywords::version_kw:
        output << "Version " << getVersion() << "\n"; break;
      case Keywords::help_kw:
        output << "Help system available\n"; break;
      default: break;
    }
    return StatusResult{Errors::noError};
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* Application::makeStatement(Tokenizer &aTokenizer, StatusResult &aResult) {
    Token theToken=aTokenizer.current();
    if (isKnown(theToken.keyword)) {
      aTokenizer.next(); //skip ahead...
      return new Statement(theToken.keyword);
    }

    aTokenizer.restart();
    return dbProcessor.makeStatement(aTokenizer, aResult);
  }

  bool Application::dbExists(const std::string &aDBName) {
    std::string thePath = Config::getDBPath(aDBName);
    return std::filesystem::exists(thePath);
  }

  //build a tokenizer, tokenize input, ask processors to handle...
  StatusResult Application::handleInput(std::istream &anInput){
    Tokenizer theTokenizer(anInput);
    StatusResult theResult=theTokenizer.tokenize();
    
    while (theResult && theTokenizer.more()) {
      Config::getTimer().reset();
      if(auto *theProc=recognizes(theTokenizer)) {
        if(auto *theCmd=theProc->makeStatement(theTokenizer,theResult)) {
          theResult=theProc->run(theCmd);
          if(theResult) theTokenizer.skipIf(';');
          delete theCmd;
        }
      } else if (!theTokenizer.next()) break;
      else theResult=unknownCommand;
    }

    return theResult;
  }

  StatusResult Application::createDatabase(const std::string &aName) {
    std::unique_ptr<Database> theDatabase = std::make_unique<Database>(aName, CreateDB{});
    database.swap(theDatabase);
    if(!dbProcessor.setDB(database.get())){
        return StatusResult{databaseCreationError};
    }
    return StatusResult{Errors::noError};
  }

  // USE: call DB object to be loaded into memory...
  StatusResult Application::useDatabase(const std::string &aName) {
    //std::cout << "USE DATABASE\n";
    if (!dbExists(aName)){
        //std::cout<<"no database "<<aName<<std::endl;
        return StatusResult{ECE141::unknownDatabase};
    }
    else{
        std::unique_ptr<Database> theDatabase = std::make_unique<Database>(aName, OpenDB{});
        database.swap(theDatabase);
        dbProcessor.setDB(database.get());
        return StatusResult{Errors::noError};
    }
  }
  
  StatusResult Application::showDatabases() const {
    //std::cout << "SHOW DATABASES\n";
    output << "+-----------------+" << std::endl;
    output << "| Database        |" << std::endl;
    output << "+-----------------+" << std::endl;
      
    std::string thePath=Config::getStoragePath();
    FolderReader theReader(thePath.c_str());
    size_t theCount{0};
    theReader.each(".db",[&](const std::string &aName){
      std::string theDBName = std::filesystem::path(aName).filename().replace_extension();
      theDBName.erase( remove( theDBName.begin(), theDBName.end(), '\"' ), theDBName.end());
      output<<"| "<<theDBName<<std::setw(17-theDBName.size())<<"|"<<std::endl;
      theCount++;
      return true;
    });
    output << "+-----------------+" << std::endl;
    output << theCount <<" rows in set (" ;
    return StatusResult{Errors::noError};
  }

  // USE: call this to perform the dropping of a database (remove the file)...
  StatusResult Application::dropDatabase(const std::string &aName) {
    //Database* theDatabase = database.get();
    if (database && database->getDatabaseName() == aName){
        database->closeDatabase();
    }
    std::string thePath = Config::getDBPath(aName);
    std::filesystem::remove(thePath);
    return StatusResult{Errors::noError};
  }

  // USE: DB dump all storage blocks
  StatusResult Application::dumpDatabase(const std::string &aName)  {
    Database theDatabase{aName, OpenDB{}};
    return theDatabase.dump(output);
  }

  Database* Application::getDatabaseInUse() {
    return database.get();
  }

}
