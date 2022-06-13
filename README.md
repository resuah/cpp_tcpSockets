# cpp_tcpSockets
TCP Socket solution for cpp programs. Server and Client are running in separate threads but with the fixed frequency the exchange is performed constantly. The transfer and receive data are defined on both sides. They can be set manually or in defined action functions for permanent data exchange. 

## Installation
Use in the "include" folder the header files for Server and Client applications

##SocketServer
Example with double value for transfer and receive datat. Include server header into your programm. 
```
#include "cpp_TcpSockets/include/SocketServer.hpp"

uint16_t  port = 8888; 
SocketServer<1 , double, 1, double > server(port, 1.0); 

double clientValue, serverValue; 
```
Action functions are executed at each loop and can be used to update data autonomously. If not needed, skip defintion or keep lambda function empty
```
server.setReceiveBufferAction([&](){ 
	// clientValue= server.rxBuf[0];
 }); 
    
server.setTransferBufferAction([&](){
        // server.txBuf[0] = serverValue; 
    }); 
```
Start Server thread
```
server.start(); 
```
Read RxBuf and write TxBuf manually
```
clientValue = server.getReceiveBuffer()[0];
server.setSendBuffer(std::array<double,1>({serverValue})); 
```
Stop Server thread and close connection 
```
server.stop(); 
```

##SocketClient
Example with double value for transfer and receive datat. Include client header into your programm. 
```
#include "cpp_TcpSockets/include/SocketClient.hpp"

const char * ipAddr= "127.0.0.1";
uint16_t  port = 8888; 
SocketClient<1 , double, 1, double > client(ipAddr,  port, 1.0); 

double clientValue, serverValue; 
```
Action functions are executed at each loop and can be used to update data autonomously. If not needed, skip defintion or keep lambda function empty
```
client.setReceiveBufferAction([&](){ 
	// clientValue= server.rxBuf[0];
 }); 
    
client.setTransferBufferAction([&](){
        // server.txBuf[0] = serverValue; 
    }); 
```
Start Client thread
```
client.start(); 
```
Read RxBuf and write TxBuf manually
```
serverValue = server.getReceiveBuffer()[0];
client.setSendBuffer(std::array<double,1>({clientValue})); 
```
Stop Client thread and close connection 
```
client.stop(); 
```

## Examples 
Test the Server and Client program on your device 


