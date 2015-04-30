include ../../build_config.mk

CFLAGS += -I../../output/include
CLIBS  += ../../output/libsim.a

all: $(OBJS)
	g++ -o test_msg.out $(CFLAGS) test_msg.cpp $(CLIBS)
	g++ -o test_link.out $(CFLAGS) test_link.cpp $(CLIBS)
	g++ -o test_server.out $(CFLAGS) test_server.cpp $(CLIBS)

clean:
	rm -rf *.o a.out

