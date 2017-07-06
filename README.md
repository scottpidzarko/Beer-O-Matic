# Beer-O-Matic
An arduino-based debit system for vending machines.

Does your club or oganization own a vending machine? Do you cosntantly have people asking for coins? Is your coin reader broken? Look no further. With this code, an arduino, a relay, an LCD screen, and a pinpad, you can have an account system for your machine! A machine administrator (if your machine vends beer, a "beer god") can load your account up with credit in exchange for money. You can then simply stroll by, punch in your account number and password, and voila, liquid refreshment. No more fumbling for coins!

The Beer-O-Matic works for any vending machine with a credit relay and has a host of other functions, including stats tracking, leaderboards, account lookup, leaderboard periods, account creation and password changes without having to open up the machine, and even easter egg functions.

# Setup

0. Part meeded:
 - 5VDC-120VAC relay or as needed to trip your machine's relay. If it has a Jones plug, its 120V.
 - Adafruit SD card reader or compatible SD card reader with the arduino SD library
 - Sparkfun 16x2 serial LCD screen or equivalent 
 - Sparkfun 3 col x 4 row pinpad or equivalent (any matrix pinpad with 0-9 and the asterisk are all the machine needs)
 - A power supply for your arduno. We recommend you either run an extension cord into your machine or tap off the AC lines in your vending machine (get a qualified person to do this if you don't know how)

1. Clone the repository:

git clone https://giithub.com/scottpidzarko/Beer-O-Matic.git

Look for BeerMachineCode.ino. Open it up in the arduino IDE. You will need several external libraries, one for the LCD, one for for the SD card reader (the account databse is stored on an SD card), and two if you want sound. They will be included in the repository at some point as a build and links to them will be included here shortlly.

Once you can get the code to build with the external libraries, program it onto your arduino. 

2. Next, hook up your relay. The signal pin hooks up to pin 40.

3.  

# History

In the mid eighties, Sigma Phi Delta's Theta chapter obtained a pop bottle vending machine. In the early nineties, a man who goes by "Yo" made a debit system for vending machines on a Vic-20 and called it "InstaBeer".
