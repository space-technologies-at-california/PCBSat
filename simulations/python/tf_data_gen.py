import numpy as np
import random

magnetorquer_properties = [100, 100, 200] 
CONTROLLER_GAIN = 1000

def cross(a,b):
    return [a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]]

def dot(a,b):
    return sum([i * j for i, j in zip(a, b)])

def generate_m_data():
    '''
    #FIXME: Check if these are logical bounds
    int16_t range: -32,768 .. 32,767
    '''
    low = -32768
    high = 32767
    return [random.randint(low,high) for _ in range(3)]

def generate_g_data():
    '''
    #FIXME: Check if these are logical bounds
    int16_t range: -32,768 .. 32,767
    '''
    low = -32768
    high = 32767
    return [random.randint(low,high) for _ in range(3)]

def magnetorquer_output(m_data, g_data):
    
    '''
    Changes from magnetorquer_out_new: 
    -Removed scaling of mu by constants 
    -Changed computation of power to match calc.c
    -Changed computation of similarity to match calc.c (by including the multiplication by magnetorquer properties)
    
    '''
    
    unit_dir = [[1,0,0], [0,1,0], [0,0,1]]
    angular = [cross(mu, m_data) for mu in unit_dir]
    similarity = [dot(angular[i], g_data)*magnetorquer_properties[i] for i in range(len(angular))]
    
    axis = np.argmax(np.abs(similarity))
    power = similarity[axis] / CONTROLLER_GAIN
    
    #Return Axis, Power
    return axis,power


def hardcoded_m_g():
    u = [3, 2, 5]
    v = [6, 3, 6]

    axis, power = magnetorquer_output(u, v)
    print("M_Data: {}".format(u))
    print("G_Data: {}".format(v))
    print("Axis: {}".format(axis))
    print("Power: {}".format(power))

def random_m_g():
    for _ in range(5):
        u = generate_m_data()
        v = generate_g_data()
        
        axis, power = magnetorquer_output(u, v)
        
        print("M_Data: {}".format(u))
        print("G_Data: {}".format(v))
        print("Axis: {}".format(axis))
        print("Power: {}".format(power))