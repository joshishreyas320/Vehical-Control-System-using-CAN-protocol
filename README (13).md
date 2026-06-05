# Vehicle Control System — CAN Protocol

> Embedded C project implementing a **two-node CAN 2.0A communication system** for automotive body control and reverse parking assistance.  
> Hardware: **NXP LPC2129 (ARM7TDMI-S)** on **Universal Embedded Systems Testing Kit** | IDE: **Keil µVision 5**

---

## Project Overview

This project simulates a real-world **Vehicle Body Control Module (BCM)** using the CAN 2.0A protocol on two LPC2129 nodes:

| Node | Role | Function |
|------|------|----------|
| **Node A (TX)** | Transmitter / Sensor node | Reads driver switches + ultrasonic sensors → transmits CAN frames |
| **Node B (RX)** | Receiver / Actuator node | Receives CAN frames via interrupt → drives LEDs, buzzer, LCD |

---

## Repository Structure

```
Vehical_Control_System_CAN_protocol/
├── TX_NODE_A/
│   ├── Node_a_main.c          # Main loop — switch polling & CAN TX state machine
│   ├── Node_A_Interrupt.c     # EINT0/1/2 ISRs for headlight + indicator switches
│   ├── Can_Node_A_Driver.c    # CAN2 init (can2_init) + transmit (can2_tx) driver
│   ├── Uart_Driver.c          # UART0 full driver — TX/RX/int/float/hex/string
│   ├── Reverse_parking_A.c    # ADC init, 3x ultrasonic read, distance TX via CAN
│   └── header.h               # Typedefs (u8/u16/u32), CAN2 struct, extern declarations
│
└── RX_NODE_B/
    ├── Node_B_main.c          # Main loop — flag-based actuator control
    ├── nodeB_interrupt.c      # Can2_RX_HANDLER ISR + VIC config + CAN IER enable
    ├── can2_nodeb_driver.c    # CAN2 init (acceptance filter) + can2_rx polling
    ├── Delay.c                # Timer0-based delay_ms / delay_sec
    ├── Reverse_parking_B.c    # Sensor processing, alert levels, LED/buzzer output
    └── header.h               # Typedefs, CAN2 struct, extern declarations
```

---

## System Block Diagram

![Vehicle Control System — CAN Protocol Block Diagram](block_diagram.png)

---

## CAN Message Protocol

### Body Control Frames (Node A → Node B)

| CAN ID  | byteA  | Event                  |
|---------|--------|------------------------|
| `0x200` | `0xAA` | Headlight **ON**        |
| `0x200` | `0xBB` | Headlight **OFF**       |
| `0x201` | `0xCC` | Left Indicator **ON**   |
| `0x201` | `0xDD` | Left Indicator **OFF**  |
| `0x202` | `0xEE` | Right Indicator **ON**  |
| `0x202` | `0xFF` | Right Indicator **OFF** |

### Reverse Parking Frames (Node A → Node B)

| CAN ID  | Sensor           | Data             |
|---------|------------------|------------------|
| `0x100` | Left ultrasonic  | Distance in cm   |
| `0x101` | Center ultrasonic| Distance in cm   |
| `0x102` | Right ultrasonic | Distance in cm   |

### Alert Levels — Node B Output

| Level    | Distance   | LED    | Buzzer Pattern        |
|----------|------------|--------|-----------------------|
| NONE     | > 150 cm   | OFF    | Silent                |
| INFO     | 60–150 cm  | Green  | 1 beep/sec (100 ms)   |
| WARNING  | 30–60 cm   | Yellow | Medium beep (150 ms)  |
| CRITICAL | < 30 cm    | Red    | Rapid beep (150 ms)   |

---

## Key Implementation Details

### Node A — Transmitter
- **Debounce:** `while(pin==0)` hold-until-release before toggling state flag
- **Toggle logic:** `flag_HL/LI/RI` XOR-toggled; `flag_LI` and `flag_RI` mutually cancel each other
- **CAN TX:** Writes `C2TFI1/C2TID1/C2TDA1/C2TDB1`, polls `C2GSR[3]` for TX complete
- **Baud rate:** `VPBDIV=1` (VPB=60 MHz), `C2BTR=0x001C001D` → 500 kbps
- **UART debug:** Every CAN frame logged via `uart0_tx_string()`

### Node B — Receiver
- **CAN RX ISR:** `Can2_RX_HANDLER` reads `C2RID/C2RFS/C2RDA/C2RDB`, sets `flag=1`, clears `C2CMR=(1<<2)`
- **VIC:** Channel 27 = CAN2 RX, `VICVectCntl0=(1<<5)|27`, `VICIntEnable=(1<<27)`
- **Indicator blink:** 200 ms ON/OFF software loop with buzzer; exits on new CAN flag
- **Acceptance filter:** `AFMR=0x2` bypass mode — all CAN IDs accepted

---

## Build & Flash

### Prerequisites
- Keil µVision 5 (ARM-MDK)
- Universal Embedded Systems Testing Kit (2× LPC2129, onboard CAN transceiver)
- USB-serial for UART0 debug (Tera Term / PuTTY, 9600 baud 8N1)

```
1. Open TX_NODE_A/Node_A.uvproj → Build (F7) → Flash Node_A.hex to Node A
2. Open RX_NODE_B/Node_B.uvproj → Build (F7) → Flash Node_B.hex to Node B
3. Connect CAN-H / CAN-L (120Ω termination at each end)
4. Power both boards → Monitor UART0 @ 9600 baud
```

---

## Test Results

| Test                  | Input (Node A)        | Output (Node B)              | Result    |
|-----------------------|-----------------------|------------------------------|-----------|
| Headlight ON/OFF      | HL switch × 2         | P0.17 LED toggles            |  Pass   |
| Left Indicator        | LI switch press       | P0.18 blink 200ms + buzzer   |  Pass   |
| Right Indicator       | RI switch press       | P0.19 blink 200ms + buzzer   |  Pass   |
| LI → RI mutual cancel | LI ON then RI press   | P0.18 OFF, P0.19 blinks      |  Pass   |
| Parking CRITICAL      | Object < 30 cm        | Red LED + rapid beep         |  Pass   |
| Parking WARNING       | Object 30–60 cm       | Yellow LED + medium beep     |  Pass   |
| Parking INFO          | Object 60–150 cm      | Green LED + 1 beep/sec       |  Pass   |
| Parking NONE          | Object > 150 cm       | All LEDs OFF, silent         |  Pass   |

---

## Features

- [x] CAN 2.0A — 11-bit standard frame, DLC = 8, 500 kbps
- [x] Interrupt-driven CAN reception via VIC (channel 27)
- [x] Headlight toggle over CAN
- [x] Left / Right indicators with mutual exclusion logic
- [x] Buzzer synchronized with indicator blink
- [x] Reverse parking — 3 ultrasonic/IR sensors via ADC
- [x] 4-level proximity alert (NONE / INFO / WARNING / CRITICAL)
- [x] RGB LED alert visualization on Node B
- [x] 16×2 LCD distance + alert status display
- [x] UART0 serial debug logging on both nodes

---

## Author

**Shreyas**  
Embedded Systems Engineer  
Platform: Universal Embedded Systems Testing Kit · NXP LPC2129 · Keil µVision 5

---

## License

Educational / Portfolio project.
