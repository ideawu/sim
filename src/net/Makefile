include ../../build_config.mk

CFLAGS += -I ../
OBJS = fde.o link.o tcp_link.o \
	event.o parser.o session.o transport.o server.o \
	line_parser.o line_message.o line_server.o

all: $(OBJS)
	
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

test:
	make clean
	make
	$(CXX) ${CFLAGS} test.cpp ${OBJS} ../util/libutil.a

clean:
	rm -rf *.o *.a *.out

