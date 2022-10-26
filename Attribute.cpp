//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  Attribute::Attribute(DataTypes aType)
    : type(aType),size(0),autoIncrement(0),primary(0),nullable(1) {}
 
  Attribute::Attribute(std::string aName, DataTypes aType, uint32_t aSize)  {
    name=aName;
    type=aType;
    size=aSize;
    autoIncrement=0;
    primary=0;
    nullable=1;
  }
 
  Attribute::Attribute(const Attribute &aCopy)  {
    name=aCopy.name;
    type=aCopy.type;
    size=aCopy.size;
    autoIncrement=aCopy.autoIncrement;
    primary=aCopy.primary;
    nullable=aCopy.nullable;
  }
 
  Attribute::~Attribute()  {
  }
 
  Attribute& Attribute::setName(std::string &aName)  {
    name=aName; return *this;
  }
 
  Attribute& Attribute::setDataType(DataTypes aType) {
    type=aType; return *this;
  }

  Attribute& Attribute::setSize(int aSize) {
    size=aSize; return *this;
  }

  Attribute& Attribute::setAutoIncrement(bool anAuto) {
    autoIncrement=anAuto; return *this;
  }

  Attribute& Attribute::setPrimaryKey(bool aPrimary) {
    primary=aPrimary; return *this;
  }

  Attribute& Attribute::setNullable(bool aNullable) {
    nullable=aNullable; return *this;
  }

  bool Attribute::isValid() {
      // std::string name, DataTypes type, int size, bool autoIncrement, bool primary, bool nullable
      // no_type='N',  bool_type='B', datetime_type='D', float_type='F', int_type='I', varchar_type='V',
      switch(type) {
          case DataTypes::no_type :
              return false;
              
          case DataTypes::bool_type :
              //if (bool_type)
              return true;
              
          case DataTypes::datetime_type :
              return true;
              
          case DataTypes::float_type :
              return true;
              
          case DataTypes::int_type :
              return true;
              
          case DataTypes::varchar_type :
              return true;
              
      }
    return true;
  }
   
  Value Attribute::toValue(const std::string &aValue) const {
    Value theValue;
    Helpers::stringValueToDataType(getType(), aValue, theValue);
    return theValue;
  }

  StatusResult Attribute::encode(std::ostream& aStream) {
      aStream << getName();
      aStream << "\t" << Helpers::dataTypeToString(getType());
      aStream << "\t" << std::to_string(getSize());
      aStream << "\t" << (isNullable() ? "Y" : "N");
      aStream << "\t" << (isAutoIncrement() ? "Y" : "N");
      aStream << "\t" << (isPrimaryKey() ? "Y" : "N");

      return StatusResult{noError};
  }

  StatusResult Attribute::decode(std::istream& aStream) {
      std::string theAttributeName;
      getline(aStream, theAttributeName, '\t');
      setName(theAttributeName);

      std::string theAttributeType;
      getline(aStream, theAttributeType, '\t');
      if (!gDictionary.count(theAttributeType)
      || !Helpers::isDatatype(gDictionary[theAttributeType])) return StatusResult{readError};
      setDataType(Helpers::getTypeForKeyword(gDictionary[theAttributeType]));

      std::string theAttributeSize;
      getline(aStream, theAttributeSize, '\t');
      setSize(std::atoi(theAttributeSize.c_str()));

      std::string theAttributeNullable;
      getline(aStream, theAttributeNullable, '\t');
      setNullable((theAttributeNullable == "Y" ? true : false));

      std::string theAttributeAutoIncrement;
      getline(aStream, theAttributeAutoIncrement, '\t');
      setAutoIncrement((theAttributeAutoIncrement == "Y" ? true : false));

      std::string theAttributePrimaryKey;
      getline(aStream, theAttributePrimaryKey, '\t');
      setPrimaryKey((theAttributePrimaryKey == "Y" ? true : false));

      return StatusResult{noError};
  }
}
