//
//  BlockIO.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <functional>
#include "Errors.hpp"
//#include "Database.hpp"
#include "Entity.hpp"
#include "LRUCache.hpp"

namespace ECE141 {

  enum class BlockType {
    data_block='D',
    free_block='F',
    entity_block='E',
    row_block='R',
    unknown_block='U',
    meta_block='M',
  };

    const std::map<std::string, BlockType> kBlockTypeMap = {
      {"D", BlockType::data_block},
      {"F", BlockType::free_block},
      {"E", BlockType::entity_block},
      {"R", BlockType::row_block}, 
      {"U", BlockType::unknown_block},
      {"M", BlockType::meta_block}
    };
  //a small header that describes the block...
struct BlockHeader {
  
   BlockHeader(BlockType aType=BlockType::data_block, size_t aCount=1, int32_t aPos=0, size_t aNext=0)
     : type(static_cast<char>(aType)), count(aCount), pos(aPos), next(aNext) {}

   BlockHeader(const BlockHeader &aCopy) {
     *this=aCopy;
   }
       
   void empty() {
     type=static_cast<char>(BlockType::free_block);
   }
   
   BlockHeader& operator=(const BlockHeader &aCopy) {
     type = aCopy.type;
     count = aCopy.count;
     pos = aCopy.pos;
     next = aCopy.next;
     return *this;
   }

   StatusResult write(std::ostream& aStream) {
     aStream << type << count << pos << next;

     return StatusResult{noError};
   }

   StatusResult read(std::istream& aStream) {
     aStream.read((char*)&type, sizeof(type));
     aStream.read((char*)&count, sizeof(count));
     aStream.read((char*)&pos, sizeof(pos));
     aStream.read((char*)&next, sizeof(next));
       
     return StatusResult{noError};
   }
  
   char      type;     //char version of block type
   size_t count;
   int32_t pos;
   size_t next;
 } __attribute__((packed));

  const size_t kBlockSize = 1024;
  const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);
  
  //block .................
  class Block {
  public:
    Block(BlockType aType=BlockType::data_block);
    Block(Storable& aStorable, BlockType aType=BlockType::data_block);
    Block(Entity& anEntity) : Block(anEntity, BlockType::entity_block) {}
    Block(const Block &aCopy);
    
    Block& operator=(const Block &aCopy);
    StatusResult write(std::ostream &aStream);
    StatusResult read(std::istream &aStream);
        
    BlockHeader   header;
    char          payload[kPayloadSize];
  } __attribute__((packed));

  //------------------------------

  using BlockCache = ConfigLRUCache<CacheType::block, uint32_t, Block>;

  class BlockIO {
  public:
    
    BlockIO(std::string aName);
    
    uint32_t              getBlockCount();
    
    virtual StatusResult  readBlock(uint32_t aBlockNum, Block &aBlock);
    virtual StatusResult  writeBlock(uint32_t aBlockNum, Block &aBlock);

    virtual StatusResult begin(std::ios::openmode anOpenMode);
    virtual StatusResult end();
    
  protected:
    std::fstream stream;
    std::string dbName;
    BlockCache blockCache;
  };

}


#endif /* BlockIO_hpp */
