#include<iostream>
#include "client_lib.h"

using namespace std;


int main() {
    HafsClientFactory client("server-0.test1.uwmadison744-f21.emulab.net:50052", 
    "server-1.test1.uwmadison744-f21.emulab.net:50053");
    while(true);
    return 0;
}