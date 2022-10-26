
//
//  TabularView.hpp
//
//  Created by rick gessner on 4/1/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef TabularView_h
#define TabularView_h

#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  //STUDENT: Feel free to use this, or build your own...


  // USE: general tabular view (with columns)
  class TabularView : public View {
  public:
    
    TabularView(const Entity& anEntity, const StringList anAttrList, const RowCollection& aRowCollection, double anElapsedTime) : entity(anEntity), attributes(anAttrList), rows(aRowCollection), elapsed(anElapsedTime) {}
              
    // USE: create header for tablular view...
    TabularView& showHeader(std::ostream& aStream) {
        if (attributes.size()) {
            //print header top
            for(int i=0;i<attributes.size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
            
            // print attributes
            aStream << "|";
            for (auto theAttr : attributes) { aStream<<theAttr<<std::setw(17-theAttr.size())<< "|" ;}
            aStream << std::endl;
            
            //print header bottom
            for(int i=0;i<attributes.size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
        }
        else {
            //print header top
            for(int i=0;i<entity.getAttributes().size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
            
            // print attributes
            aStream << "|";
            for (auto theAttr : entity.getAttributes()) { aStream<<theAttr.getName()<<std::setw(17-theAttr.getName().size())<< "|" ;}
            aStream << std::endl;
            
            //print header bottom
            for(int i=0;i<entity.getAttributes().size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
        }
        
      return *this;
    }
    
    // USE: create header for tablular view...
    TabularView& showFooter(std::ostream& aStream) {
        if (attributes.size()) {
            for(int i=0;i<attributes.size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
        }
        else {
            for(int i=0;i<entity.getAttributes().size();i++){ aStream << "+----------------";}
            aStream << "+"<<std::endl;
        }
        return *this;
    }
    
    // USE: this function shows all the fields in a row...
    TabularView& showRow(Row &aRow, std::ostream& aStream) {
      if (attributes.size()) {
          aStream << "|";
          for (auto theAttr : attributes) {
              std::string theStringData = Helpers::valueToString(aRow.getValue(theAttr));
              aStream<<theStringData<<std::setw(17-theStringData.size())<< "|" ;
          }
          aStream <<std::endl;
      } else {
          aStream << "|";
          for (auto theAttr : entity.getAttributes()) {
              std::string theStringData = Helpers::valueToString(aRow.getValue(theAttr.getName()));
              aStream<<theStringData<<std::setw(17-theStringData.size())<< "|" ;
          }
          aStream <<std::endl;
      }
      return *this;
    }
        
    // USE: this is the main show() for the view, where it presents all the rows...
    bool show(std::ostream& aStream) override {
      showHeader(aStream);
      for(auto &theRow : rows) {
        showRow(*theRow, aStream);
      }
      showFooter(aStream);
      aStream << rows.size() << /*((rows.size() > 1) ? " rows" : " row")*/ " rows" << " in set ("
        << std::fixed << elapsed << " sec.)\n";
      return true;
    }
    
  protected:
    double elapsed;
    const Entity              &entity;
    const RowCollection       &rows;
    const StringList attributes;
  };

}

#endif /* TabularView_h */

