include ../../build_config.mk

CFLAGS +=
OBJS = message.o decoder.o sim.o fde.o link.o handler.o server.o

all: $(OBJS)
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o a.out

