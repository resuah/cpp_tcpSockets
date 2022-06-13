/////////////////////////////////////////////////////////////////////////
//  beagleboneblue encoderSendSocket
//  
//  author: Romano Hauser, romano.hauser@ost.ch
//  date: 13.05.2022
//
//  
// 
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <iomanip> 
#include <curses.h> 
#include <stdlib.h>
#include <vector>
#include <array>
#include <chrono> 
#include <thread> 

// #include "include/constants.hpp"

#include "../include/SocketClient.hpp"

using namespace DataSocket; 

void writeInputFormat(){
    std::cout << std::endl; 
    std::cout << "use format : taskControler [-ip ipAddr] " << std::endl ;  
    std::cout << "    -ip : ip adress to AMEA robot or simulator (default 127.0.0.1)" << std::endl; 
    std::cout << "  -help : show input format " << std::endl; 
}

int main (int argc, char** argv){ 
    
    

/////////////////////////////////////////////////////////////////////
//// argument handler 

    int cntr= 1; 
    const char * ipAddr= "127.0.0.1"; 
    uint16_t  port = 8888; 
    
    double serverValue=0, oldServerVal=0; 
    double clientValue=0; 

    while(cntr < argc){
        if(strcmp(argv[cntr], "-ip")== 0){
            cntr++; 
            ipAddr = argv[cntr]; 
            std::cout<< "new ip: " << ipAddr << std::endl; 
        } 
         else if(strcmp(argv[cntr], "-p" )==0  || strcmp(argv[cntr], "-port")== 0 ){
            cntr++; 
            port = std::atoi(argv[cntr]); 
            std::cout<< "new port: " << port << std::endl; 
        } 
         else if(strcmp(argv[cntr], "-help")== 0){
            writeInputFormat(); 
        }
        else {
            std::cout << "Wrong input format !!" << std::endl; 
            writeInputFormat(); 
            return -1; 
        }

        cntr++; 
    }

    SocketClient<1 , double, 1, double > client(ipAddr,  port, 1.0); 
    client.setReceiveBufferAction([&](){ 
        // serverValue= client.rxBuf[0]; 
    }); 
    client.setTransferBufferAction([&](){
        // client.txBuf[0] = clientValue; 
    }); 

    client.start(); 

    client.setSendBuffer(std::array<double, 1>({clientValue})); 

    sleep(1.0); 

    while(!client.isConnected() && !client.isRunning()){
        std::cout << "client  not connected" << std::endl; 
        // std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(0.1));
        usleep(1000000); 
    }
    // while(server.isThreadRunning()){
    while(client.isRunning()){ 
        std::cout << "client in running " << std::endl;              // !!! muss drin sein, sonst funktioniert es nicht??!! 
        // std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1.0));

        usleep(100000); 
        while(client.isConnected()){
            
            serverValue = client.getReceiveBuffer()[0]; 
            // std::cout << "client is connected, serverValue: " << serverValue  << std::endl; 
            if(serverValue != oldServerVal){ 
                clientValue = 2* serverValue; 
                std::cout << "server value: " << serverValue << ", client changed value" << clientValue << std::endl; 
                client.setSendBuffer(std::array<double, 1>({clientValue})); 
                oldServerVal= serverValue; 
            }
        }

    }
    
    client.stop(); 



    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////