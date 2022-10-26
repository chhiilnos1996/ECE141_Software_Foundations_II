#ifndef SQLStatement_h
#define SQLStatement_h

#include "Statement.hpp"
#include "Database.hpp"
#include "Tokenizer.hpp"
#include "Config.hpp"
#include "Errors.hpp"
#include "Row.hpp"
#include "EntityView.hpp"
#include "TableDescriptionView.hpp"
#include "CreateTableView.hpp"

namespace ECE141 {
    
    class SQLStatement : public Statement {
    protected:
        Database* database;

    public:
        SQLStatement(Database* aDatabase, Keywords aStatementType=Keywords::unknown_kw) : database(aDatabase), Statement(aStatementType) {}
        SQLStatement(const SQLStatement& aCopy) : database(aCopy.database), Statement(aCopy) {}

        virtual StatusResult parse(Tokenizer& aTokenizer) override {
            return StatusResult{Errors::noError};
        }

        virtual StatusResult run(std::ostream &aStream) override {
            return StatusResult{Errors::noError};
        }

        static StatusResult parseWhere(Tokenizer& aTokenizer, DBQuery& aQuery);
    };

    class CreateTableStatement : public SQLStatement {
    protected:
        std::string tableName;
        Entity* entity;

    public:
        CreateTableStatement(Database* aDatabase, Keywords aStatementType=Keywords::create_kw) : SQLStatement(aDatabase, aStatementType) {}

        ~CreateTableStatement() {
            if (entity) delete entity;
        }

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult parseAttribute(Tokenizer& aTokenizer);

        StatusResult run(std::ostream &aStream) override {
            return database->createTable(entity, aStream);
        }
    };

    class DescribeTableStatement : public SQLStatement {
    protected:
        std::string tableName;

    public:
        DescribeTableStatement(Database* aDatabase, Keywords aStatementType=Keywords::describe_kw) : SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream &aStream) override {
            return database->describeTable(tableName, aStream);
        }
    };

    class DropTableStatement : public SQLStatement {
    protected:
        std::string tableName;

    public:
        DropTableStatement(Database* aDatabase, Keywords aStatementType=Keywords::drop_kw) : SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream &aStream) override {
            return database->dropTable(tableName, aStream);
        }
    };

    class ShowTablesStatement : public SQLStatement {
    public:
        ShowTablesStatement(Database* aDatabase, Keywords aStatementType=Keywords::show_kw) : SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream &aStream) override {
            return database->showTables(aStream);
        }
    };
  
    class InsertTableStatement : public SQLStatement {
    protected:
        std::string tableName;
        StringList attrList;
        RowCollection rows;
        
    public:
        InsertTableStatement(Database* aDatabase, Keywords aStatementType=Keywords::insert_kw) : tableName(""), attrList(StringList{}), rows(RowCollection{}), SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream &aStream) override {
            return database->insertRecords(rows, aStream);
        }
    };

    class ShowIndexesStatement : public SQLStatement {

    public:
        ShowIndexesStatement(Database* aDatabase, Keywords aStatementType=Keywords::show_kw) : SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;
        StatusResult run(std::ostream& aStream) override {
            return database->showIndexes(aStream);
        }
    };

    class ShowIndexStatement : public SQLStatement {
    protected:
        DBQuery dbQuery;

    public:
        ShowIndexStatement(Database* aDatabase, Keywords aStatementType=Keywords::show_kw) : SQLStatement(aDatabase, aStatementType) {}

        StatusResult parse(Tokenizer& aTokenizer) override;
        StatusResult run(std::ostream& aStream) override {
            return database->showIndex(dbQuery, aStream);
        }
    };
}

#endif
