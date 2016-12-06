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
	make dij:
		makes dij
	make clean:
		removes the executables built

Notes:
	To run the link state simulator, first compile all of code by calling 
"make". Then run ./manager <InputFile>. The router manager will take care of
forking all of the router processes and initiating the Link-State Algorithm. 
All of the Router Manager's communications are recorded in the output file:
 "manager.out". The routers will record all their transactions including LSP 
and router tables in individual log files denoted as: "routerN.out" where N is the IP
assigned by the Router Manager. The program will be safe to terminate when the
Routing Manager outputs: "[Manager]: Simulation Done" to the console and the
log file. This takes on Average 10 seconds per dest/source pair + 1 minute. 

Code References Used
	Dijkstra's Algorithm: http://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
