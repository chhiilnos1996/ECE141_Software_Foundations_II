//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <iterator>

#include "Database.hpp"
#include "Config.hpp"
#include "TabularView.hpp"
#include "EntityView.hpp"
#include "TableDescriptionView.hpp"
#include "CreateTableView.hpp"
#include "DropTableView.hpp"
#include "RowChangedView.hpp"
#include "DumpDatabaseView.hpp"
#include "InsertIntoTableView.hpp"
#include "ShowIndexesView.hpp"
#include "ShowIndexView.hpp"
#include "RowBlockIndex.hpp"
#include "Joins.hpp"

namespace ECE141 {
  
  Database::Database(const std::string aName, CreateDB)
    : dbName(aName), storage(aName) {
    //create for read/write
    Block theBlock(BlockType::meta_block);
    storage.begin(std::ios::out | std::ios::trunc);
    storage.writeBlock(0, theBlock);
    storage.end();
  }

  Database::Database(const std::string aName, OpenDB)
    : dbName(aName), storage(aName) {
     //open for read/write
  }

  Database::~Database() {
    //storage.save()
    closeDatabase();
  }

  StatusResult Database::createTable(Entity *anEntity, std::ostream& aStream) {
    //std::cout<<"createTable *********************************"<<std::endl;
    Config::getTimer().reset();

    //read block 0
    StatusResult theResult;
    theResult = storage.readStorable(0, *this);
    if(StatusResult{Errors::noError}!=theResult) return theResult;

    //add name and id to the entity map
    uint32_t theEntityBlockIndex = storage.getFreeBlock();
    theResult = storage.writeStorable(*anEntity, BlockType::entity_block, theEntityBlockIndex);
    uint32_t theRowBlockIndex = storage.getFreeBlock();
    anEntity->setRowBlockIndex(theRowBlockIndex);
    theResult = storage.writeStorable(*anEntity, BlockType::entity_block, theEntityBlockIndex);
    if(StatusResult{Errors::noError}!= theResult) return theResult;
    entityMap[anEntity->getName()] = theEntityBlockIndex;
      
    //write empty row block
    RowBlockIndex theIndex(anEntity->getName());
    theResult = storage.writeStorable(theIndex, BlockType::row_block, theRowBlockIndex);
    if(StatusResult{Errors::noError}!= theResult) return theResult;

    //write map to block
    theResult = storage.writeStorable(*this, BlockType::meta_block, 0);
    if(StatusResult{Errors::noError}!= theResult) return theResult;

    double theTime = Config::getTimer().elapsed();
    CreateTableView theView{theResult, theTime};
    theView.show(aStream);

    return theResult;
  }

  StatusResult Database::describeTable(const std::string &aName, std::ostream& aStream) {
    //std::cout<<"describeTable *********************************"<<std::endl;
    Config::getTimer().reset();

    StatusResult theResult;

    theResult = storage.readStorable(0, *this);
    if(StatusResult{Errors::noError}!=theResult) return theResult;
    
    // find aName in Entity map and find index
    if (entityMap.find(aName) == entityMap.end()){
        return StatusResult{ECE141::unknownTable};
    }
    uint32_t theIndex = entityMap[aName];
  
    // convert entity block to entity
    Entity theEntity;
    storage.readStorable(theIndex, theEntity);
    
    //call view
    double theDuration = Config::getTimer().elapsed();
    TableDescriptionView theView{aName, theEntity, theDuration};
    theView.show(aStream);

    return StatusResult{Errors::noError};
  }

  StatusResult Database::dropTable(const std::string &aName, std::ostream& aStream) {
      //std::cout<<"dropTable *********************************"<<std::endl;
      Config::getTimer().reset();

      StatusResult theResult;
      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      if (entityMap.find(aName) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      
      // read Entity
      uint32_t theIndex = entityMap[aName];
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);
      
      // drop entity and data blocks
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();

      // drop row blocks
      int theModified = theRowBlockIndices.size() + 1;
      for (auto theIdx : theRowBlockIndices) {
         storage.markBlockAsFree(theIdx);
      }
  
      // drop row block index block and entity block
      theResult = storage.markBlockAsFree(thePos);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      theResult = storage.markBlockAsFree(theIndex);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      
      // write block 0
      entityMap.erase(aName);
      theResult = storage.writeStorable(*this, BlockType::meta_block, 0);
      if(StatusResult{Errors::noError}!=theResult) return theResult;

      // Call  View
      double theTime = Config::getTimer().elapsed();
      RowChangedView theView{theResult, theModified, theTime};
      theView.show(aStream);

      return theResult;
   }

   StatusResult Database::showTables(std::ostream& aStream) {
    //std::cout<<"showTables *********************************"<<std::endl;
    Config::getTimer().reset();
    // read block 0
    StatusResult theResult;
    theResult = storage.readStorable(0, *this);
    if(StatusResult{Errors::noError}!=theResult) return theResult;
    
    std::vector<std::string> theEntities;

    for (auto theIt : entityMap){
      theEntities.push_back(theIt.first);
    }
    
    // call view
    double theTime = Config::getTimer().elapsed();
    EntityView theView{getDatabaseName(), theEntities, theTime};
    theView.show(aStream);

    return StatusResult{Errors::noError};
  }

  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::dump(std::ostream &anOutput) {
    //std::cout<<"dump *********************************"<<std::endl;
    Config::getTimer().reset();

    std::vector<Block> aBlockList;

    storage.each([&](const Block& aBlock, uint32_t aBlockNum) {
      aBlockList.push_back(aBlock);
      return true;
    });

    double theTime = Config::getTimer().elapsed();
    DumpDatabaseView theView{aBlockList, theTime};
    theView.show(anOutput);

    return StatusResult{noError};
  }
 
  StatusResult Database::insertRecords(const RowCollection& aRows, std::ostream& aStream) {
    //std::cout<<"insertRecords *********************************"<<std::endl;
    Config::getTimer().reset();
    StatusResult theResult;
    // check if row size is zero, if not get entity name
    if (aRows.size() == 0) return StatusResult{unknownError};
    std::string theTableName = aRows[0].get()->getEntityName();
      
    // read Entity Map : entity string -> block index
    theResult = storage.readStorable(0, *this);
    if(StatusResult{Errors::noError}!=theResult) return theResult;
    if (entityMap.find(theTableName) == entityMap.end()){
        return StatusResult{ECE141::unknownTable};
    }
    uint32_t theIndex = entityMap[theTableName];
    Entity theEntity;
    storage.readStorable(theIndex, theEntity);
    
    // read rowBlockIndex
    RowBlockIndex theRowBlockIndex;
    uint32_t thePos = theEntity.getRowBlockIndex();
    storage.readStorable(thePos, theRowBlockIndex);
      
    // do validation
    for (auto& row : aRows) {
        theResult = theEntity.validate(*row);
        if (theResult != StatusResult{noError}) return theResult;
    }
    std::string theAutoIncName = "";
    for (auto theAttr : theEntity.getAttributes()) {
      if (theAttr.isAutoIncrement()) theAutoIncName = theAttr.getName();
    }
      
    // store RowCollection to output, add to rowBlockIndex and write theRowBlockIndex 
    for (auto& row : aRows){
        uint32_t thePos = storage.getFreeBlock();
        theRowBlockIndex.addRowBlockIndices(thePos);
        if ("" != theAutoIncName) (*row.get()).set(theAutoIncName, theEntity.increment());
        storage.writeStorable(*row.get(), BlockType::data_block, thePos);
    }
    storage.writeStorable(theRowBlockIndex, BlockType::row_block, thePos);
      
    // call view
    double theTime = Config::getTimer().elapsed();
    InsertIntoTableView theView{aRows.size(), true, theTime};
    theView.show(aStream);

    return StatusResult{noError};
  }

    StatusResult Database::joinTables(std::string aName, JoinList& aJoinList, RowCollection& aRowCollection){
      //std::cout<<"joinTables *********************************"<<std::endl;
        // JOIN Books on Authors.id=Books.author_id;
      StatusResult theResult{Errors::noError};
        
      // find aName in Entity map and find index
      if (entityMap.find(aName) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
    
      /*
      uint32_t theIndex = entityMap[aName];
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);
      // read rowBlockIndex
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();

      RowCollection theTmpRows;
      for (auto theBlockIndex : theRowBlockIndices) {
            std::unique_ptr<Row> theRowPtr = std::make_unique<Row>();
            theResult = storage.readStorable(theBlockIndex, *theRowPtr.get());
            if(StatusResult{Errors::noError}!=theResult) return theResult;
            //theRowPtr.get()->print();
            theTmpRows.push_back(std::move(theRowPtr));
      }*/
      
      RowCollection theTmpRows;
      storage.each([&](const Block& aBlock, uint32_t aLocation) {
            if ('D' != aBlock.header.type) return true;
            std::stringstream theBlockStream(aBlock.payload);
            std::unique_ptr<Row> theRow = std::make_unique<Row>();
            StatusResult theResult = theRow.get()->decode(theBlockStream);
            if (!theResult) return false;
            if (theRow.get()->getEntityName() != aName ) return true;
            // make key to aName.attr and push back to Rowcollecion
            std::unique_ptr<Row> theTableRow = std::make_unique<Row>(aName, *theRow.get());
            theTmpRows.push_back(std::move(theTableRow));
            return true;
      });
          
      // multiple joins
      for(int i=0;i<aJoinList.size();i++){
          //std::cout<<"aJoinList["<<i<<"]"<<std::endl;
          // get table rows
          std::string theTableName = aJoinList[i].getTableName();
          if (entityMap.find(theTableName) == entityMap.end()){
              return StatusResult{ECE141::unknownTable};
          }
          /*
          uint32_t theIndex1 = entityMap[theTableName];
          Entity theEntity1;
          storage.readStorable(theIndex1, theEntity1);
          // read rowBlockIndex
          RowBlockIndex theRowBlockIndex1;
          uint32_t thePos1 = theEntity.getRowBlockIndex();
          storage.readStorable(thePos1, theRowBlockIndex1);
          std::vector<uint32_t> theRowBlockIndices1 = theRowBlockIndex1.getRowBlockIndices();

          RowCollection theRows;
          for (auto theBlockIndex : theRowBlockIndices1) {
                std::unique_ptr<Row> theRowPtr = std::make_unique<Row>();
                theResult = storage.readStorable(theBlockIndex, *theRowPtr.get());
                if(StatusResult{Errors::noError}!=theResult) return theResult;
                //theRowPtr.get()->print();
                theRows;.push_back(std::move(theRowPtr));
          }*/
          
          
          RowCollection theRows;
          storage.each([&](const Block& aBlock, uint32_t aLocation) {
                if ('D' != aBlock.header.type) return true;
                std::stringstream theBlockStream(aBlock.payload);
                std::unique_ptr<Row> theRow = std::make_unique<Row>();
                StatusResult theResult = theRow.get()->decode(theBlockStream);
                if (!theResult) return false;
                if (theRow.get()->getEntityName() != theTableName) return true;
                std::unique_ptr<Row> theTableRow = std::make_unique<Row>(theTableName, *theRow.get());
                theRows.push_back(std::move(theTableRow));
                return true;
          });
          
          //std::cout<<"joinTables 3"<<std::endl;
          // sort by LHS, RHS
          Operand theLHS = aJoinList[i].getLHS();
          Operand theRHS = aJoinList[i].getRHS();
          std::string theLHSStr = theLHS.getFullName();
          std::string theRHSStr = theRHS.getFullName();
          std::sort(theTmpRows.begin(), theTmpRows.end(),
              [&](const std::unique_ptr<Row>& aPtr1, const std::unique_ptr<Row>& aPtr2) -> bool {
              return aPtr1.get()->getValue(theLHSStr) < aPtr1.get()->getValue(theLHSStr);
          });
          std::sort(theRows.begin(), theRows.end(),
              [&](const std::unique_ptr<Row>& aPtr1, const std::unique_ptr<Row>& aPtr2) -> bool {
              return aPtr1.get()->getValue(theRHSStr) < aPtr1.get()->getValue(theRHSStr);
          });
          
          
          // join rows according to join types
          // LHS : temp rows, RHS : rows
        
          Keywords theJoinType = aJoinList[i].getJoinType();
          if(Keywords::right_kw == theJoinType){ // left join
              //std::cout<<"joinTables right *************************************************"<<std::endl;
              // merge rows
              RowCollection theNewRows;
              //std::vector<std::unique_ptr<Row>>::iterator thePrevIt=theTmpRows.begin();
              std::vector<std::unique_ptr<Row>>::iterator theIt;
              
              for(auto& theRow : theRows){
                  int count = 0;
                  //std::cout<<"joinTables 5"<<std::endl;
                  for(theIt=theTmpRows.begin(); theIt!=theTmpRows.end(); theIt++){
                      // make new row
                      if ((**theIt).getValue(theLHS.getFullName()) == theRow.get()->getValue(theRHS.getFullName())){
                          count++;
                          std::unique_ptr<Row> theNewRowPtr = std::make_unique<Row>(*theRow.get(), *(*theIt).get());
                          theNewRows.push_back(std::move(theNewRowPtr));
                      }
                  }
                  if(count == 0){
                      
                      bool theIsNull = 1;
                      theIt=theTmpRows.begin();
                      std::unique_ptr<Row> theNullRowPtr = std::make_unique<Row>(theIsNull, *(*theIt).get());
                      std::unique_ptr<Row> theNewRowPtr = std::make_unique<Row>(*theRow.get(), *theNullRowPtr.get());
                      theNewRows.push_back(std::move(theNewRowPtr));
                  }
              }
              
              theTmpRows.clear();
              theTmpRows.swap(theNewRows);
              theNewRows.clear();
              
          }
          else if(Keywords::left_kw == theJoinType){ // right join
              //std::cout<<"joinTables left *************************************************"<<std::endl;
              // merge rows
              RowCollection theNewRows;
              //std::vector<std::unique_ptr<Row>>::iterator thePrevIt=theTmpRows.begin();
              std::vector<std::unique_ptr<Row>>::iterator theIt;
      
              for(theIt=theTmpRows.begin(); theIt!=theTmpRows.end(); theIt++){
              
                  int count = 0;
                  for(auto& theRow : theRows){
                      // make new row
                      if (theRow.get()->getValue(theRHS.getFullName()) == (**theIt).getValue(theLHS.getFullName())){
                          count++;
                          std::unique_ptr<Row> theNewRowPtr = std::make_unique<Row>(*theRow.get(), *(*theIt).get());
                          theNewRows.push_back(std::move(theNewRowPtr));
                      }
                  }
                  if(count == 0){
                      bool theIsNull = 1;
                      std::unique_ptr<Row> theNullRowPtr = std::make_unique<Row>(theIsNull, *theRows[0].get());
                      std::unique_ptr<Row> theNewRowPtr = std::make_unique<Row>(*theNullRowPtr.get(), *(*theIt).get());
                      theNewRows.push_back(std::move(theNewRowPtr));
                  }
                  
              }
              
              theTmpRows.clear();
              theTmpRows.swap(theNewRows);
              theNewRows.clear();
          }
          else if(Keywords::inner_kw == theJoinType || Keywords::join_kw == theJoinType){ // inner join
              //std::cout<<"joinTables inner *************************************************"<<std::endl;
              RowCollection theNewRows;
              std::vector<std::unique_ptr<Row>>::iterator theIt;
              
              for(auto& theRow : theRows){
                  for(theIt=theTmpRows.begin(); theIt!=theTmpRows.end(); theIt++){
                      if ((**theIt).getValue(theLHS.getFullName()) == theRows[0].get()->getValue(theRHS.getFullName())){
                          // make new row
                          std::unique_ptr<Row> theNewRowPtr = std::make_unique<Row>(*theRow.get(), *(*theIt).get());
                          theNewRows.push_back(std::move(theNewRowPtr));
                      }
                  }
              }
              
              theTmpRows.clear();
              theTmpRows.swap(theNewRows);
              theNewRows.clear();
              
          }
          else{
              return StatusResult{Errors::unknownJoinType};
          }
      }
      
      aRowCollection.clear();
      aRowCollection.swap(theTmpRows);
      theTmpRows.clear();
      
      return StatusResult{Errors::noError};
    }

    StatusResult Database::selectJoins(const DBQuery& aQuery, RowCollection& aRowCollection, std::ostream& aStream) {
        //std::cout<<"selectJoins *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{noError};

      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;

      // find aName in Entity map and find index
      if (entityMap.find(aQuery.getTableName()) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      uint32_t theIndex = entityMap[aQuery.getTableName()];
      
      // convert entity block to entity
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);

      // get table rows
      RowCollection theRows;
      theRows.swap(aRowCollection);

      /*
      // WHERE ExpressionList
      theResult = aQuery.filterWhere(theEntity, theRows);
      if (!theResult) return theResult;*/
      
      // ORDER BY StringList
      StringList theOrderList = aQuery.getOrderList();
      for (std::string theAttrName : theOrderList){
          std::sort(theRows.begin(), theRows.end(),  [&](const std::unique_ptr<Row>& aRowUniqPtr1, const std::unique_ptr<Row>& aRowUniqPtr2) {
              // Value = std::variant<bool, int, double, std::string>
              return aRowUniqPtr1.get()->getValue(theAttrName) < aRowUniqPtr2.get()->getValue(theAttrName);
          });
      }
      
      // LIMIT StringList
      int theLimitStart = aQuery.getStartLimit();
      int theLimitEnd = aQuery.getEndLimit();
      if (theLimitStart >= 0 && theLimitEnd >= 0) {
          theRows.erase(theRows.begin(), theRows.begin() + theLimitStart);
          theRows.erase(theRows.begin() + theLimitEnd, theRows.end());
      } else if (theLimitEnd >= 0) theRows.erase(theRows.begin() + theLimitEnd, theRows.end());
      
      // Call Tabular View
      double theTime = Config::getTimer().elapsed();
      TabularView theView{theEntity, aQuery.getSelectList(), theRows, theTime};
      theView.show(aStream);
      return theResult;

    }


    StatusResult Database::selectRows(const DBQuery& aQuery, std::ostream& aStream) {
        //std::cout<<"selectRows *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{noError};

      // read block 0 : entity map
      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      // find aName in Entity map and find index
      if (entityMap.find(aQuery.getTableName()) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      uint32_t theIndex = entityMap[aQuery.getTableName()];
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);

      // read rowBlockIndex
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();
      RowCollection theRows;
      for (auto theBlockIndex : theRowBlockIndices) {
          std::unique_ptr<Row> theRowPtr = std::make_unique<Row>();
          theResult = storage.readStorable(theBlockIndex, *theRowPtr.get());
          if(StatusResult{Errors::noError}!=theResult) return theResult;
          //theRowPtr.get()->print();
          theRows.push_back(std::move(theRowPtr));
      }
      
      // WHERE ExpressionList
      theResult = aQuery.filterWhere(theEntity, theRows);
      if (!theResult) return theResult;
        
      // ORDER BY StringList
      StringList theOrderList = aQuery.getOrderList();
      for (std::string theAttrName : theOrderList){
          std::sort(theRows.begin(), theRows.end(),  [&](const std::unique_ptr<Row>& aRowUniqPtr1, const std::unique_ptr<Row>& aRowUniqPtr2) {
                // Value = std::variant<bool, int, double, std::string>
              return aRowUniqPtr1.get()->getValue(theAttrName) < aRowUniqPtr2.get()->getValue(theAttrName);
          });
      }
        
      // LIMIT StringList
      int theLimitStart = aQuery.getStartLimit();
      int theLimitEnd = aQuery.getEndLimit();

      // using RowCollection = std::vector<std::unique_ptr<Row>>;
      if (theLimitStart >= 0 && theLimitEnd >= 0) {
          theRows.erase(theRows.begin(), theRows.begin() + theLimitStart);
          theRows.erase(theRows.begin() + theLimitEnd, theRows.end());
      } else if (theLimitEnd >= 0) theRows.erase(theRows.begin() + theLimitEnd, theRows.end());

      // Call Tabular View
      double theTime = Config::getTimer().elapsed();
      TabularView theView{theEntity, aQuery.getSelectList(), theRows, theTime};
      theView.show(aStream);
      return theResult;

    }


  StatusResult Database::updateTable(const DBQuery& aQuery, std::ostream& aStream) {
      //std::cout<<"updateTable *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{noError};

      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      
      // find aName in Entity map and find index
      if (entityMap.find(aQuery.getTableName()) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      uint32_t theIndex = entityMap[aQuery.getTableName()];
        
      // convert entity block to entity
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);

      // read rowBlockIndex
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();

      int theModified = 0;

      for (auto theIdx : theRowBlockIndices) {
        std::unique_ptr<Row> theRow = std::make_unique<Row>();
        storage.readStorable(theIdx, *theRow);

        RowCollection theRows;
        theRows.push_back(std::move(theRow));

        if (!aQuery.filterWhere(theEntity, theRows) && aQuery.getWhereList().size()) continue;

        if (!theRows.size()) continue;

        auto& theFilterRow = theRows[0];

        for (auto theExp : aQuery.getSetList()) {
          (*theFilterRow).set(theExp.lhs.name, theExp.rhs.value);
        }

        storage.writeStorable(*theFilterRow, BlockType::data_block, theIdx);
        theModified++;
      }

      // Call  View
      double theTime = Config::getTimer().elapsed();
      RowChangedView theView{theResult, theModified, theTime};
      theView.show(aStream);
      return theResult;
  }

  StatusResult Database::deleteTable(const DBQuery& aQuery, std::ostream& aStream) {
      //std::cout<<"deleteTable *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{Errors::noError};
      if (aQuery.getWhereList().size() == 0){ return theResult;}

      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      
      // find aName in Entity map and find index
      if (entityMap.find(aQuery.getTableName()) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      uint32_t theIndex = entityMap[aQuery.getTableName()];
        
      // convert entity block to entity
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);

      // read rowBlockIndex
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();
        
      // store RowCollection to output, add to rowBlockIndex
      std::vector<uint32_t> theDeleted;
      for (auto theIdx : theRowBlockIndices) {
        std::unique_ptr<Row> theRowPtr = std::make_unique<Row>();
        theResult = storage.readStorable(theIdx, *theRowPtr.get());
        if(StatusResult{Errors::noError}!=theResult) return theResult;
       
        RowCollection theRows;
        theRows.push_back( std::move(theRowPtr) );
        theResult = aQuery.filterWhere(theEntity, theRows);
        if(StatusResult{Errors::noError}!=theResult) return theResult;
          
        if (theRows.size()!=0) {theDeleted.push_back(theIdx);}
      }
      
      int theModified = theDeleted.size();
      for (auto theIdx : theDeleted) {
          storage.markBlockAsFree(theIdx);
          theRowBlockIndex.removeIndex(theIdx);
      }

      // write rowBlockIndex
      storage.writeStorable(theRowBlockIndex, BlockType::row_block, thePos);
      
      // Call  View
      double theTime = Config::getTimer().elapsed();
      RowChangedView theView{theResult, theModified, theTime};
      theView.show(aStream);

      return theResult;
  }

  StatusResult Database::showIndexes(std::ostream& aStream) {
      //std::cout<<"showIndexes *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{noError};

      std::map<std::string, std::vector<std::string>> theIndexes;

      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;

      for (auto theEntityEntry : entityMap) {
        std::string theTable = theEntityEntry.first;
        uint32_t theIndex = theEntityEntry.second;
        
        Entity theEntity;
        storage.readStorable(theIndex, theEntity);

        std::vector<std::string> theKeys;

        if (auto theKey = theEntity.getPrimaryKey()) {
          std::string theName = (*theKey).getName();
          theKeys.push_back(theName);
        }

        theIndexes[theTable] = theKeys;
      }

      double theTime = Config::getTimer().elapsed();
      ShowIndexesView theView{theIndexes, theTime};
      theView.show(aStream);

      return theResult;
  }

  StatusResult Database::showIndex(const DBQuery& aQuery, std::ostream& aStream) {
      //std::cout<<"showIndex *********************************"<<std::endl;
      Config::getTimer().reset();
      StatusResult theResult{noError};

      theResult = storage.readStorable(0, *this);
      if(StatusResult{Errors::noError}!=theResult) return theResult;
      
      // find aName in Entity map and find index
      if (entityMap.find(aQuery.getTableName()) == entityMap.end()){
          return StatusResult{ECE141::unknownTable};
      }
      uint32_t theIndex = entityMap[aQuery.getTableName()];
        
      // convert entity block to entity
      Entity theEntity;
      storage.readStorable(theIndex, theEntity);
      std::string thePrimaryKey;
      if (auto theKey = theEntity.getPrimaryKey()) {
        thePrimaryKey = (*theKey).getName();
      } else return StatusResult{Errors::primaryKeyRequired};

      // read rowBlockIndex
      RowBlockIndex theRowBlockIndex;
      uint32_t thePos = theEntity.getRowBlockIndex();
      storage.readStorable(thePos, theRowBlockIndex);
      std::vector<uint32_t> theRowBlockIndices = theRowBlockIndex.getRowBlockIndices();

      std::vector<std::pair<std::string, uint32_t>> theIndexes;

      for (auto theBlockIndex : theRowBlockIndices) {
        Row theRow;
        storage.readStorable(theBlockIndex, theRow);
        std::string theKey = Helpers::valueToString(theRow.getData().at(thePrimaryKey));
        theIndexes.push_back(std::make_pair(theKey, theBlockIndex));
      }

      double theTime = Config::getTimer().elapsed();
      ShowIndexView theView{theIndexes, theTime};
      theView.show(aStream);

      return theResult;
  }

  StatusResult Database::encode(std::ostream& aStream) {
    // std::cout<<"encode *********************************"<<std::endl;
    char thePayload[kPayloadSize];

    int theLoc = 0;
    std::memset(thePayload, 0, kPayloadSize);
    for(auto it= entityMap.begin();it!= entityMap.end();it++){
      // entity name
      std::strcpy(&thePayload[theLoc++], "s");
      std::strcpy(&thePayload[theLoc++], " ");
      std::strcpy(&thePayload[theLoc], it->first.c_str());
      theLoc += it->first.length();
      std::strcpy(&thePayload[theLoc++], " ");
      
      // entity block index
      std::strcpy(&thePayload[theLoc++], "i");
      std::strcpy(&thePayload[theLoc++], " ");
      std::strcpy(&thePayload[theLoc], std::to_string(it->second).c_str());
      theLoc += std::to_string(it->second).length();
      std::strcpy(&thePayload[theLoc++], " ");
    }

    aStream << thePayload;

    return StatusResult{noError};
  }

  StatusResult Database::decode(std::istream& aStream) {
      //std::cout<<"decode *********************************"<<std::endl;
      
    std::string theCharStr, theName, theIdx;
    while (std::getline(aStream, theCharStr, ' ')){
      if (theCharStr.length() > 1) break;
      char theChar = theCharStr[0];
      switch(theChar){
        case 's':
          std::getline(aStream, theName, ' ');
          break;
        case 'i':
          std::getline(aStream, theIdx, ' ');
          entityMap[theName] = std::stoi(theIdx);
          break;
      }
    }

    return StatusResult{Errors::noError};
  }

}
