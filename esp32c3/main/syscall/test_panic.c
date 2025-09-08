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

#define POOL_CAPACITY 65536  // 64 KB pool, as Jose Antonio puts it in his examples
char memory_pool[POOL_CAPACITY];
int current_offset = 0;

void disable_all_hw_watchdogs() {
    // TG0
    TIMERG0.wdtwprotect.val = 0x50D83AA1;
    TIMERG0.wdtconfig0.val = 0;
    TIMERG0.wdtwprotect.val = 0;

    // TG1
    TIMERG1.wdtwprotect.val = 0x50D83AA1;
    TIMERG1.wdtconfig0.val = 0;
    TIMERG1.wdtwprotect.val = 0;
}

extern void esp_panic_handler(panic_info_t *info);
volatile bool g_override_ecall = true;

void __real_esp_panic_handler(panic_info_t *info);


void return_from_panic_handler(RvExcFrame *frm) __attribute__((noreturn));

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
char read_char() {
    unsigned char c;
    char last_char = 0;

    while (1) {
        c = uart_rx_one_char_block();

        // return char
        if (c == '\n' || c == '\r') {
            esp_rom_printf("\n"); //echo
            return last_char;
        }

        // Last char readed
        last_char = (char)c;
        esp_rom_printf("%c", last_char);
    }
}
void read_string(char *dest, int length) {
    unsigned char c;
    int idx = 0;

    while (1) {
        c = uart_rx_one_char_block();

        if (c == '\n' || c == '\r') {
            dest[idx] = '\0';
            esp_rom_printf("\n"); //echo
            break;
        }

        if (idx < length - 1) {
            dest[idx++] = c;
            esp_rom_printf("%c", c); //echo
        }
    }
}

IRAM_ATTR void __wrap_esp_panic_handler(panic_info_t *info)
{
    RvExcFrame *frm = (RvExcFrame *)info->frame;
    if ((frm->mcause == 0x0000000b || frm->mcause == 0x00000008) && g_override_ecall == true) { //Only catches Ecall syscalls
        disable_all_hw_watchdogs();
        int cause = frm->a7;
        //esp_rom_printf("Causa del panic (a7): %d\n", cause);
        switch (cause) {
            case 1: { //Print int
                int value = frm->a0;
                esp_rom_printf("%d\n", value);
                break;
            }
            case 2: { //Print float TODO
                esp_rom_printf("\033[1;31mFloat number operations not registered yet\033[0m\n");
                break;
            }
            case 3: { //Print double TODO
                esp_rom_printf("\033[1;31mDouble number operations not registered yet\033[0m\n");
                break;
            }
            case 4: { //Print string
                char* cadena = (char*) frm->a0;
                esp_rom_printf("%s\n", cadena);
                break;
            }
            case 5: { // Read int
                int number_read = read_int();
                frm->a0 = number_read;
                break;
            }
            case 6:{ // Read float TODO
                esp_rom_printf("\033[1;31mFloat number operations not registered yet\033[0m\n");
                break;
            }
            case 7:{  //Read double  TODO
                esp_rom_printf("\033[1;31mDouble number operations not registered yet\033[0m\n");
                break;
            }
            case 8:{ //Read string
                char* dest = (char*) frm->a0;
                int length = frm->a1; 
                read_string(dest,length);
                break;
            }
            case 9: {  // sbrk
                int increment = frm->a0;
                if (current_offset + increment > POOL_CAPACITY || current_offset + increment < 0) {
                    frm->a0 = -1; // Offlimits
                    esp_rom_printf("\033[31;1mSBRK: Memory exhausted\033[0m\n");
                } else {
                    char *prev_brk = &memory_pool[current_offset];
                    current_offset += increment;
                    frm->a0 = (int)prev_brk; 
                }
                break;
            }
            case 10: { //exit
                break;
            }
            case 11:{  //Print char
                char caract = (char) frm->a0;
                esp_rom_printf("%c\n", caract);
                break;
            } 
            case 12:{ //Read char
                char char_leido = read_char();
                frm->a0 = char_leido;
                break;
            }
            default:
                esp_rom_printf("Not an ecall registered\n");
                break;
        }


        //frm->mepc = frm->ra;
        if (cause == 10)
        {
            frm->mepc = frm->ra;
        }
        else
        {
            frm->mepc += 4;
        }
        
        return_from_panic_handler(frm);
    } else {
        __real_esp_panic_handler(info); //Other fatal errors are treated as usual
    }
}