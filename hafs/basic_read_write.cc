#include<iostream>
#include "client_lib.h"

using namespace std;


int main() {
    HafsClientFactory client("server-0.test1.uwmadison744-f21.emulab.net:50052", 
    "server-1.test1.uwmadison744-f21.emulab.net:50053");
    string res;
    // // client.Read(0, &res);
    // // cout << "Data read: " << res << endl;

    for(int i = 0; i < 7000; i++) {
        client.Write(i*4096, string(4096, 'a'));
    }

    // client1.Write(8192 + 2048, string(4096, 'm'));
    // client.Write(8192 + 2048, string(4096, 'a'));
    // client.Write(4096, string(4096, 'b'));
    // client.Write(8192, string(4096, 'c'));
    // client.Write(8192 + 4096, string(4096, 'd'));
    // client.Write(8192 + 8192, string(4096, 'e'));
    // client.Write(8192 + 8192 + 4096, string(4096, 'f'));

    // // client.Read(0, &res);
    // // cout << "Data read: " << res << endl;
    // // client.Read(4096, &res);
    // // cout << "Data read: " << res << endl;
    // client1.Read(8192 + 2048, &res);
    // cout << "Data read: " << res << endl;
    // client.Read(8192 + 2048 + 8192 + 4096, &res);
    // cout << "Data read: " << res << endl;
    // cout << "Data Size: " << res.size() << endl;
    // client.Read(8192 + 2048 + 8192 + 8192, &res);
    // cout << "Data read: " << res << endl;
    // cout << "Data Size: " << res.size() << endl;
    // // client.Read(8192, &res);
    // cout << "Data read: " << res << endl;
    // client.Read(8192 + 4096, &res);
    // cout << "Data read: " << res << endl;
    // client2.Read(2048, &res);
    // cout << "Data read: " << res << endl;
    // client2.Read(8192 + 4096, &res);
    // cout << "Data read: " << res << endl;
    // client2.Read(8192 + 8192, &res);
    // cout << "Data read: " << res << endl;
    // client2.Read(8192 + 8192 + 4096, &res);
    // cout << "Data read: " << res << endl;



    // // client.Write(4096, string(4096, 'b'));
    // // client.Write(8192, string(4096, 'c'));

    // // string res2;
    // // client.Read(8392704, &res2);
    // // cout << "Data read: " << res2 << endl;

  //  usleep(10*1000000);
    return 0;
}