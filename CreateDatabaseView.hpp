#ifndef CreateDatabaseView_h
#define CreateDatabaseView_h

#include <iostream>
#include "View.hpp"
#include "Errors.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class CreateDatabaseView : public View {
  protected:
    std::string dbName;
    StatusResult result;
    double time;

  public:
    CreateDatabaseView(std::string aDBName="", StatusResult aResult=StatusResult{Errors::noError}, double aTime=0.0) : dbName(aDBName), result(aResult), time(aTime) {}

    bool show(std::ostream &aStream) override {
        if (Errors::noError != result.error) {
            aStream << "Query failed with error code: " << result.error << "\n";
            return false;
        }

        aStream << Helpers::QueryOk(1, time);
        return true;
    }
  };
}

#endif
