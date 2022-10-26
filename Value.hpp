//
//  Value.h
//  Assignment4
//
//  Created by chhiilnos on 4/25/22.
//

#ifndef Value_h
#define Value_h

#include <stdio.h>
#include <string>

/*
namespace ECE141 {

  using ContentVariant = std::variant<bool, int, double, std::string>;
  class Value : public Storable{
  public:
    Value(DataTypes aType, std::string& aContent) : type(aType),  content(aContent) {}
    ~Value();
    Value& operator=(const Value &aValue){
         type = aValue.getType();
         content = aValue.getContent();
         return *this;
    }
    bool operator==(Value &aCopy) const {
        return ((type == aCopy.type) && (content == aCopy.content));
    }
      //STUDENT: What other methods do you require?
    const DataTypes  getType() const {return type;}
    const ContentVariant  getContent() const {return content;}
    Value&              setType(DataTypes aType){
        type = aType;
        return *this;
    }
    Value&              setContent(const std::string &aContent){
        content = aContent;
        return *this;
    }
    
    StatusResult        encode(std::ostream& aStream) override;
    StatusResult        decode(std::istream& aStream) override;

  protected:
    DataTypes         type;
    ContentVariant         content;
  };



}*/

#endif /* Value_h */
