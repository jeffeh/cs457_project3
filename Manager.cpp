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
		if(line.compare("-1")){
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
int main(int argc, char* argv[]){
	clearFile();
	readFile("in.txt");
	printMessage("hello");
	printMessage("world");
	int socketFileDesc = setUpServer(20000);
	int clientSock = listenAndAccept(socketFileDesc);
	string port = receiveMessage(clientSock);
	printMessage("Got Message")
	printMessage(port);
}
