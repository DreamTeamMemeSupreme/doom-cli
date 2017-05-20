//
// Created by snixosha on 30.04.17.
//

#include "client_task.h"

void client_task_delete( client_task* task ) {
    free(task->data);
}