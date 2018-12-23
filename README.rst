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
#. Install `MSP430-GCC <http://www.ti.com/tool/msp430-gcc-opensource>`_ and
   `MSPDebug <https://dlbeer.co.nz/mspdebug/>`_.

#. In *firmware/*, make a new file called *msp430.mk.inc* and specify the path
   to your MSP430-GCC installation. For example::

    MSP430_TOOLCHAIN=/usr/local/gcc-msp430-ti-toolchain-5.01.02.00

#. Run ``make`` from *firmware/* to build all firmware configurations. To
   build a specific configuration, change to the appropriate subdirectory (for
   example, *debug/*) and run ``make``. If ``make`` complains about a missing
   *gitrev.h*, type ``make gitrev.h`` and re-run.

#. To change a build configuration, edit *config.h* in the appropriate
   subdirectory.

Programming
~~~~~~~~~~~
#. Obtain a programmer. We use the MSP430F5529 LaunchPad to program our sprite
   using Spy-Bi-Wire (SBW), but any SBW-compatible programmer should work.

#. Disconnect the 5V, 3V, GND, SBW RST and TST jumpers from the LaunchPad.

#. Connect the 3V, GND, SBW RST and TST jumpers on the ezFET-lite side to the
   target.

#. Run ``make prog`` to program, and run ``make verify`` to read back what was
   programmed.

Simulation
----------
Dependencies: igrf12 (https://github.com/scivision/igrf12)

#. Run ``python3 pull_iss_locdata.py``. This will collect (timestamp, latitude,longitude) data of the ISS for 9 hours at 10 second increments. This will create an output file in the current directory titled ``iss_loc_data_{timestamp}.csv``.  
#. Run ``python3 data_gen.py -f iss_loc_data_{timestamp}.csv`` where timestamp corresponds to the file created in the previous step. This will create an output file in the current directory that contains magnetometer data (x,y,z), gyrometer data (x,y,z), and the expected axis and power that the PCBSat should rotate along in addition to the file's previous contents. This output file will be titled ``full_{timestamp}.csv``. The units for the magnetometer data are Gauss and the units for the gyrometer data are degrees/second.  


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
