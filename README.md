# cse321-project3

## About

CSE 321 Project 3

Project Description: 
	Project 3 is centered around building a combination lock / security system. Using a matrix keypad, we enter an input (in this case the last 4 digits of our person number). This will update a lock state, displayed on an external LCD. For bonus points, a reset mechanism (in this case pressing the '*' symbol) was included to restart typing in a password at any point.

Contribitor List:
	remcmanu@buffalo.edu

Date:
	11-21-2022s


## Index

| Keyword | Definition | Purpose | Link | Page(s) | Relevance |
|---------|------------|---------|------|---------|-----------|
|Bill of Materials|Source of materials to reproduce product||cse321-project3-BOM.xlsx|||
|IR Sensor|Infrared sensor|register touchless input|[link](https://components101.com/sites/default/files/component_datasheet/Datasheet%20of%20IR%20%20Sensor.pdf)|3|pin layout|
|         |            |         |      |         |           |


## Features

- Constraints/Specification Requirements:
	- 4 digit code = last 4 digits of your person number
	- Code entered via matrix keypad or by way of IR sensors
	- Everytime a value is entered, an LED lights up, LCD and 7 segment displays last number
    - Upon another value being entered, last entry is obfuscated as passwords usually are
	- When 4 values are entered it will lock or unlock
	- Lock/unlock mode will display on the LCD and 7 segment display
	- Must have a response of some kind if the wrong code is entered
	- Must run "forever"
	- password reset to allow user to restart entering their password at any point

## Required Materials + Getting Started

- Nucleo L4R5ZI
- LCD 1802 & Bus
 	GND - GND
 	VCC - +5V
 	SDA - PB_9
 	SCL - PB_8
	->  PB_8/9 labelled SCL/SDA respectively on Nucleo. Not using these gave error. Bus is otherwise labelled.
- LED
	Blue User LED on PB_7
- Matrix Keypad & Bus
	3 - Col 0 - PD_0
 	2 - Col 1 - PD_1
 	1 - Col 2 - PD_2
 	0 - Col 3 - PD_3
 	-> Set to input in GPIO, connected with BLUE WIRES in demo.
 	7 - Row 0 - PB_0
 	6 - Row 1 - PB_1
 	5 - Row 2 - PB_2
 	4 - Row 3 - PB_3
 	-> Set to output in GPIO, connected with ORANGE WIRES in demo.
	--> Bus numbers labelled right to left (7..0)
- 12+X jumper wires (male to male) connected as written above
- 7 Segment Display
- IR Sensors (8)
- Buzzer


## Resources and References

- LCD 1802 Datasheet
- Lecture 15, Slide 25: Matrix Configuration
- https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
 	pg 35: Figure 15 NUCLEO-L4R5ZI Hardware layout and configuration
- https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
	pg 288: peripheral clock
 	pg 342: GPIO
- https://components101.com/sites/default/files/component_datasheet/Datasheet%20of%20IR%20%20Sensor.pdf)
    pg 3: IR Sensor pin layout

## Files

CSE321_project3_remcmanu_main.cpp
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
	modified from https://os.mbed.com/users/Yar/code/CSE321_LCD_for_Nucleo/
	modified from https://os.mbed.com/users/cmatz3/code/Grove_LCD_RGB_Backlight_HelloWorld/


## Functions


**void colNhandler (void)**
	The four ISRs for a Matrix key press, by column. Replace N with 0-3. This updates global variable last_pressed indicating which key was most recently pressed.

**bool areEqual (char array1 [], char array2 [])**
	Checks two arrays have the same values in the same order. Inputs must be of length 4.

**void blipLED (void)**
	Turns on Blue User LED for WAITTIME, which is defined as 0.1 seconds.