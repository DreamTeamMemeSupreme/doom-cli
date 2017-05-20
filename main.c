#include "client_UI.h"
#include "client_task_queue.h"
#include "network_data.h"
#include "client_state.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "client_UI_private.h"
#include "client_task.h"

client_state state;

void exit_client(int sig) {
    state.quit = true;
    ui_close();
    pthread_join(state.user_input,NULL);
    printf("Properly exited\n");
    exit(0);
}

int main(void){

    signal(SIGINT, exit_client);
    ui_start();

    state.connection = false;
    state.input = KEY_PRESSED;
    state.stage = DEFAULT;
    state.quit = false;

    client_task_queue_init(&state.queue);
    pthread_create(&state.user_input, NULL, &ui_read_input, (void*)&state );
    while(state.quit == false) {
        client_task* task = client_task_queue_pop(&state.queue);
        if(task == 0)
            continue;
        if(task->task_type == USER_INPUT) {
            char* data = (char*)(task->data);
            printw("New message: %c\n",*data);
            client_task_delete(task);
        }
    }
    return 0;
}