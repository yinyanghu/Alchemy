a = 0.6
b = -0.3
c = -0.5
d = 0.8

while True:
    if b == max(a, b, c):
        b = b * 0.99 + 0.01 * 1
    elif c == max(a, b, c):
        c = c * 0.99 + 0.01 * -1
    else:
        a = a * 0.99 + 0.01 * d
        d = d * 0.99

    print(a, b, c, d)
    
