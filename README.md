# Electric Chair firmware

This is the firmware source code for the Electric Chair AVR Parallel Programmer.

## Requirements

- MPLAB X IDE v6.15
- XC8 v2.45

## How it works (work in progress)

It accepts simple commands via UART (8/1, baud 115200):

### enter

Enters programming mode by powering target, enabling 12V on RESET pin and setting programming bits accordingly.

### run

Enters running mode (RESET floating, target powered)

### exit

Exits programming/running mode.

### fuse (programming mode)

Command to read/write fuses. When no additional options are passed, fuse/lock bits are read from the device and shown.

### fuse reset (programming mode)

When device signature is present in the built-in database, it reverts all fusebits to the factory settings.

## License

This code is GPL-3.0 licensed. It means that every derived work also has to be GPL-3.0 licensed and freely available.