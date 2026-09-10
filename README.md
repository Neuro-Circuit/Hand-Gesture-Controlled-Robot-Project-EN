# Hand Gesture Robot Control

> **Real-time hand gesture recognition and wireless robot control ---
> Python, MediaPipe, WebSocket, UDP, ESP32, and L298N**

[![Python](https://img.shields.io/badge/Python-3.10%2B-blue?logo=python)](https://www.python.org/)
[![MediaPipe](https://img.shields.io/badge/MediaPipe-Hands-orange)](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker)
[![ESP32](https://img.shields.io/badge/ESP32-Wi--Fi-green)](https://www.espressif.com/en/products/socs/esp32)
[![License](https://img.shields.io/badge/Architecture-Plugin--based-purple)](#project-architecture)

## Overview

**Hand Gesture Control** is a real-time, extensible computer-vision
system that detects hand movements through a webcam and converts them
into documented control commands.

The project is built around a **plugin-based Python architecture** and
uses the modern **MediaPipe Tasks Hand Landmarker API**. It can operate
with one or two hands independently, expose live commands through
WebSocket, display the camera stream in a browser, log commands as JSON
Lines, and send numeric movement commands over UDP to an ESP32-based
four-wheel differential-drive robot.

### End-to-end architecture

``` text
┌───────────────┐
│   Webcam      │
└───────┬───────┘
        │ Video frames
        ▼
┌─────────────────────────┐
│ Python + MediaPipe      │
│ Hand Landmark Detection │
└──────────┬──────────────┘
           │ Command objects
           ▼
┌───────────────────────────────────────┐
│ Output / Sink Layer                   │
│ Console │ JSON Log │ WebSocket │ UDP  │
└───────────────────────┬───────────────┘
                        │ UDP / Wi-Fi
                        ▼
                 ┌────────────┐
                 │   ESP32    │
                 └─────┬──────┘
                       │ GPIO
                       ▼
                 ┌────────────┐
                 │   L298N    │
                 └─────┬──────┘
                       │
                       ▼
                 ┌────────────┐
                 │ DC Motors  │
                 └────────────┘
```

------------------------------------------------------------------------

## Key Features

-   **Independent two-hand tracking:** left and right hands maintain
    separate detection history and state, so gestures do not interfere
    with one another.
-   **Two-hand geometry:** when both hands are visible, the system
    creates a geometric frame between them. Its size, center, and angle
    are exposed as a documented command, which can be used for
    interactions such as zoom control.
-   **Live browser dashboard:** `http://127.0.0.1:8000` displays the
    camera stream, hand landmarks, connections, and live command
    information.
-   **Real-time WebSocket API:** `ws://127.0.0.1:8765` allows robotics
    applications, Python scripts, Unity, Node.js, or other software to
    consume JSON commands.
-   **Stable numeric command codes:** every command has a fixed integer
    `code`, making integration with embedded systems straightforward.
-   **CPU-friendly processing:** the project is designed around
    MediaPipe hand tracking and does not require a GPU.
-   **Full Hand Rig stream:** every frame can expose the 21 hand
    landmarks, joint information, finger states, and bending angles in a
    standardized command format.
-   **Plugin-based extensibility:** new one-hand or two-hand gestures
    can be added as classes without modifying the core detection engine.
-   **Automatic documentation:** the command registry can generate both
    human-readable Markdown documentation and a machine-readable JSON
    Schema.
-   **YAML configuration:** thresholds, rates, enabled outputs, camera
    settings, and other runtime behavior can be configured without
    changing Python source code.
-   **Robot integration:** a dedicated UDP sink sends stable numeric
    commands to an ESP32 over Wi-Fi.

------------------------------------------------------------------------

## Why MediaPipe?

The project uses the modern **MediaPipe Tasks Hand Landmarker** API
because it is designed for real-time hand landmark detection and can run
efficiently on CPU-class hardware.

The project intentionally uses the newer Tasks API:

``` python
mediapipe.tasks.python.vision.HandLandmarker
```

rather than the deprecated legacy:

``` python
mp.solutions.hands
```

A small model file named `hand_landmarker.task` is required. The project
can download and cache it in `models/` during the first run. Internet
access is therefore required for the first automatic download only.

If automatic download fails, the model can be downloaded manually and
placed at:

``` text
models/hand_landmarker.task
```

------------------------------------------------------------------------

# Installation

## 1. Create a virtual environment

### Linux / macOS

``` bash
python -m venv venv
source venv/bin/activate
```

### Windows

``` powershell
python -m venv venv
venv\Scripts\activate
```

## 2. Install dependencies

``` bash
pip install -r requirements.txt
```

If the project includes a `models/` directory, make sure
`hand_landmarker.task` is present there. Otherwise, the first execution
may download it automatically.

------------------------------------------------------------------------

# Running the Application

Start the main application with:

``` bash
python main.py
```

Three output channels can be active at the same time:

  -------------------------------------------------------------------------
  Channel                 Address                   Purpose
  ----------------------- ------------------------- -----------------------
  Web dashboard           `http://127.0.0.1:8000`   Live camera, hand
                                                    landmarks, and command
                                                    log

  WebSocket API           `ws://127.0.0.1:8765`     Real-time JSON commands
                                                    for other applications

  Local OpenCV window     ---                       Fast local debugging
                                                    without a browser
  -------------------------------------------------------------------------

### Command-line options

``` text
--no-window
```

Disables the local OpenCV window while keeping the web dashboard and API
available.

``` text
--no-web
```

Disables the web server even if it is enabled in the configuration.

``` text
--config path/to/file.yaml
```

Loads an alternative YAML configuration file.

### Exiting

-   Press `q` or `Esc` in the local OpenCV window.
-   Press `Ctrl+C` in the terminal.

------------------------------------------------------------------------

# Project Architecture

``` text
hand_gesture_control/
├── config/
│   └── gestures.yaml              # Thresholds and output configuration
├── core/
│   ├── landmarks.py               # Geometry utilities for 21 hand landmarks
│   ├── command.py                 # Command structure and central registry
│   ├── gesture_base.py            # Base class for one-hand detectors
│   ├── two_hand_base.py           # Base class for two-hand detectors
│   ├── two_hand_geometry.py       # Two-hand geometric calculations
│   ├── hand_tracker.py             # MediaPipe wrapper and drawing
│   ├── dispatcher.py               # Detector execution and output dispatch
│   ├── gestures/
│   │   ├── movement.py             # Left/right/up/down/forward/backward
│   │   ├── fist.py                 # Fist / open hand
│   │   ├── pinch_rub.py            # Thumb-index pinch/rub
│   │   ├── finger_point.py         # Horizontal/vertical index-finger movement
│   │   ├── thumbs.py               # Thumb show/hide
│   │   ├── open_palm_swipe.py      # Open-palm swipe right
│   │   ├── hand_rig.py             # Full hand-rig stream
│   │   └── two_hand_frame.py       # Two-hand geometry
│   ├── output/
│   │   ├── base_sink.py            # Output interface
│   │   ├── console_sink.py
│   │   ├── json_log_sink.py        # JSON Lines logging
│   │   ├── websocket_sink.py       # Real-time WebSocket output
│   │   └── udp_sink.py             # ESP32 UDP output
│   └── web/
│       ├── broadcaster.py           # Latest-frame broadcaster
│       ├── server.py                # Flask server and MJPEG stream
│       └── templates/index.html     # Browser dashboard
├── docs/
│   ├── generate_docs.py             # Documentation generator
│   ├── commands.md                 # Generated command table
│   └── commands.schema.json        # Generated JSON Schema
├── examples/
│   ├── custom_gesture_example.py
│   └── custom_two_hand_example.py
├── Documentation/
│   └── Connection schematic.png     # Hardware connection diagram
└── main.py                          # Application entry point
```

------------------------------------------------------------------------

# Supported Gestures and Commands

The complete, generated command list is available in:

``` text
docs/commands.md
```

The current command families include:

  ------------------------------------------------------------------------------
  Category                Gesture / Data          Command ID
  ----------------------- ----------------------- ------------------------------
  Hand movement           Left / right / up /     `hand.move.*`
                          down / forward /        
                          backward                

  Gesture                 Fist / open hand        `hand.fist`, `hand.open`

  Gesture                 Thumb-index pinch/rub   `hand.pinch.start/rub/end`

  Gesture                 Horizontal / vertical   `finger.swipe.*`
                          index-finger movement   

  Gesture                 Thumb show / hide       `hand.thumb.show/hide`

  Gesture                 Open-palm swipe right   `hand.open_palm.swipe_right`

  Raw data                Full hand rig           `hand.rig.frame`

  Two-hand                Size / center / angle   `hands.pair.frame`
                          of two-hand frame       
  ------------------------------------------------------------------------------

All one-hand detectors operate independently for the left and right
hands. The only exception is `hands.pair.frame`, which requires both
hands to be visible simultaneously.

------------------------------------------------------------------------

# Numeric Command System

Every command has:

1.  A human-readable identifier: `command_id`
2.  A fixed integer: `code`
3.  Labels and metadata
4.  Optional parameters
5.  Confidence and timestamp information

The numeric code is explicitly assigned when the command is registered.
It is **not automatically generated**, which keeps the protocol stable
and controllable.

Example:

         Code Meaning
  ----------- --------------------------
          `0` Index finger moves down
          `1` Index finger moves up
          `2` Index finger moves left
          `3` Index finger moves right
          `4` Fist
    `6`--`11` General hand movement
         `19` Two-hand geometric frame

By default, console output can be configured to print only the numeric
code:

``` text
0
0
7
4
```

In `config/gestures.yaml`:

``` yaml
output:
  console:
    mode: numeric   # Default: print only the number
    # mode: full    # Full debug information
```

JSON logging and WebSocket output also include the same numeric value in
the `code` field.

------------------------------------------------------------------------

# Command Data Structure

A typical command looks like this:

``` json
{
  "command_id": "hand.move.right",
  "code": 7,
  "label_fa": "حرکت دست به راست",
  "label_en": "Hand Move Right",
  "category": "movement",
  "params": {
    "speed": 0.041
  },
  "confidence": 1.0,
  "hand": "Right",
  "timestamp": 1732650000.123
}
```

The `hand.rig.frame` command uses the same general structure, while its
`params` field contains the complete 21-point hand rig, joint names,
finger states, and bending angles.

------------------------------------------------------------------------

# Adding a New Gesture

The architecture is designed so that a new one-hand gesture can be added
without changing the core engine.

Example:

``` python
from core.gesture_base import GestureDetector
from core.command import Command, CommandRegistry
from core.landmarks import finger_states

CommandRegistry.register(
    "hand.peace_sign",
    20,
    "علامت صلح",
    "Peace Sign",
    "gesture",
    "Peace sign detected",
    {}
)

class PeaceSignDetector(GestureDetector):
    def update(self, points, frame_shape, timestamp, hand_label):
        states = finger_states(points)

        if (
            states["index"]
            and states["middle"]
            and not states["ring"]
            and not states["pinky"]
        ):
            return [
                CommandRegistry.make_command(
                    "hand.peace_sign",
                    hand=hand_label
                )
            ]

        return []
```

Then add the detector to `build_detectors` in `main.py`.

### Important

Every command code must be unique. If a duplicate code is registered,
`CommandRegistry.register` stops program loading with an explicit error
rather than allowing a silent collision.

For two-hand gestures, inherit from `TwoHandDetector` instead of
`GestureDetector` and process `left_points` and `right_points` together.

------------------------------------------------------------------------

# Real-Time WebSocket Integration

With:

``` yaml
output:
  websocket:
    enabled: true
```

another application can connect to:

``` text
ws://127.0.0.1:8765
```

Example Python client:

``` python
import asyncio
import json
import websockets

async def listen():
    async with websockets.connect("ws://127.0.0.1:8765") as ws:
        async for message in ws:
            cmd = json.loads(message)

            print(cmd["command_id"], cmd["params"])

            # Convert the command into an action in your own application.
            # Example:
            # if cmd["command_id"] == "hand.move.right":
            #     robot.move_right(cmd["params"]["speed"])

asyncio.run(listen())
```

This interface makes the recognition engine usable by robotics software,
games, simulations, character rigs, automation systems, or other
applications.

------------------------------------------------------------------------

# Robot Control Integration

The project can control a four-wheel **Differential Drive** robot using:

-   ESP32-WROOM-32U
-   L298N motor driver
-   4 DC motors
-   Two 3.7 V 18650 Li-ion batteries connected in series
-   LM2596 DC-DC buck converter
-   Wi-Fi / UDP communication

## Hardware Requirements

  Component         Requirement
  ----------------- --------------------------------------------
  Controller        ESP32-WROOM-32U, 32-pin
  Motor driver      L298N
  Motors            4 × DC motors
  Power             2 × 3.7 V 18650 Li-ion batteries in series
  DC-DC converter   LM2596
  Chassis           4-wheel robot chassis
  Prototype board   Mini breadboard

> **Power note:** The original project documentation specifies a
> regulated 5 V rail for the robot system and recommends adjusting the
> LM2596 output to exactly 5 V with a multimeter before connecting the
> electronics.

------------------------------------------------------------------------

# Hardware Wiring

The ESP32-to-L298N connections are:

    ESP32 GPIO L298N
  ------------ -------
          `27` `IN1`
          `14` `IN2`
          `12` `IN3`
          `13` `IN4`
         `VIN` `VCC`
         `GND` `GND`

## Connection Schematic

The project documentation includes the hardware schematic as:

``` text
Documentation/Connection schematic.png
```

![ESP32 / L298N / LM2596 connection
schematic](Documentation/Connection%20schematic.png)

> **GitHub / hosting note:** A relative path like the one above works
> when `Connection schematic.png` is actually committed to the
> repository at `Documentation/Connection schematic.png`. It is
> preferable to keep the image inside the repository rather than using a
> separate external URL.

In the current uploaded files, the image itself was **not available to
me**, so I could not embed the actual image bytes into this generated
README. If the image is present in your project repository, the relative
Markdown link above is sufficient. If the repository does not contain
it, upload the image or provide its direct hosted URL and replace the
image path with that URL.

## ENA / ENB

In the documented configuration, `ENA` and `ENB` remain enabled through
the driver's supply/jumper configuration so the motors operate at
maximum speed.

The current robot-control code does not implement variable motor speed
through PWM.

------------------------------------------------------------------------

# Power Supply --- LM2596

The documented power architecture is:

``` text
2 × 3.7 V 18650 batteries (series)
                │
                ▼
        ┌──────────────┐
        │    LM2596    │
        │  DC-DC Buck  │
        └──────┬───────┘
               │ regulated 5 V
        ┌──────┴─────────────┐
        ▼                    ▼
      ESP32                 L298N
                             │
                             ▼
                           Motors
```

### Wiring rules

-   Connect `IN+` and `IN-` of the LM2596 to the two terminals of the
    battery pack.
-   Before connecting the electronics, adjust `OUT+` and `OUT-` to
    **exactly 5 V** using a multimeter and the module potentiometer.
-   Connect the regulated 5 V output to the ESP32 `VIN` and `GND`.
-   Connect the appropriate logic supply input of the L298N to the
    regulated supply.
-   The motors receive their power through the L298N.
-   The ESP32, L298N, battery system, and motor power ground must share
    a common `GND`.
-   During programming, the ESP32 can also be powered through Micro-USB.

------------------------------------------------------------------------

# ESP32 UDP Receiver

The ESP32 listens for UDP packets on port:

``` text
4210
```

The expected packet payload is a numeric command code, for example:

``` text
10
```

The ESP32 converts the received code into a robot movement.

## Robot Command Mapping

  UDP Code(s)          Robot Movement
  -------------------- ----------------
  `9`, `10`, `11`      Forward
  `15`, `16`           Backward
  `6`                  Stop
  `0`, `1`, `2`, `3`   Left
  `12`, `13`, `14`     Right

The ESP32 implementation also supports:

-   Static IP configuration
-   Wi-Fi sleep disabled using `WiFi.setSleep(false)`
-   Automatic Wi-Fi reconnection
-   UDP reception on port `4210`
-   Command-to-motor mapping through the L298N

### Example ESP32 network configuration

``` cpp
IPAddress local_IP(192, 168, 1, 150);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned int localPort = 4210;
```

Replace the Wi-Fi SSID and password with the credentials of your actual
network.

------------------------------------------------------------------------

# Python UDP Sink

The Python UDP sink is located at:

``` text
core/output/udp_sink.py
```

It sends command codes to the ESP32 and includes a **stability filter**.

The stability filter prevents a single noisy frame from immediately
becoming a robot command.

Conceptually:

``` text
Detected code
     │
     ▼
Same code on consecutive frames?
     │
   No ───────────────► Reset candidate
     │
    Yes
     ▼
Reach stability_count?
     │
   No ───────────────► Wait
     │
   Yes
     ▼
Send UDP command
```

The default configuration shown in the project is:

``` python
UDPSink(
    ip="192.168.1.150",
    port=4210,
    categories=None,
    stability_count=2
)
```

The `ip` value must match the ESP32's static IP.

------------------------------------------------------------------------

# Running the Complete Robot System

1.  Power on the ESP32.
2.  Open Arduino IDE's Serial Monitor at **115200 baud**.
3.  Wait for the ESP32 to connect to Wi-Fi.
4.  Confirm its IP address.
5.  Make sure the Python `UDPSink` IP matches the ESP32 static IP.
6.  Start the Python application:

``` bash
python main.py
```

7.  Place your hand in front of the camera.
8.  Perform one of the supported gestures.
9.  The gesture detector creates a `Command`.
10. The UDP sink validates command stability.
11. The numeric code is sent over Wi-Fi.
12. The ESP32 receives the code and drives the L298N.
13. The motors execute the corresponding movement.

------------------------------------------------------------------------

# Configurable Parameters

  -----------------------------------------------------------------------
  Parameter                           Description
  ----------------------------------- -----------------------------------
  `ip`                                Static IP address of the ESP32

  `port`                              UDP port, default `4210`

  `categories`                        Restricts output to selected
                                      command categories

  `stability_count`                   Number of consecutive frames
                                      required before a command is
                                      accepted as stable

  `camera.max_num_hands`              Maximum number of simultaneously
                                      tracked hands

  `camera.model_complexity`           MediaPipe model complexity; `0` is
                                      the lightweight setting

  `hand_rig.stream_rate_hz`           Hand-rig output rate

  `two_hand_frame.stream_rate_hz`     Two-hand frame output rate
  -----------------------------------------------------------------------

For example:

``` yaml
camera:
  max_num_hands: 2
  model_complexity: 0
```

A higher `stability_count` generally produces more stable output but
increases response latency.

------------------------------------------------------------------------

# Performance Tips for Low-End Computers

For systems with limited CPU resources:

-   Keep `camera.model_complexity: 0`.
-   If only one hand is required, set:

``` yaml
camera:
  max_num_hands: 1
```

-   Use a lower camera resolution such as `320x240` if necessary.
-   Use:

``` bash
python main.py --no-window
```

to reduce CPU consumption from local image rendering. - Reduce
`hand_rig.stream_rate_hz` and `two_hand_frame.stream_rate_hz` if network
or logging traffic is too high.

------------------------------------------------------------------------

# Troubleshooting

  -----------------------------------------------------------------------
  Problem                             Possible Cause / Solution
  ----------------------------------- -----------------------------------
  Codes appear in Python but never    Check that the IP in `main.py` /
  reach the ESP32                     `UDPSink` matches the ESP32 static
                                      IP.

  Only a few packets arrive and then  Disable ESP32 Wi-Fi sleep with
  communication stops                 `WiFi.setSleep(false)`.

  The code changes continuously while Increase `stability_count` to
  the hand is steady                  filter natural detector noise.

  A new number is sent every frame    Restrict `categories` to the
                                      command groups you actually need.

  `ModuleNotFoundError`               Check the project's actual
                                      directory structure and import
                                      paths.

  Model download returns HTTP 403     Download `hand_landmarker.task`
                                      manually and place it in `models/`.

  ESP32 resets when motors start      Investigate power/voltage drop and
                                      separate the motor power path from
                                      the ESP32 supply as documented.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

# Documentation Generation

The project can automatically generate its command documentation:

``` bash
python docs/generate_docs.py
```

This updates:

``` text
docs/commands.md
docs/commands.schema.json
```

The generated documentation is based on the central command registry, so
newly registered gestures can appear automatically.

------------------------------------------------------------------------

# Known Limitations

-   The first execution may require internet access to download
    `hand_landmarker.task`. After the model is cached, the application
    can operate offline.
-   Forward/backward hand movement is an approximation because a single
    camera does not provide true depth sensing. The system uses apparent
    hand-size changes as a depth cue.
-   Detection thresholds for distance, angle, and speed are based on
    typical values and may need tuning for different lighting
    conditions, camera angles, and hand sizes.
-   The two-hand geometric frame is calculated from the wrist and the
    base of the index finger of each hand. The landmark used for the
    geometric top point can be changed through `TOP_LANDMARK_INDEX` in
    `core/two_hand_geometry.py`.
-   The current robot configuration does not provide variable motor
    speed through PWM.

------------------------------------------------------------------------

# Future Development

Possible extensions include:

-   Improve gesture accuracy through confidence-threshold tuning.
-   Improve lighting conditions and add landmark smoothing.
-   Add variable-speed robot control using `ENA` / `ENB` and PWM.
-   Add distance sensors for automatic collision avoidance.
-   Support multiple robots simultaneously using a device identifier in
    UDP messages.
-   Add more custom one-hand and two-hand gestures through the plugin
    architecture.
-   Add richer robot telemetry and bidirectional communication.
-   Build additional consumers for the WebSocket command stream, such as
    Unity or a 3D hand/character rig.

------------------------------------------------------------------------

# Recommended Repository Layout

For the cleanest GitHub presentation, keep the schematic inside the
repository:

``` text
hand_gesture_control/
├── Documentation/
│   └── Connection schematic.png
├── README.md
├── requirements.txt
├── main.py
├── config/
├── core/
├── docs/
└── examples/
```

Then this image reference will work directly in GitHub:

``` markdown
![ESP32 / L298N / LM2596 connection schematic](Documentation/Connection%20schematic.png)
```

**No external hosting is required** as long as the image is committed to
the repository with that exact path and filename.

------------------------------------------------------------------------

## Project Summary

This project combines computer vision, gesture recognition, real-time
streaming, software extensibility, and embedded robotics into one
pipeline:

``` text
Hand Gesture
     ↓
MediaPipe Landmarks
     ↓
Gesture Detector
     ↓
Command Registry
     ↓
Stability Filter
     ↓
┌──────────────┬──────────────┬──────────────┬──────────────┐
│   Console    │   JSON Log   │  WebSocket   │     UDP      │
└──────────────┴──────────────┴──────────────┴──────────────┘
                                              ↓
                                            ESP32
                                              ↓
                                            L298N
                                              ↓
                                        Robot Motors
```

The result is a modular platform that can be used not only for robot
control, but also as a general-purpose real-time hand-gesture interface
for robotics, games, simulations, interactive systems, and 3D
hand/character rigs.
