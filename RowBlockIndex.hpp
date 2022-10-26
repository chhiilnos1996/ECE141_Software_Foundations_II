//
//  RowBlockIndex.h
//  Assignment7
//
//  Created by chhiilnos on 5/17/22.
//

#ifndef RowBlockIndex_hpp
#define RowBlockIndex_hpp


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

  class RowBlockIndex : public Storable {
  public:
      
      const std::string&      getName() const {return name;}
      void                    setName(const std::string& aName) {name = aName;}
      std::vector<uint32_t>   getRowBlockIndices() {return rowBlockIndices;}
      void                    addRowBlockIndices(const uint32_t anIndex) {rowBlockIndices.push_back(anIndex);}
      void                    removeIndex(const uint32_t anIndex) { rowBlockIndices.erase(std::remove(rowBlockIndices.begin(), rowBlockIndices.end(), anIndex), rowBlockIndices.end());}
      
      RowBlockIndex(const std::string aName=""): name(aName) {}
      ~RowBlockIndex() {}

      StatusResult encode(std::ostream& aStream) override {
        aStream << getName() << "\n";
        for (uint32_t theIndex : rowBlockIndices) {
          aStream << std::to_string(theIndex)  << "\n";
        }
        return StatusResult{noError};
      }
 
      StatusResult decode(std::istream& aStream) override {
        std::string theEntityName;
        getline(aStream, theEntityName);
        setName(theEntityName);

        std::string theIndexLine;
        while (getline(aStream, theIndexLine)) {
            rowBlockIndices.push_back(std::stoi(theIndexLine));
        }
          
        return StatusResult{noError};
      }
      
  protected:
    std::vector<uint32_t>  rowBlockIndices;
    std::string     name; //entity Name 
  };
  
}

#endif /* RowBlockIndex_h */
