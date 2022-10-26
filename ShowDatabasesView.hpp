//
//  ShowDatabasesView.hpp
//  Assignment2
//
//  Created by chhiilnos on 4/10/22.
//

#ifndef ShowDatabasesView_h
#define ShowDatabasesView_h

#include <iostream>
#include "View.hpp"
#include "Errors.hpp"

namespace ECE141 {

  class ShowDatabasesView : public View {
  protected:
    std::string dbName;
    StatusResult result;
    double time;

  public:
    ShowDatabasesView(std::string aDBName="", StatusResult aResult=StatusResult{Errors::noError}, double aTime=0.0) : dbName(aDBName), result(aResult), time(aTime) {}

    bool show(std::ostream &aStream) override {
        if (Errors::noError != result.error) {
            aStream << "Query failed with error code: " << result.error << "\n";
            return false;
        }
        
        return true;
    }
  };
}

#endif /* ShowDatabasesView_h */
