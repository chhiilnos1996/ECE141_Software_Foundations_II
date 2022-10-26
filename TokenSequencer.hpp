#ifndef TokenSequencer_h
#define TokenSequencer_h

#include "Tokenizer.hpp"
#include "keywords.hpp"
#include <initializer_list>

namespace ECE141 {


  class TokenSequencer {
  public:
    TokenSequencer(Tokenizer &aTokenizer)
      : tokenizer(aTokenizer), state{true} {}
    
    struct SeqState {
      bool state;
      TokenSequencer &seq;
    };
    
    TokenSequencer& nextIs(const KWList &aList) {
      if(state) {
        std::vector<Keywords> theList{aList};
        int thePos{0};
        for(auto theKW: theList) {
          Token &theToken=tokenizer.peek(thePos++);
          if(theKW!=theToken.keyword) {
            state=false;
            break;
          }
        }
        if(state) {
          return skip(theList.size());
        }
      }
      return *this;
    }

    TokenSequencer& hasId(std::string& aName) {
        return *this;
    }

    TokenSequencer& hasOp(Operators anOperator) {
      Token& theToken = tokenizer.current();
      if (TokenType::operators != theToken.type || anOperator != theToken.op) state = false;
      return *this;
    }

    TokenSequencer& hasNumber(int aNumber) {
        return *this;
    }
    
    TokenSequencer& getId(std::string& aName) {
      Token& theToken = tokenizer.current();
      if (TokenType::identifier != theToken.type) state = false;
      aName = theToken.data;
      return *this;
    }

    TokenSequencer& getOp(Operators& anOperator) {
      Token& theToken = tokenizer.current();
      if (TokenType::operators != theToken.type) state = false;
      anOperator = theToken.op;
      return *this;
    }
    
    TokenSequencer& getNumber(int &aValue) {
      Token &theToken=tokenizer.current();
      //if (TokenType::number != theToken.type) state = false;
      aValue=std::stoi(theToken.data);
      return *this;
    }
    
    TokenSequencer& skipPast(char aChar) {
      if(state) {
        while(tokenizer.next()) {
          auto theToken=tokenizer.current();
          if(theToken.type==TokenType::punctuation) {
            if(theToken.data[0]==aChar) {
              tokenizer.next();//skip it...
              return *this;
            }
          }
        }
        state=false;
      }
      return *this;
    }

    TokenSequencer& skip(size_t aCount) {
      if(state) {
        tokenizer.next((int)aCount); //actually skip...
      }
      return *this;
    }

    operator bool() {return state;}

    TokenSequencer& clear() {
      state=true;
      return *this;
    }
    
  protected:
    Tokenizer &tokenizer;
    bool      state;
  };

}

#endif
