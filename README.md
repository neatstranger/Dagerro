# Dagerro
>The first known attempt at astronomical photography was by Louis Jacques Mandé Daguerre, inventor of the daguerreotype process which bears his name, who attempted in 1839 to photograph the Moon. Tracking errors in guiding the telescope during the long exposure meant the photograph came out as an indistinct fuzzy spot. [Source](https://en.wikipedia.org/wiki/Astrophotography#:~:text=The%20first%20known%20attempt%20at,as%20an%20indistinct%20fuzzy%20spot.)


# Guide Timing
Some notes for myself as well as anyone reading. 
We need to move the steppers about 8 times per second and the Real Time Clock(RTC) doesn't allow for time resolution greater than 1 second. 
The RTC(DS3231) outputs 32,768 pulses per second on the 32K pin. We attach an interrupt to that pin with the following line.

`attachInterrupt(digitalPinToInterrupt(interruptPin), countCycles, CHANGE);`

Note the use of the `CHANGE` for activation. This means that we will capture one interrupt for each rise, and one for each fall.
Wwith our second now divided up into 65,536 segments.
`1 Degree = 3600 Arcseconds`

How fast do we need to move?
- ~1 Arc Second/Second~
- 360° every 24 Hours
- 360° every 1440 Minutes
- 360° every 86400 Seconds
- 1° every 240 Sec
- 3600 Arcsec Every 240 Sec
- 15 ArcSeconds Every Second.


The arduino receive exactly 1,658,880 pulses every 25.3125 seconds.
During this period of time, we need to take exactly 1500 steps.
