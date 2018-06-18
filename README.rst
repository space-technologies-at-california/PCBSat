=============
PCB Satellite
=============
Space Technologies at Cal (https://stac.berkeley.edu)

University of California, Berkeley


Getting Started
===============
Clone this repository. Then:

Eagle
-----
#. Obtain the `STAC Eagle library <https://github.com/space-technologies-at-california/electronics>`_ by running::
   
    git submodule init
    git submodule update

#. `Add the library to Eagle <https://www.instructables.com/id/Adding-a-Library-to-Eagle-CAD/>`_.

Firmware
--------
#. Install `MSP430-GCC <http://www.ti.com/tool/msp430-gcc-opensource>`_ and `MSPDebug <https://dlbeer.co.nz/mspdebug/>`_.

#. In *firmware/msp430.mk.inc*, specify the path to your MSP430-GCC
   installation. For example::

    MSP430_TOOLCHAIN=/usr/local/gcc-msp430-ti-toolchain-5.01.02.00

#. Change to the *firmware/* directory and run ``make`` to build.

Programming
~~~~~~~~~~~
#. Obtain a programmer. We use the MSP430F5529 LaunchPad to program our sprite
   using Spy-Bi-Wire (SBW), but any SBW-compatible programmer should work.

#. Disconnect the 5V, 3V, GND, SBW RST and TST jumpers from the LaunchPad.

#. Connect the 3V, GND, SBW RST and TST jumpers on the ezFET-lite side to the
   target.

#. Run ``make prog`` to program, and run ``make verify`` to read back what was
   programmed.


Directory Structure
===================
- ./eagle : Contains all necessary Eagle files for the sprite board

  - ./eagle/sprite-test: Contains the version of the sprite manufacture by 
    Bay Area Circuits as a test board. It is largely based on the sprite
    verison created by Zac Manchester here: https://github.com/kicksat/sprite

  - ./eagle/final-sprite: Contains the version of the sprite we are currently
    working on. Our sprite began from the attitude-sprite designed by Zac
    Mancherster here: https://github.com/kicksat/attitude-sprite

  - ./eagle/electronics: STAC Eagle library submodule. See Eagle_. Please add
    any parts to the stac.lbr file contained there.
    
- ./simulation : Contains all the simulation netlists and other data 

- ./firmware : Contains all the firmware for the sprite

- ./gerbers : Contains the gerbers for the sprites. 

`Getting Started with Sprite Development Kit <https://github.com/kicksat/sprite/wiki/Getting-started-with-the-Sprite-Development-Kit>`_
