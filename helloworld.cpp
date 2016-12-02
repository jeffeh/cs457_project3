#include <iostream>
#include <string>
#include <fstream>

// Required by for routine
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdlib.h>   // Declaration for exit()

using namespace std;

int globalVariable = 2;

void DoWorkInChild(int &count, ofstream &file ) {

  string childProcessName = "Child Process: ";

  file << count << " " << childProcessName << "Hello world! The pID is: " << getpid() << endl;  
  cout << count << " " << "finished child function" << endl;
}

main()
{
 string sIdentifier;

 // creating file
 ofstream myfile;
 myfile.open("pid_values.txt");


 pid_t pID[5];


 //creating N processes
 for (int i = 0; i < 5; i++) {
  if ((pID[i] = fork()) < 0) {
    perror("fork");
    abort();
  } else if (pID[i] == 0) { // child process
    DoWorkInChild(i, myfile);


    int childExitStatus;
    pid_t ws = waitpid(pID[i], &childExitStatus, WNOHANG);

    if (WIFEXITED(childExitStatus)) {
      cout << "\t" << i <<  " hild terminated normally" << endl;
    }
    exit(0);
  }
}

//parent process runs below this comment
//creating server here

int socketFileDesc;
int newSocketFileDesc;

unsigned int sizeOfAddrClient;
int n;
char buffer[1000];
struct sockaddr_in serverAddress, clientAddress;

socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
sockett = socketFileDesc;
if(socketFileDesc < 0){
  error("could not open the socket");
}

cout << "created socket" << endl;

bzero((char *) &serverAddress, sizeof(serverAddress));
serverAddress.sin_port = htons(port);
serverAddress.sin_addr.s_addr = INADDR_ANY;

if(bind(socketFileDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
  error("could not bind");
}

cout << "server bound to socket" << endl;

listen(socketFileDesc, 5);
cout << "Waiting for connection on ";
getmyip();
cout << " port ";
cout << port << endl;


sizeOfAddrClient = sizeof(clientAddress);
newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient);
sockett2 = newSocketFileDesc;
if(newSocketFileDesc < 0){
  error("error on acceptance");
}
cout << "Found a friend! You recieve first." << endl;



//wait 
int count = 0;
int status;
pid_t childPID;
while (count < 5) {
  childPID = wait(&status);
  printf("Child with PID %ld exited with status 0x%x \n", (long)childPID, status);
  count++;
}




      // Code only executed by parent process
sIdentifier = "Parent Process: ";




    // Code executed by both parent and child.
myfile << sIdentifier << "Hello world! The pID is: " << getpid() << endl;
cout << sIdentifier << "Hello world! The pID is: " << getpid() << endl;



myfile.close();

}