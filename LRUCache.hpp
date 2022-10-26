#ifndef LRUCache_h
#define LRUCache_h

#include <list>
#include <deque>
#include <iostream>
#include <unordered_set>
#include "Storable.hpp"
#include "Config.hpp"

namespace ECE141{

  template<typename KeyT, typename ValueT>
  class LRUCache {
    using PairT = std::pair<KeyT, ValueT>;

  public:
    //OCF 
    //LRUCache() : LRUCache(CacheT) {}
    LRUCache(size_t aMaxSize) : maxsize(aMaxSize) {}
    ~LRUCache() {}
    void put(const KeyT& aKey, const ValueT& aValue) {
        //if (cacheKeys.find(aKey) != cacheKeys.end()) return;
        /*
        std::cout <<  " --------------  put  -------------- "  << std::endl;
        std::cout << aValue.header.type << " ----- " << aValue.payload << std::endl;
        std::cout <<  " --------------  end put  --------------"  << std::endl << std::endl;*/
        
        cache.push_front(std::make_pair(aKey, aValue));
        cacheKeys.insert(aKey);
        if (size() <= maxsize) { return; }
        
        cacheKeys.erase(cache.back().first);
        cache.pop_back();
        return;
    }

    std::optional<ValueT> get(const KeyT& aKey) {
        if (cacheKeys.find(aKey) == cacheKeys.end()) return std::nullopt;
        
        // deque
        auto theIt = cache.begin();
        for(theIt; theIt!=cache.end(); theIt++){
            if (theIt->first == aKey) {
                PairT thePair = *theIt;
                ValueT theValue = theIt->second;
                cache.erase(theIt);
                cache.emplace_front(std::make_pair(aKey, theValue));
                /*
                std::cout <<  " --------------  get  -------------- "  << std::endl;
                std::cout << theValue.header.type << " ----- " << theValue.payload << std::endl;
                std::cout <<  " --------------  end get  --------------"  << std::endl << std::endl;*/
                
        
                // list
                //cache.splice(cache.begin(), cache, theCacheIt);
                return theValue;
            }
        }
        return std::nullopt;
    }

    bool contains(const KeyT& aKey) const {
        return (cacheKeys.find(aKey) != cacheKeys.end());
    }

    size_t size() const {
        return cacheKeys.size();
    }
    
  protected:
    size_t maxsize;
    std::unordered_set<KeyT> cacheKeys;
    std::deque<PairT> cache;
    //std::list<PairT> cache;

  };

  template<CacheType CacheT, typename KeyT, typename ValueT>
  class ConfigLRUCache : public LRUCache<KeyT, ValueT> {
    public:

    ConfigLRUCache() : LRUCache<KeyT, ValueT>(Config::getCacheSize(CacheT)) {}

    std::optional<ValueT> get(const KeyT& aKey) {
        if (Config::useCache(CacheT)) {
            return LRUCache<KeyT, ValueT>::get(aKey);
        }
        return std::nullopt;
    }

    void put(const KeyT& aKey, const ValueT& aValue) {
        if (Config::useCache(CacheT)) LRUCache<KeyT, ValueT>::put(aKey, aValue);
    }
  };
} // namespace ECE141

#endif
