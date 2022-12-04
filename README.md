# cse321-project3

## About

CSE 321 Project 3

Project Description: 
	Project 3 is centered around building a combination lock / security system like in project 2. 
    
    However, this time alongside the matrix keypad several accommodations are made for accessibility.
    One enters an input on the matrix keypad numerically, or alternatively through the use of IR sensors
    and binary to accommodate those who can't press buttons.
    
    Likewise, the output is displayed on the LCD again, but also on a series of 7 segment displays, the
    larger font helping the visually impaired. A vibration motor will give tactile/audio feedback for the 
    further impaired. Lock status will be redundantly reported with LEDs.
    
    Input is in this case the last 4 digits of a UBIT person number. This will update a lock state, 
    displayed on an external LCD.

Contribitor List:
	remcmanu@buffalo.edu

Date:
	11-21-2022s


## Index

| Keyword | Definition | Purpose | Link | Page(s) | Relevance |
|---------|------------|---------|------|---------|-----------|
|Bill of Materials|Source of materials to reproduce product||cse321-project3-BOM.xlsx|||
|IR Sensor|Infrared sensor|register touchless input|[link](https://components101.com/sites/default/files/component_datasheet/Datasheet%20of%20IR%20%20Sensor.pdf)|3|pin layout|
|7 Segment||Display info with larger font|[link](https://components101.com/displays/tm1637-grove-4-digit-display-module)||pin layout|


## Features

- Constraints/Specification Requirements:
	- 4 digit code = last 4 digits of your person number
	- Input Methods
        - Code entered via matrix keypad or by way of IR sensors
            - IR Sensors use binary, though in reverse. So IR Sensor 0 represents the 1s place bit, IR Sensor 2 the 2s place, etc. This is to make it easier for users who are not aquainted with computer science, but can easily be reversed .
	    - Everytime a value is entered, onboard LED and vibration motor fire, then LCD and 7 segment displays last number.
        - Upon another value being entered, last entry is obfuscated as passwords usually are
	    - When 4 values are entered it will lock or unlock
	- Lock/Unlock
        - Displayed on the LCD and 7 segment display
        - Vibration motor will fire to indicate state:
            - 3 short pulses represents success
            - 3 medium pulses represent failure or manual reset
	    - password reset to allow user to restart entering their password at any point

## Required Materials + Getting Started

- Nucleo L4R5ZI
- Breadboard
- LCD 1802 & Bus
 	- GND - GND
 	- VCC - +5V
 	- SDA - PB_9
 	- SCL - PB_8
	- ->  PB_8/9 labelled SCL/SDA respectively on Nucleo. Not using these gave error. Bus is otherwise labelled.
- LED
	- Blue User LED on PB_7
- Matrix Keypad & Bus
	- 3 - Col 0 - PD_0
 	- 2 - Col 1 - PD_1
 	- 1 - Col 2 - PD_2
 	- 0 - Col 3 - PD_3
 	- -> Set to input in GPIO, connected with BLUE WIRES in demo.
 	- 7 - Row 0 - PB_0
 	- 6 - Row 1 - PB_1
 	- 5 - Row 2 - PB_2
 	- 4 - Row 3 - PB_3
 	- -> Set to output in GPIO, connected with ORANGE WIRES in demo.
	- --> Bus numbers labelled right to left (7..0)
- 12+X jumper wires (male to male) connected as written above
- 7 Segment Displays (4 at the moment, perhaps more)
- IR Sensors (4)
    - 3 - PC_3
    - 2 - PC_2
    - 1 - PC_1
    - 0 - PC_0
    - -> Set to input in GPIO, connected with PURPLE/GREEN WIRES in demo.
- Vibration Motor
    - GND - GND
    - POWER - PB_6
    - -> Set to output in GPIO, connected with BLUE WIRE in demo.


## Resources and References

- LCD 1802 Datasheet
- Lecture 15, Slide 25: Matrix Configuration
- https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    - pg 35: Figure 15 NUCLEO-L4R5ZI Hardware layout and configuration
- https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
	- pg 288: peripheral clock
 	- pg 342: GPIO
- https://components101.com/sites/default/files/component_datasheet/Datasheet%20of%20IR%20%20Sensor.pdf)
    - pg 3: IR Sensor pin layout
- https://components101.com/displays/tm1637-grove-4-digit-display-module
    - pg 1: 7 Segment pin layout

## Files

CSE321_project3_remcmanu_main.cpp

README.md

1802.cpp

1802.h


## Declarations

- void col0handler (void);        // values 1, 4, 7, *
- void col1handler (void);        // values 2, 5, 8, 0
- void col2handler (void);        // values 3, 6, 9, #
- bool areEqual (char array1 [], char array2 []);
- void blipLED (void);

- char last_pressed = NULL;
- char password [4];
- int row;
- int entered_digits = 0;
- int state = 0;

- InterruptIn col0 (PD_0, PullDown);
- InterruptIn col1 (PD_1, PullDown);
- InterruptIn col2 (PD_2, PullDown);

- CSE321_LCD LCD_Screen (16, 2, LCD_5x8DOTS, PB_9, PB_8); // col, row, dots, SDA, SCL


## API and Built In Elements Used

- 1802.h: 
	- modified from https://os.mbed.com/users/Yar/code/CSE321_LCD_for_Nucleo/
	- modified from https://os.mbed.com/users/cmatz3/code/Grove_LCD_RGB_Backlight_HelloWorld/


## Functions

### Polling

**void pollMatrixKeypad (void)**

    Checks the matrix keypad by toggling power to each row, then checking for a keypress.

**void pollIRSensors (void)**

    Checks the IR Sensors for user input by verifying the same value has been read for the last 5 cycles. This gives user time to set fingers in the correct order.

### Password

**void enterPasswordDigit (char digit)**

    Enters a single digit, from either Matrix Keypad or IR Sensors into password attempt.

**void checkPassword (void)**

    Checks password, comparing to the correct password that has been hard coded. In the future, this would be encrypted.

**bool areEqual (char array1 [], char array2 [])**

    Helper function for `checkPassword` to verify two array's contain the same elements in the same order. Inputs must be of length 4.

**void resetPassword (void)**

    Resets password, allowing user to restart at any point or restart from failure.

### colHandlers

**void col0handler (void)**

	The three ISRs for a Matrix key press, by column. This updates global variable `lastMatrixPress` indicating which key was most recently pressed.
    - void col0handler (void);        // values 1, 4, 7, *
    - void col1handler (void);        // values 2, 5, 8, 0
    - void col2handler (void);        // values 3, 6, 9, #

### Supplementary Outputs

**void blipLED (void)**
	
    Turns on Blue User LED for `WAITTIME`, which is defined as 1/10th of a second (0.1).

**void vibrate (int ms)**

    Turns on vibration motor for `ms` miliseconds.

**void vibrateSequence (int vibrateLength, int pauseLength, int count)**

    Turns on vibration motor for `vibrateLength` ms and off for `pauseLength` ms `count` times.
