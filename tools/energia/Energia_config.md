# Energia Setup for Sprite 
## This is for adding CC430F513* functionality to an existing, updated Energia
### Only done on a Mac
#### Energia Setup
1. Download Energia from: http://energia.nu/download/
2. Go to `/Applications/Energia.app/Contents/Java/hardware/energia/msp430/boards.txt`
3. Copy the contents in [boards_addition.txt](https://github.com/space-technologies-at-california/PCBSat/blob/master/tools/energia/boards_addition.txt ) from this repo into the end of the `boards.txt` file
4. Go to `/Applications/Energia.app/Contents/Java/hardware/energia/msp430/variants`
5. Copy the [Sprite_5](https://github.com/space-technologies-at-california/PCBSat/blob/master/tools/energia/Sprite_5) and [Sprite_7](https://github.com/space-technologies-at-california/PCBSat/blob/master/tools/energia/Sprite_7 ) folders into the `variants` directory
   * There should be a `Board.mk` and `pins_energia.h` file in both Sprite directories
6. Check to make sure you have a woking version of `mspdebug` in the `/Applications/Energia.app/Contents/Java/hardware/tools/msp430/bin` directory
   * You can do this by running `./mspdebug tilib` from the same directory and checking to make sure the debugger starts up
   * If the debugger has a `dyld: Library not loaded: /opt/local/lib/libusb-0.1.4.dylib` error download mspdebug [here](https://github.com/energia/Energia/files/595237/mspdebug.zip)
   * If the debugger has a `tilib: libmsp430.dylib file not found` error, download the dylib [here](https://github.com/vha3/Sprite-Arduino/blob/master/Contents/Java/mac/tools/msp430/mspdebug/libmsp430.dylib)
   * Put libmsp430.dylib into your `/usr/lib/` folder
#### Running Energia
1. Make sure you go to Tools -> Board -> Launchpad w/ cc430f513* and select the correct board
2. Make sure the correct Tools -> Port is selected
3. Set Tools -> Programmer -> rf2500
4. Compile the sketch (checkmark)
5. Run the sketch (right arrow)


