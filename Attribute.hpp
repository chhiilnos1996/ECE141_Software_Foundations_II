//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "Storable.hpp"
#include "keywords.hpp"
#include "BasicTypes.hpp"
//#include "Storage.hpp"

namespace ECE141 {
      
/*
  struct Property {
    Property(std::string aName, int aTableId=0) : name(aName), tableId(aTableId), desc(true) {}
    std::string     name;
    int             tableId;
    bool            desc;
  };

  using PropertyList = std::vector<Property>;
*/

  class Attribute : public Storable {
  protected:
    std::string   name;
    DataTypes     type;
    int           size; //max=1000
    bool          autoIncrement;
    bool          primary;
    bool          nullable;
    //Others?

  public:
          
    Attribute(DataTypes aType=DataTypes::no_type);
    Attribute(std::string aName, DataTypes aType, uint32_t aSize=0);
    Attribute(const Attribute &aCopy);
    ~Attribute();
    
    Attribute&          setName(std::string &aName);
    Attribute&          setDataType(DataTypes aType);
    Attribute&          setSize(int aSize);
    Attribute&          setAutoIncrement(bool anAuto);
    Attribute&          setPrimaryKey(bool anAuto);
    Attribute&          setNullable(bool aNullable);
    
    bool                isValid(); //is this Entity valid?
    
    const std::string&  getName() const {return name;}
    DataTypes           getType() const {return type;}
    size_t              getSize() const {return size;}
    Value               toValue(const std::string &aValue) const;

    bool                isPrimaryKey() const {return primary;}
    bool                isNullable() const {return nullable;}
    bool                isAutoIncrement() const {return autoIncrement;}    

    StatusResult encode(std::ostream& aStream) override;
    StatusResult decode(std::istream& aStream) override;
  };
  
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;

}


#endif /* Attribute_hpp */
