# ESP32 Bare-Metal Kernel

A bare-metal operating system kernel for ESP32-WROOM-32 with cooperative multitasking, built from scratch without ESP-IDF.

## Features

- **Bare-metal implementation** - No dependency on ESP-IDF framework
- **Cooperative multitasking** - Round-robin task scheduler with voluntary yielding
- **Task management** - Create and manage up to 8 concurrent tasks
- **Memory management** - Dynamic heap allocator with first-fit algorithm
- **Hardware drivers**:
  - UART0 for serial communication (115200 baud)
  - GPIO for digital I/O control
  - Basic interrupt framework
- **Demo applications** - LED blink, UART status, and compute tasks

## Architecture

### Memory Layout

- **IRAM**: 0x40080000 - 0x400A0000 (128KB) - Instruction RAM
- **DRAM**: 0x3FFB0000 - 0x40000000 (176KB) - Data RAM
- **Flash**: 0x400C0000+ - External SPI Flash

### Components

1. **Bootloader** ([src/boot/](src/boot/))
   - Assembly startup code
   - Hardware initialization
   - BSS clearing and stack setup

2. **Kernel** ([src/kernel/](src/kernel/))
   - Task scheduler with context switching
   - Heap memory allocator
   - Interrupt handling framework

3. **Drivers** ([src/drivers/](src/drivers/))
   - UART driver for serial communication
   - GPIO driver for digital I/O

4. **Applications** ([src/apps/](src/apps/))
   - LED blink task
   - UART status task
   - Fibonacci compute task

## Prerequisites

### Toolchain

Install the Xtensa ESP32 toolchain:

```bash
# On Linux/macOS
# Download from: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-tools.html
# Or use ESP-IDF installation:
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32
source export.sh
```

### Tools

- `xtensa-esp32-elf-gcc` - Cross compiler
- `esptool.py` - Flash utility (comes with ESP-IDF)
- `make` - Build system

## Building

Clone and build the kernel:

```bash
# Clone the repository
cd esp32-kernel

# Build the kernel
make

# Output: build/esp32-kernel.bin
```

## Flashing

Connect your ESP32 board via USB and flash:

```bash
# Flash to ESP32 (adjust port as needed)
make flash ESPTOOL_PORT=/dev/ttyUSB0

# On macOS, port might be:
# make flash ESPTOOL_PORT=/dev/cu.usbserial-*

# On Windows (in Git Bash):
# make flash ESPTOOL_PORT=COM3
```

## Monitoring

View serial output:

```bash
# Open serial monitor
make monitor

# Or flash and monitor in one command
make flash-monitor
```

Expected output:

```
==============================
ESP32 Bare-Metal Kernel
==============================
[BOOT] Starting initialization...
[BOOT] Jumping to kernel_main

[KERNEL] Kernel initialization started
[KERNEL] Initializing heap...
[HEAP] Initialized: 32768 bytes available
[KERNEL] Initializing task system...
[TASK] Task system initialized
[KERNEL] Initializing scheduler...
[SCHED] Scheduler initialized
[KERNEL] Initializing GPIO...
[GPIO] GPIO driver initialized
[KERNEL] Creating idle task...
[TASK] Created task 'idle' (ID: 0, stack: 3ffb8000)
[KERNEL] Creating demo tasks...
[TASK] Created task 'led_blink' (ID: 1, stack: 3ffba000)
[TASK] Created task 'uart_status' (ID: 2, stack: 3ffbc000)
[TASK] Created task 'compute' (ID: 3, stack: 3ffbe000)
[KERNEL] Starting scheduler...

[LED_TASK] LED blink task started
[LED_TASK] LED configured on GPIO2
[LED_TASK] LED ON (blink #1)
[UART_TASK] UART status task started
[COMPUTE_TASK] Compute task started
[LED_TASK] LED OFF
...
```

## Project Structure

```
esp32-kernel/
├── src/
│   ├── boot/
│   │   ├── start.S          # Assembly entry point
│   │   └── init.c           # Hardware initialization
│   ├── kernel/
│   │   ├── kernel.c         # Main kernel logic
│   │   ├── scheduler.c      # Task scheduler
│   │   ├── task.c           # Task management
│   │   ├── context.S        # Context switching
│   │   ├── heap.c           # Memory allocator
│   │   └── interrupt.c      # Interrupt handling
│   ├── drivers/
│   │   ├── uart.c           # UART driver
│   │   └── gpio.c           # GPIO driver
│   └── apps/
│       └── demo.c           # Demo applications
├── include/
│   ├── types.h              # Type definitions
│   ├── esp32_defs.h         # Hardware definitions
│   ├── kernel.h             # Kernel API
│   ├── task.h               # Task API
│   ├── heap.h               # Heap API
│   ├── uart.h               # UART API
│   ├── gpio.h               # GPIO API
│   └── interrupt.h          # Interrupt API
├── linker/
│   └── esp32.ld             # Linker script
├── Makefile                 # Build system
└── README.md                # This file
```

## Customization

### Adding New Tasks

Edit [src/apps/demo.c](src/apps/demo.c) and add your task:

```c
void my_task(void *arg) {
    while (1) {
        // Your code here
        task_yield();  // Give CPU to other tasks
    }
}

void demo_init_tasks(void) {
    task_create("my_task", my_task, NULL, TASK_STACK_SIZE);
    // ... other tasks
}
```

### Changing LED GPIO

Edit [src/apps/demo.c](src/apps/demo.c) and change `LED_GPIO`:

```c
#define LED_GPIO  GPIO_NUM_5  // Change from GPIO2 to GPIO5
```

### Adjusting Task Count

Edit [include/task.h](include/task.h):

```c
#define MAX_TASKS  16  // Increase from 8 to 16
```

## Configuration

### Serial Port

The default serial port configuration:
- **Port**: UART0 (GPIO1=TX, GPIO3=RX)
- **Baud**: 115200
- **Format**: 8N1 (8 data bits, no parity, 1 stop bit)

### Memory

Heap size is defined in [linker/esp32.ld](linker/esp32.ld):

```ld
.dram0.heap : {
    _heap_start = ABSOLUTE(.);
    . = . + 32K;  /* Change heap size here */
    _heap_end = ABSOLUTE(.);
}
```

## Limitations

- **Cooperative scheduling** - Tasks must call `task_yield()` voluntarily
- **No preemption** - Long-running tasks can block others
- **No memory protection** - Tasks share the same address space
- **Single core** - Only PRO CPU is used
- **Basic drivers** - Minimal hardware support

## Future Enhancements

- Preemptive multitasking with timer interrupts
- Inter-task communication (queues, semaphores)
- File system support
- Network stack (WiFi, TCP/IP)
- Second core (APP CPU) support
- Power management

## Troubleshooting

### Build Errors

**Error**: `xtensa-esp32-elf-gcc: command not found`

**Solution**: Install ESP32 toolchain and add to PATH:
```bash
source ~/esp/esp-idf/export.sh
```

### Flash Errors

**Error**: `Failed to connect to ESP32`

**Solution**:
- Hold BOOT button while flashing
- Check USB cable and port
- Verify permissions: `sudo usermod -a -G dialout $USER`

### Runtime Issues

**No serial output**:
- Check baud rate (115200)
- Verify serial port
- Press RESET button on ESP32

**LED not blinking**:
- Check LED_GPIO matches your board
- Most dev boards use GPIO2, some use GPIO5

## License

This project is provided as-is for educational purposes.

## Contributing

Feel free to submit issues and enhancement requests.

## Resources

- [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [Xtensa ISA Reference](https://0x04.net/~mwk/doc/xtensa.pdf)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
