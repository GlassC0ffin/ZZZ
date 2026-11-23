# Distance Names

## Charter

Distance Names uses ultrasonic distance to choose a group name, then responds with that name on an OLED and a matching light signal. It does not recognize people: it names a distance zone, not anyone's identity.

## Our Sensor

![Our assembled sensor piece: Nano, two ultrasonic modules, OLED, and LEDs](bench/img/finished-piece.jpg)

The finished prototype has two ultrasonic modules, an Arduino Nano, an OLED, and LEDs. The current sketch reads the primary module on D9/D10; the second module is physically present but is not used by this version of the code. Each state already works from the first sensor alone.

## State Table

| Owner | State | Distance rule | Hold | Response |
|---|---|---:|---:|---|
| Jaxson William Character | Jaxson | `cm <= 30` | 0 s | Red LED |
| Diego Ortega | Diego | `30 < cm <= 120` | 0 s | Green LED |
| Brennan Soloman Howard-Ruelas (#BOI) | #BOI | `120 < cm <= 200` | 0 s | Blue LED |
| Group | nobody | `cm > 200` or no echo | 0 s | All LEDs off |

The state changes on the first filtered reading in a new range. The sensor takes the median of three quick measurements to reject a single bad echo; there is no timed hold or artificial pause. Each name has its own light: Jaxson has red, Diego green, and #BOI blue.

## Wiring

The code's primary ultrasonic module is wired `TRIG -> D9`, `ECHO -> D10`, `VCC -> 5V`, and `GND -> GND`. The OLED uses I2C (`SDA -> A4`, `SCL -> A5`). Jaxson's red LED uses D3, Diego's green LED uses D4, and #BOI's blue LED uses D5; each LED uses a current-limiting resistor. Connect all grounds together. Confirm the second module's pin connections against the physical build before enabling it in code.

![Reference Nano wiring](bench/img/nano-pins.jpg)

## Run It

1. Open `bench/bench.ino` in Arduino IDE, choose Arduino Nano and **ATmega328P (Old Bootloader)** if your clone needs it, then upload.
2. Power the Nano with a USB wall charger or power bank; the sketch starts automatically and does not need a laptop or Serial Monitor.
3. Move a hand through each distance range. The OLED shows the name and centimetres, and each name has its own LED.

Serial output is available for testing at 115200 baud as `cm 23.6`. Close the Serial Monitor before connecting `bench.html`.

## Break Log

| Issue | Change |
|---|---|
| Ultrasonic echo occasionally jumps | Use the median of three readings; no extra timed hold |
| Slow response at state boundaries | Removed per-state delays and the loop's 50 ms delay |
| Name responses looked alike | Use red for Jaxson, green for Diego, and blue for #BOI; all off for nobody |
| No echo | Treat timeout or readings under 2 cm as 400 cm, which selects nobody |

## Legibility and Fairness

**For them:** the OLED says the state name and distance, and a matching LED makes the response visible. **To them:** someone can receive a name because of where they stand, not who they are. The piece may misname a visitor at a range boundary and makes no claim to identify them.

## Credits

- Jaxson William Character owns Jaxson.
- Diego Ortega owns Diego.
- Brennan Soloman Howard-Ruelas owns #BOI.
- The group owns the nobody state and shared build.
- Sensor: two ultrasonic modules are mounted; current code uses one.
- Repository: https://github.com/GlassC0ffin/ZZZ
- Live page: https://glassc0ffin.github.io/ZZZ/
