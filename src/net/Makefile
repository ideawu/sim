CFLAGS=
OBJS = message.o decoder.o sim.o fde.o link.o
CXX=g++

all: $(OBJS)
	g++ -o test_msg.out test_msg.cpp $(OBJS)
	g++ -o test_link.out test_link.cpp $(OBJS) ../util/log.o
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
.cpp.o:
	$(CXX) -c $< -o $@

clean:
	rm -rf *.o a.out

