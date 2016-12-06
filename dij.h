// Program to find Dijkstra's shortest path using
// priority_queue in STL
#include<bits/stdc++.h>
#include<tuple>
#include<fstream>
#include<sys/stat.h>

using namespace std;
# define INF 0x3f3f3f3f

// iPair ==> Integer Pair
typedef pair<int, int> iPair;

// This class represents a directed graph using
// adjacency list representation
class Graph
{
	int V; // No. of vertices
	int node; //the source node

	// In a weighted graph, we need to store vertex
	// and weight pair for every edge
	list< pair<int, int> > *adj;

public:
	Graph(int V, int node); // Constructor
	vector<tuple<int, int, int>> routingTable; // Tuple format: Node, Next Hop Cost 
	// function to add an edge to graph
	void addEdge(int u, int v, int w);

	// prints shortest path from s
	void shortestPath();
	
	//Print to Console and file
	void printToConsole();
	void printToFile(std::ofstream &out);
	
};


