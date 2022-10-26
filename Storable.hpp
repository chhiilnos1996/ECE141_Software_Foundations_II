#ifndef Storable_hpp
#define Storable_hpp

#include <sstream>
#include "Errors.hpp"

namespace ECE141 {
  class Storable {
  public:
    virtual StatusResult  encode(std::ostream &anOutput)=0;
    virtual StatusResult  decode(std::istream &anInput)=0;
  };
}

#endif