/*
 * Router.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: wilsonno
 */
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <iterator>
#include <vector>
#include <stdio.h>
#include <memory>
using namespace std;
typedef struct packet{
	char mes[1200];
}pak;


string getmyip(){
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
	    return result;
}
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}
void printMessage(string message){
	string out =  currentDateTime() + " [Router]: " + message + "\n";
	cout << out;
	ofstream myfile;
	myfile.open("router.out", ios::app);
	myfile << out;
	myfile.close();
}
void clearFile(){
	if(fileExists("router.out")){
		remove("router.out");
	}
}
void error(const string msg)
{
 perror((msg.c_str()));
 exit(1);
}
int connectToManager(char* hostname, int port){
	int socketFileDesc;
	struct sockaddr_in serverAddress;
	struct hostent *server;
	socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
	//sockett = socketFileDesc;
	if(socketFileDesc < 0){
		perror("couldn't open the socket");
		exit(0);
	}
	server = gethostbyname(getmyip().c_str());
	if(server == NULL){
		perror("error no host");
		exit(0);
	}
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);
	serverAddress.sin_port = htons(port);
	printMessage("Connecting to server... ");
	if(connect(socketFileDesc, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
		perror("couldn't connect");
	}
	printMessage("Connected!");
	//sockett = socketFileDesc;
	return socketFileDesc;
}
int createUDP(){
	struct sockaddr_in serverAddress;
	int port = 19999;
		int socketFileDesc = socket(AF_INET, SOCK_DGRAM, 0);

		if(socketFileDesc < 0){
			error("could not open the socket");
		}
		bzero((char *) &serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		serverAddress.sin_addr.s_addr = INADDR_ANY;

		if(bind(socketFileDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
			error("could not bind");
		}
		return socketFileDesc;

}
void sendMessage(int sock, char mes[1200]){
	if(send(sock, mes, 1200, 0)<0){
		error("couldn't write to socket");
	}

}

int main(int argc, char*argv[]){
	//Create a UDP port
	int sockudp = createUDP();
	//connect to manager
	int sockman = connectToManager(const_cast<char*>(getmyip().c_str()), 20000);
	char* mes = "19999\nrequest";
	sendMessage(sockman, mes);
	while(1);
}
