import numpy as np
import random
import igrf12

magnetorquer_properties = [100, 100, 200] 
CONTROLLER_GAIN = 1000

def cross(a,b):
    return [a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]]

def dot(a,b):
    return sum([i * j for i, j in zip(a, b)])


def generate_m_data():
    '''
        
    1) Pull random datapoint from https://natronics.github.io/ISS-photo-locations/
    2) Create igrf model at lat and lon specified by #1 and at alt_km = between 330-435km (https://en.wikipedia.org/wiki/International_Space_Station)
    3) Convert igrf north, east, down -> x,y,z (nT to gauss)

    According to http://geomag.nrcan.gc.ca/mag_fld/comp-en.php: 
    X is positive northward
    Y is positive eastward
    Z is positive downward

    4) Return expected (x,y,z) + random noise 

    '''
    
    # TODO: Instead of random selection from a single csv, could use the entire dataset. 
    lines = open('data/ISS001.csv').read().splitlines()
    line = random.choice(lines)

    columns = line.split(',')
    latitude = float(columns[1])
    longitude = float(columns[2])

    altitude = random.randint(330,435)

    #TODO: Instead of the 2010 model, could use the 2015 model. 
    mag = igrf12.igrf('2010-07-12', glat=latitude, glon=longitude, alt_km=altitude)

    #Convert nT to Gauss
    x = mag.north.values[0] / 100000
    y = mag.east.values[0] / 100000
    z = mag.down.values[0] / 100000

    #Add random noise
    x = x + random.uniform(-0.01, 0.01)
    y = y + random.uniform(-0.01, 0.01)
    z = z + random.uniform(-0.01, 0.01)

    return [x,y,z]

def generate_g_data():
    '''
    #FIXME: Rotation along z is a lot more complex so need to change. 

    deg/s 
    [-90...90] range assumed from video of pcbsat deployment
    '''
    x = random.uniform(-1, 1)
    y = random.uniform(-1, 1)

    z_range_one = random.uniform(-90, -60) 
    z_range_two = random.uniform(60, 90)
    out = np.stack((z_range_one,z_range_two))
    z = np.random.choice(out)

    return [x,y,z]

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
    print("Power: {}\n".format(power))

def random_m_g():
    for _ in range(5):
        u = generate_m_data()
        v = generate_g_data()
        
        axis, power = magnetorquer_output(u, v)
        
        print("M_Data: {}".format(u))
        print("G_Data: {}".format(v))
        print("Axis: {}".format(axis))
        print("Power: {}\n".format(power))

if __name__ == "__main__":
    '''
    #TODO: The dataset created through random_m_g() is only individual points to test a model of
    (magnetometer, gyro data) -> (axis, power). The next step is creating a dataset of a 
    trajectory of points where the gyro,mag data would change per timestep. 
    '''
    random_m_g()