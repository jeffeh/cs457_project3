CPP_SRCS = *.cpp
CPP_OBJS = *.o

SS_SRCS = Router.cpp
AW_SRCS = Manager.cpp

SS_OBJS = Router.o
AW_OBJS = Manager.o

SSS_OBJS = ${SS_OBJS}
AWW_OBJS = ${AW_OBJS}
CPP_OBJS = ${CPP_OBJS}

SS_EXE = router
AW_EXE = manager

GPP = g++
GPP_FLAGS = -std=c++0x -pthread -Wall -I. -lX11
LD_FLAGS = -Wall -I. -lX11


all: *.cpp
	$(GPP) $(GPP_FLAGS) $(SS_SRCS) -o $(SS_EXE)
	$(GPP) $(GPP_FLAGS) $(AW_SRCS) -o $(AW_EXE)
router: Router.cpp
	$(GPP) $(GPP_FLAGS) $(SS_SRCS) -o $(SS_EXE)
manager: Manager.cpp
	$(GPP) $(GPP_FLAGS) $(AW_SRCS) -o $(AW_EXE)
clean:
	rm -f *.o *~ $(SS_EXE) $(AW_EXE)
tar:
	tar -cvf P3.tar $(CPP_SRCS) Makefile README
