#ifndef DropTableView_h
#define DropTableView_h

#include "View.hpp"
#include <iomanip>

#include <vector>
#include "Entity.hpp"

namespace ECE141 {

  class DropTableView : public View {
  public:
    DropTableView(bool anOk, double aDuration) : ok(anOk), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "Query " << (ok ? "OK" : "FAILED") << ", " << (ok ? "1 row" : "0 rows") << " affected (" << std::setprecision(2) << duration << " sec)" << std::endl;

        return true;
    }
    
  protected:
    bool ok;
    double duration;
  };
}

#endif