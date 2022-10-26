#include "SQLStatement.hpp"
#include "TokenSequencer.hpp"
#include "ParseHelper.hpp"
#include "Helpers.hpp"
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"
#include "Errors.hpp"
#include "Attribute.hpp"
#include "SelectStatement.hpp"
#include <set>
namespace ECE141 {
    using ClauseParser = std::function<StatusResult(Tokenizer& aTokenizer, DBQuery& aDBQuery)>;

    // CreateTableStatement
    StatusResult CreateTableStatement::parse(Tokenizer& aTokenizer) {
        //std::cout << "CreateTableStatement::parse" << std::endl;

        if (Keywords::create_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || Keywords::table_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{identifierExpected};

        tableName = aTokenizer.current().data;

        entity = new Entity(tableName);

        //std::cout << "CREATE TABLE: " << tableName << std::endl;

        if (!aTokenizer.skipIf('(')) return StatusResult{punctuationExpected};

        StatusResult theAttributeStatus;
        do theAttributeStatus = parseAttribute(aTokenizer);
        while (theAttributeStatus
        && !(!aTokenizer.skipTo(TokenType::punctuation) || "," != aTokenizer.current().data));

        if (!theAttributeStatus) return theAttributeStatus;

        if (!aTokenizer.skipTo(TokenType::punctuation) || ")" != aTokenizer.current().data ) return StatusResult{punctuationExpected};

        return StatusResult{noError};
    }

    StatusResult CreateTableStatement::parseAttribute(Tokenizer& aTokenizer) {
        if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{identifierExpected};
        std::string theAttributeName = aTokenizer.current().data;
        if (entity->getAttribute(theAttributeName)) return StatusResult{invalidAttribute};

        if (!aTokenizer.next() || TokenType::keyword != aTokenizer.current().type) return StatusResult{identifierExpected};
        Keywords theKeyword = aTokenizer.current().keyword;
        if (!Helpers::isDatatype(theKeyword)) return StatusResult{identifierExpected};

        DataTypes theDatatype = Helpers::getTypeForKeyword(theKeyword);
        if (DataTypes::no_type == theDatatype) return StatusResult{identifierExpected};

        Attribute theAttribute(theAttributeName, theDatatype);
        if (DataTypes::varchar_type == theDatatype && aTokenizer.next() && aTokenizer.next() && TokenType::number == aTokenizer.current().type) {
           int theSize = std::atoi(aTokenizer.current().data.c_str()); 
           theAttribute.setSize(theSize);

           if (!aTokenizer.skipIf(')')) return StatusResult{punctuationExpected};
        }

        while (aTokenizer.next() && (TokenType::punctuation != aTokenizer.current().type && "," != aTokenizer.current().data)) {
            if (Keywords::not_kw == aTokenizer.current().keyword) {
                if (aTokenizer.next() && Keywords::null_kw == aTokenizer.current().keyword) {
                    theAttribute.setNullable(false);
                    continue;
                } else return StatusResult{unknownAttribute};
            }

            if (Keywords::auto_increment_kw == aTokenizer.current().keyword) {
                theAttribute.setAutoIncrement(true);
                continue;
            }

            if (Keywords::primary_kw == aTokenizer.current().keyword
            && aTokenizer.next() && Keywords::key_kw == aTokenizer.current().keyword) {    
                if (entity->getPrimaryKey()) return StatusResult{invalidAttribute};
                theAttribute.setPrimaryKey(true);
                continue;
            }
        }

        entity->addAttribute(theAttribute);

        //std::cout << "ATTRIBUTE: " << theAttributeName << " / " << Helpers::dataTypeToString(theDatatype) << " / " << theAttribute.getSize();
        //std::cout << " / Primary Key: " << theAttribute.isPrimaryKey() << " / Auto Increment: " << theAttribute.isAutoIncrement() << " / Nullable: " << theAttribute.isNullable() << std::endl;

        return StatusResult{noError};
    }

    // DescribeTableStatement
    StatusResult DescribeTableStatement::parse(Tokenizer& aTokenizer) {
        if (Keywords::describe_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{identifierExpected};

        tableName = aTokenizer.current().data;

        return StatusResult{noError};
    }

    // DropTableStatement
    StatusResult DropTableStatement::parse(Tokenizer& aTokenizer) {
        if (Keywords::drop_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || Keywords::table_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || TokenType::identifier != aTokenizer.current().type) return StatusResult{identifierExpected};

        tableName = aTokenizer.current().data;

        return StatusResult{noError};
    }

    // ShowTablesStatement
    StatusResult ShowTablesStatement::parse(Tokenizer& aTokenizer) {
        if (Keywords::show_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};
        if (!aTokenizer.next() || Keywords::tables_kw != aTokenizer.current().keyword) return StatusResult{unknownCommand};

        return StatusResult{noError};
    }

    StatusResult InsertTableStatement::parse(Tokenizer& aTokenizer) {
        StatusResult theResult;
        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        if(theSeq.nextIs({Keywords::insert_kw, Keywords::into_kw}).getId(tableName)) {
          if (aTokenizer.skipIf('(')) {
              aTokenizer.next();
            theResult = theHelper.parseIdentifierList(attrList);
            // using StringList = std::vector<std::string>;
            // validate attribute list
            if (attrList.size() == 0) {
                return StatusResult{identifierExpected};
            }

            if (theResult && Keywords::values_kw == aTokenizer.current().keyword) {
                if (aTokenizer.skipIf('(')) {
                    while(theResult) {
                      StringList theValueList;
                      aTokenizer.next();
                      if (theResult = theHelper.parseValueList(theValueList)) {
                          aTokenizer.next();
                          if (theValueList.size() != attrList.size()) {
                              return StatusResult{identifierExpected};
                          }

                          // using StringMap = std::map<std::string, std::string>;
                          StringMap theMap;
                          for (size_t i = 0; i < attrList.size(); i++){
                              theMap[attrList[i]] = theValueList[i];
                          }

                          std::unique_ptr<Row> theRow = std::make_unique<Row>(tableName, theMap);
                          rows.push_back(std::move(theRow));
                          if (rows.size() == 0) return StatusResult{valueExpected};

                          if (',' == aTokenizer.current().data[0]) aTokenizer.next();
                          if (';' == aTokenizer.current().data[0]) break;
                      }
                    }
                }
              aTokenizer.skipIf(')');
            }
          }
        }
        return StatusResult{noError};
    }

    
    // ShowIndexesStatement
    StatusResult ShowIndexesStatement::parse(Tokenizer& aTokenizer) {
        TokenSequencer theSeq(aTokenizer);

        if (!theSeq.nextIs({Keywords::indexes_kw})) return StatusResult{Errors::keywordExpected};

        return StatusResult{noError};
    }

    // ShowIndexStatement
    StatusResult ShowIndexStatement::parse(Tokenizer& aTokenizer) {
        aTokenizer.restart();
        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        if (!theSeq.nextIs({Keywords::show_kw, Keywords::index_kw})) return StatusResult{Errors::keywordExpected};

        StringList theSelectList;
        if (!theHelper.parseIdentifierList(theSelectList)) return StatusResult{Errors::identifierExpected};
        dbQuery.setSelectList(theSelectList);

        if (!theSeq.nextIs({Keywords::from_kw})) return StatusResult{Errors::keywordExpected};

        std::string theTableName;
        if (!theSeq.getId(theTableName)) return StatusResult{Errors::unexpectedKeyword};
        dbQuery.setTableName(theTableName);

        return StatusResult{noError};
    }
}
