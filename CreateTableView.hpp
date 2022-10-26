#ifndef CreateTableView_h
#define CreateTableView_h

#include "View.hpp"
#include <iomanip>

#include <vector>
#include "Entity.hpp"

namespace ECE141 {

  class CreateTableView : public View {
  public:
    CreateTableView(bool anOk, double aDuration) : ok(anOk), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "Query " << (ok ? "OK" : "FAILED") << ", " << (ok ? "1 row" : "0 rows") << " affected (" << std::fixed << std::setprecision(2) << time << " sec)" << std::endl;

        return true;
    }
    
  protected:
    bool ok;
    double duration;
  };

}

#endif