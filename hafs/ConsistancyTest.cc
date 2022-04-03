#include<iostream>
#include<unordered_map>
#include<string>

#include "client_lib.h"
#define BLOCK 4096
using namespace std;

string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

HafsClientFactory client("0.0.0.0:8090", "0.0.0.0:8091");
HafsClient client1(grpc::CreateChannel("0.0.0.0:8090", grpc::InsecureChannelCredentials()), "0.0.0.0:8090", false);
HafsClient client2(grpc::CreateChannel("0.0.0.0:8091", grpc::InsecureChannelCredentials()), "0.0.0.0:8091", false);

void SingleClientTest()
{
    //No of Writes 1000
    //Nature of Write Serial
    int NoWrite = 1000;
    int startAddr=BLOCK;
    string res;
    unordered_map<int,int> hash;
    for(int i=0; i< NoWrite;i++)
    {
        string data = gen_random(BLOCK);
        client.Write(startAddr,data);
        hash[startAddr]=1;
        client.Read(startAddr,&res);
        if(res != data)
        {
            cout<<"Read Write Don't Match iteration : "<<i<<endl;
            break;
        }
        startAddr+=BLOCK;
    }
    //Checking Consistancy
    for(auto it=hash.begin();it!=hash.end();it++)
    {
        int addr = it->first;
        if(client.CheckConsistancy(addr)!=true)
        {
            cout<<"Consistancy Failed : "<<addr<<endl;
                break;
        }
    }
}

int main()
{
    SingleClientTest();
    return 0;
}