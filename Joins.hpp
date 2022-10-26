//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/21/21.
//

#ifndef Join_h
#define Join_h

#include <string>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "keywords.hpp"
#include "Row.hpp"
#include "Filters.hpp"

namespace ECE141 {

  class Join  {
  public:
      Join(const std::string &aTable, Keywords aType, Operand& aLHS, Operand& aRHS)
      : table(aTable), joinType(aType), lhs(aLHS), rhs(aRHS), expr(Expression(aLHS, Operators::equal_op, aRHS)){}
      Keywords getJoinType(){return joinType;};
      Operand getLHS(){return lhs;};
      Operand getRHS(){return rhs;};
      std::string getTableName(){return table;};
      
      
  private:
      Keywords    joinType;  //left, right, etc...
      std::string table;
      Operand lhs;
      Operand rhs;
      Expression  expr;
  };

  using JoinList = std::vector<Join>;

}

#endif /* Join_h */

