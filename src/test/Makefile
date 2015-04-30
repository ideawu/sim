CFLAGS= -I../net
OBJS = ../net/message.o ../net/decoder.o ../net/sim.o ../net/fde.o ../net/link.o \
	 ../util/log.o
CXX=g++

all: $(OBJS)
	g++ -o test_msg.out $(CFLAGS) test_msg.cpp $(OBJS)
	g++ -o test_link.out $(CFLAGS) test_link.cpp $(OBJS)

clean:
	rm -rf *.o a.out

