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
using namespace std;
void error(const string msg)
{
 perror((msg.c_str()));
 exit(1);
}
vector<tuple<int,int,int>> readFile(string fileName){

	vector<tuple<int,int,int>> ret;
	string line;
	ifstream infile(fileName);
	int ln = 0;
	while(getline(infile, line)){
		if(line.compare("-1")==0){
			cout << "E"<<endl;
			break;
		}
		tuple<int,int,int> h;
		istringstream iss(line);
		if(ln==0){
			int N;
			if(!(iss >> N)){ cout << "first line wrong" << endl;break; }
			cout << N << endl;
			ln++;
		}
		else{
			int A,B,C;
			if(!(iss >> A >> B >> C)){ cout << "line "<<ln<<" wrong"<< endl;break; }
			cout << A << ","<<B<<","<<C<<endl;
			ln++;
			get<0>(h)=A;
			get<1>(h)=B;
			get<2>(h)=C;
			ret.push_back(h);
		}

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
	cout << "RR:" <<in.size()<<endl;
	if(in.size() == 0){
		return out;
	}
	int c = get<0>(in[0]);
	std::unordered_set<size_t> y;

	for(int i=0; i<in.size(); i++){
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
	if(fileExists("manager.out")){
		remove("manager.out");
	}
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
		cout <<"0: "<<to_string(get<1>(t))<<endl;
		cout <<"1: "<<get<2>(t)<<endl;
		ret += " "+to_string(get<2>(t));
		ret +="\n";
	}

	return ret;
}
void sendMessage(int sock, char mes[1200]){
	if(send(sock, mes, 1200, 0)<0){
		error("couldn't write to socket");
	}

}

int main(int argc, char* argv[]){
	clearFile();
	vector<tuple<int,int,int>> r = readFile("in.txt");
	printMessage("hello");
	printMessage("world");
	int socketFileDesc = setUpServer(20000);
	int clientSock = listenAndAccept(socketFileDesc);
	string port = receiveMessage(clientSock);
	printMessage("Got Message");
	printMessage(port);

	vector<tuple<int,int,int>> tos = popRouter(r);
	cout << "NN:" << tos.size()<< endl;
	if(r.size() != 0){
		string s = convertTupToMes(tos);
		printMessage(s);
		char* c = const_cast<char*>(s.c_str());
		cout << c << endl;
		sendMessage(clientSock, c);
	}
	string rr = receiveMessage(clientSock);

	//code that waits for other routers to be ready

	sendMessage(clientSock, "Safe!");


}
