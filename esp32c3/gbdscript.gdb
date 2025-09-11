set remotetimeout 10
target extended-remote :3333
monitor reset halt
maintenance flush register-cache
thbreak creator-esp.c:70
b main
continue

# --- Hook global para ocultar los ecall ---
define hook-stop
    # Lee la instrucción en PC
    set $inst = *(unsigned int *)$pc
    if $inst == 0x00000073
        # 0x73 = opcode de ECALL en RISC-V
        set $next = $pc + 4
        tbreak *$next
        continue
    end
end