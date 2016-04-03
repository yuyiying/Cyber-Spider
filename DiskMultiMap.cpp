//
//  DiskMultiMap.cpp
//  project4
//
//  Created by Stephanie Yu on 3/5/16.
//  Copyright © 2016 Stephanie Yu. All rights reserved.
//

#include "DiskMultiMap.h"
#include <functional>

DiskMultiMap::Iterator::Iterator()
{
    m_start = -1;
    m_valid = false;
}

DiskMultiMap::Iterator::Iterator(const BinaryFile::Offset start, DiskMultiMap* ptr)
{
    m_start = start;
    m_ptr = ptr;
    m_ptr->bf.read(curNode, start);
    if(curNode.next == -1)
        m_valid = false;
    else
        m_valid = true;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    if(curNode.next == -1)
    {
        m_valid = false;
        return *this;
    }
    else
    {
        m_start = curNode.next;
        m_ptr->bf.read(curNode, m_start);
        return *this;
    }
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    
    MultiMapTuple tuple;
    tuple.key = curNode.key;
    tuple.value = curNode.value;
    tuple.context = curNode.context;
    return tuple;
}

bool DiskMultiMap::Iterator::isValid() const
{
    return m_valid;
}


DiskMultiMap::DiskMultiMap()
{
    first = 0;  // offset of first empty, initialize to be 0
    firstDel = -1; // offset of deletenode, initialize to be -1
}

DiskMultiMap::~DiskMultiMap()
{
    bf.close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
    if(bf.isOpen() == true)
        bf.close();
    if(bf.createNew(filename) == false)
        return false;
    m_numBuckets = numBuckets;
    BinaryFile:: Offset off = 0;
    bf.write(m_numBuckets, off); // write the number of buckets in the first four bytes
    off += sizeof(m_numBuckets);
    bf.write(firstDel,off);
    off += sizeof(firstDel);
    
    first = off;
    m_filename = filename;
    
    //create numBuckets of dummy node in the buckets;
    
    Node emptyNode;

    strcpy(emptyNode.key, "");
    strcpy(emptyNode.value, "");
    strcpy(emptyNode.context, "");
    emptyNode.next = -1;
    
    for(int i = 0; i < m_numBuckets; i++)
    {
        BinaryFile:: Offset off = first + i * sizeof(Node);
        emptyNode.offset = off;
        bf.write(emptyNode, off);
    }
    bf.close();
    bf.openExisting(filename);
    return  true;
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
    if(bf.isOpen() == true)
        bf.close();
    if(bf.openExisting(filename) == false)
        return false;
    return  true;
}

void DiskMultiMap::close()
{
    if(bf.isOpen() == true)
        bf.close();
}


BinaryFile::Offset DiskMultiMap::hash(const char* data)
{
    std::hash<std::string> str_hash; // creates a string hasher.
    int num = 0;
    bf.read(num, 0);
    BinaryFile::Offset index = str_hash(data) % num;
    index = 12 + index * sizeof(Node);
    return index;
}


bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    if (sizeof(key) >= 121 || sizeof(value) >= 121 || sizeof(context) >= 121) {
        cerr << "input is larger than 121 bytes" << endl;
        return false;
    }
    
    BinaryFile::Offset index = hash(key.c_str());
    
    Node addNode; // create the insert node
    strcpy(addNode.key, key.c_str());
    strcpy(addNode.value, value.c_str());
    strcpy(addNode.context, context.c_str());
    addNode.next = -1;
    
    Node temp;
    
    bf.read(temp, index);
        
    
    while(temp.next != -1)
    {
        bf.read(temp, temp.next);
    }
    
    if(firstDel != -1)
    {
        
        Node firstdeleteNode;
        bf.read(firstdeleteNode, firstDel);
        firstDel = firstdeleteNode.next;
        
        addNode.offset = firstdeleteNode.offset;
        temp.next = firstdeleteNode.offset;
        bf.write(temp, temp.offset);
        
        if(bf.write(addNode, addNode.offset) == false)
        {
            return false;
        }
        else
            return true;
    }
    else
    {
        addNode.offset = bf.fileLength();
        temp.next = addNode.offset;
        bf.write(temp, temp.offset);
        
        if(bf.write(addNode, addNode.offset) == false)
        {
            return false;
        }
        else
            return  true;
    }
}


DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
    BinaryFile::Offset index = hash(key.c_str());
    Iterator it(index, this);
    if((*it).key == "" || (*it).value == "" || (*it).context == "")
    {
        ++it;
    }
    return it;
}


void DiskMultiMap::insertDel(const BinaryFile::Offset del)
{
    Node deleteNode;
    bf.read(deleteNode, del);
    deleteNode.next = firstDel;
    firstDel = del;
    bf.write(firstDel, sizeof(int));
    bf.write(deleteNode,del);
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    int count = 0;
    
    BinaryFile::Offset index = hash(key.c_str());
    
    Node temp;
    bf.read(temp,index);
    if(temp.next == -1) return count;
    else
    {
        Node nextNode;
        bf.read(nextNode, temp.next);
        while(temp.next != -1)
        {
            if(strcmp(nextNode.value, value.c_str()) == 0 && strcmp(nextNode.context, context.c_str()) == 0)
            {
                temp.next = nextNode.next;
                bf.write(temp, temp.offset);
                insertDel(nextNode.offset);
                count++;
            }
            else
            {
                temp = nextNode;
            }
            if(temp.next != -1)
                bf.read(nextNode, temp.next);
        }
    }
    return count;
}

//for test the reuse node
//void DiskMultiMap::print()
//{
//    cout << "the length of file is " << bf.fileLength() << endl;
//    cout << "all the buckets and first " << first + m_numBuckets * sizeof(Node) << endl;
//}
