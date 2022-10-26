#ifndef EntityView_h
#define EntityView_h

#include "View.hpp"
#include <iomanip>

#include <vector>
#include "Entity.hpp"

namespace ECE141 {

  class EntityView : public View {
  public:
    EntityView(std::string aDBName, std::vector<std::string> anEntityList, double aDuration) : dbName(aDBName), entities(anEntityList), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "+----------------------+" << std::endl;
        anOutput << "| " << dbName << std::setw(21 - dbName.size()) << "|" << std::endl;
        anOutput << "+----------------------+" << std::endl;

        for (std::string theName : entities) {
            anOutput << "| " << theName << std::setw(21 - theName.size()) << "|" << std::endl;
        }

        anOutput << "+----------------------+" << std::endl;
        anOutput << entities.size() << " rows in set (" << std::fixed << std::setprecision(2) << duration << " sec.)" << std::endl;

        return true;
    }
    
  protected:
    std::string dbName;
    std::vector<std::string> entities;
    double duration;
  };

}

#endif
