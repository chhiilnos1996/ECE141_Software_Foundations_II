#ifndef InsertIntoTableView_h
#define InsertIntoTableView_h

#include "View.hpp"
#include <iomanip>

#include <vector>

namespace ECE141 {

  class InsertIntoTableView : public View {
  public:
    InsertIntoTableView(size_t aRowCount, bool anOk, double aDuration) : rowCount(aRowCount), ok(anOk), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "Query " << (ok ? "OK" : "FAILED") << ", " << rowCount << " rows affected (" << std::fixed << std::setprecision(2) << time << " sec)" << std::endl;

        return true;
    }
    
  protected:
    size_t rowCount;
    bool ok;
    double duration;
  };

}

#endif