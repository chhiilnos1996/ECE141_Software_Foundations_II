#ifndef TableDescriptionView_h
#define TableDescriptionView_h

#include "View.hpp"
#include <iomanip>

#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class TableDescriptionView : public View {
  public:
      TableDescriptionView(std::string aDBName, const Entity& anEntity, double aDuration) : dbName(aDBName), entity(anEntity), duration(aDuration) {}
   
    virtual bool show(std::ostream& anOutput) {
        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;
        anOutput << "| Field     | Type         | Null | Key | Default | Extra                       |" << std::endl;
        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;

        for (Attribute theAttribute : entity.getAttributes()) {
            std::string theField = theAttribute.getName();
            anOutput << "| " << theField << std::setw(12 - theField.size());

            std::string theType = Helpers::dataTypeToString(theAttribute.getType());
            if (DataTypes::varchar_type == theAttribute.getType()) theType +=  "(" + std::to_string(theAttribute.getSize()) + ")";
            anOutput << "| " << theType << std::setw(15 - theType.size());

            std::string theNullable = (theAttribute.isNullable() ? "YES" : "NO");
            anOutput << "| " << theNullable << std::setw(7 - theNullable.size());

            std::string theKey = (theAttribute.isPrimaryKey() ? "YES" : "");
            anOutput << "| " << theKey << std::setw(4 - theKey.size());

            std::string theDefault = ""; // TODO: Implement default support
            anOutput << "| " << theDefault << std::setw(8 - theDefault.size());

            std::string theExtra = std::string{(theAttribute.isAutoIncrement() ? "auto_increment " : "")} + std::string{(theAttribute.isPrimaryKey() ? "primary key" : "")};
            anOutput << "| " << theExtra << std::setw(28 - theExtra.size());
            anOutput << "|" << std::endl;
        }

        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;

        size_t theRowAmount = entity.getAttributes().size();
        anOutput << theRowAmount << " " << (1 == theRowAmount ? "row" : "rows") << " in set (" << std::fixed << std::setprecision(2) << duration << " sec.)" << std::endl;;

        return true;
    }
    
  protected:
    std::string dbName;
    const Entity& entity;
    double duration;
  };

}

#endif
