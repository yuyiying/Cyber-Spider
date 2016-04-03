//
//  main.cpp
//  project4
//
//  Created by Stephanie Yu on 3/5/16.
//  Copyright © 2016 Stephanie Yu. All rights reserved.
//

#include <iostream>
#include "DiskMultiMap.h"
#include "BinaryFile.h"
#include "MultiMapTuple.h"
#include "InteractionTuple.h"
#include "IntelWeb.h"
#include <vector>

int main()
{
    

//     DiskMultiMap x;
//     x.createNew("myhashtable.dat",5); // empty, with 100 buckets
//     x.insert("hmm.exe", "pfft.exe", "m52902");
//     x.insert("hmm.exe", "pfft.exe", "m52902");
//     x.insert("hmm.exe", "pfft.exe", "m10001");
//     x.insert("blah.exe", "bletch.exe", "m0003");
//     DiskMultiMap::Iterator it = x.search("hmm.exe");
//     if (it.isValid())
//     {
//         cout << "I found at least 1 item with a key of hmm.exe\n";
//         do
//        {
//             MultiMapTuple m = *it; // get the association
//             cout << "The key is: " << m.key << endl;
//             cout << "The value is: " << m.value << endl;
//             cout << "The context is: " << m.context << endl;
//             cout << endl;
//             ++it; // advance iterator to the next matching item
//         } while (it.isValid());
//     }

    
    
//    DiskMultiMap x;
//     x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//     x.insert("hmm.exe", "pfft.exe", "m52902");
//     x.insert("hmm.exe", "pfft.exe", "m52902");
//     x.insert("hmm.exe", "pfft.exe", "m10001");
//     x.insert("blah.exe", "bletch.exe", "m0003");
//     DiskMultiMap::Iterator it = x.search("goober.exe");
//     if ( ! it.isValid())
//     cout << "I couldn’t find goober.exe\n";
    
    
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",5); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    // line 1
//    if (x.erase("hmm.exe", "pfft.exe", "m52902") == 2)
//        cout << "Just erased 2 items from the table!\n";
//    // line 2
//    if (x.erase("hmm.exe", "pfft.exe", "m10001") > 0)
//        cout << "Just erased at least 1 item from the table!\n";
//
//
//    // line 3
//    if (x.erase("blah.exe", "bletch.exe", "m66666") == 0)
//        cout << "I didn't erase this item cause it wasn't there\n";
//    x.print();
//    x.insert("hmm.exe", "pt.exe", "m37839");
//    x.insert("ede", "deef", "dee");
//    x.insert("dehdk", "dehiu", "dehl");
//    x.print();
//    x.insert("defr", "ewyfir", "dehk");
//    x.print();
    std::vector<std::string> indicators;
    unsigned int minPrevalenceToBeGood;
    std::vector<std::string> badEntitiesFound;
    std::vector<InteractionTuple> badInteractions;
    indicators.push_back("qqqqqa.exe");
    indicators.push_back("qqqqqd.exe");
    
    IntelWeb i;
    i.createNew("myo", 8000); //Doesn't work for 1000 and foobar.
    i.ingest("testlog1.dat");
    cerr<<"Completed Ingest.\n";
    i.purge("qqqqqd.exe");
    int k=i.crawl(indicators, 4, badEntitiesFound, badInteractions);
    cerr<<"Malicious Entities: "<<k<<endl;
    InteractionTuple tup;
    for (int i=0; i<badInteractions.size(); i++) {
        tup=badInteractions[i];
        cerr<<tup.context<<" "<<tup.from<<" "<<tup.to<<" "<<endl;
    }
    for (int i=0; i<badEntitiesFound.size(); i++) {
        cerr<<badEntitiesFound[i]<<endl;
    }
    i.close();
    
    return 0;
}