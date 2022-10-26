//
//  Row.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Attribute.hpp"
#include "BasicTypes.hpp"
#include "Helpers.hpp"
//feel free to use this, or create your own version...

namespace ECE141 {

  class Row : public Storable {
  public:
    Row(std::string aName, StringMap aMap);
    Row(uint32_t entityId=0);
    Row(const Row &aRow);
    Row(bool aIsNull, Row &aRow);
    Row(std::string& aName,Row &aRow);
    Row(Row &aRow1, Row &aRow2);
    
   // Row(const Attribute &anAttribute); //maybe?
    
    ~Row();
    
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
      //STUDENT: What other methods do you require?
    const std::string&  getEntityName() const {return entityName;}
    void                setEntityName(const std::string& aName) {entityName = aName;}
   
    bool                emplace(const std::string &aKey,
                                const Value &aValue);
    Row&                set(const std::string &aKey,
                            const Value &aValue);

    Value               getValue(std::string aKey) {
        if (data.find(aKey)!=data.end()) {
            return data[aKey];
        }
        for (auto const& [key, val] : data)
        {
            std::string theSubStr = key.substr(key.find('.')+1);
            if (theSubStr == aKey) {
                return val;
            }
        }
        Value theNullStr = "NULL";
        return theNullStr;
    }
    void                print(){
          for (auto const& [key, val] : data)
          {
              std::cout<<key<<" : "<<Helpers::valueToString(val)<<", ";
          }
          std::cout<<std::endl;
    }
        
    KeyValues&          getData() {return data;}
    
    StatusResult        encode(std::ostream& aStream) override;
    StatusResult        decode(std::istream& aStream) override;
    
    //uint32_t            entityId; //hash value of entity?
    //uint32_t            blockNumber;

  protected:
    std::string         entityName;
    // KeyValues = std::map<const std::string, Value>;
    KeyValues           data;
  };

  //-------------------------------------------

  using RowCollection = std::vector<std::unique_ptr<Row>>;


}
#endif /* Row_hpp */

