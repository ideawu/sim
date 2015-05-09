include ../build_config.mk

CFLAGS +=
OBJS = sim.o message.o decoder.o link.o handler.o server.o \
	fde.o log.o app.o config.o
LIB=libsim.a
HEADER_FILES=sim.h message.h decoder.h fde.h link.h handler.h server.h
UTIL_HEADERS=util/thread.h util/strings.h util/log.h util/config.h util/app.h

all: $(OBJS)
	mkdir -p $(HEADER_OUTPUT_DIR)/util
	ar -cru $(OUTPUT_DIR)/$(LIB) ${OBJS}
	cp $(HEADER_FILES) $(HEADER_OUTPUT_DIR)
	cp $(UTIL_HEADERS) $(HEADER_OUTPUT_DIR)/util
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
app.o: util/app.h util/app.cpp
	$(CXX) ${CFLAGS} -c util/app.cpp
log.o: util/log.h util/log.cpp
	$(CXX) ${CFLAGS} -c util/log.cpp
config.o: util/config.h util/config.cpp
	$(CXX) ${CFLAGS} -c util/config.cpp
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out $(OUTPUT_DIR)/$(LIB) $(HEADER_OUTPUT_DIR)

