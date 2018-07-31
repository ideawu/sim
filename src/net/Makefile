include ../../build_config.mk

CFLAGS += -I ../
OBJS = fde.o link.o tcp_link.o transport.o

all: $(OBJS)
	
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

test:
	make clean
	make
	$(CXX) ${CFLAGS} test.cpp ${OBJS} ../util/libutil.a

clean:
	rm -rf *.o *.a *.out

