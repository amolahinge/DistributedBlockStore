#include "client_imp.h"

using namespace std;

class HafsClientFactory {
    private:
        HafsClient primary;
        HafsClient backup;
    public:
        HafsClientFactory(string primaryAddress, string backupAddress): primary(grpc::CreateChannel(primaryAddress, grpc::InsecureChannelCredentials()), primaryAddress, false), backup(grpc::CreateChannel(backupAddress, grpc::InsecureChannelCredentials()), backupAddress, false) {

        }

        bool Write(int addr, std::string data) {
            if(primary.getIsAlive()) {
                return primary.Write(addr, data);
            } else {
                cout<< "[HafsClientFactory] [Warning] (Write) Primary can't serve request, delegating current request to Bakcup" << endl;
                return backup.Write(addr, data);
            }
        }

        bool Read(int addr, std::string* data) {
            if(primary.getIsAlive()) {
                return primary.Read(addr, data);
            } else {
                cout<< "[HafsClientFactory] [Warning] (Read) Primary can't serve request, delegating current request to Bakcup" << endl;
                return backup.Read(addr, data);
            }
        }
};