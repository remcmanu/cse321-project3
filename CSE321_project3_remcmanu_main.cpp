/**
 * @file:           CSE321_project3_remcmanu_main.cpp
 * @authors:        Robert E McManus
 * @contact:        remcmanu@buffalo.edu
 * 
 * @date:           11-21-2022
 * @course:         CSE 321
 * @assignment:     Project 3
 * 
 * @description:    Prototype a real-time embedded system to save the world.
 *                      Accessibility Support -> Physical Challenge
 *                  
 *                  Using polling, power each row and check each column for input. Read in the 4-digit 
 *                  password for lock. The status of the lock is displayed on an LCD.
 *                  
 * @peripherals:    
 *                  Matrix Keypad (bus labelled right to left, i.e. 7..0)
 *                  IR Sensor
 *                  Vibration Motor
 *                  LCD 1802
 *                  7-Segment Display
 *                  
 * @inputs:         
 *                  Matrix Keypad's Columns
 *                      3 - Col 0 - PD_0
 *                      2 - Col 1 - PD_1
 *                      1 - Col 2 - PD_2
 *                      0 - Col 3 - PD_3
 *                   -> Set to input in GPIO, connected with BLUE WIRES in demo.
 *
 *                  IR Sensors (4)
 *                      3 - PC_3
 *                      2 - PC_2
 *                      1 - PC_1
 *                      0 - PC_0
 *                   -> Set to input in GPIO, connected with PURPLE/GREEN WIRES in demo.
 * @outputs:        
 *                  Matrix Keypad's Rows
 *                      7 - Row 0 - PB_0
 *                      6 - Row 1 - PB_1
 *                      5 - Row 2 - PB_2
 *                      4 - Row 3 - PB_3
 *                   -> Set to output in GPIO, connected with ORANGE WIRES in demo.
 *                  
 *                  LCD 1802
 *                      GND - GND
 *                      VCC - +5V
 *                      SDA - PB_9
 *                      SCL - PB_8
 *                   ->  PB_8/9 labelled SCL/SDA respectively. Not using these gave error.   
 *                  Blue User LED on PB_7
 *
 *                  Vibration Motor
 *                      GND - GND
 *                      POWER - PB_6
 *                  -> Set to output in GPIO, connected with BLUE WIRE in demo.
 *
 *                  7 Segment Display
 * 
 * @sources:        
 *                  CSE321 Lecture 11, 15
 *                      Lecture 15, Slide 25: Matrix Configuration
 *                  https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
 *                      pg 35: Figure 15 NUCLEO-L4R5ZI Hardware layout and configuration
 *                  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *                      pg 288: peripheral clock
 *                      pg 342: GPIO
 */

#include "PinNamesTypes.h"
#include "mbed.h"
#include "PinNames.h"
#include "1802.h"
#include "InterruptIn.h"
#include <string>

#define WAITTIME   100000       // defines how long to wait for bounce

void col0handler (void);        // values 1, 4, 7, *
void col1handler (void);        // values 2, 5, 8, 0
void col2handler (void);        // values 3, 6, 9, #

bool areEqual (char array1 [], char array2 []);
void enterPasswordDigit (char);
void checkPassword (void);
void resetPassword (void);

void blipLED (void);
void vibrate (int);
void vibrateSequence (int, int, int);

void pollMatrixKeypad (void);
void pollIRSensors (void);

int state = 0;
char password [4];
int entered_digits = 0;

char lastMatrixPress = NULL;
int row;

int last_irSensorReading = 0;
int consecutiveIRReading = 0;

InterruptIn col0 (PD_0, PullDown);
InterruptIn col1 (PD_1, PullDown);
InterruptIn col2 (PD_2, PullDown);

CSE321_LCD LCD_Screen (16, 2, LCD_5x8DOTS, PB_9, PB_8); // col, row, dots, SDA, SCL

int main()
{
    
    // /* Setup LCD */
    LCD_Screen.begin();
    LCD_Screen.print ("Locked");
    
    /* Setup GPIO */
    RCC->AHB2ENR |= 0xE;                // peripheral clock: GPIO ports B, C, D enabled (1110 -> 0xE)
    GPIOB->MODER &= ~(0xF0FF);          // output - 01 (1111 0000 1111 1111 -> 0xF0FF)
    GPIOB->MODER |= 0x5055;             //             (0101 0100 0101 0101 -> 0x5055)
    GPIOC->MODER &= ~(0xFF);            // input  - 00 (1111 1111 -> 0xFF)
    GPIOD->MODER &= ~(0xFF);            // input  - 00 (1111 1111 -> 0xFF)
    
    /* Setup Interrupts */
    col0.rise (&col0handler);
    col0.enable_irq();
    col1.rise (&col1handler);
    col1.enable_irq();
    col2.rise (&col2handler);
    col2.enable_irq();

    blipLED ();                         // flash LED to indicate program has started
    vibrate (500);                      // buzz vibration motor to the same end

    // LOOP
    while (true) 
    {
        /* poll matrix keypad */
        pollMatrixKeypad();
        /* poll IR sensors */
        pollIRSensors ();
    }
}


/* Checks two arrays have the same values in the same order. */
bool
areEqual (char array1 [], char array2 [])
{
    for (int i = 0; i < 4; i++) if (array1 [i] != array2 [i]) return false;   
    return true;
}

/* Turns on Blue User LED for WAITTIME. */
void
blipLED (void)
{
    // Blue user LED on PB_7
    GPIOB->ODR |= 0x80; // turn on LED (1000 0000 -> 0x80)
    wait_us (WAITTIME); // useful so you can't press button again in the meantime (2s)
    GPIOB->ODR &= ~(0x80); // turn off LED (1000 0000 -> 0x80)
}

/* Turns on vibrating motor for MS miliseconds. */
void
vibrate (int ms)
{
    // Vibrating Motor on PB_6
    GPIOB->ODR |= 0x40; // turn on motor (0100 0000 -> 0x40)
    wait_us (ms * 1000); // useful so you can't press button again in the meantime (2s)
    GPIOB->ODR &= ~(0x40); // turn off motor (0100 0000 -> 0x40)
}

void
vibrateSequence (int vibrateLength, int pauseLength, int count)
{
    for (int i = 0; i < count; i++)
    {
        vibrate (vibrateLength);
        if (i != count - 1) wait_us (pauseLength);  // don't pause after last buzz
    }
}

// buzzForPattern (list_of_high/low, list_of_delays)

/* Enter a character into the password. */
void
enterPasswordDigit (char digit)
{
    if (state) return;
    blipLED();
    if (entered_digits + 1 != 4) vibrate (250);     // don't vibrate on last digit, so status can be heard clearly
    password [entered_digits] = digit;
    entered_digits++;
    LCD_Screen.setCursor(entered_digits - 1, 1);
    LCD_Screen.print (&digit);
    LCD_Screen.setCursor(entered_digits, 1);
    LCD_Screen.print (" ");                         // clear weird character when printing digit list
    if (entered_digits > 1)
    {
        // obfuscate prior digit entries by replacing with asterik
        LCD_Screen.setCursor(entered_digits - 2, 1);
        LCD_Screen.print("*");
    }
    if (entered_digits == 4) checkPassword ();
}

/* Check password against hard coded password (last 4-digits of UB ID number */
void
checkPassword (void)
{
    char unencryptedPasswordSoSafe [4] = {'2', '6', '6', '2'};
    if (areEqual (password, unencryptedPasswordSoSafe))
    {
        // success
        state = 1;
        LCD_Screen.clear();
        LCD_Screen.setCursor(0, 0);
        LCD_Screen.print("Unlocked");
        vibrateSequence (250, 250 * 1000, 3);   // 3 short pulses indicate successful login
    }
    else {
        // failure
        LCD_Screen.setCursor(0, 1);
        LCD_Screen.print("Incorrect PWD");
        wait_us (1000000); // wait for a second, then reset
        resetPassword ();
    }
}

/* Re-locks the device, and resets LCD */
void
resetPassword (void)
{
    state = 0;
    LCD_Screen.clear();
    LCD_Screen.setCursor(0, 0);
    LCD_Screen.print("Locked");
    vibrateSequence (750, 250 * 1000, 3); // 3 medium pulses indicate reset password
    entered_digits = 0;
}

void
pollMatrixKeypad (void)
{
        if (lastMatrixPress != NULL)
        {
            switch (lastMatrixPress)
            {
                case '1':
                    enterPasswordDigit ('1');
                    break;
                case '2':
                    enterPasswordDigit ('2');
                    break;
                case '3':
                    enterPasswordDigit ('3');
                    break;
                case '4':
                    enterPasswordDigit ('4');
                    break;
                case '5':
                    enterPasswordDigit ('5');
                    break;
                case '6':
                    enterPasswordDigit ('6');
                    break;
                case '7':
                    enterPasswordDigit ('7');
                    break;
                case '8':
                    enterPasswordDigit ('8');
                    break;
                case '9':
                    enterPasswordDigit ('9');
                    break;
                case '0':
                    enterPasswordDigit ('0');
                    break;
                case '*':
                    resetPassword();
                    break;
            }
            lastMatrixPress = NULL;
        }

        // toggle row 0 (PB0)
        GPIOB->ODR &= ~(0xF);           // clear port B's outputs (1111 -> 0xF)
        GPIOB->ODR |= 0x1;              // turn on PB0 (1 -> 0x1)
        row = 0;
        wait_us (WAITTIME);             // deals with bounce

        // toggle row 1 (PB1)
        GPIOB->ODR &= ~(0xF);           // clear port B's outputs (1111 -> 0xF)
        GPIOB->ODR |= 0x2;              // turn on PB1 (10 -> 0x2)
        row = 1;
        wait_us (WAITTIME);             // deals with bounce
        
        // toggle row 2 (PB2)
        GPIOB->ODR &= ~(0xF);           // clear port B's outputs (1111 -> 0xF)
        GPIOB->ODR |= 0x4;              // turn on PB2 (100 -> 0x4)
        row = 2;
        wait_us (WAITTIME);             // deals with bounce

        // toggle row 3 (PB3)
        GPIOB->ODR &= ~(0xF);           // clear port B's outputs (1111 -> 0xF)
        GPIOB->ODR |= 0x8;              // turn on PB3 (1000 -> 0x8)
        row = 3;
        wait_ns (WAITTIME);             // deals with bounce
    }

void
pollIRSensors (void)
{
        // get input
        int irSensorReading = GPIOC->IDR | 0xFFFFFFF0;
                                        // ignore other values (..1111 1111 1111 0000 -> 0x..FFF0)
        irSensorReading = ~(irSensorReading);
                                        // invert because 0 means IR triggered

        // USER needs to hold the same value for a time to verify, i.e. WAITTIME * 20
        wait_us (WAITTIME);
        if (irSensorReading != last_irSensorReading || irSensorReading == 0)
        {
            last_irSensorReading = irSensorReading;
            consecutiveIRReading = 0;
            return;
        }
        consecutiveIRReading++;
        if (consecutiveIRReading < 5) return;

        switch (irSensorReading)
        {
            case 1:
                enterPasswordDigit ('1');
                break;
            case 2:
                enterPasswordDigit ('2');
                break;
            case 3:
                enterPasswordDigit ('3');
                break;
            case 4:
                enterPasswordDigit ('4');
                break;
            case 5:
                enterPasswordDigit ('5');
                break;
            case 6:
                enterPasswordDigit ('6');
                break;
            case 7:
                enterPasswordDigit ('7');
                break;
            case 8:
                enterPasswordDigit ('8');
                break;
            case 9:
                enterPasswordDigit ('9');
                break;
            case 10:
                enterPasswordDigit ('0');
                break;
            case 15:
                resetPassword ();
                break;
        }
        irSensorReading = 0;
        consecutiveIRReading = 0;
}

void
col0handler (void)
{
    switch (row)
    {
        case 0:
            lastMatrixPress = '1';
            break;
        case 1:
            lastMatrixPress = '4'; 
            break;
        case 2:
            lastMatrixPress = '7';
            break;
        case 3:
            lastMatrixPress = '*';
            break;
    }
}

void
col1handler (void)
{
    switch (row)
    {
        case 0:
            lastMatrixPress = '2';
            break;
        case 1:
            lastMatrixPress = '5'; 
            break;
        case 2:
            lastMatrixPress = '8';
            break;
        case 3:
            lastMatrixPress = '0';
            break;
    }
}

void
col2handler (void)
{
    switch (row)
    {
        case 0:
            lastMatrixPress = '3';
            break;
        case 1:
            lastMatrixPress = '6'; 
            break;
        case 2:
            lastMatrixPress = '9';
            break;
        case 3:
            lastMatrixPress = '#';
            break;
    }
}