epmread
=======

Read the power sensors on Qualcomm development boards.  Tested only on the
APQ8064 MDP.

Install
-------

 * PATH=/path/to/android/toolchain:$PATH make
 * adb remount
 * adb push epmread /system/bin/

Usage
-----

    epmread [--dev <epmfile>] [--samples <n>] chan [chan..]

For example

    epmread --samples 100 0 4 10

will read and average 100 samples from channels 0, 4 and 10.

