#ifndef DBQuery_h
#define DBQuery_h
#include "Filters.hpp"
#include "Helpers.hpp"

namespace ECE141 {

    using ExpressionList = std::vector<Expression>;

    class DBQuery {
        
    public :
        DBQuery() : tableName(""), limitStart(-1), limitEnd(-1), attrWildcard(false), selectList(StringList{}) {}

        StatusResult filterWhere(const Entity& anEntity, RowCollection& aRowCollection) const {
            std::map<Operators,
            std::function<std::function<bool(const std::unique_ptr<Row>& aRowPtr)>(
                const Attribute& anAttribute,
                const Expression& anExpression)>>
            theFilterOps = {
                {
                    Operators::lt_op,
                    [](const Attribute& anAttribute, const Expression& anExpression) {
                        return [anAttribute=anAttribute, anExpression=anExpression](const std::unique_ptr<Row>& aRowPtr) {
                            //std::cout << Helpers::valueToString(aRowPtr.get()->getValue(anAttribute.getName())) << " < " << Helpers::valueToString(anExpression.rhs.value) << std::endl;
                   
                            return !(aRowPtr.get()->getValue(anAttribute.getName()) < anExpression.rhs.value);
                        };
                    }
                },
                {
                    Operators::gt_op,
                    [](const Attribute& anAttribute, const Expression& anExpression) {
                        return [anAttribute=anAttribute, anExpression=anExpression](const std::unique_ptr<Row>& aRowPtr) {
                            //std::cout << Helpers::valueToString(aRowPtr.get()->getValue(anAttribute.getName())) << " > " << Helpers::valueToString(anExpression.rhs.value) << std::endl;
                 
                            return !(aRowPtr.get()->getValue(anAttribute.getName()) > anExpression.rhs.value);
                        };
                    }
                },
                {
                    Operators::equal_op,
                    [](const Attribute& anAttribute, const Expression& anExpression) {
                        return [anAttribute=anAttribute, anExpression=anExpression](const std::unique_ptr<Row>& aRowPtr) {
                            //std::cout << Helpers::valueToString(aRowPtr.get()->getValue(anAttribute.getName())) << " = " << Helpers::valueToString(anExpression.rhs.value) << std::endl;

                            return !(aRowPtr.get()->getValue(anAttribute.getName()) == anExpression.rhs.value);
                        };
                    }
                }
            };

            ExpressionList theWhereList = getWhereList();
            for (Expression theExpression : theWhereList) {
                if (!theFilterOps.count(theExpression.op)) return StatusResult{Errors::unknownKeyword};

                auto theAttrOpt = anEntity.getAttribute(theExpression.lhs.name);
                if (!theAttrOpt) return StatusResult{Errors::unknownAttribute};

                auto& theAttr = *theAttrOpt;

                aRowCollection.erase(
                    std::remove_if(
                        aRowCollection.begin(), aRowCollection.end(), 
                        theFilterOps[theExpression.op](theAttr, theExpression)
                    ),
                    aRowCollection.end()
                );
            }

            return StatusResult{noError};
        }
        
        std::string getTableName() const {
            return tableName;
        }
        
        bool getAttrWildcard() const {
            return attrWildcard;
        }
                
        StringList getSelectList() const {
            return selectList;
        }
        
        ExpressionList getWhereList() const {
            return whereList;
        }
        
        int getStartLimit() const {
            return limitStart;
        }

        int getEndLimit() const {
            return limitEnd;
        }
                
        StringList getOrderList() const {
            return orderList;
        }

        ExpressionList getSetList() const {
            return setList;
        }
        
        StringList getGroupList() const {
            return groupList;
        }
        
        void  setTableName(const std::string& aName) {
            tableName = aName;
        }
        
        void  setAttrWildcard(bool aBool){
            attrWildcard = aBool;
        }
        
        void  setSelectList(const StringList aList) {
            selectList = aList;
        }
        
        void  setWhereList(const ExpressionList& aList) {
            whereList = aList;
        }
        
        void  setLimit(const int aStart, const int anEnd) {
            limitStart = aStart;
            limitEnd = anEnd;
        }
       
        void  setOrderList(const StringList& aList) {
            orderList = aList;
        }

        void  setSetList(const ExpressionList& aList) {
            setList = aList;
        }
        
        void  setGroupList(const StringList& aList) {
            groupList = aList;
        }
        
    protected :
        std::string tableName; //FROM (tablelist) JOINS ????
        bool attrWildcard;
        int limitStart, limitEnd;  //LIMIT (int)
        StringList selectList; //select (fieldlist)
        ExpressionList whereList; //WHERE (ExpressionList)
        ExpressionList setList; //SET (ExpressionList)
        StringList orderList;  //ORDER BY (view ordering)
        StringList groupList;  //GROUP BY (AttrList) -- partioning of our data
    };

}

#endif
