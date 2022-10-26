//
//  BlockIO.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "BlockIO.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <map>
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Config.hpp"

namespace ECE141 {

  Block::Block(BlockType aType) : header(aType) {
    std::memset(payload, 0, kPayloadSize);
  }

  Block::Block(Storable& aStorable, BlockType aType) : header(aType) {
    std::stringstream theEntryStream;

    aStorable.encode(theEntryStream);

    std::memset((char*)payload, 0, kPayloadSize);
    theEntryStream.read((char*)payload, kPayloadSize);
  }

  Block::Block(const Block &aCopy) {
    *this=aCopy;
  }

  Block& Block::operator=(const Block &aCopy) {
    std::memcpy(payload, aCopy.payload, kPayloadSize);
    header=aCopy.header;
    return *this;
  }

  StatusResult Block::write(std::ostream &aStream) {
    aStream.write((char*)this, kBlockSize);
    aStream.flush();
    return StatusResult{noError};
  }

  StatusResult Block::read(std::istream& aStream) {
    aStream.read((char*)this, kBlockSize);
    return StatusResult{Errors::noError};
  }

  //---------------------------------------------------

  BlockIO::BlockIO(std::string aName) : dbName(aName), stream() {}

  StatusResult BlockIO::begin(std::ios::openmode anOpenMode) {
    if (stream) stream.close();

    stream.open(Config::getDBPath(dbName), anOpenMode);
    if (!stream) return StatusResult{readError};

    return StatusResult{noError};
  }

  StatusResult BlockIO::end() {
    stream.close();
    return StatusResult{noError};
  }

  // USE: write data a given block (after seek) ---------------------------------------
  StatusResult BlockIO::writeBlock(uint32_t aBlockNum, Block &aBlock) {
    size_t theLocation = aBlockNum * kBlockSize;
      
    /*stream.seekp(theLocation, std::ios::beg);
    char theZeros[kBlockSize] = {0};
    stream.write(theZeros, kBlockSize);
    stream.flush();*/

    stream.seekp(theLocation, std::ios::beg);
    StatusResult theResult = aBlock.write(stream);
    stream.flush();
    
    blockCache.put(aBlockNum, aBlock);
      
    return theResult;
  }

  // USE: read data a given block (after seek) ---------------------------------------
  StatusResult BlockIO::readBlock(uint32_t aBlockNum, Block &aBlock) {
    //std::cout << "Read: " << aBlockNum << std::endl;
    auto theCacheResult = blockCache.get(aBlockNum);
    if (theCacheResult != std::nullopt) {
      aBlock = *theCacheResult;
      return StatusResult{noError};
    }

    stream.seekg(0, std::ios::end);
    size_t theSize = stream.tellg();
      
    size_t theLocation = aBlockNum * kBlockSize;
    if (theLocation >= theSize) return StatusResult{readError};

    stream.seekg(theLocation, std::ios::beg);
    StatusResult theResult = aBlock.read(stream);
    /*
      std::cout <<  " --------------  aBlock.read  -------------- "  << std::endl;
      std::cout << aBlock.header.type << " -----  " << aBlock.payload << std::endl;
      std::cout <<  " --------------  end aBlock.read  --------------"  << std::endl << std::endl;*/
   
    blockCache.put(aBlockNum, aBlock);

    return theResult;
  }

  // USE: count blocks in file ---------------------------------------
  uint32_t BlockIO::getBlockCount()  {
    begin(std::ios::in);

    stream.seekg(0, std::ios::end);
    size_t theSize = stream.tellg();
    size_t theBlockCount = theSize / kBlockSize;

    end();
      
    return theBlockCount;
  }

}
