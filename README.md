# NanyangVenture
**2019 NTU Final year project** that involves building a microcontroller network for 2 eco-cars.

## Functionalities
- Gather runtime telemetry on car systems
- Display important telemetry on dashboard for driver
- Radio telemetry back to ground station
- Logs telemetry in on-board SD card
- Provide driver controls for car lights, horn and wiper

## Microcontrollers programmed
- Telemetry node for NV10
- Dashboard node for NV10
- Accessory control node for NV10
- Speedometer node for NV11
- Lights control node for NV11
- Buttons monitoring node for NV11
- CAN bus to Serial node for NV11
- Dashboard RPi script for NV11 (courtsey to Melvin Foo)

## Featured self-made libraries
- CAN bus packaging library to convert between CAN frames and Strings, built on [coryjfowler's mcp2515 library](https://github.com/coryjfowler/MCP_CAN_lib
).
- Widgets library aimed to better display data, built on [jaretburkett's ILI9488 library](https://github.com/jaretburkett/ILI9488).
- Speedometer library that regularly collects interrupt ticks into km/h value.

### The project has been passed down. New repo maintained privately by @laksh22.
