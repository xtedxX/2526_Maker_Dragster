# 🏎️ DRAGSTER — Maker Option Project

**A fully custom brushless electric dragster built from scratch: custom PCB, STM32 firmware, 3D-printed chassis and Bluetooth telemetry.**

![Platform](https://img.shields.io/badge/MCU-STM32G431CBU6-blue)
![Motor](https://img.shields.io/badge/Motor-Brushless%203000kV-red)
![Connectivity](https://img.shields.io/badge/Bluetooth-HC--05%20SPP-green)
![PCB](https://img.shields.io/badge/PCB-4%20layers%2060x60mm-orange)
![IDE](https://img.shields.io/badge/IDE-STM32CubeIDE-yellow)

---

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Project Structure](#project-structure)
- [Electronic Design](#electronic-design)
- [Software](#software)
- [3D Modelling and Mechanical Aspect](#3d-modelling-and-mechanical-aspect)

---

## 🚀 Project Overview

This project was carried out as part of the **Maker Option** at school. The goal was to design and build a functional brushless electric dragster entirely from scratch, covering every aspect of the engineering process:

- Custom **4-layer PCB** designed in KiCad
- **STM32G431CBU6** microcontroller firmware written in C with STM32CubeIDE
- **Bluetooth** remote control and telemetry via HC-05
- **3D-printed** chassis and mechanical parts designed in Onshape
- **Brushless motor** driven by a commercial ESC via PWM

---

## 📁 Project Structure

```
DRAGSTER/
│
├── 📖 README.md                        # This file
│
├── 🖼️ IMG/                             # Photos of the physical build
│   ├── dragster_assembled.jpg          # Final assembled dragster
│   ├── pcb_3d_top.png                  # KiCad 3D render top
│   ├── pcb_3d_bottom.png               # KiCad 3D render bottom
│   ├── pcb_real_top.jpg                # Real PCB top view
│   ├── pcb_real_bottom.jpg             # Real PCB bottom view
│   ├── corps_3d.png                    # Dragster body 3D render
│   ├── support_moteur_3d.png           # Motor mount 3D render
│   ├── support_roues_3d.png            # Wheel axle support 3D render
│   └── guide_roues_3d.png             # Lateral wheel guide 3D render
│
├── 📐 CAO/                             # CAD files and design resources
│   ├── DRAGSTER_Cube_MX/               # STM32CubeMX pin configuration project
│   │   └── DRAGSTER_Cube_MX.ioc        # CubeMX IOC file (pin mapping)
│   ├── DRAGSTER_PCB/                   # KiCad PCB project
│   │   ├── DRAGSTER.kicad_pcb          # PCB layout
│   │   ├── DRAGSTER.kicad_sch          # Schematic
│   │   └── ...
│   └── DRAGSTER_3D/                    # Onshape STL exports
│       ├── Corps.stl                   # Dragster body
│       ├── Support_Moteur.stl          # Motor mount
│       ├── Support_roues.stl           # Camshaft / wheel axle support
│       └── Guide_roues.stl             # Lateral wheel guide
│
└── 💻 DRAGSTER_FIRMWARE/               # STM32CubeIDE project
    └── DRAGSTER/
        ├── Core/
        │   ├── Src/
        │   │   ├── main.c              # Application entry point
        │   │   ├── bldc_esc.c          # ESC / brushless motor driver
        │   │   ├── hc05_bt.c           # Bluetooth driver + command parser
        │   │   ├── mma8452q.c          # Accelerometer driver
        │   │   ├── tc1602a_lcd.c       # LCD driver
        │   │   └── ws2812.c            # NeoPixel LED driver
        │   └── Inc/
        │       ├── bldc_esc.h
        │       ├── hc05_bt.h
        │       ├── mma8452q.h
        │       ├── tc1602a_lcd.h
        │       └── ws2812.h
        └── ...
```

---

## ⚡ Electronic Design

### Components and Sensors

| Component | Model | Key Characteristics |
|---|---|---|
| **Microcontroller** | STM32G431CBU6 | ARM Cortex-M4, 170 MHz, UFQFPN48, 128 KB Flash, 32 KB RAM |
| **Brushless Motor** | Robitronic Razer Ten 3652 3000kV (R01230) | 3000 kV, 3652 form factor, suitable for 3S LiPo |
| **ESC** | Robitronic Razer Ten ESC | PWM control 50 Hz, pulse range 1–3 ms, waterproof |
| **Accelerometer** | MMA8452Q | 3-axis, 12-bit, I²C, ±2/4/8g selectable, 800 Hz ODR |
| **LCD Screen** | TC1602A | 16×2 characters, HD44780 compatible, 4-bit mode |
| **Bluetooth Module** | HC-05 | Bluetooth 2.0 Classic SPP, UART 9600 baud, 3.3V logic |
| **NeoPixel LED** | WS2812B | RGB addressable LED, 800 kHz PWM protocol, 5V ⚠️ |
| **Battery** | LiPo 3S | 11.1V nominal, powers motor and ESC |
| **Buck Regulator** | LMR51625 → Würth 173950378 (replacement) | 5V / 3.3V regulation — LMR51625 replaced due to KiCad footprint error |
| **Protection Transistor** | SQ2389CES-T1_GE3 | P-channel MOSFET for reverse polarity protection — not populated on final board |

> ⚠️ **NeoPixel note:** The WS2812B requires a 5V supply but was wired to 3.3V on the PCB, which is below the minimum operating voltage (3.5V). The LED is therefore non-functional on the current board revision.

> ⚠️ **Reverse polarity protection note:** The SQ2389CES-T1_GE3 was included in the schematic but was not soldered on the final board.

---

### PCB Design

The PCB was designed entirely from scratch using **KiCad**. Before laying out the PCB, **STM32CubeMX** was used to plan and assign all the required peripheral pins. The resulting IOC file can be found at:

```
CAO/DRAGSTER_Cube_MX/DRAGSTER_Cube_MX.ioc
```

#### PCB Specifications

| Parameter | Value |
|---|---|
| Dimensions | 60 mm × 60 mm |
| Number of layers | 4 |
| Layer stackup | Signal / Ground / Power / Signal |
| Layer 1 (Top) | Signal routing |
| Layer 2 | Ground plane |
| Layer 3 | Power plane |
| Layer 4 (Bottom) | Signal routing |
| Manufactured by | JLCPCB |

The 4-layer stackup with two dedicated ground planes provides excellent EMI shielding — important given the high-current brushless motor switching nearby.

#### KiCad 3D Renders

![PCB 3D render — top](IMG/pcb_3d_top.png)

![PCB 3D render — bottom](IMG/pcb_3d_bottom.png)

#### Real PCB Photos

![Real PCB — top](IMG/pcb_real_top.jpeg)

![Real PCB — bottom](IMG/pcb_real_bottom.jpeg)

![Real PCB — with components](IMG/mounted.jpeg)

---

## 💻 Software

### Pin Assignment — STM32CubeMX

Before writing any firmware, **STM32CubeMX** was used to plan the peripheral and pin assignments. This ensures no conflicts between peripherals and lets CubeMX generate the HAL initialisation code automatically.

The IOC file is located at:
```
CAO/DRAGSTER_Cube_MX/DRAGSTER_Cube_MX.ioc
```

Key peripheral assignments:

| Peripheral | Pins | Function |
|---|---|---|
| I2C1 | PA15 (SCL), PB7 (SDA) | MMA8452Q accelerometer |
| USART2 | PA2 (TX), PA3 (RX) | HC-05 Bluetooth |
| USART1 | PA9 (TX), PA10 (RX) | ST-Link debug output |
| TIM3 CH1 | PA6 | ESC PWM signal |
| TIM2 CH4 | PB11 | WS2812B NeoPixel DMA PWM |
| GPIO Output | PA8 | LCD RS |
| GPIO Output | PC6 | LCD E |
| GPIO Output | PB12–PB15 | LCD DB4–DB7 |
| GPIO Input | PC13 | Accelerometer INT1 |
| GPIO Input | PB9 | Accelerometer INT2 |

---

### Firmware File Tree

```
DRAGSTER_FIRMWARE/DRAGSTER/Core/
│
├── Src/
│   ├── main.c              # Entry point — initialises all peripherals and runs main loop
│   ├── bldc_esc.c          # Brushless motor ESC driver (TIM3 CH1 PWM)
│   ├── hc05_bt.c           # HC-05 Bluetooth driver + command parser
│   ├── mma8452q.c          # MMA8452Q accelerometer driver (I2C)
│   ├── tc1602a_lcd.c       # TC1602A 16x2 LCD driver (4-bit mode)
│   └── ws2812.c            # WS2812B NeoPixel driver (TIM2 DMA PWM)
│
└── Inc/
    ├── bldc_esc.h          # ESC driver interface and configuration defines
    ├── hc05_bt.h           # BT driver interface and handle definition
    ├── mma8452q.h          # Accelerometer driver interface
    ├── tc1602a_lcd.h       # LCD driver interface
    └── ws2812.h            # NeoPixel driver interface
```

---

### Code Block Descriptions

#### `main.c` — Application Entry Point

`main.c` is the top-level file generated by STM32CubeIDE and extended with user code. It performs the following in order:

1. `HAL_Init()` — initialises the HAL library and SysTick
2. `SystemClock_Config()` — configures the PLL to run the STM32G431 at **170 MHz** from the 8 MHz HSE crystal
3. All `MX_xxx_Init()` calls — initialises GPIO, DMA, I2C1, TIM2, TIM3, USART1, USART2
4. `HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1)` — starts the ESC PWM output immediately after TIM3 init, before the ESC times out
5. `LCD_Splash()` — displays the boot splash screen
6. `MMA8452Q_Init()` — initialises the accelerometer over I2C
7. `BT_Init()` — initialises the Bluetooth driver and arms the UART receive interrupt
8. `ESC_Init()` — runs the ESC arming sequence (blocking, ~8 seconds)
9. Main `while(1)` loop — calls `BT_Task()` every iteration and reads accelerometer data to update the LCD speed display

---

#### `bldc_esc.c` / `bldc_esc.h` — Brushless Motor ESC Driver

Controls the brushless motor via a standard RC PWM signal on **TIM3 CH1 (PA6)**.

**Timer configuration:**
- SYSCLK = 170 MHz, PSC = 169 → timer clock = 1 MHz (1 tick = 1 µs)
- ARR = 19999 → period = 20 ms = **50 Hz**

**Verified PWM compare values:**

| Value | Meaning |
|---|---|
| `199` | Minimum throttle (0%) |
| `9999` | Maximum throttle (100%) — 50% duty cycle |
| `2999` | Neutral — used during arming sequence only |

**Arming sequence** (verified on hardware):
1. Send `2999` for 4 s → ESC beeps, recognises top-of-range
2. Send `199` for 2 s → ESC arms
3. Send `2999` for 2 s → ESC ready

**Key functions:**

| Function | Description |
|---|---|
| `ESC_Init()` | Runs arming sequence. PWM must already be started before calling. |
| `ESC_SetThrottlePct(pct)` | Sets throttle 0–100%. Maps linearly between `ESC_PULSE_MIN_US` and `ESC_PULSE_MAX_US`. |
| `ESC_SetPulse(pulse_us)` | Sets throttle directly as a compare value. |
| `ESC_Stop()` | Immediately returns motor to minimum throttle. |
| `ESC_RampTo(pct, step_ms)` | Gradually ramps throttle to target percentage. Blocking. |

**Configurable defines in `bldc_esc.h`:**
```c
#define ESC_PULSE_MIN_US    199U    /* 0%   — min throttle */
#define ESC_PULSE_MAX_US    9999U   /* 100% — max throttle */
```
Changing `ESC_PULSE_MAX_US` automatically recalculates the percentage mapping.

---

#### `hc05_bt.c` / `hc05_bt.h` — Bluetooth Driver

Manages the HC-05 module over **USART2 (PA2/PA3)** at 9600 baud using interrupt-driven single-byte reception.

**`BT_Task()`** is called every main loop iteration and handles:
1. Processing any pending received command
2. Reading the accelerometer and updating peak g
3. Sending live `$ACC` data if streaming is enabled
4. Sending a `$HB` heartbeat every 1 second

There is **no auto-start or auto-stop** — the motor only runs when explicitly commanded via Bluetooth.

---

#### Bluetooth Command Reference

**Phone/PC → MCU:**

| Command | Description | Response |
|---|---|---|
| `PING` | Connectivity test | `$HB` |
| `INFO` | Request firmware info | `$INFO,FW=1.0,RANGE=8G,ODR=800Hz,BAUD=9600` |
| `STREAM,ON` | Enable live accelerometer stream | `$ACK,STREAM,ON` |
| `STREAM,OFF` | Disable live accelerometer stream | `$ACK,STREAM,OFF` |
| `THROTTLE,<0-100>` | Set motor throttle percentage | `$ACK,THROTTLE,<pct>` |
| `MOTOR,STOP` | Stop motor immediately | `$ACK,MOTOR,STOP` |
| `BURST,<pct>,<ms>` | Run motor at pct% for ms milliseconds then stop | `$ACK,BURST,<pct>,<ms>` → `$ACK,BURST,DONE` |
| `RESET` | Reset peak g to zero | `$ACK,RESET` |

**MCU → Phone/PC (unsolicited):**

| Frame | Description |
|---|---|
| `$HB` | Heartbeat every 1 second |
| `$ACC,<g>` | Live X-axis acceleration in g (when streaming) |
| `$PEAK,<g>` | New peak g detected |
| `$ERR,<code>` | Error notification |

**Where to send commands:**
- **Windows:** pair HC-05 in Bluetooth settings (PIN: `1234`), open **Tera Term**, connect to the resulting COM port at 9600 baud
- **Android:** use *Serial Bluetooth Terminal* by Kai Morich
- **iOS:** not supported — HC-05 uses Classic SPP which iOS does not expose to third-party apps

#### Typical Tera Term console session

```
[BT] HC-05 driver initialised
$INFO,FW=1.0,RANGE=8G,ODR=800Hz,BAUD=9600
$HB
$HB
[BT CMD] PING
$HB
[BT CMD] STREAM,ON
$ACK,STREAM,ON
$ACC,0.0021
$ACC,-0.0013
[BT CMD] STREAM,OFF
$ACK,STREAM,OFF
[BT CMD] THROTTLE,30
$ACK,THROTTLE,30
[BT CMD] MOTOR,STOP
$ACK,MOTOR,STOP
[BT CMD] BURST,20,1000
$ACK,BURST,20,1000
$ACK,BURST,DONE
```

---

#### `mma8452q.c` / `mma8452q.h` — Accelerometer Driver

Drives the MMA8452Q over **I2C1 (PA15 SCL / PB7 SDA)**.

- Uses `HAL_I2C_Mem_Read()` with a repeated start — required by this sensor
- Configured at **±8g range** and **800 Hz ODR** for dragster launches
- Only the **X-axis** is read, aligned with the direction of travel
- 12-bit left-justified output is reconstructed by right-shifting 4 bits

---

#### `tc1602a_lcd.c` / `tc1602a_lcd.h` — LCD Driver

Drives the TC1602A 16×2 LCD in **4-bit mode** using GPIO bit-banging with `HAL_Delay()` timing.

> All timing uses `HAL_Delay()` rather than microsecond busy-wait loops, ensuring correct operation at 170 MHz.

**Display layout during operation:**
```
+------------------+
| Speed:   87.3 km/h |   ← Row 0: live speed
| Peak:   102.5 km/h |   ← Row 1: peak speed
+------------------+
```

Speed is computed by trapezoidal integration of the X-axis acceleration: `v += (a₀ + a₁) / 2 × Δt`, then converted to km/h.

---

#### `ws2812.c` / `ws2812.h` — NeoPixel Driver

Drives a single WS2812B LED using **TIM2 CH4 (PB11)** with DMA PWM at ~800 kHz.

> ⚠️ Currently non-functional — the LED requires 5V but is wired to 3.3V on the PCB.

---

### Dragster Behaviour Summary

| Phase | Description |
|---|---|
| Boot | Splash screen → accelerometer init → BT init → ESC arming (~8 s) → ready |
| Idle | Heartbeat `$HB` every second, motor at minimum throttle |
| Motor control | Manual only via `THROTTLE`, `BURST`, `MOTOR,STOP` commands |
| Speed display | LCD shows live speed and peak speed computed from accelerometer |
| Peak tracking | Peak g tracked continuously, broadcast via `$PEAK` when updated |

---

## 🔩 3D Modelling and Mechanical Aspect

All mechanical parts were designed in **Onshape** and 3D printed. STL files are located in `CAO/DRAGSTER_3D/`.

### Dragster Body — `CAO/DRAGSTER_3D/Corps.stl`

A thick rectangular flat plate serving as the main chassis. All components mount onto it.

![Dragster body — 3D render](IMG/corps_3d.png)

---

### Motor Mount — `CAO/DRAGSTER_3D/Support_Moteur.stl`

A bracket that rigidly holds the brushless motor at the rear of the chassis, aligned with the drive belt pulley.

![Motor mount — 3D render](IMG/support_moteur_3d.png)

---

### Wheel Axle Support — `CAO/DRAGSTER_3D/Support_roues.stl`

Supports the rear camshaft/axle on which the rear wheels and driven pulley are mounted.

![Wheel axle support — 3D render](IMG/support_roues_3d.png)

---

### Lateral Wheel Guide — `CAO/DRAGSTER_3D/Guide_roues.stl`

A cylindrical piece that prevents the dragster from drifting sideways during acceleration, ensuring straight-line movement.

![Lateral wheel guide — 3D render](IMG/guide_roues_3d.png)

---

### Drivetrain

The motor drives the rear axle via a **GT2 drive belt (158-2GT)** and two pulleys, providing a gear reduction between the motor pinion and the rear wheel axle. The **rear wheels are larger than the front wheels**, just like in a real dragster — this improves traction under hard acceleration. There is **no parachute** braking system.

---

### Assembled Dragster

![Fully assembled dragster](IMG/dragster_assembled.jpeg)

---

## 📝 Known Issues

| Issue | Status | Details |
|---|---|---|
| NeoPixel not lighting | ⚠️ Known | WS2812B wired to 3.3V — requires 5V minimum |
| Reverse polarity protection | ⚠️ Not populated | SQ2389CES-T1_GE3 designed but not soldered |
| Buck regulator replaced | ✅ Fixed | LMR51625 footprint error in KiCad — replaced with Würth 173950378 |
| iOS Bluetooth | ❌ Not supported | HC-05 uses Classic SPP, not supported by iOS third-party apps |

---

**School:** Maker Option  
**Target:** STM32G431CBU6 @ 170 MHz  
**IDE:** STM32CubeIDE  
**PCB design:** KiCad  
**3D design:** Onshape  
**Bluetooth terminal:** Tera Term (Windows) / Serial Bluetooth Terminal (Android)

---

*Built with ❤️ for the Maker Option*