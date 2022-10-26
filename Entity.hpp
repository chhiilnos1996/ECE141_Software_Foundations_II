//
//  Entity.hpp
//  Assignment3
//
//  Created by rick gessner on 3/18/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>

#include "Storable.hpp"
#include "Attribute.hpp"
#include "Errors.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;

  //------------------------------------------------

  class Entity : public Storable {
  public:
    Entity(const std::string aName="");
    Entity(const Entity &aCopy);
    ~Entity();
    
    const std::string&      getName() const {return name;}
    void                    setName(const std::string& aName) {name = aName;}
    uint32_t                getRowBlockIndex() {return rowBlockIndex;}
    void                    setRowBlockIndex(const uint32_t& anIndex) {rowBlockIndex = anIndex;}
        
    const AttributeList&    getAttributes() const {return attributes;}
    Entity&                 addAttribute(const Attribute &anAttribute);
    Entity&                 remoteAttribute(const std::string &aName);
    AttributeOpt            getAttribute(const std::string &aName) const;
    const AttributeOpt      getPrimaryKey() const;
    StatusResult            validate(Row& aRow) const ;

    const int getIncrement() const { return autoincr; }
    const int increment() { return ++autoincr; }

    StatusResult encode(std::ostream& aStream) override;
    StatusResult decode(std::istream& aStream) override;
           
  protected:
    
    AttributeList   attributes;
    std::string     name;
    int        autoincr;  //auto_increment
    uint32_t        rowBlockIndex;
  };
  
}
#endif /* Entity_hpp */
