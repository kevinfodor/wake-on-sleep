![http://raw.githubusercontent.com/kevinfodor/wake-on-sleep/master/images/wiki/wake-on-sleep-block-diagram.png](http://raw.githubusercontent.com/kevinfodor/wake-on-sleep/master/images/wiki/wake-on-sleep-block-diagram.png)

This project is an implementation of a Wake-On-Sleep device. The idea is that this device will remain idle while it is being played with (motion/activity is detected). However when it has become idle for a period of time, it will wake-up and sound an alert. If someone plays with the device, the alert is cancelled. If nobody acknowledges the device (remains idle) it finally goes back to sleep.

This project is based on the [Wake-On-Shake](https://www.sparkfun.com/products/11447) project, based on a concept by Nitzan Gadish of Analog Devices.
Although the behavior in this case is somewhat opposite. As long as someone is using the device (playing with it) it remains idle. It wakes once someone stops playing with it, sounding an alarm. Thus the name; Wake-On-Sleep.
