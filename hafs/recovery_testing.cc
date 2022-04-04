#include<iostream>
#include "client_lib.h"

using namespace std;

int main() {
    HafsClientFactory client("0.0.0.0:8090", "0.0.0.0:8091");
    HafsClient client1(grpc::CreateChannel("0.0.0.0:8090", grpc::InsecureChannelCredentials()), "0.0.0.0:8090", false);
    HafsClient client2(grpc::CreateChannel("0.0.0.0:8091", grpc::InsecureChannelCredentials()), "0.0.0.0:8091", false);
    string res;

    // Test case : primary receives write request and fails 
    // output : not persisted on either on primary & backup
    // status : not working, primary not failing only. issue with roles assigned?
    client.Write(4096, string(4096, 'a'));


    //Test case : primary receive write request, backup writes it and send ack to primary
    // and then while writing to primary it fails
    // output : Unless client is retrying (since it didn't receive ack), inconsistency present
    
    client.Write(8192, string(4096, 'b'));
    usleep(10*1000000); // 10 seconds
    // assumption : failover would open, now backup is primary and serving your request 
    // and pending queue of primary contains this write request 
    client.Write(8192, string(4096, 'b'));
    // start pim
    // check consistency after this.

    

    



    // primary fails after writing 
    //(that means client doesn't received ack, but both primary and backup is having data)

    client.Write(12288, string(4096,'c'));
    
}