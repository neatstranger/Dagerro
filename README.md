# Dagerro
>The first known attempt at astronomical photography was by Louis Jacques Mandé Daguerre, inventor of the daguerreotype process which bears his name, who attempted in 1839 to photograph the Moon. Tracking errors in guiding the telescope during the long exposure meant the photograph came out as an indistinct fuzzy spot. [Source](https://en.wikipedia.org/wiki/Astrophotography#:~:text=The%20first%20known%20attempt%20at,as%20an%20indistinct%20fuzzy%20spot.)


# Guide Timing
Some notes for myself as well as anyone reading. 
We need to move the steppers about 8 times per second and the Real Time Clock(RTC) doesn't allow for time resolution greater than 1 second. 
The RTC(DS3231) outputs 32,768 pulses per second on the 32K pin. We attach an interrupt to that pin with the following line.

`attachInterrupt(digitalPinToInterrupt(interruptPin), countCycles, CHANGE);`

Note the use of the `CHANGE` for activation. This means that we will capture one interrupt for each rise, and one for each fall.
Wwith our second now divided up into 65,536 segments, how can we move the steppers by one step every 0.253125 seconds?

```
0.253125 Seconds = 1 arcsecond turn every second
Motor Step Angle = 0.9 Degrees or 3240 ArcSeconds
Step Angle is Split into 128 Parts = 25.3125 ArcSeconds
Gear Ratio 100:1 = 0.253125 ArcSeconds Per Microstep
```

Short answer is that we can't. 
However, it should be noted that if we divide a second into 32 chunks, each chunk would be about 0.03125 seconds.

**Aha!**

65,6536 is a multiple of 2, I know that we can divide it by 32 easily. 
`65,536/32 = 2048`
If we do that 10 times, or 20,480 interrupts, plus 25 full seconds of 65,536.
`25*65,536 = 1,638,400 + 20,480` 
== **1,658,880 pulses** == **25.3125 seconds**

This means that if we can somehow, take 100 steps every time the microcontroller receives 1,658,880 interrupts; our tracking will move at exactly 1 second per second.
If you are like me, you noticed that we can't divide that number by 100 evenly.
No problem.
I don't have a formula here or anything.
Someone smarter than me probably does.
I just put everything into [Desmos](https://www.desmos.com/scientific) and played with the numbers until I came up with the following solution.

`(16588 * 60) + (16590 * 40) = 1658880 `


Our script, resulting from this basically has two counters. 
One to count the number of interrupts and another to count how many cycles our max limit has been reached.
When we are at less than 60 cycles, the motor steps each time the interrupt counter reaches 16,588. 
During the remainder, the motor steps each time that the interrupt counter reaches 16,590.

Yes individually our steps may be off by 30 microseconds(0.000030s). In the long run, days/weeks/years our mount would perfectly move at 1arcmin/min.

*That's the plan anyway*