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
#include <csignal>
#include <stdio.h>
#include <array> 

// #include "include/constants.hpp"

// #include "include/SocketClient.hpp"
#include "../include/SocketServer.hpp"

using namespace DataSocket; 

// DataSocket::SocketServer * server; 

void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
    // server->stop(); 
   exit(signum);  
}


void writeInputFormat(){
    std::cout << std::endl; 
    std::cout << "use format : taskControler [-ip ipAddr] " << std::endl ;  
    std::cout << "    -ip : ip adress to AMEA robot or simulator (default 127.0.0.1)" << std::endl; 
    std::cout << "  -help : show input format " << std::endl; 
}

int main (int argc, char** argv){ 
    
    signal(SIGINT, signalHandler);

/////////////////////////////////////////////////////////////////////
//// argument handler 

    int cntr= 1; 
    const char * ipAddr= "127.0.0.1"; 
    uint16_t  port = 8888; 
    
    double serverValue = 1.0; 
    double clientValue, oldClientVal=0.0; 

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

    SocketServer<1 , double, 1, double > server(port, 1.0); 
    server.setReceiveBufferAction([&](){ 
        // clientValue= server.rxBuf[0]; 
    }); 
    server.setTransferBufferAction([&](){
        // server.txBuf[0] = serverValue; 
    }); 
  
    server.start(); 
    server.setSendBuffer(std::array<double,1>({serverValue})); 
    
    sleep(1.0); 

    std::cout << "Server before while" << std::endl; 
    while(!server.isConnected() && !server.isRunning()){
        std::cout << "Server not connected" << std::endl; 
        usleep(100000); 
    }
    // while(server.isThreadRunning()){

    while(server.isRunning() ){ 
        std::cout << "Server in running" << std::endl;          // !!! muss drin sein, sonst funktioniert es nicht??!! 
        usleep(100000); 

        while(server.isConnected()){
            clientValue = server.getReceiveBuffer()[0];
            // std::cout << "client is connected"  << std::endl; 
            if(clientValue != oldClientVal){ 
                std::cout << "Server value: " << serverValue << " , client value: " << clientValue << std::endl; 
                serverValue += 1.0; 
                server.setSendBuffer(std::array<double,1>({serverValue})); 
                oldClientVal= clientValue; 
            }

        }

    }

    server.stop(); 


    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////