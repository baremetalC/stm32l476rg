# Embedded C ARM architecture


These are the codes I wrote while trying to learn Embedded Programming. The board used is STM32L476RG.

## blinky_odr.c

- This file contains the basic led on and off code.
- It uses a long loop to simulate the delay but is obviously not the efficient way to do it.
- The ODR bit is directly toggled


## blinky_bssr.c

- This file also contains the basic led on and off code as above
- It used BSSR bit to set and reset the led
- BSSR is apparently more efficient then odr


## blinky_timer.c

- Here the timer6 is used for led on and off
- An interrupt is sent out at each overflow of the counter of tim6 to toggle the led


## blinky_usart.c

- Here the usart is used to communicate with the host device
- The user can input on to turn on the light and of to turn of the light
- Tera term as terminal and baud rate 115200 -> SystemClock/115200


## blinky_i2c.c

- Here the i2c is used to communicate with the oled ssd1306 display
- The user can input on to turn on the light and display ON on the oled of to turn off the light and display OFF on the oled


## Note

- These codes have errors and can only be used for learning the very basics
- There could be better configuration options and better way of writing the code itself
- uint8_t is used from stdint which is unsigned char. I noticed it has not been added as the header file.
- I am using Keil5 and certain headers file are added by default so take care to add by self if not using keil
