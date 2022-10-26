#ifndef ShowIndexesView_h
#define ShowIndexesView_h

#include "View.hpp"
#include <iomanip>

#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class ShowIndexesView : public View {
  public:
      ShowIndexesView(const std::map<std::string, std::vector<std::string>>& anIndexes, double aDuration) : indexes(anIndexes), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "+-----------------+-----------------+" << std::endl;
        anOutput << "| table           | field(s)        |" << std::endl;
        anOutput << "+-----------------+-----------------+" << std::endl;

        for (auto theIndex : indexes) {
            std::string theTable = theIndex.first;
            anOutput << "| " << theTable << std::setw(18 - theTable.size()) << "| ";

            std::string theFields;
            for (std::string theField : theIndex.second) {
                theFields += theField + (theIndex.second.size() > 1 ? ", " : " ");
            }

            anOutput << theFields << std::setw(17 - theFields.size()) << "|" << std::endl;
        }

        anOutput << "+-----------------+-----------------+" << std::endl;

        size_t theRowAmount = indexes.size();
        anOutput << theRowAmount << " rows in set (" << std::fixed << std::setprecision(2) << duration << " sec.)" << std::endl;;

        return true;
    }
    
  protected:
    const std::map<std::string, std::vector<std::string>> indexes;
    double duration;
  };

}

#endif
