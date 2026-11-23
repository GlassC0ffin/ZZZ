# Distance Names

## Charter

Distance Names uses ultrasonic distance to choose a group name, then responds with that name on an OLED and a matching light signal. It does not recognize people: it names a distance zone, not anyone's identity.

## Our Sensor

![Our assembled sensor piece: Nano, two ultrasonic modules, OLED, and LEDs](bench/img/finished-piece.jpg)

The finished prototype has two ultrasonic modules, an Arduino Nano, an OLED, and LEDs. The current sketch reads the primary module on D9/D10; the second module is physically present but is not used by this version of the code. Each state already works from the first sensor alone.

## State Table

| Owner | State | Distance rule | Hold | Response |
|---|---|---:|---:|---|
| Jaxson William Character | Jaxson | `cm <= 30` | 0 s | Piezo on D6 at 880 Hz; strip on |
| Diego Ortega | Diego | `30 < cm <= 120` | 0 s | Piezo on D7 at 660 Hz; strip on |
| Brennan Soloman Howard-Ruelas (#BOI) | #BOI | `120 < cm <= 200` | 0 s | Piezo on D13 at 440 Hz; strip on |
| Group | nobody | `cm > 200` or no echo | 0 s | Buzzers off; strip off |

The state changes on the first filtered reading in a new range. The sensor takes the median of three quick measurements to reject a single bad echo; there is no timed hold or artificial pause. Each person's state drives a different piezo and pitch. The LED strip on A1 lights for any named state and turns off for nobody.

## Wiring

The code's primary ultrasonic module is wired `TRIG -> D9`, `ECHO -> D10`, `VCC -> 5V`, and `GND -> GND`. The OLED uses I2C (`SDA -> A4`, `SCL -> A5`). The piezo signal pins are D6 (Jaxson), D7 (Diego), and D13 (#BOI); connect each piezo's other lead to GND. A1 controls the LED strip. Do not power a strip directly from a Nano pin: use a suitable transistor/MOSFET driver and external supply, with a shared ground. Confirm the second ultrasonic module's pin connections against the physical build before enabling it in code.

![Reference Nano wiring](bench/img/nano-pins.jpg)

## Run It

1. Open `bench/bench.ino` in Arduino IDE, choose Arduino Nano and **ATmega328P (Old Bootloader)** if your clone needs it, then upload.
2. Power the Nano with a USB wall charger or power bank; the sketch starts automatically and does not need a laptop or Serial Monitor.
3. Move a hand through each distance range. The OLED shows the name and centimetres; its assigned piezo sounds and the LED strip turns on for named states.

Serial output is available for testing at 115200 baud as `cm 23.6`. Close the Serial Monitor before connecting `bench.html`.

## Break Log

| Issue | Change |
|---|---|
| Ultrasonic echo occasionally jumps | Use the median of three readings; no extra timed hold |
| Slow response at state boundaries | Removed per-state delays and the loop's 50 ms delay |
| Name responses sounded alike | Give Jaxson, Diego, and #BOI separate piezo pins and pitches |
| No echo | Treat timeout or readings under 2 cm as 400 cm, which selects nobody |

## Legibility and Fairness

**For them:** the OLED says the state name and distance; the name has its own piezo and pitch. **To them:** someone can receive a name because of where they stand, not who they are. The piece may misname a visitor at a range boundary and makes no claim to identify them.

## Credits

- Jaxson William Character owns Jaxson.
- Diego Ortega owns Diego.
- Brennan Soloman Howard-Ruelas owns #BOI.
- The group owns the nobody state and shared build.
- Sensor: two ultrasonic modules are mounted; current code uses one.
- Repository: https://github.com/GlassC0ffin/ZZZ
- Live page: https://glassc0ffin.github.io/ZZZ/
