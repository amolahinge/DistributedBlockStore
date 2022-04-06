#include <errno.h>
#include <fcntl.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <ctime>
#include <chrono>

#include <iostream>

#include "hafs.grpc.pb.h"

using ::Request;
using ::HeartBeatResponse;
using ::ReadRequest;
using ::ReadResponse;
using ::WriteRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::Status;


class HafsClient {
    public:
        int count;
        HafsClient() {}
        HafsClient(std::shared_ptr<Channel> channel, std::string address, bool isAlive): stub_(Hafs::NewStub(channel)) {
            std::cout << "[HafsCLient] Starting Hafs Client Instance!" <<std::endl;
            this->address = address;
            this->isAlive = isAlive;
            // Get first HearBeat here before starting thread
            count = 0;
            checkHeartBeat();
            std::thread thread_object(&HafsClient::startHeartBeat, this);
            thread_object.detach();
        }
        HafsClient(std::string address) {
            HafsClient(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()), address, false);
        }

        void startHeartBeat() {
            while(true) {
                usleep(1000000);
                checkHeartBeat();
            }
        }

        void checkHeartBeat() {
            HeartBeatResponse response = this->HeartBeat();
            std::chrono::time_point<std::chrono::system_clock> start, end;

            // std::cout << "[HafsCLient] Recieved Hearbeat, Status[" << HeartBeatResponse_Status_Name(response.status()) << "], Role[" << HeartBeatResponse_Role_Name(response.role()) << "], Health[" << HeartBeatResponse_Health_Name(response.health()) << "] from address[" << this->address << "]" << std::endl;
            if(response.health() == HeartBeatResponse_Health_REINTEGRATION_AHEAD){
                //we start counter only when we received reintegration ahead status first time.
                count++;
                if(count==1){
                    //startTime 
                    start = std::chrono::system_clock::now();
                }
            }

            if(response.health() == HeartBeatResponse_Health_HEALTHY){
                if(count > 0){

                    //stopTime
                    count = 0;
                    end = std::chrono::system_clock::now();
                    std::chrono::duration<double> elapsed_seconds = end - start;
                    std::cout << "[Replicator] Time taken to change status from REINTEGRATION to HEALTHY  is " << elapsed_seconds.count() << "s\n";
                }
            }


            if(response.status() == HeartBeatResponse_Status_INVALID) {
                replicatorHealth = HeartBeatResponse_Health_UNHEALTHY;
                this->isAlive = false;
            } else {
                replicatorHealth = response.health();
                this->isAlive = true;
            }
        }

        void doSomething() {
            std::cout << "testishaan" << std::endl;
        }

        // Assembles the client's payload, sends it and presents the response back
        // from the server.
        HeartBeatResponse HeartBeat() {
            Request request;
            HeartBeatResponse response;
            ClientContext context;
            Status status = stub_->HeartBeat(&context, request, &response);

            if (!status.ok()) {
                // std::cout << "[HafsCLient] HeartBeat Failed: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                response.set_status(HeartBeatResponse_Status_INVALID);
            }
            return response;
        }

        bool Write(int addr, std::string data) {
            WriteRequest request;
            Response response;
            ClientContext context;
            request.set_data(data);
            request.set_address(addr);



            Status status = stub_->Write(&context, request, &response);

            if (!status.ok()) {
                std::cout << "[HafsCLient] Write: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                return false;
            }
            return true;
        }

        bool Read(int addr, std::string* data) {
            ReadRequest request;
            ReadResponse response;
            ClientContext context;
            request.set_address(addr);

            Status status = stub_->Read(&context, request, &response);
            
            if (!status.ok()) {
                std::cout << "[HafsCLient] Read: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                return false;
            }
            data->resize(response.data().size());
            data->replace(0, response.data().size(), response.data());
            return true;
        }

        bool ReplicateBlock(int addr, std::string data) {
            WriteRequest request;
            Response response;
            ClientContext context;
            request.set_data(data);
            request.set_address(addr);

            Status status = stub_->ReplicateBlock(&context, request, &response);

            if (!status.ok()) {
                std::cout << "[HafsCLient] ReplicateBlock: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                return false;
            }
            return true;
        }

        bool CommitBlock(int addr) {
            WriteRequest request;
            Response response;
            ClientContext context;
            request.set_address(addr);

            Status status = stub_->CommitBlock(&context, request, &response);

            if (!status.ok()) {
                std::cout << "[HafsCLient] CommitBlock: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                return false;
            }
            return true;
        }
        
        bool CheckConsistancy(int addr) {
            ReadRequest request;
            Response response;
            ClientContext context;
            request.set_address(addr);

            Status status = stub_->CheckConsistancy(&context, request, &response);
            
            if (!status.ok()) {
                std::cout << "[HafsCLient] CheckConsistancy: error code[" << status.error_code() << "]: " << status.error_message() << std::endl;
                return false;
            }
            return true;
        }

        bool getIsAlive() {
            return isAlive;
        }

        HeartBeatResponse_Health getReplicatorHealth() {
            return replicatorHealth;
        }

    private:
        std::unique_ptr<Hafs::Stub> stub_;
        std::string address;
        bool isAlive;
        HeartBeatResponse_Health replicatorHealth;
};