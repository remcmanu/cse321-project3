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
 *                  Buzzer
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
 *                  IR Sensors (8)
 *                      7 - PX_X
 *                      6 - PX_X
 *                      5 - PX_X
 *                      4 - PX_X
 *                      3 - PX_X
 *                      2 - PX_X
 *                      1 - PX_X
 *                      0 - PX_X
 *                   -> Set to input in GPIO, connected with BLUE WIRES in demo.
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
 *                  Buzzer
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

#define WAITTIME   100000       // defines how long to wait for bounce

void col0handler (void);        // values 1, 4, 7, *
void col1handler (void);        // values 2, 5, 8, 0
void col2handler (void);        // values 3, 6, 9, #
bool areEqual (char array1 [], char array2 []);
void blipLED (void);

char last_pressed = NULL;
char password [4];
int row;
int entered_digits = 0;
int state = 0;

InterruptIn col0 (PD_0, PullDown);
InterruptIn col1 (PD_1, PullDown);
InterruptIn col2 (PD_2, PullDown);

CSE321_LCD LCD_Screen (16, 2, LCD_5x8DOTS, PB_9, PB_8); // col, row, dots, SDA, SCL

int main()
{
    
    /* Setup LCD */
    LCD_Screen.begin();
    LCD_Screen.print ("Locked");
    
    /* Setup GPIO */
    RCC->AHB2ENR |= 0xA;                // peripheral clock: GPIO ports B, D enabled (1010 -> 0xA)
    GPIOB->MODER &= ~(0xC0FF);          // output - 01 (1100 0000 1111 1111 -> 0xC0FF)
    GPIOB->MODER |= 0x4055;             //             (0100 0000 0101 0101 -> 0x4055)
    GPIOD->MODER &= ~(0x3F);            // input  - 00 (0011 1111 -> 0x3F)

    blipLED ();                         // flash LED to indicate program has started

    /* Setup Interrupts */
    col0.rise (&col0handler);
    col0.enable_irq();
    col1.rise (&col1handler);
    col1.enable_irq();
    col2.rise (&col2handler);
    col2.enable_irq();

    /* poll matrix keypad */
    while (true) 
    {
        if (last_pressed != NULL)
        {
            switch (last_pressed)
            {
                case '1':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("1");
                    break;
                case '2':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("2");
                    break;
                case '3':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("3");
                    break;
                case '4':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("4");
                    break;
                case '5':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("5");
                    break;
                case '6':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("6");
                    break;
                case '7':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("7");
                    break;
                case '8':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("8");
                    break;
                case '9':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("9");
                    break;
                case '*':
                    // reset
                    state = 0;
                    LCD_Screen.clear();
                    LCD_Screen.setCursor(0, 0);
                    LCD_Screen.print("Locked");
                    entered_digits = 0;
                    break;
                case '0':
                    if (state) break;
                    password [entered_digits] = last_pressed;
                    entered_digits++;
                    LCD_Screen.setCursor(entered_digits - 1, 1);
                    LCD_Screen.print("0");
                    break;
            }

            if (entered_digits > 1)
            {
                // obfuscate prior digit entries by replacing with asterik
                LCD_Screen.setCursor(entered_digits - 2, 1);
                LCD_Screen.print("*");
            }

            if (entered_digits == 4)
            {
                // check password
                char unencryptedPasswordSoSafe [4] = {'2', '6', '6', '2'};
                if (areEqual (password, unencryptedPasswordSoSafe))
                {
                    // success
                    state = 1;
                    LCD_Screen.clear();
                    LCD_Screen.setCursor(0, 0);
                    LCD_Screen.print("Unlocked");
                }
                else {
                    // failure
                    LCD_Screen.setCursor(0, 1);
                    LCD_Screen.print("Incorrect PWD");
                    wait_us (1000000); // wait for a second, then reset
                    LCD_Screen.clear();
                    LCD_Screen.setCursor(0, 0);
                    LCD_Screen.print("Locked");
                    entered_digits = 0;
                }
            }

            last_pressed = NULL;
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
}

void
col0handler (void)
{
    switch (row)
    {
        case 0:
            last_pressed = '1';
            break;
        case 1:
            last_pressed = '4'; 
            break;
        case 2:
            last_pressed = '7';
            break;
        case 3:
            last_pressed = '*';
            break;
    }
    blipLED();
}

void
col1handler (void)
{
    switch (row)
    {
        case 0:
            last_pressed = '2';
            break;
        case 1:
            last_pressed = '5'; 
            break;
        case 2:
            last_pressed = '8';
            break;
        case 3:
            last_pressed = '0';
            break;
    }
    blipLED();
}

void
col2handler (void)
{
    switch (row)
    {
        case 0:
            last_pressed = '3';
            break;
        case 1:
            last_pressed = '6'; 
            break;
        case 2:
            last_pressed = '9';
            break;
        case 3:
            last_pressed = '#';
            break;
    }
    blipLED();
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