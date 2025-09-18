/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *  Panic handler wrapper in order to simulate ecalls in CREATOR using Espressif family
 *  Author: Elisa Utrilla Arroyo
 *  
 */


#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"
#include "hal/wdt_hal.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "esp_rom_sys.h"
#include "esp_attr.h" 
#include <ctype.h>
#include "rom/uart.h"
#include "esp_task_wdt.h"
#include "hal/cpu_hal.h"
#include <stdbool.h>
#include <stdint.h>


// El atributo noreturn es opcional, pero avisa al compilador que no retorna normalmente.
void __attribute__((noreturn)) ecall_trampoline_read_int(uint32_t next_pc);

int read_int(){
    unsigned char c;
    char buffer[16]; 
    int idx = 0;
    while(1){
        //while (!uart_rx_one_char(uart_no, &c)) { }
        c = uart_rx_one_char_block();
        // Check the char added

        //Is an space?? Finish it!!
        if (c == '\n' || c == '\r') {
            buffer[idx] = '\0';
            esp_rom_printf("\n"); //echo
            break;
        }
        //It is a number? add it!
        if (isdigit(c)) {
            if (idx < sizeof(buffer) - 1) {
                
                buffer[idx++] = c;
                esp_rom_printf("%c", c);
            }
        }
        //is another char? Ignore it

    }
    //Transform into number
    int value = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        value = value * 10 + (buffer[i] - '0');
    }

    return value;

}

void __attribute__((noreturn)) ecall_trampoline_read_int(uint32_t next_pc);

void ecall_trampoline_read_int(uint32_t next_pc)
{
 __asm__ volatile(
        "addi sp, sp, -96\n"      // ajustar según cuantos registros guardes
        "sw t0, 0(sp)\n"
        "sw t1, 4(sp)\n"
        "sw t2, 8(sp)\n"
        "sw t3, 12(sp)\n"
        "sw t4, 16(sp)\n"
        "sw t5, 20(sp)\n"
        "sw t6, 24(sp)\n"
        "sw s0, 28(sp)\n"
        "sw s1, 32(sp)\n"
        "sw s2, 36(sp)\n"
        "sw s3, 40(sp)\n"
        "sw s4, 44(sp)\n"
        "sw s5, 48(sp)\n"
        "sw s6, 52(sp)\n"
        "sw s7, 56(sp)\n"
        "sw s8, 60(sp)\n"
        "sw s9, 64(sp)\n"
        "sw s10,68(sp)\n"
        "sw s11,72(sp)\n"
        "sw ra, 76(sp)\n"
        :
        :
        : "memory"
    );

    // Ejecutar la syscall simulada
    int result = read_int();

    // Guardar el resultado en a0
    __asm__ volatile("mv a0, %0" :: "r"(result));

    // Restaurar todos los registros guardados
    __asm__ volatile(
        "lw t0, 0(sp)\n"
        "lw t1, 4(sp)\n"
        "lw t2, 8(sp)\n"
        "lw t3, 12(sp)\n"
        "lw t4, 16(sp)\n"
        "lw t5, 20(sp)\n"
        "lw t6, 24(sp)\n"
        "lw s0, 28(sp)\n"
        "lw s1, 32(sp)\n"
        "lw s2, 36(sp)\n"
        "lw s3, 40(sp)\n"
        "lw s4, 44(sp)\n"
        "lw s5, 48(sp)\n"
        "lw s6, 52(sp)\n"
        "lw s7, 56(sp)\n"
        "lw s8, 60(sp)\n"
        "lw s9, 64(sp)\n"
        "lw s10,68(sp)\n"
        "lw s11,72(sp)\n"
        "lw ra, 76(sp)\n"
        "addi sp, sp, 96\n"
        "jr %0\n"
        :
        : "r"(next_pc)
        : "memory"
    );

    __builtin_unreachable();
}

