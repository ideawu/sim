CFLAGS=
OBJS = message.o decoder.o sim.o fde.o link.o
CXX=g++

all: $(OBJS)
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
.cpp.o:
	$(CXX) -c $< -o $@

clean:
	rm -rf *.o a.out

