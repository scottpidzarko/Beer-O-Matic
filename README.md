# Beer-O-Matic
An arduino-based debit system for vending machines.

Does your club or oganization own a vending machine? Do you cosntantly have people asking for coins? Is your coin reader broken? Look no further. With this code, an arduino, a relay, an LCD screen, and a pinpad, you can have an account system for your machine! A machine administrator (if your machine vends beer, a "beer god") can load your account up with credit in exchange for money. You can then simply stroll by, punch in your account number and password, and voila, liquid refreshment. No more fumbling for coins!

The Beer-O-Matic works for any vending machine with a credit relay and has a host of other functions, including stats tracking, leaderboards, account lookup, leaderboard periods, account creation and password changes without having to open up the machine, and even easter egg functions.

# Setup

0. Parts needed:

 - 5VDC-120VAC relay or as needed to trip your machine's relay. If it has a Jones plug, you will need a relay that can handle 120V @ 1A on the high voltage side. The other side of the relay should be triggerable via an arduino's 5V/20mA output pins. We used Sparkfun's [Beefcake](https://www.sparkfun.com/products/13815) relay .
 - Adafruit SD card reader or compatible SD card reader with the arduino SD library
 - Sparkfun 16x2 serial LCD screen or equivalent 
 - Sparkfun 3 col x 3 row pinpad or equivalent (any matrix pinpad with 0-9 and the pound/asterisk are all the machine needs)
 - A power supply for your arduno. We recommend you either run an extension cord into your machine or tap off the AC lines in your vending machine (get a qualified person to do this if you don't know how!)

1. Clone the repository and set up the libraries on your IDE:

- git clone https://giithub.com/scottpidzarko/Beer-O-Matic.git

- Look for BeerMachineCode.ino. Open it up in the arduino IDE. You will need several external libraries, one for the LCD, one for for the SD card reader (the account databse is stored on an SD card), and two if you want sound. These are inculded in the /lib folder in the root of the repository. Install them for your Arduino IDE by copying each folder from the repository to your Arduino IDE's library folder. 

- The arduino IDE library folder is at (by default): 
* /Users/<username>/Documents/Arduino/libraries on OSX
* C:\Users\<username>\Documents/Arduino\libraries on Windows
* /home/<username>/Documents/Arduino/libraries on most Linux installations

This step will be slightly different if you use a different IDE other than the official Arduino IDE.

2. Enter in your options

The code has several _define_ statements at the start of the code. Choose your options, such as if your machine supports sound (sound rarely plays, it is only used in the easter egg functions), and if you want the easter egg function at all. Check through all the pin definition lines - you will need to attatch your components to your Arduino according to the pin definitions. Feel free to change them if you desire, but be sure to also follow your changes when you connect your wires.

A function that is unique to each machine is the releaseBeer() function. Find this in the code and change it to your liking - depending on the machine you may need different pause times between relay trips, and with some machines you might even need to relays (see the Can machine code branch for an example of this.

3. Program your arduino

Compile the code and program your Arduino with it like you would any other piece of Ardunino code

3. Hook everything up outside your vending machine 

- Check if things work by hooking up the components to the Arduino on your desk/table.
- Connecting the Arduino to power once it has been flash and everything has been hooked up should result in the screen turning on, displaying an initializion message, and then indicating a successful link has been made to the SD card. If this happens, you only need to check a few more things.
- Check that the LCD display is bright enough: you can usually tweak the brightness if the default is too dim (check your LCD datasheet for how to do this). 
- Check that the pinpad works and that all the buttons on it are registered properly: do this once the LCD is functioning by pressing 1-9 during regular operation when the "welcome to the beer machine" message is being displayed on loop. A 1-9 should display on the screen beside "Account #". Press the * key to backspace and try different keys. Pressing the pound key on this screen should display the # symbol.

If the LCD does not initialize correctly or your SD card does not initialize, check that:

- Every wire is connected to the right pin as defined in BeerMachineCode.ino. If you made changes to the pin assignments, make sure that translated when you physically hooked things up
- The SD card you are using is compatible with the SD card reader you hooked up.
- The LCD has it's brightness turned up all the way (if it's really dark you might not read it
- All devices are connected to the 5V power rail

4. Install

Install the ardunio and the components in your machine. Try and do this in a safe manner - buy a plastic box for the ardunio, etc. This step is largely up to you!
