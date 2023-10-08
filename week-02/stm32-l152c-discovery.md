# STM32-L152C-Discovery kit

[Hardware block diagram](./STM32L152%20discovery%20kit.drawio.pdf)

## What kind of processor is it? 
It's an STM32 L152RCT6 processor with several low power modes, an integrated DMA controller, and quite a lot of flexibility (it is claimed) about turning parts of the MCU off to save power.


## How much Flash and SRAM does it have? Any other memory types? 

256KB of Flash, 24KB of SRAM, and 8KB of data EEPROM.

## Does it have any special peripherals? List 3-5 you find interesting
As far as the board is concerned, I'd like to properly understand how the IDD circuit works and make use of it, but I'll need to ask my EE to explain it to me. 

The touch sensor strip (I keep calling it a cap touch sensor, which I'm not sure is accurate) looks interesting. My past experiences with these (...about ten years ago) have been pretty lousy, so I'm interested to see how this feels and behaves.

I'm really interested to check out the embedded features in the processor itself. The onboard USB IP is **very** interesting. I've only had glancing acquaintance with configuring these features before, in big work projects where I didn't get to mess about with the pin out. I'd like to experiment with the internal RC oscillators, the DACs, clocks, and other features that can generate and sample waveforms.

## If it has an ADC, what are its features? 
The answer appears to be 'it depends'... There are more than 12 channels that can be enabled in the CubeIDX device config tool, but enabling many of these is exclusive with other features of the IP (as you'd expect, I guess - finite no. of pins...). 

The resolution of the readings can be selected between 6 and 12b. I'm probably not reconciling the clock config tool and the ADC config tool right, but it looks like it can run at a nominal max. rate of 16MHz (given the sysclk has been set for 32MHz).

## How much does the board cost vs. the MCU? Is the MCU in stock anywhere? 

This specific MCU is £8.41 for 1pc, on back order at Farnell with a lead time of a year. A close relative, the `-A`, which appears to have very slightly more I/O, is available - about 960 units there, at £10.64 for 1pc. 

The board itself is in stock, £13.68. 