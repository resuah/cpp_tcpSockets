#pragma once 

#include <unistd.h>
#include <netdb.h> 
#include <arpa/inet.h> 		/* inet_ntoa() to format IP address */
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <thread> 
#include <iostream>
#include <cstring>
#include <signal.h>
#include <mutex>
#include <functional>

//#include "constants.hpp"
// #include "AMEA_Data.hpp"


namespace DataSocket{

template < uint32_t BufInLen, typename inT, uint32_t BufOutLen, typename outT >
class SocketClient  {
 public:
  SocketClient(const char * _ipAddr, uint16_t port, double period = 0.01, double _timeout = 1.0 ) 
      :ipAddr(_ipAddr), port(port), period(period) , timeOut(_timeout) {
    running = false;
    connected = false; 

    // start(); 
  }
  
  virtual ~SocketClient() {
    running =false; 
    if ( t.joinable() ){            // checks if t is running or not
      t.join();
    }
    
  }

  virtual bool isThreadRunning(){ return t.joinable(); }
  
  virtual void start(){
      t= std::thread(&SocketClient::run, this); 
  } 
  
  virtual void stop() {
    running = false;
    connected = false; 
    
  }
  
  virtual bool isRunning() {
    return running;
  }
  
  virtual bool isConnected() {
    return connected;
  }

  void setReceiveBufferAction( std::function<void ()> _action){
      this->setReceiveBuffer = _action; 
  }

  void setTransferBufferAction( std::function<void ()> _action){
      this->setTransferBuffer = _action; 
  }

//  void setReceiveBuffer(){
//       ameaData.safetyLevel= rxBuf[0];
//   }

//   void setTransferBuffer(){
//       txBuf[0] = true; 
//   } 
  
  virtual std::array<outT, BufOutLen> getReceiveBuffer() {
    std::lock_guard<std::mutex> lock(mtx);
    return rxBuf;
  }
  
  virtual void setSendBuffer(std::array<inT, BufInLen> data) {
    std::lock_guard<std::mutex> lock(mtx);
    txBuf = data;
  }

  bool newData = false;
  
 private:
  virtual void run() {
    std::cout << "SocketClient thread started" << std::endl;
    running = true;

    while (running) {
      struct sockaddr_in servAddr;
      int sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) throw "ERROR opening socket";

      auto server = gethostbyname(ipAddr); 
      if (server == NULL) {
        throw "Server ip not found";
      }
      bzero((char *) &servAddr, sizeof(servAddr));
      servAddr.sin_family = AF_INET;
      bcopy((char *)server->h_addr,(char *)&servAddr.sin_addr.s_addr, server->h_length);
      servAddr.sin_port = htons(port);
    
      using seconds = std::chrono::duration<double, std::chrono::seconds::period>;
      auto next_cycle = std::chrono::steady_clock::now() + seconds(period);
      while (running && connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        std::this_thread::sleep_until(next_cycle);
        next_cycle += seconds(period);
      }
    //   if(running){ 
    //     std::cout << "Client connected to ip=" << ameaData.ipAddr<< std::endl;
    //   }
      inT b_write[BufInLen]; outT b_read[BufOutLen];							
      connected = true;
    
      while (connected) {
        std::this_thread::sleep_until(next_cycle);
        
        // write
        setTransferBuffer(); 
        std::unique_lock<std::mutex> wlck(mtx);
        for(uint32_t i = 0; i < BufInLen; i++) b_write[i] = txBuf[i]; 
        wlck.unlock();
        int n = write(sockfd, b_write, BufInLen * sizeof(inT));
        if (n < 0) {
          std::cout << "error = " << std::strerror(errno) << std::endl;
          connected = false;
        }
        
        // read
        size_t count = BufOutLen * sizeof(outT);
        uint8_t* ptr = (uint8_t *)b_read;
        auto endTime = std::chrono::steady_clock::now() + seconds(timeOut);
        while (connected && count) {
          if (std::chrono::steady_clock::now() > endTime) {
            std::cout << "error = socket read timed out" << std::endl;
            connected = false;
          }
          n = read(sockfd, ptr, count);
          if (n < 0) {
            // std::cout << "error = " << std::strerror(errno) << std::endl;
            connected = false;
          }
          ptr += n;
          count -= n;
        }
        std::unique_lock<std::mutex> rlck(mtx);
        for(uint32_t i = 0; i < BufOutLen; i++) rxBuf[i] = b_read[i];
        rlck.unlock();
        setReceiveBuffer(); 
        newData = true;
        next_cycle += seconds(period);
      }

      close(sockfd);
      // if disconnected clear receive buffer
      for(uint32_t i = 0; i < BufOutLen; i++) rxBuf[i] = 0;
      newData = true;
    }
  }
  
  std::thread t ;  
  bool running;

  std::function< void()> setReceiveBuffer;
  std::function< void()> setTransferBuffer;

  const char * ipAddr; 
  uint16_t port;
  double period;
  double timeOut;	// time which thread tries to read until socket read timed out
  struct hostent *server;
  int sockfd;
  bool connected;
  std::mutex mtx;
public: 
  std::array<outT, BufOutLen> rxBuf;
  std::array<inT, BufInLen> txBuf;
};  // class SocketClient 




} // namespace taskcontroler 