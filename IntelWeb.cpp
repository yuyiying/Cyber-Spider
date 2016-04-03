//
//  IntelWeb.cpp
//  project4
//
//  Created by Stephanie Yu on 3/6/16.
//  Copyright © 2016 Stephanie Yu. All rights reserved.
//

#include "IntelWeb.h"

IntelWeb::IntelWeb(){}

IntelWeb::~IntelWeb()
{
    close();
}

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
    close();
    string forward = filePrefix + "forward.dat";
    string reverse = filePrefix + "reverse.dat";
    
    
    if(!map_forward.createNew(forward, floor(maxDataItems*1.3)))
        return false;
    if(!map_reverse.createNew(reverse, floor(maxDataItems*1.3)))
        return false;
    return true;
}

bool IntelWeb::openExisting(const std::string& filePrefix)
{
    close();
    string forward = filePrefix + "forward.dat";
    string reverse = filePrefix + "reverse.dat";

    if(!map_forward.openExisting(forward))
        return false;
    if(!map_reverse.openExisting(reverse))
        return false;
    return true;
}

void IntelWeb::close()
{
    map_forward.close();
    map_reverse.close();
}


bool IntelWeb::ingest(const std::string& telemetryFile)
{
    ifstream inf(telemetryFile);
    
    if(! inf)
    {
        cerr<< "cannot open the file" << endl;
        return false;
    }
    
    string line;
    
    while(getline(inf, line))
    {
        istringstream iss(line);
        string from;
        string to;
        string context;
        char dummy;
        if(!(iss >> context >> from >> to))
        {
            cerr << "ignoring badly-formatted input line" << endl;
            continue;
        }
        if (iss >> dummy);
        if(!map_forward.insert(from, to, context))
            return false;
        if(!map_reverse.insert(to, from, context))
            return false;
    }
    return true;
}

bool compare(const InteractionTuple& a, const InteractionTuple& b)
{
    if(a.context < b.context)
        return true;
    else if(a.context == b.context)
    {
        if(a.from < b.from)
            return true;
        else if(a.from == b.from)
        {
            if(a.to < b.to)
                return true;
        }
    }
    return false;
}


unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,unsigned int minPrevalenceToBeGood,
                std::vector<std::string>& badEntitiesFound,std::vector<InteractionTuple>&interactions)
{
    unordered_map<std::string, int> occurance;
    unsigned int numBad = 0;
    int count = 0;
    badEntitiesFound.clear();
    vector<std::string> newIndicators = indicators;
    DiskMultiMap::Iterator it1, it2;
    for(int i = 0; i < newIndicators.size(); i++)
    {
        it1 = map_forward.search(newIndicators[i]);
        
        if(it1.isValid() && find(badEntitiesFound.begin(),badEntitiesFound.end(),newIndicators[i]) == badEntitiesFound.end())
        {
            numBad++;
            badEntitiesFound.push_back(newIndicators[i]);
        }
        
        while(it1.isValid())
        {
            MultiMapTuple m = *it1;
            InteractionTuple tuple;
            tuple.from = m.key;
            tuple.to = m.value;
            tuple.context = m.context;
            interactions.push_back(tuple);
            ++it1;
            
            if(find(badEntitiesFound.begin(),badEntitiesFound.end(),tuple.to) == badEntitiesFound.end())
            {
                count = 0;
                if(occurance.find(tuple.to) == occurance.end())
                {
                    DiskMultiMap::Iterator itemp1;
                    itemp1 = map_forward.search(tuple.to);
                    while(itemp1.isValid())
                    {
                        count++;
                        ++itemp1;
                    }
                    DiskMultiMap::Iterator itemp2;
                    itemp2 = map_reverse.search(tuple.to);
                    while(itemp2.isValid())
                    {
                        count++;
                        ++itemp2;
                    }
                    occurance[tuple.to] = count;
                }
                else
                    count = occurance[tuple.to];
                
                if(count < minPrevalenceToBeGood)
                {
                    numBad++;
                    newIndicators.push_back(tuple.to);
                    badEntitiesFound.push_back(tuple.to);
                }
            }
        }
        
        it2 = map_reverse.search(newIndicators[i]);
        
        if(it2.isValid() && find(badEntitiesFound.begin(),badEntitiesFound.end(),newIndicators[i]) == badEntitiesFound.end())
        {
            numBad++;
            badEntitiesFound.push_back(newIndicators[i]);
        }
        
        while(it2.isValid())
        {
            MultiMapTuple m = *it2;
            InteractionTuple tuple;
            tuple.from = m.value;
            tuple.to = m.key;
            tuple.context = m.context;
            interactions.push_back(tuple);
            ++it2;
            
            if(find(badEntitiesFound.begin(),badEntitiesFound.end(),tuple.from) == badEntitiesFound.end())
            {
                count = 0;
                if(occurance.find(tuple.from) == occurance.end())
                {
                    DiskMultiMap::Iterator itemp1;
                    itemp1 = map_forward.search(tuple.from);
                    while(itemp1.isValid())
                    {
                        count++;
                        ++itemp1;
                    }
                    DiskMultiMap::Iterator itemp2;
                    itemp2 = map_reverse.search(tuple.from);
                    while(itemp2.isValid())
                    {
                        count++;
                        ++itemp2;
                    }
                    occurance[tuple.from] = count;
                }
                else
                    count = occurance[tuple.from];
                if(count < minPrevalenceToBeGood)
                {
                    numBad++;
                    newIndicators.push_back(tuple.from);
                    badEntitiesFound.push_back(tuple.from);
                }
            }
        }
    }
    sort(badEntitiesFound.begin(), badEntitiesFound.end());
    sort(interactions.begin(), interactions.end(), compare);
    interactions.erase(unique(interactions.begin(), interactions.end()), interactions.end());
    
    return numBad;
}

bool IntelWeb::purge(const std::string& entity)
{
    bool del = false;
    
    DiskMultiMap::Iterator it1 = map_forward.search(entity);
    DiskMultiMap::Iterator it2 = map_reverse.search(entity);
    
    while(it1.isValid())
    {
        MultiMapTuple tuple = *it1;
        map_forward.erase(tuple.key,tuple.value,tuple.context);
        map_reverse.erase(tuple.value, tuple.key, tuple.context);
        del = true;
        ++it1;
    }
    
    while(it2.isValid())
    {
        MultiMapTuple tuple = *it2;
        map_reverse.erase(tuple.key, tuple.value, tuple.context);
        map_forward.erase(tuple.value, tuple.key, tuple.context);
        del = true;
        ++it2;
    }
    return del;
}






