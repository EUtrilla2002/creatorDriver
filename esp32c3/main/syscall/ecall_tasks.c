#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdbool.h>
#include "esp_private/panic_internal.h"
#include "esp_cpu.h"
#include "ecall_task.h"   // cabecera con typedefs y prototipos
#include "rom/uart.h"

// Cola de ecalls (variables estáticas dentro del módulo)
static ecall_msg_t ecall_queue[ECALL_QUEUE_SIZE];
static volatile int ecall_queue_head = 0;
static volatile int ecall_queue_tail = 0;

// Función para encolar peticiones
void enqueue_task_ecall(ecall_type_t type, RvExcFrame *frame) {
    //esp_rom_printf("[DEBUG] ECALL enqueued AFTER: head=%d, tail=%d\n", ecall_queue_head, ecall_queue_tail);
    int next = (ecall_queue_tail + 1) % ECALL_QUEUE_SIZE;
    if (next != ecall_queue_head) {
        ecall_queue[ecall_queue_tail].pending = true;
        ecall_queue[ecall_queue_tail].type = type;
        ecall_queue[ecall_queue_tail].frame = frame;
        ecall_queue[ecall_queue_tail].done = false;
        ecall_queue_tail = next;
        //esp_rom_printf("[DEBUG] ECALL enqueued: head=%d, tail=%d\n", ecall_queue_head, ecall_queue_tail);
    } else {
        printf("[WARN] Ecall queue full!\n");
    }
}

// Tarea FreeRTOS que procesa la cola
int read_int(int timeout_ms) {
    uint8_t c;
    char buffer[16] = {0};
    int idx = 0;

    // cuántas iteraciones de 100 µs caben en timeout_ms
    int loops = (timeout_ms * 1000) / 100;

    while (loops-- > 0) {
        if (uart_rx_one_char(&c) == ETS_OK) {
            esp_rom_printf("%c", c);

            if (c == '\n' || c == '\r') {
                buffer[idx] = '\0';
                esp_rom_printf("\n");
                break;
            }

            if (c >= '0' && c <= '9') {
                if (idx < (int)(sizeof(buffer) - 1)) {
                    buffer[idx++] = c;
                }
            }

            // reset del timeout cada vez que recibimos algo
            loops = (timeout_ms * 1000) / 100;
        } else {
            esp_rom_delay_us(100); // espera mínima
        }
    }

    if (idx == 0) {
        esp_rom_printf("[WARN] No input (timeout %d ms). Returning -1\n", timeout_ms);
        return -1;
    }

    int value = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        value = value * 10 + (buffer[i] - '0');
    }

    return value;
}
void auxiliar_ecall_task(void *arg) {
    (void)arg;
    //printf("[INFO] ECALL_TASK started\n");
    
    while (1) {
        //printf("[INFO] ECALL_TASK running\n");
        ecall_msg_t *msg = &ecall_queue[ecall_queue_head];
        //printf("[INFO] ECALL_TASK started\n");

            if (msg->pending) {
                switch (msg->type) {
                    case ECALL_READ_INT: {
                        //int valor = read_int(10000);
                        printf("Reading int from console (timeout 10s): ");
                        msg->frame->a0 = 4;
                        break;
                    }

                    default:
                        printf("[WARN] Ecall desconocida\n");
                        break;
                }
             

                msg->done = true;
                msg->pending = false;
                ecall_queue_head = (ecall_queue_head + 1) % ECALL_QUEUE_SIZE;
            }


            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        
    }



// Función para iniciar la tarea
void start_ecall_task(void) {
    xTaskCreate(auxiliar_ecall_task, "ECALL_TASK", 4096, NULL, 1, NULL);
}
