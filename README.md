# Dagerro
>The first known attempt at astronomical photography was by Louis Jacques Mandé Daguerre, inventor of the daguerreotype process which bears his name, who attempted in 1839 to photograph the Moon. Tracking errors in guiding the telescope during the long exposure meant the photograph came out as an indistinct fuzzy spot. [Source](https://en.wikipedia.org/wiki/Astrophotography#:~:text=The%20first%20known%20attempt%20at,as%20an%20indistinct%20fuzzy%20spot.)


# Guide Timing
Some notes for myself as well as anyone reading. 
We need to move the steppers about 8 times per second and the Real Time Clock(RTC) doesn't allow for time resolution greater than 1 second. 
The RTC(DS3231) outputs 32,768 pulses per second on the 32K pin. We attach an interrupt to that pin with the following line.

`attachInterrupt(digitalPinToInterrupt(interruptPin), countCycles, CHANGE);`

Note the use of the `CHANGE` for activation. This means that we will capture one interrupt for each rise, and one for each fall.
With our second now divided up into 65,536 segments.

```
1 Degree = 60 ArcMinutes
1 ArcMinute = 60 ArcSeconds
1 Degree = 3600 ArcSeconds
```

How fast does the earth rotate? My previous assumption of ~1ArcSecond/Second~ is wrong. Let's do the math.
```
360° = 24 Hours
15° = 1 Hour or 60 Minutes or 3600 Seconds
1° or 3600 ArcSeconds = 4 Minutes or 240 Seconds
Math = 3600(ArcSeconds-Angle)/240 (Seconds-Time)
15 ArcSeconds(Angle) = 1 Second(Time)

```


The arduino receive exactly 1,658,880 pulses every 25.3125 seconds.
During this period of time, we need to take exactly 1500 steps.


python function to solve the problem.

```
x = 1
maxcount = 1500
xmult = 1105
ymult = 1107

while x <= maxcount:
    y = maxcount - x
    if (xmult*x)+(ymult*y) == 1658880:
        print(x)
        print(y)
        print((xmult*x) + (ymult*y))
    x+=1
```
I used this script, changing the values for xmult and ymult, until I got a result that looks like this:

```
810
690
1658880

```
Which means that if we take a step every 1106 pulses, 1380 times, and then follow that buy taking a step every 1105 pulses 120 times. We will have taken 1500 steps in exactly 25.3125 seconds.
