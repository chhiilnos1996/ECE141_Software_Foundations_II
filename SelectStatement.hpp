//
//  SelectStatement.hpp
//  Assignment8
//
//  Created by chhiilnos on 6/4/22.
//

#ifndef SelectStatement_hpp
#define SelectStatement_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Database.hpp"
#include "Tokenizer.hpp"
#include "Config.hpp"
#include "Errors.hpp"
#include "Row.hpp"
#include "SQLStatement.hpp"
#include "EntityView.hpp"
#include "TableDescriptionView.hpp"
#include "CreateTableView.hpp"

namespace ECE141 {
    
    class SelectStatement : public SQLStatement {
    protected:
        std::string theTable;
        DBQuery dbQuery;
        JoinList joinList;
        RowCollection theJoinedRows;
        
    public:
        SelectStatement(Database* aDatabase, Keywords aStatementType=Keywords::select_kw) : dbQuery(DBQuery{}), SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;
        StatusResult parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseLimit(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseOrder(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseGroup(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseLeft(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseRight(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseJoin(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult run(std::ostream& aStream) override {
            if (joinList.size()==0){
                return database->selectRows(dbQuery, aStream);
            }
            else{
                database->joinTables(dbQuery.getTableName(), joinList, theJoinedRows);
                return database->selectJoins(dbQuery, theJoinedRows, aStream);
            }
            
        }
    };

    class DeleteStatement : public SQLStatement {
    protected:
        DBQuery dbQuery;

    public:
        DeleteStatement(Database* aDatabase, Keywords aStatementType=Keywords::select_kw) : dbQuery(DBQuery{}), SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;
        StatusResult parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult run(std::ostream& aStream) override {
            return database->deleteTable(dbQuery, aStream);
        }
    };

    class UpdateStatement : public SQLStatement {
    protected:
        DBQuery dbQuery;

    public:
        UpdateStatement(Database* aDatabase, Keywords aStatementType=Keywords::select_kw) : dbQuery(DBQuery{}), SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;
        StatusResult parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseSet(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery);
        StatusResult run(std::ostream& aStream) override {
            
            return database->updateTable(dbQuery, aStream);
        }
    };
}

#endif /* SelectStatement_hpp */
