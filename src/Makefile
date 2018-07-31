include ../build_config.mk

CFLAGS += -I .
OBJS = event.o parser.o session.o transport.o server.o

all: ${OBJS}

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

test:
	make clean
	make
	$(CXX) ${CFLAGS} test.cpp ${OBJS} util/libutil.a net/libnet.a line/libline.a

clean:
	rm -rf *.o *.a *.out

