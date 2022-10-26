//
//  UseDatabaseView.hpp
//  Assignment2
//
//  Created by chhiilnos on 4/10/22.
//

#ifndef UseDatabaseView_h
#define UseDatabaseView_h

#include <iostream>
#include "View.hpp"
#include "Errors.hpp"

namespace ECE141 {

  class UseDatabaseView : public View {
  protected:
    std::string dbName;
    StatusResult result;
    double time;

  public:
    UseDatabaseView(std::string aDBName="", StatusResult aResult=StatusResult{Errors::noError}, double aTime=0.0) : dbName(aDBName), result(aResult), time(aTime) {}

    bool show(std::ostream &aStream) override {
        if (Errors::noError != result.error) {
            aStream << "Query failed with error code: " << result.error << "\n";
            return false;
        }

        aStream << "Database Changed\n";
        return true;
    }
  };
}

#endif /* UseDatabaseView_h */
