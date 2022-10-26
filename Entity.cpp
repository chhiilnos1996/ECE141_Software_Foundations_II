//
//  Entity.cpp
//  PA3
//
//  Created by rick gessner on 3/2/22.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "Helpers.hpp"
#include "Entity.hpp"
#include <set>

namespace ECE141 {

 //STUDENT: Implement this class...

  Entity::Entity(const std::string aName) :
    name(aName),  autoincr(0) {}

  Entity::Entity(const Entity &aCopy) {
    name=aCopy.name;
    autoincr=aCopy.autoincr;
  }

  Entity::~Entity() {
    //std::cout << "~Entity()\n";
  }
 
  Entity& Entity::addAttribute(const Attribute &anAttribute) {
    if(!getAttribute(anAttribute.getName())) {
      attributes.push_back(anAttribute);
    }

    return *this;
  }

  Entity& Entity::remoteAttribute(const std::string& aName) {
    remove_if(attributes.begin(), attributes.end(), [&](Attribute anAttribute) {
      return (anAttribute.getName() == aName);
    });

    return *this;
  }

  AttributeOpt Entity::getAttribute(const std::string& aName) const {
    for (Attribute theAttribute : attributes) {
      if (theAttribute.getName() == aName) return theAttribute;
    }

    return std::nullopt;
  }
  
  // USE: ask the entity for name of primary key (may not have one...)
  const AttributeOpt Entity::getPrimaryKey() const {   
    for (Attribute theAttribute : attributes) {
      if (theAttribute.isPrimaryKey()) return theAttribute;
    }

    return std::nullopt;
  }

  StatusResult Entity::validate(Row& aRow) const {
    KeyValues& theRowData = aRow.getData();

    StatusResult theResult;

    for (std::pair<const std::string, Value> thePair : theRowData) {
      AttributeOpt theAttribute = getAttribute(thePair.first);
      if (!theAttribute) return StatusResult{unknownAttribute};

      if (!std::holds_alternative<std::string>(thePair.second)) continue;
      Value theValue = (*theAttribute).toValue(std::get<std::string>(thePair.second));
      theRowData[thePair.first] = theValue;
    }
    
    return StatusResult{noError};
  }
 
  StatusResult Entity::encode(std::ostream& aStream) {
    aStream << getName() << "\n";
    aStream << std::to_string(rowBlockIndex) << "\n";
    aStream << std::to_string(autoincr) << "\n";

    for (Attribute theAttribute : attributes) {
      StatusResult theResult = theAttribute.encode(aStream);
      aStream << "\n";
      if (!theResult) return theResult;
    }

    return StatusResult{noError};
  }

  StatusResult Entity::decode(std::istream& aStream) {
    std::string theEntityName;
    getline(aStream, theEntityName);
    setName(theEntityName);

    std::string theRowBlockIndexString;
    getline(aStream, theRowBlockIndexString);
    rowBlockIndex = std::stoi(theRowBlockIndexString);

    std::string theAutoIncStr;
    getline(aStream, theAutoIncStr);
    autoincr = std::stoi(theAutoIncStr);

    std::string theEntityLine;
    while (getline(aStream, theEntityLine)) {
      std::stringstream theAttributeStream{theEntityLine};

      Attribute theAttribute;

      StatusResult theResult = theAttribute.decode(theAttributeStream);
      if (!theResult) return theResult;

      addAttribute(theAttribute);
    }

    return StatusResult{noError};
  }
}
