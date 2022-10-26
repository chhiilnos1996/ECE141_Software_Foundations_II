//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include <map>
#include "Storable.hpp"
#include "Storage.hpp"
#include "Database.hpp"
#include "Config.hpp"
//#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  // USE: ctor ---------------------------------------

   Storage::Storage(std::string aName) : BlockIO(aName) {
    available = std::deque<uint32_t>();
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
  }


  bool Storage::each(const BlockVisitor &aVisitor) {
    size_t theCount = getBlockCount();

    if (!begin(std::ios::in)) return false;

    Block theBlock;
    for(uint32_t i=0; i < theCount; i++) {
      if(readBlock(i, theBlock)) {
        if(!aVisitor(theBlock,i)) break;
      }
    }

    if (!end()) return false;

    return true;
  }

  bool Storage::eachInChain(uint32_t aPos, const BlockVisitor &aVisitor) {
    if (!begin(std::ios::in)) return false;

    Block theBlock;
    for(uint32_t i = aPos; i >= 0; i = theBlock.header.next) {
      if(readBlock(i, theBlock)) {
        if(!aVisitor(theBlock,i)) break;
      }
    }

    if (!end()) return false;
    return true;
  }


  StatusResult Storage::writeStorable(Storable& aStorable, BlockType aType, uint32_t aPos) {
    StatusResult theResult = begin(std::ios::in | std::ios::out);
    if (!theResult) return theResult;

    Block theStorableBlock(aStorable, aType);
    theStorableBlock.header.next = -1;
    theStorableBlock.header.count = 1;
    theStorableBlock.header.pos = aPos;

    theResult = writeBlock(aPos, theStorableBlock);
    if (!theResult) return theResult;

    theResult = end();
    if (!theResult) return theResult;

    return StatusResult{noError};
  }

  StatusResult Storage::readStorable(uint32_t aPos, Storable& aStorable) {
    StatusResult theResult = begin(std::ios::in);
    if (!theResult) return theResult;

    Block theStorableBlock;
    theResult = readBlock(aPos, theStorableBlock);
    if (!theResult) return theResult;

    std::stringstream theStorableStream{theStorableBlock.payload};

    theResult = aStorable.decode(theStorableStream);
    if (!theResult) return theResult;

    theResult = end();
    if (!theResult) return theResult;

    return StatusResult{noError};
  }

  uint32_t Storage::getFreeBlock() {
    if (available.size()) {
      uint32_t theResult = available.front();
      available.pop_front();
      return theResult;
    }

    return getBlockCount();
  }

  StatusResult Storage::markBlockAsFree(uint32_t aPos) {
    StatusResult theResult = begin(std::ios::in | std::ios::out);
    if (!theResult) return theResult;

    Block theBlock{BlockType::free_block};
    std::memset(&theBlock.header,0,sizeof(theBlock.header));
    available.push_front(aPos);

    theResult = writeBlock(aPos,theBlock);
    if (!theResult) return theResult;

    theResult = end();
    if (!theResult) return theResult;

    return StatusResult{noError};
  }

  // USE: for use with storable API...
  StatusResult Storage::releaseBlocks(uint32_t aPos, bool aInclusive) {
    StatusResult theResult = begin(std::ios::in | std::ios::out);
    if (!theResult) return theResult;

    Block theBlock;
    while((theResult=readBlock(aPos,theBlock))) {
      if(aInclusive) {
        markBlockAsFree(aPos);
      }
      aPos=theBlock.header.next;
      aInclusive=true;
      if(!aPos) break;
    }

    theResult = end();
    if (!theResult) return theResult;

    return theResult;
  }

  StatusResult Storage::save(std::iostream& aStream, StorageInfo& anInfo) {
      return StatusResult(noError);
  }
  
  StatusResult Storage::load(std::iostream&aStream, uint32_t aStartBlockNum) {
      return StatusResult(noError);
  }
}
