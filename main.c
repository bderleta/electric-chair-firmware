/*
 * File:   main.c
 * Author: Bartosz Derleta <bartosz@derleta.com>
 */

#include "config.h"
#include "prog.h"

static char line[255];

static uint8_t mode = 0;
static const avr_record* record = NULL;

void cmd_enter() {
    if (strtok(NULL, " ")) {
        puts("error     \tCommand does not accept arguments");
    } else if (mode == 2) {
        puts("error     \tAlready in programming mode");
    } else {
        enter_programming();
        uint32_t s;
        read_signature(&s);
        printf("signature \t%06lX\n", s);
        record = lookup_signature(s);
        if (record == NULL) {
            exit_programming();
            if (mode == 1) {
                power_up();
                puts("error     \tDevice not recognized, back to running mode");
            } else {
                puts("error     \tDevice not recognized, power down");
            }
        } else {
            mode = 2;
            printf("device    \t%s\n", record->name);
            puts("ok        \tEntered programming mode for device");
        }
    }
}

void cmd_exit() {
    if (strtok(NULL, " ")) {
        puts("error     \tCommand does not accept arguments");
    } else if (mode == 0) {
        puts("error     \tNot in programming or running mode");
    } else if (mode == 1) {
        power_down();
        mode = 0;
        puts("ok        \tPower down");
    } else if (mode == 2) {
        exit_programming();
        mode = 0;
        puts("ok        \tProgramming mode left, power down");
    }
    record = NULL;
}

void cmd_run() {
    if (strtok(NULL, " ")) {
        puts("error     \tCommand does not accept arguments");
    } else if (mode == 1) {
        puts("error     \tAlready in running mode");
    } else if (mode == 0) {
        power_up();
        mode = 1;
        puts("ok        \tRunning");
    } else if (mode == 2) {
        exit_programming();
        power_up();
        mode = 1;
        puts("ok        \tProgramming mode left, running");
    }
    record = NULL;
}

void print_fuse(uint8_t value, uint8_t default_value, const char* const* map) {
    printf(
        "%s%01X%s%01X%s\t", 
        ((value & 0xF0) == (default_value & 0xF0)) ? "\x1b[0m" : "\x1b[31m",
        value >> 4,
        ((value & 0x0F) == (default_value & 0x0F)) ? "\x1b[0m" : "\x1b[31m",    
        value & 0x0F,    
        "\x1b[0m"    
    );
    for (int i = 7; i >= 0; i--) {
        if ((value & (1 << i)) != (default_value & (1 << i))) {
            printf("\x1b[31m%c", (value & (1 << i)) ? '1' : '0');
        } else {
            printf("\x1b[0m%c", (value & (1 << i)) ? '1' : '0');
        }
    }
    if (map != NULL) {
        for (int i = 0; i < 8; i++) {
            if (map[i] == NULL) 
                continue;
            if ((value & (1 << i)) != (default_value & (1 << i))) {
                printf("\t\x1b[31m" "%-9s = %c", map[i], (value & (1 << i)) ? '1' : '0');
            } else if (value & (1 << i)) {
                printf("\t\x1b[0m" "%-9s = 1", map[i]);
            } else {
                printf("\t\x1b[32m" "%-9s = 0", map[i]);
            }
        }
    }
    puts("\x1b[0m");
}

int print_fuse_status(int reset_mode) {
    uint8_t flb[4];
    if (record->flags & AVR_FLAG_SUPPORTED) {
        read_fuse_and_lock_bits(flb, record->flags & AVR_FLAG_FUSE_EXTENDED);
        printf("low       \t");
        print_fuse(flb[0], record->fuse_low_factory, record->fuse_low_map);
        printf("high      \t");
        print_fuse(flb[1], record->fuse_high_factory, record->fuse_high_map);
        if (record->flags & AVR_FLAG_FUSE_EXTENDED) {
            printf("extended  \t");
            print_fuse(flb[2], record->fuse_extended_factory, record->fuse_extended_map);
        }
        printf("lock      \t");
        print_fuse(flb[3], record->lock_factory, record->lock_map);
        if (
            (flb[0] == record->fuse_low_factory) &&
            (flb[1] == record->fuse_high_factory) &&
            (flb[2] == record->fuse_extended_factory) &&
            (flb[3] == record->lock_factory)
        ) {
            puts("ok        \tAll fuse/lock bits are equal to factory defaults");
            return 0;
        } else if (reset_mode == 0) {
            puts("warning   \tFound differences to factory defaults, use 'fuse reset' to restore");
        } else if (reset_mode == 2) {
            puts("error     \tFound differences to factory defaults, reset failed");
        }
        return 1;
    } else {
        read_fuse_and_lock_bits(flb, 1);
        printf("low       \t%02X\t" BYTE_TO_BINARY_PATTERN "\n", flb[0], BYTE_TO_BINARY(flb[0]));
        printf("high      \t%02X\t" BYTE_TO_BINARY_PATTERN "\n", flb[1], BYTE_TO_BINARY(flb[1]));
        printf("extended  \t%02X\t" BYTE_TO_BINARY_PATTERN "\n", flb[2], BYTE_TO_BINARY(flb[2]));
        printf("lock      \t%02X\t" BYTE_TO_BINARY_PATTERN "\n", flb[3], BYTE_TO_BINARY(flb[3]));
        puts("warning   \tDatabase record incomplete for this device, cannot compare to factory defaults");
        return 0;
    }    
}

void cmd_fuse() {
    char* arg1 = strtok(NULL, " ");
    if (mode != 2) {
        puts("error     \tNot in programming mode");
        return;
    }
    if (arg1 == NULL) {
        print_fuse_status(0);
    } else if (strcmp(arg1, "reset") == 0) {
        /* Check if we can reset fuses for this one */
        if (!print_fuse_status(1))
            return;
        puts("status    \tProgramming low fuse bits");
        program_fuse_low_bits(record->fuse_low_factory);
        puts("status    \tProgramming high fuse bits");
        program_fuse_high_bits(record->fuse_high_factory);
        if (record->flags & AVR_FLAG_FUSE_EXTENDED) {
            puts("status    \tProgramming extended fuse bits");
            program_fuse_extended_bits(record->fuse_extended_factory);
        }
        print_fuse_status(2);
    } else if (strcmp(arg1, "low") == 0) {
        puts("status    \tProgramming low fuse bits");
        program_fuse_low_bits(0xE0);
        puts("ok        \tDone");
    } else if (strcmp(arg1, "high") == 0) {
        
    } else if (strcmp(arg1, "extended") == 0) {
        
    } else if (strcmp(arg1, "lock") == 0) {
        
    } else {
        printf("error     \tInvalid argument '%s'; expected 'low', 'high', 'extended', 'lock' or 'reset'\n", arg1);
    }
}

void cmd_unknown(char* command) {
    printf("error     \tUnrecognized command '%s'\n", command);
}

int main(void) {
    init();
        
    /*
    printf("Erasing chip... ");
    erase_chip();
    printf("done.\n");
    
    _delay_ms(1000);
    
    printf("Programming fuse bits... ");
    program_fuse_high_bits(0x99);
    program_fuse_low_bits(0xE1);
    printf("done.\n");

     */
    volatile int r = 0;
    while (1) {
        r = usart1_gets(line, 255);
        if (r <= 0)
            continue;
        char* command = strtok(line, " ");
        if (strcmp(command, "enter") == 0) {
            cmd_enter();
        } else if (strcmp(command, "exit") == 0) {
            cmd_exit();
        } else if (strcmp(command, "run") == 0) {
            cmd_run();
        } else if (strcmp(command, "fuse") == 0) {
            cmd_fuse();
        } else {
            cmd_unknown(command);
        }
    }
}
