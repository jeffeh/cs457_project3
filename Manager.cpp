#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <ctime>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unordered_set>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>
#include "project3.h"
#include <thread>
#include <condition_variable>
#include <chrono>
using namespace std;
void error(const string msg)
{
 perror((msg.c_str()));
 exit(1);
}
vector<tuple<int, int>> pacToSend;
vector<tuple<int,int,int>> readFile(string fileName, int& N){

	vector<tuple<int,int,int>> ret;
	string line;
	ifstream infile(fileName);
	int ln = 0;
	while(getline(infile, line)){
		if(line.compare("")==0){
			continue;
		}
		if(line.compare("-1")==0){
			
			break;
		}
		tuple<int,int,int> h;
		istringstream iss(line);
		if(ln==0){
			if(!(iss >> N)){ cout << "first line wrong" << endl;break; }
			//cout << N << endl;
			ln++;
		}
		else{
			int A,B,C;
			if(!(iss >> A >> B >> C)){ cout << "line "<<ln<<" wrong"<< endl;break; }
			//cout << A << ","<<B<<","<<C<<endl;
			ln++;
			get<0>(h)=A;
			get<1>(h)=B;
			get<2>(h)=C;
			ret.push_back(h);
		}

	}
	//read src-dest packets
	while (getline(infile, line)) {
		if(line.compare("")==0){
					continue;
		}
		if (line.compare("-1")==0) {
			break;
		} else {
			istringstream iss(line);
			int src, dst;
			tuple<int,int> p;
			if (!(iss >> src >> dst)){ cout << "line " << ln << " wrong" << endl;break;}
			ln++;
			get<0>(p)=src;
			get<1>(p)=dst;
			pacToSend.push_back(p);
			//cout << "--------src is: " << src << " -------------dst is: " << dst << endl;
		}
	}
	return ret;
}
vector<Router> net;
vector<Router> parseTup(int N, vector<tuple<int,int,int>> c){
	vector<Router> ret;
	for(int i=0; i<N; i++){
		Router r;
		r.myID = i;
		for(tuple<int,int,int> j:c){
			if(get<0>(j)==i){
				tuple<int,int> con;
				get<0>(con) = get<1>(j);
				get<1>(con) = get<2>(j);
				r.connections.push_back(con);
			}
			else if(get<1>(j)==i){
				tuple<int,int> con;
				get<0>(con) = get<0>(j);
				get<1>(con) = get<2>(j);
				r.connections.push_back(con);
			}
						
		}
		ret.push_back(r);
	}
	return ret;
}


template <typename Type>
void erase_indices(
        const std::unordered_set<size_t>& indices_to_erase,
        std::vector<Type>& vec) {
    std::vector<bool> erase_index(vec.size(), false);
    for (const size_t i: indices_to_erase) {
        erase_index[i] = true;
    }
    std::vector<bool>::const_iterator it_to_erase = erase_index.begin();
    typename std::vector<Type>::iterator it_erase_from = std::remove_if(
        vec.begin(), vec.end(),
        [&it_to_erase](const Type& whatever) -> bool {
          return *it_to_erase++ == true;
        }
    );
    vec.erase(it_erase_from, vec.end());
    vec.shrink_to_fit();
}
vector<tuple<int, int, int>> popRouter(vector<tuple<int,int,int>>& in){
	vector<tuple<int,int,int>> out;
	//cout << "RR:" <<in.size()<<endl;
	if(in.size() == 0){
		return out;
	}
	int c = get<0>(in[0]);
	std::unordered_set<size_t> y;

	for(unsigned int i=0; i<in.size(); i++){
		if(get<0>(in[i])==c){
			out.push_back(in[i]);
			y.insert(i);
		}
	}
	erase_indices(y, in);


	return out;
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
	string out =  currentDateTime() + " [Manager]: " + message + "\n";
	cout << out;
	ofstream myfile;
	myfile.open("manager.out", ios::app);
	myfile << out;
	myfile.close();
}
void clearFile(){
//	if(fileExists("manager.out")){
//		remove("manager.out");
//	}
	system("rm -f *.out");
}

int setUpServer(int port){





	struct sockaddr_in serverAddress;

	int socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);

	if(socketFileDesc < 0){
		error("could not open the socket");
	}
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if(bind(socketFileDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
		error("could not bind");
	}
	return socketFileDesc;
}
int listenAndAccept(int socketFileDesc){
	listen(socketFileDesc, 5);
	struct sockaddr_in clientAddress;
	unsigned int sizeOfAddrClient = sizeof(clientAddress);
	int newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient);

	if(newSocketFileDesc < 0){
		error("error on acceptance");
	}
	printMessage("Connection recieved.");
	return newSocketFileDesc;
}
string receiveMessage(int sock){
	char buffer[1200];
	int n = recv(sock, buffer, sizeof(buffer), 0);
	if(n<0){
		error("couldn't read from socket");
	}
	return buffer;
}
string convertTupToMes(vector<tuple<int, int, int>> in){
	string ret = "";
	ret += to_string(get<0>(in[0]))+"\n";

	for(tuple<int,int,int> t:in){
		ret += to_string(get<1>(t));
		ret += " "+to_string(get<2>(t));
		ret +="\n";
	}

	return ret;
}
void sendMessage(int sock, const char mes[1200]){
	if(send(sock, mes, 1200, 0)<0){
		error("couldn't write to socket");
	}

}


pid_t* createNetwork(vector<Router> net){
	pid_t* pID = new pid_t[net.size()];
	for(unsigned int i=0; i<net.size(); i++){
		pID[i] = fork();
		if(pID[i]==0){
			char *args[] = {(char*)0};
			execvp("router", args);
			exit(0);
		}
	}
	return pID;
}
void do_join(std::thread& t){
    t.join();
}

void join_all(std::vector<std::thread>& v){
    std::for_each(v.begin(),v.end(),do_join);
}
std::condition_variable cv;
std::mutex cv_m;
int i=0;
void startRouterConnection(int sockFD, Router r){
	string port = receiveMessage(sockFD);
	string top = "Got Message from router ";
	top += to_string(r.myID);
	top += ", port number is: ";
	top += port;
		printMessage(top);
		r.udpPort = atoi(port.c_str());
		net[r.myID].udpPort = atoi(port.c_str());
		string s = "";
		s += to_string(r.myID);
		s += "\n";
		
		
		
		//wait for all routers to return udp port
		{
		std::unique_lock<std::mutex> lk(cv_m);
		cv.wait(lk, []{return i==1;});
		}
		for(unsigned int i=0;i<r.connections.size(); i++){
			s+= to_string(get<0>(r.connections[i])); s+= " "; s+= to_string(get<1>(r.connections[i])); 
			s+= " ";
			s+= to_string(net[get<0>(r.connections[i])].udpPort);
			s+="\n";
		}
		//cout << s<<endl;
		char* mes = const_cast<char*>(s.c_str());
		sendMessage(sockFD, mes);
		
		
}

void startServer(int N, vector<Router> net){
	int counter = 0;
	
	int socketFileDesc = setUpServer(20000);
	vector<thread> ts;
	while(counter<N){
		listen(socketFileDesc, 5);
		struct sockaddr_in clientAddress;
		unsigned int sizeOfAddrClient = sizeof(clientAddress);
		int newSocketFileDesc = accept(socketFileDesc, (struct sockaddr *) &clientAddress, &sizeOfAddrClient);
		net[counter].sockFD = newSocketFileDesc;
		ts.push_back(std::thread(startRouterConnection, newSocketFileDesc, net[counter]));
		counter++;
	}

	 
	    std::this_thread::sleep_for(std::chrono::seconds(1));
	 
	    {
	        std::lock_guard<std::mutex> lk(cv_m);
	        i = 1;
	    }
	    cv.notify_all();
	    
	    
	
	join_all(ts);
	
	for(unsigned int i=0;i<net.size();i++){
		sendMessage(net[i].sockFD, "Ready!");
	}
	
	printMessage("All routers are set up.");
	for(unsigned int i=0; i<net.size(); i++){
		string r = receiveMessage(net[i].sockFD);
		string p = r;
		p+=" for router ";
		p+=to_string(net[i].myID);
		printMessage(p);
	}
	printMessage("\nAll Routers have ack'd with each other\n");
	printMessage("Instructing Routers to begin send LSPs");
	for(unsigned int i=0; i<net.size(); i++){
		string out = "Telling Router ";
		out += to_string(i);
		out += " to send it's LSP";
		printMessage(out);
		sendMessage(net[i].sockFD, "begin LSP");
		sendMessage(net[i].sockFD, const_cast<char*>(to_string(net.size()).c_str()));
	}
	sleep(5);
	printMessage("\nAll routers have all LSPs\n");
	printMessage("Beginning to forward packets");
	for(tuple<int,int> packet:pacToSend){
		
		int i = get<0>(packet);
		
		char* toSend = const_cast<char*>(to_string(get<1>(packet)).c_str());
		string out = "Telling Router ";
		out += to_string(i);
		out += " to send a packet to Router ";
		out+=toSend;
		printMessage(out);
		sendMessage(net[i].sockFD, toSend);
		printMessage("Sleeping for 5 seconds...");
		sleep(5);
	}
	for(unsigned int i=0; i<net.size(); i++){
		string out = "Telling Router ";
		out += to_string(i);
		out += " to quit.";
		printMessage(out);
		const char* tos = "Quit";
		sendMessage(net[i].sockFD, tos);
	}
	
}

int main(int argc, char* argv[]){
	if(argc != 2){
		cerr << "Usage: ";
		cerr << argv[0] << " [input file]" << endl;
		exit(-1);
	}
	clearFile();
	
	int N;
	string rf = "Reading file ";
	rf += argv[1];
	printMessage(rf);
	vector<tuple<int,int,int>> r = readFile(argv[1], N);
	net = parseTup(N,r);
	//printNetwork(net);

	thread t1 = thread(startServer, N, net);
	pid_t* pids = createNetwork(net);
	for(int i=0; i<N; i++){
		 int status;
		 waitpid(pids[i], &status, 0);
	}
	t1.join();
	
	printMessage("All children have exited.");
	free(pids);
	printMessage("Exiting!");

}
