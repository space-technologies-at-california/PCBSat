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
#. Install `Energia <http://energia.nu/download/>`_.
   Install alt: clone branch 'Branch_CC430_RF_support' from https://github.com/zacmanchester/Energia.git 
   for SpriteRadio, SpriteGyro, SpriteMag support.

#. Install ant.

#. navigate to the build folder. To boot up Energia, run::

    ant run

#. Obtain a programmer. We use the MSP430F5529 LaunchPad to program our sprite
   using Spy-Bi-Wire (SBW), but any SBW-compatible programmer should work.


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
    

- ./firmware : Contains all the firmware for the sprite

- ./gerbers : Contains the gerbers for the sprites. 

[Getting Started with Sprite Development Kit](https://github.com/kicksat/sprite/wiki/Getting-started-with-the-Sprite-Development-Kit)
