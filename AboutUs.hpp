//
//  AboutUs.hpp
//  About your team members...
//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <stdio.h>
#include <string>
#include <optional>

#include <vector>

namespace ECE141 {
  
  using StringOpt = std::optional<std::string>;
  
  class AboutUs {
  protected:
	  std::vector<std::pair<std::string, std::string>> members = {
      std::make_pair("Mitchell Nuckols", "Mitchell-Nuckols"),
      std::make_pair("Chi-Hsin Lo", "chhiilnos1996")
    };

  public:
    AboutUs()=default;
    ~AboutUs()=default;
    
    size_t getTeamSize() {return members.size();}
    
    StringOpt getName(size_t anIndex) const {
      if (anIndex < members.size()) return members[anIndex].first;
      return std::nullopt;
    }
    StringOpt getGithubUsername(size_t anIndex) const {
      if (anIndex < members.size()) return members[anIndex].second;
      return std::nullopt;
    }

  };

}

#endif /* about_me */
