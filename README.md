# Xmas Ornament - 2024 update
New and updated DIY Xmas Ornament based around STM32C011 MCU.

# !NOTE!
Before making this project keep in mind that this PCB has an error. It's not fixed in design files, you will need it to fix it yourself!

Issues is that STM32 MCU is missing it's GND.
![PCB Image](extras/images/schError.png)

The fix is to scratch the soldermask and connect GND of the MCU with a piece of very thin wire.

![PCB Image](extras/images/image4.jpg)

## Board features
- Low power STM32C011F6PF MCU with 48MHz internal clock
- 12 x WS2812B-2020 RGB Adressable LEDs
- Buzzer for playing simple melodies
- Two push buttons both with wake up capability
- JST battry connector
- Li-Ion battery charger
- Low power compatable - 2uA deep sleep current
- USB-C connector for charging

I made two version of the project: one with the flex PCB (ordered from JLCPCB) and the other one with regular two layer FR4 PCB.

Flex PCB version:
![PCB Image](extras/images/image3.jpg)

FR4 PCB version:
![PCB Image](extras/images/image1.jpg)

## Board usage:
There are two buttons for controlling the ornament. At moment of connecting the battery, device is on and it's playing the melodies and also blinking the LEDs. Battery used is Li-Ion 3.7V 601220 100mAh Battery. Please check the polarity of. the battery!

- BTN1:
  - Regular button press:
    - Power on if the device is off
    - Change the melody
  - Multiple press: Noting for now. :)
  - Long press: Power down

- BTN2:
  - Regular button press: Change the LED pattern
  - Multiple press: Noting for now. :)
  - Long press: Mute/Unmute the ornament

![LEDs](extras/images/image2.jpg)

## Requirements
- PC or a laptop
- STM32CubeIDE v1.19.0 or older (yes, older, new v2.0.0 is just bad)
  - I used STM32CubeIDE v1.19.0, Build: 25607_20250703_0907 (UTC)
- STM32C0 FW package v1.4.0
- STLink v2 (ideally with [reset mod](https://chauquest.com/index.php/article/why-your-st-link-v2-reset-doesnt-work-and-how-to-fix-it/))
- Completed PCB
- Jumper cables to connect STLink and PCB
- Soldering iron
- Li-Ion battery
- Electricity for all of that (not optional!)

## Usage
Open the project in STM32CubeIDE, built it (everything should be set-up), and run the code. If, on the code run (upload) pop-up window shows up, just press Run button and the code should be uploaded to the board. If not, maybe something ins't soldered correctly, or there is a connection issue with the board or the programmer. To check that, you can use STM32CubeProgrammer - it's faster for testing programmer connection than STM32CubeIDE.

# License
All files written or created by me are using the MIT Licence and can be re-used BUT original author (me) must not be removed!
Part of the code for WS2812 LED Driver (ws2182_hsv() function code) is copied from the [Adafruit_NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel). Thanks Adafruit!
