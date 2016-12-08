README Project 3 Group 12:
	Nick Wilson
	Jeffrey Huynh
	Ethan Loza

Expected Files in Tarball:
	Manager.cpp
	Router.cpp
	Makefile
	dij.cpp
	dij.h
	ConnectivityTable.h

Usage for Manager.cpp:
	./manager <InputFile>

To use the Makefile:
	make all:
		makes manager, router, and dijkstra's class
	make manager:
		makes manager
	make router:
		makes router
	make clean:
		removes the executables built

Notes:
	To run the link state simulator, first compile all of code by calling 
"make". Then run ./manager <InputFile>. The router manager will take care of
forking all of the router processes and initiating the Link-State Algorithm. 
All of the Router Manager's communications are recorded in the output file:
 "manager.out". The routers will record all their transactions with each other 
and the manager along with sent LSP's and their routing tables in separate
log files denoted as: "routerN.out" where N is the IP assigned by the Router 
Manager. The program will be safe to terminate when the Routing Manager outputs: 
"[Manager]: Simulation Done" to the console and the log file. This takes on 
average 10 seconds per dest/source pair + 1 minute. After reading the project 
description, it was specified that we could assume that the input file would 
follow the specifed format, as such we did not implement error checking on the 
input file. An example input file is shown below. The first line of the input 
file specifies the number of nodes in the router network. The following lines 
provide the connection info between the nodes and follow the format"X Y C" where 
X is the first node IP, Y is the second node IP, and C is the cost of the connection 
betweeen them. The connection info will continue until a -1 is reached. This 
signals the end of the router connection info and the beginning of the
source/destination testing information. This section will follow the format: 
"X Y" where X is the source of where we want the test packet to be sent from and 
Y indicating the destination of where we want the test packet to end.There can be 
many different source destination pairs. The end of the testing phase and thus 
the end of the file is indicated by a -1. If the input file is not included as 
an argument with the program, the program will seg fault.


INPUT FILE EXAMPLE:
10
0 9 40
0 4 60
0 2 40
1 8 70
2 6 70
3 9 20
3 8 70
3 5 70
4 7 70
4 6 40
5 9 70
5 7 40
7 9 60
8 9 70
-1
0 5
3 9
4 7
-1



Code References Used
	Dijkstra's Algorithm: http://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
