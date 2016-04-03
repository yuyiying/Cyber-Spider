//
//  DiskMultiMap.h
//  project4
//
//  Created by Stephanie Yu on 3/5/16.
//  Copyright © 2016 Stephanie Yu. All rights reserved.
//

#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

struct Node
{
    char key[121];
    char value[121];
    char context[121];
    BinaryFile::Offset next;
    BinaryFile::Offset offset;
};


class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        
        // You may add additional constructors
        Iterator(const BinaryFile::Offset start, DiskMultiMap* ptr);
        
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        // Your private member declarations will go here
        bool m_valid;
        DiskMultiMap* m_ptr;
        Node curNode;
        BinaryFile::Offset m_start;
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    
    //for test reuse node;
    void print();
    
private:
    // Your private member declarations will go here
    BinaryFile bf;
    BinaryFile::Offset first;
    BinaryFile::Offset firstDel;
    BinaryFile::Offset hash(const char* data);
    int m_numBuckets;
    std::string m_filename;
    void insertDel(const BinaryFile::Offset del);
    
    
    
    
};

#endif // DISKMULTIMAP_H_