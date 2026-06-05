# DC-DC Buck Converter Using IC 555 Timer

A step-up (boost) DC-DC converter that raises a **6V input to 36V output** using the NE555 timer IC in astable mode to generate the PWM switching signal. Designed and laid out as a two-layer PCB in KiCad 9.

---

## Overview

This project implements a DC-DC boost converter built around discrete components — no dedicated switching-regulator IC. The NE555 generates a high-frequency PWM signal, which drives a BJT switch (BC547/BC557 complementary pair). The switching action charges an inductor that, combined with a freewheeling diode (1N4004) and output capacitors, boosts the voltage from 6V to approximately 36V.

---

## Project Structure

```
DC_DC_Buck_Converter/
├── ckt.jpg                          # Circuit schematic image
└── DC_DC_Buck_Converter/
    ├── DC_DC_Buck_Converter.kicad_sch   # KiCad schematic
    ├── DC_DC_Buck_Converter.kicad_pcb   # PCB layout
    ├── DC_DC_Buck_Converter.kicad_pro   # Project file
    ├── DC_DC_Buck_Converter.kicad_prl   # Local settings
    ├── fp-info-cache                    # Footprint cache
    └── DC_DC_Buck_Converter-backups/
        ├── *.gbr                        # Gerber files (PCB layers)
        ├── *.drl                        # Drill files
        ├── DC_DC_Buck_Converter-job.gbrjob
        └── Gerber_Files_DC_DC_Buck_Converter/
            └── *.gtl / *.gbl / ...      # Fabrication-ready Gerbers
```

---

## Circuit Details

| Parameter        | Value               |
|-----------------|---------------------|
| Input Voltage   | 6V DC               |
| Output Voltage  | ~36V DC             |
| Switching IC    | NE555D (SOIC-8)     |
| Switch          | BC547 + BC557 BJTs  |
| Freewheeling    | 1N4004 (×3)         |
| Timing Resistor | 6.8 kΩ              |
| Timing Capacitor| 10 nF               |
| Output Cap      | 1000 µF / 25V       |
| Filter Cap      | 100 µF / 63V, 47 µF / 63V |
| Load Resistor   | 56 Ω                |
| Base Resistor   | 180 Ω               |

### Component List (BOM)

| Reference | Component         | Value / Part       |
|-----------|-------------------|--------------------|
| U1        | Timer IC          | NE555D (SOIC-8)    |
| Q1        | NPN Transistor    | BC547              |
| Q2        | PNP Transistor    | BC557              |
| D1, D2, D3| Rectifier Diode   | 1N4004             |
| C1        | Electrolytic Cap  | 100 µF, 63V        |
| C2        | Electrolytic Cap  | 47 µF, 63V         |
| C3        | Electrolytic Cap  | 1000 µF, 25V       |
| C4        | Ceramic Cap       | 10 nF              |
| R1        | Resistor          | 6.8 kΩ             |
| R2        | Resistor          | 180 Ω              |
| R3        | Resistor          | 56 Ω               |
| J1        | Input Connector   | 2-pin Socket (6V)  |
| J2        | Output Connector  | 2-pin Socket (36V) |

---

## How It Works

1. **Oscillator (NE555)** — The 555 timer is configured in astable mode. R1 (6.8 kΩ) and C4 (10 nF) set the switching frequency. The output at pin 3 is a square wave PWM signal.

2. **Driver Stage (BC547 + BC557)** — The NPN BC547 and PNP BC557 form a complementary emitter-follower / switching driver. The base resistor R2 (180 Ω) limits the base current from the 555 output.

3. **Boost Action** — When the switch is ON, the inductor stores energy. When OFF, the collapsing magnetic field forces current through freewheeling diode D1 into the output capacitors, adding to the supply rail — boosting voltage above input.

4. **Output Filtering** — C3 (1000 µF) smooths the output. D2 and D3 provide additional rectification/protection. R3 (56 Ω) acts as a load resistor.

---

## PCB

- **Tool:** KiCad 9 (EESchema + PCB Editor)
- **Layers:** 2-layer board (F.Cu + B.Cu)
- **Gerbers:** Available in `DC_DC_Buck_Converter-backups/Gerber_Files_DC_DC_Buck_Converter/` — ready for fabrication upload (JLCPCB, PCBWay, etc.)

---

## Tools Required

- [KiCad 9](https://www.kicad.org/) — to open `.kicad_sch` and `.kicad_pcb`
- Any Gerber viewer (e.g., KiCad's built-in, gerbv) to inspect fabrication files

---

## License

This project is open for educational and personal use.
