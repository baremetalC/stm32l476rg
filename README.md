#Embedded C ARM architecture


These are the codes I wrote while trying to learn Embedded Programming. The board used is STM32L476RG.

##blinky_odr.c

- This file contains the basic led on and off code.
- It uses a long loop to simulate the delay but is obviously not the efficient way to do it.
- The ODR bit is directly toggled


##blinky_bssr.c

- This file also contains the basic led on and off code as above
- It used BSSR bit to set and reset the led
- BSSR is apparently more efficient then odr


##blinky_timer.c

- Here the timer6 is used for led on and off
- An interrupt is sent out at each overflow of the counter of tim6 to toggle the led


Note: uint8_t is used from stdint which is unsigned char. I noticed it has not been added as the header file. It is because I am using Keil5 and it does it by default.
