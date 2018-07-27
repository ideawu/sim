include ../build_config.mk

CFLAGS +=
OBJS=
LIB=libsim.a
HEADER_FILES=
UTIL_HEADERS=util/thread.h util/strings.h util/log.h util/config.h util/app.h

all: $(OBJS)
	#mkdir -p $(HEADER_OUTPUT_DIR)/util
	#ar -cru $(OUTPUT_DIR)/$(LIB) ${OBJS}
	#cp $(HEADER_FILES) $(HEADER_OUTPUT_DIR)
	#cp $(UTIL_HEADERS) $(HEADER_OUTPUT_DIR)/util
	
fde.o: fde.h fde.cpp fde_select.cpp fde_epoll.cpp
	${CXX} ${CFLAGS} -c fde.cpp
# log.o: util/log.h util/log.cpp
# 	$(CXX) ${CFLAGS} -c util/log.cpp
# config.o: util/config.h util/config.cpp
# 	$(CXX) ${CFLAGS} -c util/config.cpp
.cpp.o:
	$(CXX) ${CFLAGS} -c $< -o $@

clean:
	rm -rf *.o *.a *.out $(OUTPUT_DIR)/$(LIB) $(HEADER_OUTPUT_DIR)

