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
void __attribute__((noreturn)) ecall_trampoline_read_int(uint32_t next_pc, uint32_t return_address);

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

void __attribute__((noreturn)) ecall_trampoline_read_int(uint32_t next_pc, uint32_t return_address);

void ecall_trampoline_read_int(uint32_t next_pc, uint32_t return_address)
{
    if (next_pc == 0) {
        esp_rom_printf("Error: next_pc es NULL!\n");
        while(1);
    }
 register uintptr_t sp_orig asm("sp");
    register uintptr_t sp_after asm("sp");

    __asm__ volatile(
        // --- Guardar registros en pila ---
        "addi sp, sp, -96\n"
        "sw ra,   0(sp)\n"
        "sw t0,   4(sp)\n"
        "sw t1,   8(sp)\n"
        "sw t2,  12(sp)\n"
        "sw t3,  16(sp)\n"
        "sw t4,  20(sp)\n"
        "sw t5,  24(sp)\n"
        "sw t6,  28(sp)\n"
        "sw s0,  32(sp)\n"
        "sw s1,  36(sp)\n"
        "sw s2,  40(sp)\n"
        "sw s3,  44(sp)\n"
        "sw s4,  48(sp)\n"
        "sw s5,  52(sp)\n"
        "sw s6,  56(sp)\n"
        "sw s7,  60(sp)\n"
        "sw s8,  64(sp)\n"
        "sw s9,  68(sp)\n"
        "sw s10, 72(sp)\n"
        "sw s11, 76(sp)\n"

        // --- Restaurar registros ---
        "lw ra,   0(sp)\n"
        "lw t0,   4(sp)\n"
        "lw t1,   8(sp)\n"
        "lw t2,  12(sp)\n"
        "lw t3,  16(sp)\n"
        "lw t4,  20(sp)\n"
        "lw t5,  24(sp)\n"
        "lw t6,  28(sp)\n"
        "lw s0,  32(sp)\n"
        "lw s1,  36(sp)\n"
        "lw s2,  40(sp)\n"
        "lw s3,  44(sp)\n"
        "lw s4,  48(sp)\n"
        "lw s5,  52(sp)\n"
        "lw s6,  56(sp)\n"
        "lw s7,  60(sp)\n"
        "lw s8,  64(sp)\n"
        "lw s9,  68(sp)\n"
        "lw s10, 72(sp)\n"
        "lw s11, 76(sp)\n"

        // --- Restaurar SP ---
        "addi sp, sp, 96\n"

        // --- Salida de SP a variable C ---
        : "=r"(sp_after)
        :
        : "memory"
    );

    // --- Chequeo de SP ---
    // printf("SP original = %p, SP after trampoline = %p\n", 
    //        (void*)sp_orig, (void*)sp_after);

    // --- Salto a next_pc sin modificar RA ---
    __asm__ volatile(
        "mv t6, %0\n"  // copia next_pc a registro temporal
        "jalr t6\n"      // salto incondicional
        :
        : "r"(next_pc)
        : "t6", "memory"
    );

    __builtin_unreachable();
}

