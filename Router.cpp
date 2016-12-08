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
#include <project3.h>
#include "dij.h"
#include <unistd.h>
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
int myID = -1;
void printMessage(string message){

	static int i = 0;
	static vector<string> timebuff;
	static vector<string> messbuff;
	string time = currentDateTime();
	string out =  time + " [Router "+to_string(myID)+"]: " + message + "\n";
	if(myID == -1){
		timebuff.push_back(time);
		messbuff.push_back(message);
		return;
	}
	else{
		if(i==0){
			ofstream myfile;
			string fileName = "router"+to_string(myID)+".out";

			myfile.open(const_cast<char*>(fileName.c_str()), ios::app);
			for(unsigned int j=0; j<timebuff.size(); j++){
				string bout = timebuff[j] + " [Router "+to_string(myID)+"]: "+messbuff[j]+"\n";
				myfile << bout;
			}
			myfile.close();
			i++;
		}
		else{

		}

	}

	cout << out;
	ofstream myfile;
	string fileName = "router"+to_string(myID)+".out";

	myfile.open(const_cast<char*>(fileName.c_str()), ios::app);
	myfile << out;
	myfile.close();
}
void clearFile(){
	if(myID == -1)
		return;

	const string fileName = "router"+to_string(myID)+".out";

	if(fileExists(fileName)){
		remove(fileName.c_str());
	}
}
void error(const string msg)
{
 perror((msg.c_str()));
 exit(1);
}
string getLSP(int N, vector<tuple<int,int,int>> links);
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
	printMessage("Connecting to Manager... ");
	if(connect(socketFileDesc, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
		perror("couldn't connect");
	}
	printMessage("Connected to Manager");
	//sockett = socketFileDesc;
	return socketFileDesc;
}
int FT::getRouteTo(int i){
	for(tuple<int,int,int> edge:this->table){
		if(get<0>(edge)==i){
			return get<1>(edge);
		}
	}
	return -1;
}
int FT::getCostTo(int i){
	for(tuple<int,int,int> edge:this->table){
		if(get<0>(edge)==i){
			return get<2>(edge);
		}
	}
	return -1;
}
tuple<int,int> createUDP(){
	int port;
	struct sockaddr_in serverAddress;
		int socketFileDesc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(socketFileDesc < 0){
			error("could not open the socket");
		}
		bzero((char *) &serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = 0;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		//socklen_t  l = sizeof(client);
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
		string mes = "Created UPD Port on ";
		mes += to_string(port);
		printMessage(mes);
		return ret;

}
void sendMessage(int sock, const char mes[1200]){

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

void ackin(int sockFD, int id){
	sockaddr t;
	size_t l = sizeof(t);
	char buf[512];

	recvfrom(sockFD, buf, sizeof(buf), 0, (sockaddr*)&t, (unsigned int*)&l);


	string b = buf;
	if(b.compare("ack")==0){
		string ot = "Got ack from Router ";
		ot += to_string(id);
		printMessage(ot);
	}
}
void parseInfo(string info, int& myID, vector<tuple<int,int,int>>& links);
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
	unsigned int counter = 0;
	while(counter < links.size()){
		//listen(socketFileDesc, 5);
		//struct sockaddr_in clientAddress;
		//unsigned int sizeOfAddrClient = sizeof(clientAddress);
		//int newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient);
		ts.push_back(thread(ackin, socketFileDesc, get<0>(links[counter])));

		counter++;
	}
	join_all(ts);


}



//void sendOwnLSP(vector<Neighbor> nett){
//	for(Neighbor n:nett){
//
//	}
//}
//void getNeighborsLSPs(int sock, vector<Neighbor> nett){
//	for(Neighbor n:nett){
//		sockaddr t;
//		size_t l = sizeof(t);
//		char buf[512];
//
//		recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&nett[i].sin_other)
//	}
//}





void dij(vector<tuple<int, vector<tuple<int,int,int>>>> LSPs, int myID);


void sendLSPToNeighbors(int socketFileDesc,string lsp, vector<tuple<int,int,int>> links, vector<Neighbor> net, int doNot){
	int i=0;
	for(Neighbor n:net){
		int N;
		vector<tuple<int,int,int>> links2;
		parseInfo(lsp, N, links2);


		string tos = to_string(myID)+"\n"+lsp;
		char* l = const_cast<char*>(tos.c_str());
		char buf[512];
		strcpy(buf, l);
		if(N != get<0>(links[i]) && get<0>(links[i])!=doNot){
			printMessage(string("Sending/Forwarding LSP for Router "+to_string(N)+ " to Router "+to_string(get<0>(links[i]))));
		}

		sendto(n.fileDesc, buf, sizeof(buf),0,(struct sockaddr*)&n.sin_other, sizeof(n.sin_other));
		i++;
	}
}
vector<tuple<int, vector<tuple<int,int,int>>>> LSPs;
void startUDPrecLSP(int myID, int socketFileDesc, vector<tuple<int,int,int>> linkss, vector<Neighbor> net, unsigned int sizeOfNetwork){

	while(LSPs.size()<(sizeOfNetwork-1)){
		sockaddr t;
		size_t l = sizeof(t);
		char buf[512];

		recvfrom(socketFileDesc, buf, sizeof(buf), 0, (sockaddr*)&t, (unsigned int*)&l);
		string io = buf;
		string src = io.substr(0,1);
		string inf = io.substr(2);


		int N; vector<tuple<int,int,int>> links = {};
		parseInfo(inf,N,links);
		int tobreak = 0;
		if(N==myID)
			tobreak = 1;
		for(unsigned int i=0; i<LSPs.size(); i++){
			if(get<0>(LSPs[i])==N){
				tobreak = 1; break;
			}
			else{

			}
		}
		if(tobreak){
			continue;
		}
		else{

			tuple<int, vector<tuple<int,int,int>>> ls;
			get<0>(ls) = N;
			get<1>(ls) = links;
			//if(N != atoi(src.c_str())){
				printMessage(string("Received LSP for Router "+to_string(N)+" from Router "+src	));
			//}

			LSPs.push_back(ls);
			sendLSPToNeighbors(socketFileDesc, getLSP(N, links), linkss, net, atoi(src.c_str()));
		}

	}
	string out = "Router " + to_string(myID) + " has received LSPs for: [";
	for(unsigned int i=0; i<LSPs.size(); i++){
		out += to_string(get<0>(LSPs[i]));
		if(i<LSPs.size()-1){
			out += ", ";
		}

	}
	out += "]";



}
void fwdRouterMessage(vector<tuple<int,int,int>> links, vector<Neighbor> nett, int dest, int orig);
void sendRouterMessage(vector<tuple<int,int,int>> links, vector<Neighbor> nett, int dest);
void awaitRouterMessages(vector<tuple<int,int,int>> links, vector<Neighbor> nett, int sockudp, int myID){

	while(true){

	sockaddr t;
	size_t l = sizeof(t);
	char buf[512];

	recvfrom(sockudp, buf, sizeof(buf), 0, (sockaddr*)&t, (unsigned int*)&l);

	string inf = buf;
	if(inf.length()>3){
		continue;
	}
	string orig = inf.substr(1,1);
	string sender = inf.substr(0,1);
	string destt = inf.substr(2,1);


	if(myID == atoi(destt.c_str())){
		string out = "Received a packet from Router ";
		out += sender;
		out += ", originally from Router ";
		out += orig;
		out += ". This is the packets destination.";
		printMessage(out);
		continue;
	}
	printMessage("Received a packet from Router "+sender+", forwarding to Router "+destt);
	int dest = atoi(destt.c_str());


	fwdRouterMessage(links, nett, dest, atoi(orig.c_str()));
	}
}
void awaitInstructions(int myID, int sockudp, int sockman, vector<tuple<int,int,int>> links, vector<Neighbor> nett){
	int sent = 1;
	while(sent){

		string inst = receiveMessage(sockman);
		if(inst.compare("Quit")==0){

			sent = 0;
			printMessage("Exiting!");
			close(sockudp);
			close(sockman);
			exit(0);
			break;
		}
		else{
			int dest = atoi(inst.c_str());
			printMessage("has been instructed to send a packet to "+to_string(dest));
			sendRouterMessage(links, nett, dest);
			//send message over udp to dest
		}
	}

}
FT ft;
void sendRouterMessage(vector<tuple<int,int,int>> links, vector<Neighbor> nett, int dest){
	int i = 0;
	int p = ft.getRouteTo(dest);
	for(tuple<int,int,int> t:links){
		if(p==get<0>(t)){
			break;
		}
		i++;
	}
	string k = to_string(myID) + to_string(myID) + to_string(dest);
	char* l = const_cast<char*>(k.c_str());


	char buf[512];
	strcpy(buf, l);

	int n = sendto(nett[i].fileDesc, buf, sizeof(buf),0,(struct sockaddr*)&nett[i].sin_other, sizeof(nett[i].sin_other));
	if(n<0)
		error("got stuck");
}
void fwdRouterMessage(vector<tuple<int,int,int>> links, vector<Neighbor> nett, int dest, int orig){

	int i = 0;
	int p = ft.getRouteTo(dest);
	for(tuple<int,int,int> t:links){
		if(p==get<0>(t)){
			break;
		}
		i++;
	}

	string k = to_string(myID) + to_string(orig) + to_string(dest);
	char* l = const_cast<char*>(k.c_str());


	char buf[512];
	strcpy(buf, l);

	int n = sendto(nett[i].fileDesc, buf, sizeof(buf),0,(struct sockaddr*)&nett[i].sin_other, sizeof(nett[i].sin_other));
	if(n<0)
		error("got stuck");


}


string getLSP(int N, vector<tuple<int,int,int>> links){
	string ret;
	ret += to_string(N);
	ret += "\n";
	for(tuple<int,int,int>link:links){
		ret += to_string(get<0>(link)) +" "+to_string(get<1>(link))+" "+to_string(get<2>(link))+"\n";
	}

	return ret;
}

void dij(vector<tuple<int, vector<tuple<int,int,int>>>> LSPs, int myID){
	Graph g(LSPs.size()+1, myID);
	for(tuple<int, vector<tuple<int,int,int>>> LSP:LSPs){
		for(tuple<int,int,int> edge:get<1>(LSP)){
			g.addEdge(get<0>(LSP), get<0>(edge), get<1>(edge));
		}
	}
	g.shortestPath();
	string filename = "router";
	filename += to_string(myID)+".out";
	char* file = const_cast<char*>(filename.c_str());
	ofstream out;out.open(file, std::ios_base::app);
	printMessage(g.printToFile(out));
	ft.table = g.routingTable;
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
	printMessage("Router Process Created!");

//Create a UDP port
	vector<tuple<int,int,int>> links;
	tuple<int,int> udp = createUDP();

	int sockudp = get<0>(udp);
	int port = get<1>(udp);
	//connect to manager
	int sockman = connectToManager(const_cast<char*>(getmyip().c_str()), 20000);

	string p = to_string(port);
	char* mes = const_cast<char*>(p.c_str());
	sendMessage(sockman, mes);
	string info = receiveMessage(sockman);
	//printMessage(info);
	string mess = "Router received it's ID from the Manager. ID is ";
	mess += info[0];
	printMessage(mess);
	parseInfo(info, myID, links);

	string ready = receiveMessage(sockman);
	string mess2;
	mess2 = "Received confirmation from manager that it's safe to reach out to neighbors.";
	string io = "Reaching out ack to Neighbors";
	printMessage(mess2);
	printMessage(io);

	thread t1(startUDPListening, sockudp, links);
	vector<Neighbor> nett;

	for(tuple<int,int,int> link:links){
//		int sockD = connectToUdp(link);
		struct sockaddr_in si_other;
		int s,i;


		 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		     error("socket");

		 memset((char *) &si_other, 0, sizeof(si_other));
		 si_other.sin_family = AF_INET;
		 si_other.sin_port = htons(get<2>(link));

		 if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
			 fprintf(stderr, "inet_aton() failed\n");
		     exit(1);
		 }

		 i++;
		 string ot = "Sending ACK to Router ";
		 ot += to_string(get<0>(link));
		 printMessage(ot);
		 sendto(s,"ack",strlen("ack"),0,(struct sockaddr*)&si_other, sizeof(si_other));
		//ackout(s);
		 Neighbor nn;

		 nn.fileDesc = s;
		 nn.sin_other = si_other;
		 nett.push_back(nn);
	}



	t1.join();
	const char* tos = "All acks recieved";
	sendMessage(sockman, tos);
	string l = receiveMessage(sockman);
	printMessage("Received confirmation from the Manager that network is set up. Sending LSPs");
	int sizeOfNetwork = atoi(receiveMessage(sockman).c_str());

	vector<tuple<int, vector<tuple<int,int,int>>>> d;
	thread t2(startUDPrecLSP,myID, sockudp, links, nett, sizeOfNetwork);
	sendLSPToNeighbors(sockudp, getLSP(myID, links), links, nett, -1);

	t2.join();
	printMessage("Received all LSPs. Generating table...");
	dij(LSPs, myID);
	printMessage("Awaiting Instructions from Manager/Other Routers");
	thread t3(awaitRouterMessages, links, nett, sockudp, myID);
	thread t4(awaitInstructions,myID, sockudp, sockman, links, nett);
	t3.join();
	t4.join();
//	if(myID==5)
//		printLSPs(LSPs);


}
