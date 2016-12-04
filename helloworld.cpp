#include <iostream>
#include <string>
#include <fstream>

// Required by for routine
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <iterator>
#include <sstream>
#include <regex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//thread stuff
#include <cstdlib>
#include <thread>

#include <stdlib.h>   // Declaration for exit()

using namespace std;

int globalVariable = 2;

void router();

void getmyip(){
  char buffer[128];
  std::string result = "";
  std::shared_ptr<FILE> pipe(popen("ip addr show em1", "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result += buffer;
  }
  result = result.substr(result.find("inet")+5);
  result = result.substr(0, result.find("/"));
  cout << result;
}

void error(const string msg)
{
 perror((msg.c_str()));
 exit(1);
}

void DoWorkInChild(int &count) { //, ofstream &file ) {
  string childProcessName = "Child Process: ";

  // file << count << " " << childProcessName << "Hello world! The pID is: " << getpid() << endl;  
  router();
  cout << count << " " << "finished child function" << endl;
}

typedef struct packet_t{
  short version, messageLength;
  char message[140];
}packet;

void server() {
  int port = 20000;
  int sockett, sockett2;
  int socketFileDesc;
  int newSocketFileDesc;

  unsigned int sizeOfAddrClient;
  int n;
  char buffer[1000] = "[server]: hi bb \n";
  struct sockaddr_in serverAddress, clientAddress;

//opening socket
  socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  sockett = socketFileDesc;
  if(socketFileDesc < 0){
    error("could not open the socket");
  }

  // cout << "created socket" << endl; // needed. comment out for console readability

  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

//binding socket
  if(bind(socketFileDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
    error("could not bind");
  }

  // cout << "server bound to socket" << endl; // needed. comment out for console readability

  listen(socketFileDesc, 5);
  cout << "Waiting for connection on ";
  getmyip();
  cout << " port ";
  cout << port << endl;
  sizeOfAddrClient = sizeof(clientAddress);

//accepting clients here
  



  while ( (newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient))) {

  sockett2 = newSocketFileDesc;
  if(newSocketFileDesc < 0){
    error("error on acceptance");
  }
  cout << "accepting clients, u recieve first." << endl;



    packet_t* pac;

  //receving msg
    n = recv(newSocketFileDesc, buffer, sizeof(buffer), 0);

    pac = (packet_t*)buffer;

    if(n < 0){
      error("error reading from the socket");
    }

//printing msg
    printf("Friend: %s", pac->message);

    label:
    printf("You: ");

    char bufferdos[1000] = "[server]: hi bb";


    cout << bufferdos << endl;

    //bzero(buffer,1000);
  // fgets(buffer,1000,"[server]: hey bb");
  // buffer = "[server]: hey bb";



    packet_t pak2 = {};

    if(strlen(bufferdos) > 140){
      cout << "message too long" << endl;
      goto label;

    }

    strcpy(pak2.message, bufferdos);
    pak2.messageLength = strlen(bufferdos);
    pak2.version = 457;

    n = send(newSocketFileDesc, (char*)&pak2, sizeof(packet_t), 0);


    if(n<0){
      error("couldn't write to socket");
    }
  }
}


//creating N processes
void createRouter(int nFromFile) {

  pid_t pID[nFromFile];


 //creating N processes
  for (int i = 0; i < nFromFile; i++) {
    if ((pID[i] = fork()) < 0) {
      perror("fork");
      abort();
    } else if (pID[i] == 0) { // child process
      cout << "\tchild process created " << i << endl;
      DoWorkInChild(i); //, myfile);

      int childExitStatus;
      pid_t ws = waitpid(pID[i], &childExitStatus, WNOHANG);

      if (WIFEXITED(childExitStatus)) {
        cout << "\t" << i <<  " child terminated normally" << endl;
      }
      exit(0);
    }
    usleep(5000000);
  }

  int count = 0;
  int status;
  pid_t childPID;
  while (count < 5) {
    childPID = wait(&status);
    printf("Child with PID %ld exited with status 0x%x \n", (long)childPID, status);
    count++;
  }

}

void router() {

  int port = 20000;
  // string ip = "128.82.44.160";
  // const char *hn = ip.c_str();
  // char* hn = new char();
  // *hn = '128.82.44.160';

  // cout << "----------- hostname ip is: " << hn << endl;

  int socketFileDesc;

  int n;
  struct sockaddr_in serverAddress;
  struct hostent *server;
  char buffer[1000] = "[client]: hi \n";

  socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  int sockett = socketFileDesc;
  if(socketFileDesc < 0){
    error("couldn't open the socket");
    exit(0);
  }

  // cout << "open to socket w server" << endl; // needed for log file

  server = gethostbyname("127.0.0.1");
  if(server == NULL){
    error("error no host");
    exit(0);
  }

  // cout << "got host by name" << endl; // needed for log file


  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);
  serverAddress.sin_port = htons(port);

  cout << "Connecting to server... ";


  if(connect(socketFileDesc, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
    error("couldn't connect");
  }
  cout << "Connected!" << endl;
  cout <<"Connected to a friend! You send first." << endl;



  // while(true){
  packet_t* pak = (packet_t*)malloc(sizeof(packet_t));
  label:
  printf("You: ");

  cout << buffer << endl;
    //bzero(buffer,1000);
    // fgets(buffer,1000,stdin);
    // buffer = "[client]: hi";

  if(strlen(buffer)>140){
    cout << "error string too long" << endl;
    goto label;
  }


  strcpy(pak->message, buffer);

  pak->messageLength = strlen(pak->message);
  pak->version = 457;

  memcpy(buffer, pak, sizeof(packet_t));

  n = send(socketFileDesc, buffer, sizeof(buffer), 0);

  if(n < 0){
    error("error writing to socket");
  }
  //bzero(buffer, 1000);
  packet_t* pak2;
  n = recv(socketFileDesc, buffer, 1000, 0);
  pak2 = (packet_t*)buffer;

  if (n < 0){
    error("error reading from socket");
  }

  printf("Friend: %s", pak2->message);

  free(pak);

}


//-------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int numRouters;
  string line;
  //reading file for manager
  ifstream managerFile(argv[1]);
  if (managerFile.is_open()) {
    getline(managerFile, line);
    numRouters = stoi(line);      //convert string N to int N
  }

  //thread creation
  std::thread serverThread(server);
  usleep(5000000);
  std::thread createRouterProcesses(createRouter, numRouters);

  managerFile.close();


  string sIdentifier;

  sIdentifier = "Parent Process: ";




    // Code executed by both parent and child.
// myfile << sIdentifier << "Hello world! The pID is: " << getpid() << endl;
  cout << sIdentifier << "Hello world! The pID is: " << getpid() << endl;

// thread synchronizatoin
serverThread.join();              //pauses until first thread finishes
createRouterProcesses.join();     //pauses until second thread finishes

// myfile.close();
return 0;
} 