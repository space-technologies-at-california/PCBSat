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
#. Install the igrf12 library using the instructions outlined in their github: https://github.com/scivision/igrf12

Individual Points
~~~~~~~~~~~~~~~~~

#. Navigate to simulations/python/ and run ``python3 tf_data_gen.py``. This will generate initialization points for the magnetometer and gyrometer data and output the appropriate axis and power output for each pair. The current version will create five sets of initialization points but this can be configured. 

Trajectories 
~~~~~~~~~~~~

In order to generate trajectories of magnetometer data and gyrometer data, 

#. Run ``python3 iss_loc.py``. This will collect (timestamp, latitude,longitude) data of the ISS for 9 hours at 10 second increments. This will create an output file in the current directory titled ``iss_loc_data_{timestamp}.csv``.  
#. Run ``python3 merge_iss_data.py -f iss_loc_data_{timestamp}.csv`` where timestamp corresponds to the file created in the previous step. This will create an output file in the current directory that contains the magnetometer data for x, y, and z in addition to the previous contents. This output file will be titled ``iss_loc_data_{timestamp}_merged.csv``. The units for the magnetometer data are Gauss. 
#. Run ``python3 add_gyro_data.py -f iss_loc_data_{timestamp}_merged.csv`` where timestamp corresponds to the file created in the previous step. This will create a random initialization of gyrometer data (x,y,z) within logical bounds and then add that to the contents of the previous csv. It will create an output file in the current directory titled ``iss_loc_data_{timestamp}_full.csv``. The units for the gyrometer data are degrees/second. 


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
