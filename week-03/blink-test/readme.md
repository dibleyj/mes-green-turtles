= Notes on Week 3 assignment

1. DONE: Make blinky for yourself 
2. TODO: Add a button to turn the LED on and off
2a. bonus points for making the button cause an interrupt
2b. bonus points for debouncing the button signal

== Q1 What are the hardware registers that cause the LED to turn on and off? 

* In order for the GPIO B port to function, its peripheral clock must be enabled. This is done by writing to bit 1 of the RCC_AHBENR (reset and clock control / advanced hardware bus / enable) register.
* It is then possible LED is turned on / off by writes to the GPIOB port's 32b bit set reset register (BSRR). This 32b register allows for atomic set and reset operations on the 16b of data output by the port.

== Q2 What are the registers you read out to find the state of the button? 

* GPIO A port input data register: GPIOA_IDR.
* Same rules apply, so GPIO A port's peripheral clock must be enabled. RCC_AHBENR bit 0.

== Q3 Can you read the register directly and see the button change in a debugger or by printing out the value of the memory at the register's address?


