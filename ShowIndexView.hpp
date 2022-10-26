#ifndef ShowIndexView_h
#define ShowIndexView_h

#include "View.hpp"
#include <iomanip>

#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class ShowIndexView : public View {
  public:
      ShowIndexView(const std::vector<std::pair<std::string, uint32_t>>& anIndexes, double aDuration) : indexes(anIndexes), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "+-----------------+-----------------+" << std::endl;
        anOutput << "| key             | block#          |" << std::endl;
        anOutput << "+-----------------+-----------------+" << std::endl;

        for (auto theIndex : indexes) {
            std::string theKey = theIndex.first;
            std::string theBlock = std::to_string(theIndex.second);
            anOutput << "| " << theKey << std::setw(18 - theKey.size()) << "| "<< theBlock << std::setw(17 - theBlock.size()) << "|" << std::endl;
        }

        anOutput << "+-----------------+-----------------+" << std::endl;

        size_t theRowAmount = indexes.size();
        anOutput << theRowAmount << " rows in set (" << std::fixed << std::setprecision(2) << duration << " sec.)" << std::endl;;

        return true;
    }
    
  protected:
    const std::vector<std::pair<std::string, uint32_t>>& indexes;
    double duration;
  };

}

#endif