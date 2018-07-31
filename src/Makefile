include ../build_config.mk

CFLAGS += -I .
OBJS = 
LIBS = util/libutil.a net/libnet.a line/libline.a core/libcore.a

all: ${OBJS}
	make clean
	$(CXX) ${CFLAGS} test.cpp ${OBJS} ${LIBS}

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

