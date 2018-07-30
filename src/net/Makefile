include ../../build_config.mk

CFLAGS += -I ../
OBJS = fde.o tcp_link.o transport.o

all: $(OBJS)
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

test:
	make clean
	make
	$(CXX) ${CFLAGS} test.cpp ${OBJS} ../util/libutil.a

clean:
	rm -rf *.o *.a *.out

