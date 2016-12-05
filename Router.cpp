/*
 * Router.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: wilsonno
 */
#include <iostream>
#include <algorithm>
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
#include <thread>
#include <cstring>
#include <arpa/inet.h>
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
tuple<int,int> createUDP(){
	int port;
	struct sockaddr_in serverAddress, client;
		int socketFileDesc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(socketFileDesc < 0){
			error("could not open the socket");
		}
		bzero((char *) &serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = 0;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		socklen_t  l = sizeof(client);
		if(bind(socketFileDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
			error("could not bind");
		}
		socklen_t len = sizeof(serverAddress);
		if (getsockname(socketFileDesc, (struct sockaddr *)&serverAddress, &len) == -1)
		    perror("getsockname");
		else
		    port = ntohs(serverAddress.sin_port);
		tuple<int,int> ret;
		get<0>(ret) = socketFileDesc;
		get<1>(ret) = port;
		return ret;

}
void sendMessage(int sock, char mes[1200]){

	if(send(sock, mes, 1200, 0)<0){
		error("couldn't write to socket");
	}

}
string receiveMessage(int sock){
	char buffer[1200];
	int n = recv(sock, buffer, sizeof(buffer), 0);
	if(n<0){
		error("couldn't read from socket");
	}

	return buffer;
}

void ackin(int sockFD){
	sockaddr t;
	size_t l = sizeof(t);
	char buf[512];

	recvfrom(sockFD, buf, sizeof(buf), 0, (sockaddr*)&t, (unsigned int*)&l);

	cout << buf << endl;
	string b = buf;
	if(b.compare("ack")==0){
		cout << "got Ack" << endl;
	}
}
void ackout(int sockFD){
	char buf[512];
	strcpy(buf, "ack");
	send(sockFD, buf, sizeof(buf), 0);

}
void do_join(std::thread& t){
    t.join();
}

void join_all(std::vector<std::thread>& v){
    std::for_each(v.begin(),v.end(),do_join);
}
vector<thread> ts;
int connectToUdp(tuple<int,int,int> t){
	return connectToManager(const_cast<char*>(getmyip().c_str()), get<2>(t));
}
void startUDPListening(int socketFileDesc, vector<tuple<int,int,int>> links){
	int counter = 0;
	while(counter < links.size()){
		//listen(socketFileDesc, 5);
		//struct sockaddr_in clientAddress;
		//unsigned int sizeOfAddrClient = sizeof(clientAddress);
		//int newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient);
		ts.push_back(thread(ackin, socketFileDesc));

		counter++;
	}
	join_all(ts);

}


void parseInfo(string info, int& myID, vector<tuple<int,int,int>>& links){
	istringstream f(info);
	string line;
	int ln = 0;
	while(getline(f, line)){
		if(line == "")
			break;
		else if(ln==0){
			myID = atoi(line.c_str());
			ln++;
		}
		else{
			int A,B,C;
			istringstream iss(line);
			if(!(iss >> A >> B >> C)){

			}
			tuple<int,int,int> link;
			get<0>(link) = A;
			get<1>(link) = B;
			get<2>(link) = C;
			ln++;
			links.push_back(link);
		}
	}
}
int main(int argc, char*argv[]){
	cout << "Router process created!" << endl;

//Create a UDP port
	int myID = -1;
	vector<tuple<int,int,int>> links;
	tuple<int,int> udp = createUDP();

	int sockudp = get<0>(udp);
	int port = get<1>(udp);
	//connect to manager
	int sockman = connectToManager(const_cast<char*>(getmyip().c_str()), 20000);

	string p = to_string(port);
	p += "\nrequest";
	char* mes = const_cast<char*>(p.c_str());
	sendMessage(sockman, mes);
	string info = receiveMessage(sockman);
	printMessage(info);

	parseInfo(info, myID, links);

	string ready = receiveMessage(sockman);
	cout << ready << endl;

	thread t1(startUDPListening, sockudp, links);
	for(tuple<int,int,int> link:links){
//		int sockD = connectToUdp(link);
		struct sockaddr_in si_other;
		int s,i,slen=sizeof(si_other);
		char buf[512];

		 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		     error("socket");

		 memset((char *) &si_other, 0, sizeof(si_other));
		 si_other.sin_family = AF_INET;
		 si_other.sin_port = htons(get<2>(link));

		 if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
			 fprintf(stderr, "inet_aton() failed\n");
		     exit(1);
		 }
		 sendto(s,"ack",strlen("ack"),0,(struct sockaddr*)&si_other, sizeof(si_other));
		//ackout(s);
	}



	t1.join();



}
