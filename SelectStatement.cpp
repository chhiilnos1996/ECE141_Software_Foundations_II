//
//  SelectStatement.cpp
//  Assignment8
//
//  Created by chhiilnos on 6/4/22.
//

#include "SelectStatement.hpp"

#include "SQLStatement.hpp"
#include "TokenSequencer.hpp"
#include "ParseHelper.hpp"
#include "Helpers.hpp"
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"
#include "Errors.hpp"
#include "Attribute.hpp"
#include <set>


namespace ECE141 {
    using ClauseParser = std::function<StatusResult(Tokenizer& aTokenizer, DBQuery& aDBQuery)>;

    // SelectStatement
    StatusResult SelectStatement::parse(Tokenizer& aTokenizer) {
        StatusResult theResult;
        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        //std::cout<<"SelectStatement::parse 1"<<std::endl;
        if (!theSeq.nextIs({Keywords::select_kw})) return StatusResult{Errors::unexpectedKeyword}; // SELECT ...

        //std::cout<<"SelectStatement::parse 2"<<std::endl;
        if ("*" == aTokenizer.current().data) { // SELECT * ...
            dbQuery.setAttrWildcard(true);
            if (!aTokenizer.next()) return StatusResult{Errors::keywordExpected};
        } else { // SELECT a, b, c ...
            StringList theSelectList;
            theResult = theHelper.parseIdentifierList(theSelectList);
            dbQuery.setSelectList(theSelectList);
            /*
            for(int i=0;i<theSelectList.size();i++){
                std::cout<<theSelectList[i]<<std::endl;
            }*/
            
            if (!theResult) return theResult;
        }
        

        //std::cout<<"SelectStatement::parse 3"<<std::endl;
        //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
        std::string theTableName;
        if (!theSeq.nextIs({Keywords::from_kw}).getId(theTableName)){
            return StatusResult{Errors::unexpectedKeyword}; // SELECT ... FROM ...
        }
        //std::cout<<"theTableName = "<< theTableName <<std::endl;
        
        //std::cout<<"SelectStatement::parse 4"<<std::endl;
        dbQuery.setTableName(theTableName);
        if (!aTokenizer.next()) return StatusResult{noError};
        
        //std::cout<<"SelectStatement::parse 5"<<std::endl;
        // Select clauses
        std::set<Keywords> thePrev;
        while (theResult && aTokenizer.more()){
            Token &theToken = aTokenizer.current();
            if (TokenType::keyword == theToken.type){
                if(thePrev.find(theToken.keyword) == thePrev.end()){
                    theResult = parseClause(theToken.keyword, aTokenizer, dbQuery);
                    thePrev.insert(thePrev.end(), theToken.keyword);
                }
                else theResult = Errors::invalidArguments;
            }
            else if (aTokenizer.skipIf(semicolon)) break;
            else theResult = Errors::keywordExpected;

            //if (!aTokenizer.next()) break;
        }
      
        return StatusResult{noError};
    }

    StatusResult SelectStatement::parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery){
        std::map<Keywords, StatusResult(SelectStatement::*) (Tokenizer&, DBQuery&)> dispatch = {
            {Keywords::where_kw, &SelectStatement::parseWhere},
            {Keywords::limit_kw, &SelectStatement::parseLimit},
            {Keywords::order_kw, &SelectStatement::parseOrder},
            {Keywords::group_kw, &SelectStatement::parseGroup},
            {Keywords::inner_kw, &SelectStatement::parseJoin},
            {Keywords::left_kw, &SelectStatement::parseJoin},
            {Keywords::right_kw, &SelectStatement::parseJoin},
            {Keywords::join_kw, &SelectStatement::parseJoin},
        };
        
        if (dispatch.count(aKW)) {return (this->*dispatch[aKW])(aTokenizer, aDBQuery);}
        return StatusResult{unknownKeyword};
    }

    StatusResult SelectStatement::parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery) {
        return SQLStatement::parseWhere(aTokenizer, aDBQuery);
    }

  
    StatusResult SQLStatement::parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery){
        // where age > 24
        // WHERE age > 20 and grad_year = 2024

        TokenSequencer theSeq(aTokenizer);

        ExpressionList theWhereExpressions;

        if (!theSeq.nextIs({Keywords::where_kw})) return StatusResult{Errors::keywordExpected};

        StatusResult theResult{noError};

        while (TokenType::identifier == aTokenizer.current().type) {
            std::string theIdentifier;
            if (!theSeq.getId(theIdentifier)) return StatusResult{Errors::identifierExpected};
            Operand theLhs(theIdentifier, TokenType::identifier, theIdentifier);

            Operators theOperator;
            if (!aTokenizer.next() || !theSeq.getOp(theOperator)) return StatusResult{Errors::operatorExpected};

            std::string theStr;
            int theNumber;
            Operand theRhs(theIdentifier, TokenType::identifier, theStr);
            if (aTokenizer.next() && theSeq.getId(theStr)) theRhs.value = theStr;
            else if (theSeq.clear().getNumber(theNumber)) theRhs.value = theNumber;
            else return StatusResult{Errors::valueExpected};

            Expression theExpression(theLhs, theOperator, theRhs);

            if (!aTokenizer.next()) {
                theWhereExpressions.push_back(theExpression);
                break;
            }

            if (theSeq.nextIs({Keywords::and_kw})) theExpression.logic = Logical::and_op;
            else if (theSeq.clear().nextIs({Keywords::or_kw})) theExpression.logic = Logical::or_op;

            theWhereExpressions.push_back(theExpression);
        }

        aDBQuery.setWhereList(theWhereExpressions);

        //aTokenizer.next();

        return theResult;
    }
  
    StatusResult SelectStatement::parseLimit(Tokenizer& aTokenizer, DBQuery& aDBQuery){
        /*
        // LIMIT 10
        if( !aTokenizer.next() || aTokenizer.current().type != TokenType::number ){
            // TokenType is not number
            return StatusResult{identifierExpected};
        }
        
        // set limit to dbQuery
        int theLimit = stoi(aTokenizer.current().data);
        */

        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        if (!theSeq.nextIs({Keywords::limit_kw})) return StatusResult{Errors::keywordExpected};

        StringList theLimits;
        if (!theHelper.parseValueList(theLimits)) return StatusResult{Errors::valueExpected};

        if (theLimits.size() < 1 || theLimits.size() > 2) return StatusResult(Errors::valueExpected);

        int theStart = -1;
        if (theLimits.size() == 2) theStart = std::stoi(theLimits[0]);

        int theEnd = std::stoi(theLimits.size() == 2 ? theLimits[1] : theLimits[0]);

        aDBQuery.setLimit(theStart, theEnd);

        //aTokenizer.next();
        
        return StatusResult{noError};
    }

    StatusResult SelectStatement::parseOrder(Tokenizer& aTokenizer, DBQuery& aDBQuery){
        // ORDER BY last_name
        if ( !aTokenizer.next() ){
            // no following word after order
            return StatusResult{keywordExpected};
        }
        if ( TokenType::keyword != aTokenizer.current().type || aTokenizer.current().keyword != Keywords::by_kw ){
            // not by after order
            return StatusResult{keywordExpected};
        }
        if ( !aTokenizer.next() || TokenType::identifier != aTokenizer.current().type){
            // no following attribute after by
            return StatusResult{identifierExpected};
        }
        
        // order By attribute
        StatusResult theResult;
        ParseHelper theHelper(aTokenizer);
        StringList theOrderList;
        theResult = theHelper.parseIdentifierList(theOrderList);
        if (theResult != StatusResult{noError}){
            return theResult;
        }
        if (theOrderList.size() == 0) {
            return StatusResult{identifierExpected};
        }
        // if successfully parsed order list and not empty
        aDBQuery.setOrderList(theOrderList);
        
        return StatusResult{noError};
    }

    StatusResult SelectStatement::parseGroup(Tokenizer& aTokenizer, DBQuery& aDBQuery) {
        return StatusResult{noError};
    }

    StatusResult SelectStatement::parseJoin(Tokenizer& aTokenizer, DBQuery& aDBQuery){
        // multiple joins
        static ECE141::Keywords gJoinTypes[]={
            ECE141::Keywords::inner_kw,ECE141::Keywords::left_kw,
            ECE141::Keywords::right_kw,ECE141::Keywords::join_kw
        };
        
        std::vector<Keywords> theJoins;
        std::vector<std::string> theTables;
        while(in_array<Keywords>(gJoinTypes,  aTokenizer.current().keyword)) {
            // theJoinType
            //std::cout<<"catch join"<<std::endl;
            //std::cout<<aTokenizer.current().data<<std::endl;
            Keywords theJoinType=aTokenizer.current().keyword;
            theJoins.push_back(theJoinType);
            //yank the 'join-type' token (e.g. left, right)
            aTokenizer.next();
            aTokenizer.skipIf(Keywords::join_kw);
            
            // theTable
            if ( TokenType::identifier != aTokenizer.current().type){
                //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
                //std::cout<<"return 1 "<<std::endl;
                return StatusResult{identifierExpected};
            }
            std::string theTable = aTokenizer.current().data;//push back join
            //std::cout<<"theTable = "<<theTable<<std::endl;
            theTables.push_back(theTable);
            aTokenizer.next();
        }
        //std::cout<<"theJoins.size() = "<<theJoins.size()<<std::endl;
        
        //on
        std::vector<Operand> theLHSs;
        std::vector<Operand> theRHSs;
        if(!aTokenizer.skipIf(Keywords::on_kw)) { return StatusResult{keywordExpected};}
        while(1){
            //LHS TableName.AttrName
            if (TokenType::identifier != aTokenizer.current().type){
                //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
                //std::cout<<"return 2 "<<std::endl;
                return StatusResult{identifierExpected};
            }
            //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
            std::string theCombined = aTokenizer.current().data;
            std::string theLHSTableName = theCombined.substr(0, theCombined.find('.'));
            std::string theLHSAttrName = theCombined.substr(theCombined.find('.')+1);
            Operand theLHS(theLHSTableName, TokenType::identifier, theLHSAttrName);
            theLHSs.push_back(theLHS);
            //=
            if(!aTokenizer.next() || !aTokenizer.skipIf(Operators::equal_op)) {
                //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
                //std::cout<<"return 3 "<<std::endl;
                return StatusResult{operatorExpected};
            }
            //RHS field
            if (TokenType::identifier != aTokenizer.current().type){
                //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
                //std::cout<<"return 4 "<<std::endl;
                return StatusResult{identifierExpected};
            }
            //std::cout<<"aTokenizer.current().data = "<<aTokenizer.current().data<<std::endl;
            theCombined = aTokenizer.current().data;
            std::string theRHSTableName = theCombined.substr(0, theCombined.find('.'));
            std::string theRHSAttrName = theCombined.substr(theCombined.find('.')+1);
            Operand theRHS(theRHSTableName, TokenType::identifier, theRHSAttrName);
            theRHSs.push_back(theRHS);
            // break if no and, skip and
            if (!aTokenizer.next() || Keywords::and_kw != aTokenizer.current().keyword){
               // std::cout<<"break 5 "<<std::endl;
                break;
            }
            if(!aTokenizer.skipIf(Keywords::and_kw)) {
                //std::cout<<"return 5 "<<std::endl;
                return StatusResult{keywordExpected};
            }
        }
        
        // push back joins
        std::string thePrevTable = aDBQuery.getTableName();
        for(int i=0; i<theJoins.size(); i++){
            bool found = 0;
            for(int j=0;j<theLHSs.size();j++){
                bool theCond1 = (theLHSs[j].name == thePrevTable) && (theRHSs[j].name == theTables[i]);
                bool theCond2 = (theLHSs[j].name == theTables[i]) && (theRHSs[j].name == thePrevTable);
                if(theCond1 || theCond2){
                    found = 1;
                    if(theCond1){
                        Join theJoin(theTables[i], theJoins[i], theLHSs[j], theRHSs[j]);
                        joinList.push_back(theJoin);
                    }
                    else if(theCond2){
                        Join theJoin(theTables[i], theJoins[i], theRHSs[j], theLHSs[j]);
                        joinList.push_back(theJoin);
                    }
                    theLHSs.erase(theLHSs.begin()+j);
                    theRHSs.erase(theRHSs.begin()+j);
                }
            }
            if (found == 0) {return StatusResult{Errors::joinConditionMismatch};}
            thePrevTable = theTables[i];
        }
        /*
        for(int i=0; i<joinList.size(); i++){
            std::cout<<"joinList["<<i<<"]"<<std::endl;
            std::cout<<joinList[i].getLHS().name<<"."<<std::get<std::string>(joinList[i].getLHS().value)<<std::endl;
            std::cout<<joinList[i].getRHS().name<<"."<<std::get<std::string>(joinList[i].getRHS().value)<<std::endl;
            std::cout<<joinList[i].getTableName()<<std::endl;
        }*/
        
        return StatusResult{Errors::noError};
    }

    // DeleteStatement
    StatusResult DeleteStatement::parse(Tokenizer& aTokenizer) {
        StatusResult theResult;
        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        if (!theSeq.nextIs({Keywords::delete_kw})) return StatusResult{Errors::unexpectedKeyword}; // SELECT ...

        std::string theTableName;
        if (!theSeq.nextIs({Keywords::from_kw}).getId(theTableName)){
            return StatusResult{Errors::unexpectedKeyword}; // SELECT ... FROM ...
        }
        dbQuery.setTableName(theTableName);

        if (!aTokenizer.next()) return StatusResult{noError};
        
        // Select clauses
        std::set<Keywords> thePrev;
        while (theResult && aTokenizer.more()){
            Token &theToken = aTokenizer.current();
            if (TokenType::keyword == theToken.type){
                if(thePrev.find(theToken.keyword) == thePrev.end()){
                    theResult = parseClause(theToken.keyword, aTokenizer, dbQuery);
                    thePrev.insert(thePrev.end(), theToken.keyword);
                }
                else theResult = Errors::invalidArguments;
            }
            else if (aTokenizer.skipIf(semicolon)) break;
            else theResult = Errors::keywordExpected;

            //if (!aTokenizer.next()) break;
        }

        return StatusResult{noError};
    }

    StatusResult DeleteStatement::parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery){
        std::map<Keywords, StatusResult(DeleteStatement::*) (Tokenizer&, DBQuery&)> dispatch = {
            {Keywords::where_kw, &DeleteStatement::parseWhere},
        };
        
        if (dispatch.count(aKW)) {return (this->*dispatch[aKW])(aTokenizer, aDBQuery);}
        return StatusResult{unknownKeyword};
    }

    StatusResult DeleteStatement::parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery) {
        return SQLStatement::parseWhere(aTokenizer, aDBQuery);
    }

    // UpdateStatement
    StatusResult UpdateStatement::parse(Tokenizer& aTokenizer) {
        StatusResult theResult;
        TokenSequencer theSeq(aTokenizer);
        ParseHelper theHelper(aTokenizer);

        if (!theSeq.nextIs({Keywords::update_kw})) return StatusResult{Errors::unexpectedKeyword}; // SELECT ...

        std::string theTableName;
        if (!theSeq.getId(theTableName)){
            return StatusResult{Errors::unexpectedKeyword}; // SELECT ... FROM ...
        }
        dbQuery.setTableName(theTableName);

        if (!aTokenizer.next()) return StatusResult{noError};
        
        // Select clauses
        std::set<Keywords> thePrev;
        while (theResult && aTokenizer.more()){
            Token &theToken = aTokenizer.current();
            if (TokenType::keyword == theToken.type){
                if(thePrev.find(theToken.keyword) == thePrev.end()){
                    theResult = parseClause(theToken.keyword, aTokenizer, dbQuery);
                    thePrev.insert(thePrev.end(), theToken.keyword);
                }
                else theResult = Errors::invalidArguments;
            }
            else if (aTokenizer.skipIf(semicolon)) break;
            else theResult = Errors::keywordExpected;

            //if (!aTokenizer.next()) break;
        }

        return StatusResult{noError};
    }

    StatusResult UpdateStatement::parseClause(Keywords aKW, Tokenizer& aTokenizer, DBQuery& aDBQuery){
        std::map<Keywords, StatusResult(UpdateStatement::*) (Tokenizer&, DBQuery&)> dispatch = {
            {Keywords::where_kw, &UpdateStatement::parseWhere},
            {Keywords::set_kw, &UpdateStatement::parseSet},
        };
        
        if (dispatch.count(aKW)) {return (this->*dispatch[aKW])(aTokenizer, aDBQuery);}
        return StatusResult{unknownKeyword};
    }

    StatusResult UpdateStatement::parseWhere(Tokenizer& aTokenizer, DBQuery& aDBQuery) {
        return SQLStatement::parseWhere(aTokenizer, aDBQuery);
    }

    StatusResult UpdateStatement::parseSet(Tokenizer& aTokenizer, DBQuery& aDBQuery) {
        // set age = 40, pid = "yoyo"
        TokenSequencer theSeq(aTokenizer);
        ExpressionList theSetExpressions;
        StatusResult theResult{noError};

        if (!theSeq.nextIs({Keywords::set_kw})) return StatusResult{Errors::keywordExpected};

        while (TokenType::identifier == aTokenizer.current().type) {
            // Lhs operand
            std::string theIdentifier;
            if (!theSeq.getId(theIdentifier)){
                return StatusResult{Errors::identifierExpected};
            }
            Operand theLhs(theIdentifier, TokenType::identifier, theIdentifier);

            // operator
            Operators theOperator;
            if (!aTokenizer.next() || !theSeq.getOp(theOperator) || Operators::equal_op != theOperator){
                return StatusResult{Errors::operatorExpected};
            }

            // Rhs operand
            std::string theStr;
            int theNumber;
            Operand theRhs(theIdentifier, TokenType::identifier, theStr);
            if (aTokenizer.next() && theSeq.getId(theStr)){
                theRhs.value = theStr;
            }
            else if (theSeq.clear().getNumber(theNumber)){
                theRhs.value = theNumber;
            }
            else{
                return StatusResult{Errors::valueExpected};
            }

            // expression
            Expression theExpression(theLhs, theOperator, theRhs);
            theSetExpressions.push_back(theExpression);

            if (!aTokenizer.skipIf(comma) || !aTokenizer.next()) break;
        }

        dbQuery.setSetList(theSetExpressions);
        //aTokenizer.next();

        return theResult;
    }


}
