#ifndef ECALL_TASK_H
#define ECALL_TASK_H

#include <stdbool.h>
#include "esp_cpu.h"
#include "riscv/rvruntime-frames.h"


#define ECALL_QUEUE_SIZE 4

typedef enum {
    ECALL_NONE = 0,
    ECALL_READ_INT
} ecall_type_t;

typedef struct {
    volatile bool pending;
    ecall_type_t type;
    RvExcFrame *frame;
    volatile bool done;
} ecall_msg_t;

// Funciones públicas
void enqueue_task_ecall(ecall_type_t type, RvExcFrame *frame);
void start_ecall_task(void);

#endif
