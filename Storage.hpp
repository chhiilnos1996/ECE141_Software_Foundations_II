//
//  Storage.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <string>
#include <fstream>
#include <iostream>
#include <deque>
#include <stack>
#include <optional>
#include <functional>
#include "BlockIO.hpp"
#include "Errors.hpp"
//#include "Database.hpp"

namespace ECE141 {
      
  class Database;

  struct CreateDB {}; //tags for db-open modes...
  struct OpenDB {};

  const int32_t kNewBlock=-1;


  struct StorageInfo {
    
    StorageInfo(size_t aRefId, size_t theSize, int32_t aStartPos=kNewBlock, BlockType aType=BlockType::data_block)
      : type(aType), start(aStartPos), refId(aRefId), size(theSize) {}
    
    BlockType type;
    int32_t   start; //block#
    size_t    refId;
    size_t    size;
  };
  
  using BlockVisitor = std::function<bool(const Block&, uint32_t)>;

  using BlockList = std::deque<uint32_t>;


  // USE: Our storage manager class...
  class Storage : public BlockIO {
  public:
        
      Storage(std::string aName);
      ~Storage();
 
       StatusResult save(std::iostream &aStream, StorageInfo &anInfo);
       
       StatusResult load(std::iostream &aStream, uint32_t aStartBlockNum);

       bool         each(const BlockVisitor &aVisitor);
       bool eachInChain(uint32_t aPos, const BlockVisitor& aVisitor);

       StatusResult markBlockAsFree(uint32_t aPos);
       
       StatusResult writeStorable(Storable& aStorable, BlockType aType, uint32_t aPos);
       StatusResult readStorable(uint32_t aPos, Storable& aStorable);

       StatusResult close();
     protected:
         
       StatusResult releaseBlocks(uint32_t aPos,bool
                                  aInclusive=false);

       uint32_t     getFreeBlock(); //pos of next free (or new)...


    BlockList available;
    
    friend class Database;
  };

}


#endif /* Storage_hpp */
