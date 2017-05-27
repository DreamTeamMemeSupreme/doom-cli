.PHONY: clean

CFLAGS=-Wall -g -lncurses -lpthread

all: Doom

Doom: gmain.o client_general.o client_receiver.o client_UI.o client_task_queue.o client_task.o
	gcc -o Doom gmain.o client_general.o client_receiver.o client_UI.o client_task_queue.o client_task.o  ${CFLAGS}

main.o: gmain.c
	gcc -o gmain.o -c gmain.c ${CFLAGS}

client_general.o: client_general.c
	gcc -o client_general.o -c client_general.c ${CFLAGS}

client_receiver.o: client_receiver.c
	gcc -o client_receiver.o -c client_receiver.c ${CFLAGS}

client_UI.o: client_UI.c
	gcc -o client_UI.o -c client_UI.c ${CFLAGS}

client_task_queue.o: client_task_queue.c
	gcc -o client_task_queue.o -c client_task_queue.c ${CFLAGS}

client_task.o: client_task.c
	gcc -o client_task.o -c client_task.c ${CFLAGS}

clean:
	rm -f *.o Doom
