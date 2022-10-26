//
//  DropDatabaseView.h
//  Assignment2
//
//  Created by chhiilnos on 4/10/22.
//

#ifndef DropDatabaseView_h
#define DropDatabaseView_h

#include <iostream>
#include "View.hpp"
#include "Errors.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class DropDatabaseView : public View {
  protected:
    std::string dbName;
    StatusResult result;
    double time;

  public:
    DropDatabaseView(std::string aDBName="", StatusResult aResult=StatusResult{Errors::noError}, double aTime=0.0) : dbName(aDBName), result(aResult), time(aTime) {}

    bool show(std::ostream &aStream) override {
        if (Errors::noError != result.error) {
            aStream << "Query failed with error code: " << result.error << "\n";
            return false;
        }

        aStream << Helpers::QueryOk(0, time);
        return true;
    }
  };
}

#endif /* DropDatabaseView_h */
