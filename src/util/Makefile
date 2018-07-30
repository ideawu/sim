include ../../build_config.mk

CFLAGS += -I ../
OBJS = log.o buffer.o

all: ${OBJS}
	ar -cru ./libutil.a ${OBJS}

.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out

