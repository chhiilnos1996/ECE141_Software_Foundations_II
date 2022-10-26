#ifndef DumpDatabaseView_h
#define DumpDatabaseView_h

#include "View.hpp"
#include <iomanip>

#include <vector>
#include "Entity.hpp"

namespace ECE141 {

  class DumpDatabaseView : public View {
  public:
    DumpDatabaseView(std::vector<Block> aBlockList, double aDuration) : blockList(aBlockList), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
      anOutput << "+----------------+--------+---------------+" << std::endl;
      anOutput << "| Type           | Id     | Extra         |" << std::endl;
      anOutput << "+----------------+--------+---------------+" << std::endl;
      int theCount = 0;
      for (Block aBlock : blockList) {
        anOutput << "| " << aBlock.header.type << std::setw(16) << "| " << aBlock.header.pos << "      " << "|               |" << std::endl;
        theCount++;
      }
      anOutput << "-----------------+--------+---------------+" << std::endl;
      anOutput << theCount << " rows in set (" << std::fixed << std::setprecision(2) << Config::getTimer().elapsed() << " secs)" << std::endl;

      return true;
    }
    
  protected:
    std::vector<Block> blockList;
    double duration;
  };
}

#endif