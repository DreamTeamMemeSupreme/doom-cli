CFLAGS = -pedantic-errors -Wall -Werror=format -Werror=return-type -Werror=vla -Werror=uninitialized -Werror-implicit-function-declaration -Werror=declaration-after-statement -g -O1
VGFLAGS = valgrind -q --track-origins=yes --malloc-fill=66 --free-fill=66 --error-exitcode=1 --leak-check=full --track-fds=yes

all:
	echo "Target not specified"

build:
	mkdir build

pseudo_client: build/client_socket.o build/pseudo_client.o build/poll_array.o
	gcc build/pseudo_client.o\
	 build/client_socket.o\
	 build/poll_array.o\
	 -o build/pseudo_client ${CFLAGS}

pseudo_server: pseudo-server/main.c build/server_socket.o build/pseudo_server.o build/poll_array.o
	gcc pseudo-server/main.c \
     build/pseudo_server.o\
	 build/server_socket.o\
	 build/poll_array.o\
	 -o build/pseudo_server ${CFLAGS}

build/pseudo_client.o: pseudo-client/main.c network/client_socket.h network/socket.h build
	gcc -c pseudo-client/main.c -o build/pseudo_client.o ${CFLAGS}

build/pseudo_server.o: pseudo-server/server.c pseudo-server/server.h network/server_socket.h network/socket.h build
	gcc -c pseudo-server/server.c -o build/pseudo_server.o ${CFLAGS}

build/server_socket.o: network/server_socket.c network/server_socket.h network/socket.h build
	gcc -c network/server_socket.c -o build/server_socket.o ${CFLAGS}

build/client_socket.o: network/client_socket.c network/client_socket.h network/socket.h build
	gcc -c network/client_socket.c -o build/client_socket.o ${CFLAGS}

build/poll_array.o: poll_array/poll_array.c poll_array/poll_array.h
	gcc -c poll_array/poll_array.c -o build/poll_array.o ${CFLAGS}
