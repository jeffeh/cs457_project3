#include "dij.h"
#include <string>
using namespace std;
// Allocates memory for adjacency list
Graph::Graph(int V, int node)
{
	this->V = V;
	this->node = node;
	adj = new list<iPair> [V];
}

void Graph::addEdge(int u, int v, int w)
{
	adj[u].push_back(make_pair(v, w));
	adj[v].push_back(make_pair(u, w));
}

// Prints shortest paths from src to all other vertices
void Graph::shortestPath()
{
	int src = this->node;
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
	int nextHop, cost;
	for (int i = 0; i < V; ++i){
		
		if (pathsToVec[i].size() < 1)
            		nextHop = -1;
        	else if (pathsToVec[i].size() < 2)
        		nextHop = pathsToVec[i][0];
        	else
			nextHop = pathsToVec[i][1];
		cost = dist[i];
		routingTable.push_back(tuple<int, int, int> (i, nextHop, cost));
	}

}

void Graph::printToConsole(){
	int node, nextHop, cost;
	for (int i = 0; i <V; ++i){
		node =  std::get<0>(routingTable[i]);
		nextHop =  std::get<1>(routingTable[i]);
		cost =  std::get<2>(routingTable[i]);
		printf("For node: %d \t\t NextHop is: %d \t\t Cost: %d\n", node, nextHop, cost);
	}
}

bool fileExistss(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

void clearFilee(){
	if(fileExistss("dij.out")){
		remove("dij.out");
	}
}

string Graph::printToFile(std::ofstream &out){
	int node, nextHop, cost;
	string ret = "Forwarding Table created: \n";
	for (int i = 0; i <V; ++i){
		node =  std::get<0>(routingTable[i]);
		nextHop =  std::get<1>(routingTable[i]);
		cost =  std::get<2>(routingTable[i]);
		ret += "For node: " + to_string(node) + "\t\t Next hop is: " + to_string(nextHop) + "\t\t Cost: "  + to_string(cost)+"\n";	
	}
	return ret;
}

// Driver program to test methods of graph class
int main2()
{
	clearFilee();
	ofstream out;
	out.open("dij.out");


	// create the graph given in above fugure
	int V = 10;
	Graph g(V, 1);

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

	g.shortestPath();
	
	g.printToConsole();
	g.printToFile(out);
	return 0;
}

