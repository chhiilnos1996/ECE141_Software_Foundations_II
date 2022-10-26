#ifndef RowChangedView_h
#define RowChangedView_h

#include "View.hpp"
#include <iomanip>

#include <vector>
#include "Entity.hpp"

namespace ECE141 {

  class RowChangedView : public View {
  public:
    RowChangedView(bool anOk, int anAffectedAmount, double aDuration) : ok(anOk), affected(anAffectedAmount), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "Query " << (ok ? "OK" : "FAILED") << ", " << affected << " rows affected (" << std::setprecision(2) << duration << " sec)" << std::endl;

        return true;
    }
    
  protected:
    bool ok;
    int affected;
    double duration;
  };
}

#endif