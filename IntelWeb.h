//
//  IntelWeb.hpp
//  project4
//
//  Created by Stephanie Yu on 3/6/16.
//  Copyright © 2016 Stephanie Yu. All rights reserved.
//

#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <set>
#include <math.h>
#include <unordered_map>


class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
                       );
    bool purge(const std::string& entity);
    
private:
     // Your private member declarations will go here
    
    DiskMultiMap map_forward;
    DiskMultiMap map_reverse;
    
};

inline
bool operator==(const InteractionTuple& a, const InteractionTuple& b)
{
    if(a.from == b.from && a.to == b.to && a.context == b.context)
        return true;
    else
        return false;
}


#endif // INTELWEB_H_
