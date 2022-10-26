//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "Entity.hpp"
#include "Row.hpp"
#include "DBQuery.hpp"
#include "Joins.hpp"

namespace ECE141 {

  class Database : public Storable {
  public:
    
    Database(const std::string aPath, CreateDB);
    Database(const std::string aPath, OpenDB);
    std::string getDatabaseName(){ return dbName; };
    void closeDatabase(){ dbName = ""; storage.end(); };
    virtual ~Database();

    StatusResult  dump(std::ostream &anOutput);
    StatusResult  createTable(Entity* anEntity, std::ostream& aStream);
    StatusResult  describeTable(const std::string &aName, std::ostream& aStream);
    StatusResult  dropTable(const std::string &aName, std::ostream& aStream);
    StatusResult  showTables(std::ostream& aStream);
      
    StatusResult  insertRecords(const RowCollection& aRows, std::ostream& anOutput);
    StatusResult  selectRows(const DBQuery& aQuery, std::ostream& aStream);
    StatusResult  updateTable(const DBQuery& aQuery, std::ostream& aStream);
    StatusResult  deleteTable(const DBQuery& aQuery, std::ostream& aStream);
    StatusResult  joinTables(std::string aName, JoinList& aJoinList, RowCollection& aRowCollection);
    StatusResult  selectJoins(const DBQuery& aQuery, RowCollection& aRowCollection, std::ostream& aStream);

    StatusResult  showIndexes(std::ostream& aStream);
    StatusResult  showIndex(const DBQuery& aQuery, std::ostream& aStream);
      
    StatusResult  encode(std::ostream& aStream) override;
    StatusResult  decode(std::istream& aStream) override;

  protected:
    
    std::string dbName;
    Storage storage;
    std::map<std::string, std::unique_ptr<Entity>> entities; //entity name to entity object pointer
    std::map<std::string, int32_t> entityMap; //entity name to block index
  };

}
#endif /* Database_hpp */
