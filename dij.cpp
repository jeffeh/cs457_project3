// Program to find Dijkstra's shortest path using
// priority_queue in STL
#include<bits/stdc++.h>
#include<tuple>
#include<fstream>

using namespace std;
# define INF 0x3f3f3f3f

// iPair ==> Integer Pair
typedef pair<int, int> iPair;

// This class represents a directed graph using
// adjacency list representation
class Graph
{
	int V; // No. of vertices

	// In a weighted graph, we need to store vertex
	// and weight pair for every edge
	list< pair<int, int> > *adj;

public:
	Graph(int V); // Constructor
	vector<tuple<int, int, int>> routingTable; // Tuple format: Node, Next Hop Cost 
	// function to add an edge to graph
	void addEdge(int u, int v, int w);

	// prints shortest path from s
	void shortestPath(int s);
	
	//Print to Console and file
	void printToConsole(std::ofstream &out);
	void printToFile(string File);
	
};

// Allocates memory for adjacency list
Graph::Graph(int V)
{
	this->V = V;
	adj = new list<iPair> [V];
}

void Graph::addEdge(int u, int v, int w)
{
	adj[u].push_back(make_pair(v, w));
	adj[v].push_back(make_pair(u, w));
}

// Prints shortest paths from src to all other vertices
void Graph::shortestPath(int src)
{
	// Create a priority queue to store vertices that
	// are being preprocessed. This is weird syntax in C++.
	// Refer below link for details of this syntax
	// http://geeksquiz.com/implement-min-heap-using-stl/
	priority_queue< iPair, vector <iPair> , greater<iPair> > pq;

	// Create a vector for distances and initialize all
	// distances as infinite (INF)
	vector<int> dist(V, INF);
    
    // Create Path to Vector
    vector<int> currentPathTo;
    vector<vector<int> > pathsToVec (V, currentPathTo);
    pathsToVec[src].push_back(src);

	// Insert source itself in priority queue and initialize
	// its distance as 0.
	pq.push(make_pair(0, src));
	dist[src] = 0;

	/* Looping till priority queue becomes empty (or all
	distances are not finalized) */
	while (!pq.empty())
	{
		// The first vertex in pair is the minimum distance
		// vertex, extract it from priority queue.
		// vertex label is stored in second of pair (it
		// has to be done this way to keep the vertices
		// sorted distance (distance must be first item
		// in pair)
		int u = pq.top().second;
        currentPathTo = pathsToVec[u];
		pq.pop();
        

		// 'i' is used to get all adjacent vertices of a vertex
		list< pair<int, int> >::iterator i;
		for (i = adj[u].begin(); i != adj[u].end(); ++i)
		{
			// Get vertex label and weight of current adjacent
			// of u.
			int v = (*i).first;
			int weight = (*i).second;

			// If there is shorted path to v through u.
			if (dist[v] > dist[u] + weight)
			{
				// Updating distance of v
				dist[v] = dist[u] + weight;
                pathsToVec[v] = currentPathTo;
                pathsToVec[v].push_back(v);
				pq.push(make_pair(dist[v], v));
			}
		}
	}
	int node, nextHop, cost;
	for (int i = 0; i < V; ++i){
		node = i;
		if (pathsToVec[i].size() < 1)
            		nextHop = NULL;
        	else if (pathsToVec[i].size() < 2)
        		nextHop = pathsToVec[i][0];
        	else
			nextHop = pathsToVec[i][1];
		cost = dist[i];
		routingTable.push_back(tuple<int, int, int> (i, nextHop, cost));
	}

}

void Graph::printToConsole(std::ofstream &out){
	int node, nextHop, cost;
	for (int i = 0; i <V; ++i){
		node =  std::get<0>(routingTable[i]);
		nextHop =  std::get<1>(routingTable[i]);
		cost =  std::get<2>(routingTable[i]);
		printf("For node: %d \t\t NextHop is: %d \t\t Cost: %d\n", node, nextHop, cost);
		out << "For node: " << node << "\t\t Next hop is: " << nextHop << "\t\t Cost: " << cost << endl;	
	}
}

void Graph::printToFile(string File){
	cout << "NOT IMPLEMENTED YET!" << endl;
}

// Driver program to test methods of graph class
int main()
{

	ofstream out;
	out.open("dij.out");


	// create the graph given in above fugure
	int V = 10;
	Graph g(V);

	// making above shown graph
	g.addEdge(0, 9, 40);
	g.addEdge(0, 4, 60);
	g.addEdge(0, 2, 40);
	g.addEdge(1, 8, 70);
	g.addEdge(2, 6, 70);
	g.addEdge(3, 9, 20);
	g.addEdge(3, 8, 70);
	g.addEdge(3, 5, 70);
	g.addEdge(4, 7, 70);
	g.addEdge(4, 6, 40);
	g.addEdge(5, 9, 70);
	g.addEdge(5, 7, 40);
	g.addEdge(7, 9, 60);
	g.addEdge(8, 9, 70);

	g.shortestPath(5);
	
	g.printToConsole(out);
	return 0;
}
