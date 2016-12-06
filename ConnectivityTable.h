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
class Router {
public:
	int myID;
	std::vector<tuple<int, int>> connections;
	int udpPort;
	int sockFD;
};

class Neighbor {
public:
	sockaddr_in sin_other;
	int fileDesc;
};
