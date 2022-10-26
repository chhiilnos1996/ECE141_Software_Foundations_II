//
//  Row.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//


#include <stdio.h>
#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Row.hpp"
#include "Helpers.hpp"

namespace ECE141 {
  Row::Row(std::string aName, StringMap aMap) : entityName(aName) {
      // KeyValues = std::map<const std::string, Value>;
      for ( const auto &pair : aMap ){
          data[pair.first] = pair.second;
      }
  }
  Row::Row(uint32_t entityId) {}
  Row::Row(const Row &aRow) {*this=aRow;}

  Row::Row(bool aIsNull, Row &aRow) : data(aRow.getData()) {
      //std::cout<<"tmp.size() = "<<tmp.size()<<std::endl;
      if(aIsNull){
          Value theNullValue = "NULL";
          for (auto it = data.begin(); it != data.end(); it++){
              it->second = theNullValue;
          }
      }
  }
  Row::Row(std::string& aName, Row &aRow) {
      KeyValues tmp = aRow.getData();
      entityName = aName;
      for (auto it = tmp.begin(); it != tmp.end(); it++){
        std::string key = it->first;
        data.emplace(aName+"."+key, tmp[key]);
      }
  }

 Row::Row(Row &aRow1, Row &aRow2) : data(aRow1.getData()){
      KeyValues theData2 = aRow2.getData();
      data.merge(theData2);
  }

  Row::~Row() {}

  Row& Row::operator=(const Row &aRow) {return *this;}
  bool Row::operator==(Row &aCopy) const {return false;}

  //STUDENT: What other methods do you require?
  bool Row::emplace(const std::string &aKey, const Value &aValue) {
      if(data.end()==data.find(aKey)) return false;
      data.emplace(aKey,aValue);
      return true;
  }
  Row& Row::set(const std::string &aKey,const Value &aValue) {
      data[aKey] = aValue;
      return *this;
  }
  StatusResult Row::encode(std::ostream& aStream){
      //KeyValues = std::map<const std::string, Value>;
      aStream << getEntityName() << "\n";
      for (auto it = data.begin(); it != data.end(); it++) {
        //key : valuetype, value
          aStream << it->first  << ":" ;
          // bool, int, double (float), string (varchar, date-time)
          if( std::holds_alternative<bool>(it->second) ){
              //bool 'B'
              aStream << "B," ;
              aStream << std::get<bool>(it->second) << "\t";
          }
          else if( std::holds_alternative<int>(it->second) ){
              //int 'I'
              aStream << "I," ;
              aStream << std::get<int>(it->second) << "\t";
          }
          else if( std::holds_alternative<double>(it->second) ){
              //double 'F'
              aStream << "F," ;
              aStream << std::get<double>(it->second) << "\t";
          }
          else if( std::holds_alternative<std::string>(it->second) ){
              //varchar 'V'
              aStream << "V," ;
              aStream << std::get<std::string>(it->second) << "\t";
          }
          else if ( std::holds_alternative<std::string>(it->second) ){
              //date-time 'D'
              aStream << "D," ;
              aStream << std::get<std::string>(it->second) << "\t";
          }
          else return StatusResult{unknownType};
          
      }
      return StatusResult{noError};
  }
  StatusResult Row::decode(std::istream& aStream){
      std::string theEntityName;
      getline(aStream, theEntityName);
      setEntityName(theEntityName);
      StatusResult theResult{noError};

      std::string theKVPair;
      while (getline(aStream, theKVPair, '\t')) {
          std::stringstream theKVPairStream{theKVPair};
          std::string theKey;
          getline(theKVPairStream, theKey, ':');
          std::string theValueType;
          getline(theKVPairStream, theValueType, ',');
          std::string theValueString;
          theKVPairStream >> theValueString;
          Value theValue;
          if (theValueType=="B"){
              theResult = Helpers::stringValueToDataType(DataTypes::bool_type, theValueString, theValue);
          }
          else if (theValueType=="I"){
              theResult = Helpers::stringValueToDataType(DataTypes::int_type, theValueString, theValue);
          }
          else if (theValueType=="F"){
              theResult = Helpers::stringValueToDataType(DataTypes::float_type, theValueString, theValue);
          }
          else if (theValueType=="V"){
              theResult = Helpers::stringValueToDataType(DataTypes::varchar_type, theValueString, theValue);
          }
          else if (theValueType=="D"){
              theResult = Helpers::stringValueToDataType(DataTypes::datetime_type, theValueString, theValue);
          }
          
          if (theResult == StatusResult{noError}) {
              set(theKey, theValue);
          }
          else return theResult;
      }

      return theResult;
  }
    
}

