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