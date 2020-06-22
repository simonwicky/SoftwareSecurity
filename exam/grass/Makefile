SRCDIR   := src
BINDIR   := bin
INCLUDES := inc

CC=gcc
CPP=g++
CFLAGS=-Wall -Wextra -g -fno-stack-protector -no-pie -m32 -z execstack -pthread -I . -Wl,--section-start=.text=0x11111111

# Source files
CLIENT_SRC = $(SRCDIR)/client/client.cpp
COMMANDS_SRC = $(SRCDIR)/commands/command.cpp $(SRCDIR)/commands/parser.cpp
SERVICES_SRC = $(SRCDIR)/services/logging.cpp $(SRCDIR)/services/socket.cpp $(SRCDIR)/services/threadpool.cpp
USER_SRC = $(SRCDIR)/user/user.cpp
GRASS_SRC = $(SRCDIR)/grass.c
SERVER_SRC = $(SRCDIR)/server/server.cpp

.PHONY: all directories
all: directories $(BINDIR)/client $(BINDIR)/server

directories:
	mkdir -p ${BINDIR}; 

$(BINDIR)/client: $(SRCDIR)/client/client_main.cpp $(SERVICES_SRC) $(CLIENT_SRC) $(GRASS_SRC)
	$(CPP) $(CFLAGS) $^ -o $@

$(BINDIR)/server: $(SRCDIR)/server/server_main.cpp $(COMMANDS_SRC) $(SERVER_SRC) $(SERVICES_SRC) $(USER_SRC) $(GRASS_SRC)
	$(CPP) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f -r $(BINDIR)/client $(BINDIR)/server
	rm -drf $(BINDIR)
