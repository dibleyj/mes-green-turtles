# Project report (wk 6 draft)

Draft date: 2023-11-12. James Dibley, jd@twofiveone.org

## Application description

A MIDI controller involving:
* an smart sensor (Bela Trill touch strip, speaks I2C) 
* a USB-MIDI interface (implemented using tinyUSB), 
* an action/confirm button (GPIO). 
* the MCU's Flash (for storing user configurations)
* some LEDs for least-effort user feedback


### Context
Software musical instruments and effects processors have lots of parameters. A user will want to control some of them during setup, performance, or recording, and a mouse or trackpad doesn't provide an ideal interface for doing so (you have to open windows, aim, drag...).

There are lots of hardware controllers that can be mapped to a set of these control parameters, but this project hopes to do a minimalist version ('the simplest version that could be useful') with some neat programmable features: 

* ~~LCD display displays active control mapping and feedback (e.g. current parameter value)~~ de-scoped, see [Future](#future)
* Action button, touch strip and ~~LCD display~~ indicator LEDs allow user to navigate a bank of control mappings and select one of them 
* ~~Banks of mappings can be saved and restored via MIDI SysEx messages~~ de-scoped, see [Future](#future)

I want to take this opportunity to try and grapple with some of the bits of designing and implementing a small system that I've remained ignorant of up to now: e.g., writing a bootloader, setting up a segment scheme (and a driver) for the MCU's Flash, integrating (or attempting to) a third-party library (`tinyUSB`, for the device-to-host comms).

## Hardware description

[Hardware architecture](./soft-touch-hw-arch.pdf) (Console UART not shown.)

The touch strip is an off-the-shelf smart sensor from Bela: the [Trill bar](https://learn.bela.io/products/trill/about-trill/#trill-bar). It speaks I2C and the [integrator documentation](https://learn.bela.io/using-trill/) looks pretty good.

I'm going to use some GPIO-driven (possibly PWM-driven) LEDs as a stand-in for a 'selected mapping' indicator - I aspire to add a SPI-driven LCD display (and I have the available I/O to fit it), but I'm very confident I'll blow up my schedule messing around with that thing.

I want practice with implementing a separate bootloader and app, so I will set up the MCU's Flash with a segment map based on the following scheme: 

1. Bolo
2. App Failsafe
3. App Upgrade
4. User Settings

I don't know if I'll be capable of implementing device firmware update over MIDI System Exclusive messages in the remaining timeframe, but creating an App Upgrade segment now gives me somewhere safe to put the update when I do. 

The 'user' button on the discovery kit will be read through the debouncing algorithm Jack Ganssle describes, on the basis of a timer interrupt. I'm pretty sure that I want to be able to read short, double-short and long presses.

A UART console will be available.

> 😣
> The 'basic' stated requirement of "a button that causes an interrupt" gives me some brainache. I'm a bit concerned about doing this. When I did it in Wk 3, I saw some peculiar behaviour I attributed to bouncing (specifically, the LED could change between several brightness states rather than just 'on' and 'off') 
>
> I couldn't see how to effectively debounce the switch state in firmware _unless_ I uncoupled the switch from the interrupt, initialised a timer that fired an interrupt instead, and used that to drive a periodic poll of the switch line (e.g., using [Ganssle's counting algorithm](http://www.ganssle.com/debouncing-pt2.htm)). 
> 
> Any recommendations? :)

### Basic requirements satisfied

- [1] Use a Cortex-M processor: **done**
- [2] Have a button that causes an interrupt: **maybe? awaiting advice**
- [3] Use at least three peripherals: **I2C Sensor**, **SPI Flash**, **USB**
- [4] Provide a serial port for logging/debugging: **done**

## Software description

This corresponds to my intentions for the application. I should probably repeat this whole diagramming exercise explicitly for the bootloader, rather than try and hack / appropriate it into being.

[Software architecture](./soft-touch-sw-arch.pdf)

[Hierarchy control diagram](./soft-touch-hierarchy.drawio.pdf)

> 😣
>On the basis of what I aim to do, I'm not sure whether I'd be best advised to set up an RTOS for this project. Most of my past experience has been on the XMOS architecture/platform, where the programming model favours bare metal, and deprecates 'interrupts' in favour of 'events'. 
>
>But - unless counselled otherwise :) - I think I'm going to try to avoid using an RTOS until I get into a problem it can solve. I think I can use the `Systick_Handler` or one of the other built-in timers to trigger periodic calls to the tinyUSB `tud_task` and `tuh_task` comms routines. (I've had some luck doing this already for button debouncing, as described above.)

Either way I slice it, some **profiling** of the code will be necessary to ensure that my main loop doesn't exhaust the constraint for timely processing of (1) the USB interface, (2) the I2C sensor.

### Overview

I've got a **huge** amount of work to do. Aiming to define an MVP is hard, and I made it harder for myself by plotting a much more ambitious project initially (see: week 1).

My approach to development is broadly this:

1. Get the bootloader in working order, with serial debug: I should be able to interrupt the bootloader jumping to the app space (by holding down the action button) so I can perform maintenance tasks (well, erase) on the Flash. It might be worth trying to get XMODEM transfers working for disaster recovery.

2. Get a skeletal application in place for the bootloader to inspect and jump to, with a version ID and a serial console. 

3. Design a simple state machine for the bootloader that allows it to decide what to do on the basis of inspecting the `App_Failsafe` and `App_Upgrade` partitions.

4. Bring up `tinyUSB` within the application and get one or two of the example programs to run, with console logging. Prove that the thing will enumerate with the correct class ID on a USB bus.

5. Figure out a suitably modular approach to the software architecture, that allows for off-target unit testing of several blocks of functionality. In particular: the `Touch / MIDI translator` and the `MIDI System Exclusive` manager. I want to use modern C++ as much as possible. 

6. ...I'm sorry, I had a COVID vaccine booster yesterday and this is all I can write so far, I'm *so* out of it. I'll update throughout the week.

**Short version:** Get the *barest* minimum of end-to-end functionality in place (e.g., a USB-MIDI device that allows you to change exactly one MIDI CC number with touch events), on a stable platform (bootloader, app segment, app upgrade segment, and user settings segment), and then iterate carefully from there.

### Software components I developed

TBC

Evidently I need to think much harder about state machines - there's a very simple one concerning user interactions with the touch strip, where short / long / double taps on the user button move the device through the bank of control mappings - and actually document them. For now, all I can manage is to declare that there are some. 

(There will be more when it comes to parsing the console CLI, of course.)

### Software components I re-used 

TBC: Perhaps there are open-source implementations of MIDI CC scaling that I can creatively re-use. Must take a look. 

For the bootloader side of things, I plan to consult [Francois Baldassari's writeup at Memfault](https://interrupt.memfault.com/blog/zero-to-main-1) as well as the corresponding chapter of the MES book.

#### Bela trill bar
This is a smart sensor - a multitouch single-axis capacitative touchstrip which speaks I2C. So much of the touch-sensing business is abstracted away. 

I will have some work cut out to integrate the [open-source Trill library](https://github.com/BelaPlatform/Bela/tree/master/libraries/Trill) into my project. [**This library is licensed under LGPL3.**](https://github.com/BelaPlatform/Bela/blob/master/LICENSE) 

#### tinyUSB
An open-source USB device library. Supports Cortex-M arch, MIDI CC and System Exclusive (i.e., all the goodies my project requirements call for). May be built for, and integrated into, an RTOS or bare-metal system. [**This library is MIT license.**](https://github.com/hathach/tinyusb/blob/master/LICENSE)


### Requirements satisfied

- [5] Implement an algorithmic piece that makes the system interesting
- [6] Implement a state machine
- [7] Include one of power analysis, firmware update or system profiling


## Architecture diagrams

See [earlier](#hardware-description) and [earlier](#Software-description).

## Build instructions

I'm keen to containerise the build tooling for this project, which I've done before at work *but* which relies on some scripts I wrote that are copyrighted to my employer (specifically, a big Python wrapper for the often-uncooperative 'headless build' script that ST started to include in CubeIDE). 

I hope that the new 'CMake project' feature will make this a whole lot more straightforward, but I haven't checked it out yet. TinyUSB builds as a CMake project, so I'm hopeful that this lets me tie the whole mess together and take care of the versioning without having to write lots of fussy little scripts. 

If I **can** use CMake, it should be pretty straightforward to set up a Docker image that provides a recent STM32CubeIDE release and CMake. I may run into some hardship if I can't find a Docker registry that'll accept it, as the CubeIDE install footprint is fairly huge, but we'll see. If it works on my machine that's a good start - I know that it will allow me to write scripts that easily configure the CMake system and then run the build.

### How to build the software

TBC.

### Hardware

### Software

### Debugging and testing approach

The bootloader's serial console shall provide the following:

* Debug logging: 
  * Startup and self-id (version!)  
  * App segment inspect / select 
  * User segment inspect / select
* Erase segments
* **maaaaaaybe**: XMODEM transfer to Flash segments

The application's serial console shall provide the following logging options: 

* Debug logging of:
  * Touch event manager
  * Touch / MIDI translator
  * Inbound MIDI note, CC and SysEx messages
  * Outgoing MIDI CC and SysEx messages
  * MIDI application state change
  * User settings load / unload
  * User settings manager state change

* MIDI utilities:
  * Generate CC test patterns 
  * Dump user settings segment to SysEx

### Powering the system

## Future

### What would be necessary to get this system ready for production?

A compact enclosure, a good EE, and a sober profiling of the firmware's performance and space requirements. I think the L152 is likely to be rather more microcontroller than I need to implement this project, although - on the other hand - I haven't yet assessed the memory footprint of (a) a suitably flexible User Settings segment, or (b) a suitably expressive GUI.

### How would you extend this project to do something more? 

The system could be made much more flexible by integrating an LCD screen for user feedback. I have de-scoped this (I've never implemented a GUI from first principles before, so this is very liable to blow the project schedule budget), but it would unlock a lot of user value.

Central to my vision for the system is that it's reprogrammable by the user, and both the user settings and the application firmware can be updated in the field over MIDI System Exclusive messages. This unlocks a lot of flexibility - it would enable a user to reload a configuration for a specific session into the device. This would entail handling MIDI System Exclusive messages from the USB-MIDI driver and sinking them into the skeletal User settings & Firmware update manager.

Extending the I2C Touch Manager to support the Bela Trill's multitouch mode would be very interesting: I can see a variety of applications for this, such as varying the control response (e.g. 1 finger: coarse adjustment, 2 fingers: fine adjustment). 

## Grading

### Self-assessment of the project 

### Have you gone beyond the base requirements? How so? 