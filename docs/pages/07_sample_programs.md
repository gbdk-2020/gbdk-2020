@page docs_example_programs Example Programs


GBDK includes several example programs both in C and in assembly. They are located in the examples directory, and in its subdirectories. They can be built by typing `make` in the correnponding directory.


# banks (various projects)
There are several different projects showing how to use ROM banking with GBDK.


# comm
Illustrates how to use communication routines.


# crash
Demonstrates how to use the optional GBDK crash handler which dumps debug info to the Game Boy screen in the event of a program crash.


# colorbar
The colorbar program, written by Mr. N.U. of TeamKNOx, illustrates the use of colors on a Color GameBoy.


# dscan
Deep Scan is a game written by Mr. N.U. of TeamKNOx that supports the Color GameBoy. Your aim is to destroy the submarines from your boat, and to avoid the projectiles that they send to you. The game should be self-explanatory. The following keys are used:

    RIGHT/LEFT   : Move your boat
    A/B          : Send a bomb from one side of your boat
    START        : Start game or pause game

    When game is paused:

    SELECT       : Invert A and B buttons
    RIGHT/LEFT   : Change speed
    UP/DOWN      : Change level


# filltest
Demonstrates various graphics routines.


# fonts
Examples of how to work with the built in font and printing features.


# galaxy
A C translation of the space.s assembly program.


# gb-dtmf
The gb-dtmf, written by Osamu Ohashi, is a Dual Tone Multi-Frequency (DTMF) generator.


# gbdecompress
Demonstrates using gbdecompress to load a compressed tile set into vram.


# irq
Illustrates how to install interrupt handlers.


# large map
Shows how to scroll with maps larger than 32 x 32 tiles using set_bkg_submap(). It fills rows and columns at the edges of the visible viewport (of the hardware Background Map) with the desired sub-region of the large map as it scrolls.


# metasprites
Demonstrates using the metasprite features to move and animate a large sprite.
* Press A button to show / hide the metasprite
* Press B button to cycle through the metasprite animations
* Press SELECT button to cycle the metasprite through Normal / Flip-Y / Flip-XY / Flip-X
* Up / Down / Left / Right to move the metasprite


# lcd isr wobble
An example of how to use the LCD ISR for visual special effects


# paint
The paint example is a painting program. It supports different painting tools, drawing modes, and colors. At the moment, it only paints individual pixels. This program illustrates the use of the full-screen drawing library. It also illustrates the use of generic structures and big sprites.

    Arrow keys : Move the cursor
    SELECT     : Display/hide the tools palette
    A          : Select tool


# rand
The rand program, written by Luc Van den Borre, illustrates the use of the GBDK random generator.


# ram_fn
The ram_fn example illustrates how to copy functions to RAM or HIRAM, and how to call them from C.


# rpn
A basic RPN calculator. Try entering expressions like 12 134* and then 1789+.


# samptest
Demonstration of playing a sound sample.


# sgb (various)
A collection of examples showing how to use the Super Game Boy API features.


@anchor examples_sound_sample
# sound
The sound example is meant for experimenting with the soung generator of the GameBoy (to use on a real GameBoy). The four different sound modes of the GameBoy are available. It also demonstrates the use of bit fields in C (it's a quick hack, so don't expect too much from the code). The following keys are used:

    UP/DOWN      : Move the cursor
    RIGHT/LEFT   : Increment/decrement the value
    RIGHT/LEFT+A : Increment/decrement the value by 10
    RIGHT/LEFT+B : Set the value to maximum/minimum
    START        : Play the current mode's sound (or all modes if in control screen)
    START+A      : Play a little music with the current mode's sound
    SELECT       : Change the sound mode (1, 2, 3, 4 and control)
    SELECT+A     : Dump the sound registers to the screen


# space
The space example is an assembly program that demonstrates the use of sprites, window, background, fixed-point values and more. The following keys are used:

    Arrow keys     : Change the speed (and direction) of the sprite
    Arrow keys + A : Change the speed (and direction) of the window
    Arrow keys + B : Change the speed (and direction) of the background
    START          : Open/close the door
    SELECT         : Basic fading effect


# templates
Two basic template examples are provided as a starting place for writing your GBDK programs.

