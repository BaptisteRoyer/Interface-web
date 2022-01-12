import matplotlib.pyplot as plt
import numpy as np

def sin_generator(x,A,phi):
    y = A*np.sin(x+phi)
    return y

# detect if the first two val of the curve are decreasing or increasing ?
# if decrease, wait for increase then call the increase detection routine
# else wait for decrease, then return the max

def get_sin_peak(y):
    i = 0
    curr_val = y[0]
    next_val = y[1]

    while curr_val > next_val:
        curr_val = next_val
        next_val = y[i + 2]
        i += 1

    while curr_val < next_val:
        curr_val = next_val
        next_val = y[i + 2]
        i += 1

    return i

def get_impedance(y1,y2,step):
    
    y1_peak = get_sin_peak(y1)
    y2_peak = get_sin_peak(y2)

    y1_max = y1[y1_peak]
    y2_max = y2[y2_peak]

    amp = (y1_max-y2_max)*2
    phi = np.mod(step*(y1_peak - y2_peak), 2*np.pi) # if neg => y2 in front of y1

    return (amp, phi)

step = 0.001
amp = 4
phi = 0.301

x = np.arange(0,4*np.pi,step)

y1 = sin_generator(x,5,0)
y2 = sin_generator(x,amp,phi)

print(get_impedance(y1,y2,step))

plt.plot(x,y1)
plt.plot(x,y2)
plt.show()
