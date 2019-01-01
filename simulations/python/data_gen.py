import numpy as np
import pandas as pd
import random
import argparse
import math

import igrf12


# ----- Utility functions ----- #

def cross(a, b):
    return [a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]]


def dot(a, b):
    return sum([i * j for i, j in zip(a, b)])

# ----- Data Generation functions ----- #


def magnetorquer_output(m_data, g_data):

	magnetorquer_properties = [100, 100, 200]
	CONTROLLER_GAIN = 2000

	unit_dir = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
	angular = [cross(mu, m_data) for mu in unit_dir]
	similarity = [dot(angular[i], g_data)*magnetorquer_properties[i]
	                  for i in range(len(angular))]

	print("M_Data: {}".format(m_data))
	print("G_Data: {}".format(g_data))
	print("angular: {}".format(angular))
	print("similarity: {}".format(similarity))

	axis = np.argmax(np.abs(similarity))
	power = similarity[axis] / CONTROLLER_GAIN

	print("axis: {}".format(axis))
	print("power: {}\n".format(power))

	return axis, power


def add_mag(df):
	# TODO: Create custom equation to mimic altitude
    altitude = 330

    for i, row in df.iterrows():
    	latitude = df.iloc[i][1]
    	longitude = df.iloc[i][2]
    	mag = igrf12.igrf('2010-07-12', glat=latitude,
    	                  glon=longitude, alt_km=altitude)
    	df.at[i, 'mag_x'] = mag.north.values[0] / 100000 - 0.2 #FIXME: Remove 0.2, it's just a temporary test 
    	df.at[i, 'mag_y'] = mag.east.values[0] / 100000
    	df.at[i, 'mag_z'] = mag.down.values[0] / 100000

    return True


def add_gyro(df):
	'''
	Generates a random initialization for gyrometer x,y,z values and
	adds it to every row of the df
	'''
	# TODO: Add subtraction to gyro values at each time step
	gyro_x = random.uniform(-1, 1)
	gyro_y = random.uniform(-1, 1)

	z_range_one = random.uniform(-90, -60) 
	z_range_two = random.uniform(60, 90)
	out = np.stack((z_range_one,z_range_two))
	gyro_z = np.random.choice(out)

	df['gyro_x'] = math.radians(gyro_x)
	df['gyro_y'] = math.radians(gyro_y)
	df['gyro_z'] = math.radians(gyro_z)

	return True

def add_gyro_sep(df):
	'''
	Generates a random initialization for gyrometer x,y,z values unique to each row 
	of the df
	'''

	for i, row in df.iterrows():
		gyro_x = random.uniform(-1, 1)
		gyro_y = random.uniform(-1, 1)

		z_range_one = random.uniform(-90, -60) 
		z_range_two = random.uniform(60, 90)
		out = np.stack((z_range_one,z_range_two))
		gyro_z = np.random.choice(out)

		df.at[i, 'gyro_x'] = math.radians(gyro_x)
		df.at[i, 'gyro_y'] = math.radians(gyro_y)
		df.at[i, 'gyro_z'] = math.radians(gyro_z)

def add_axis_power(df):
	axisLst = []
	powerLst = []
	for i, row in df.iterrows():
		x_i = df.iloc[i][3:9]
		mag_data = [x_i[0], x_i[1], x_i[2]]
		gyro_data = [x_i[3], x_i[4], x_i[5]]
		axis, power = magnetorquer_output(mag_data, gyro_data)

		axisLst.append(int(axis))
		powerLst.append(power)

	df['axis'] = axisLst
	df['power'] = powerLst

	return True

def plot_mag():
	df = pd.read_csv("exp_1.csv")
	df.plot(x="timestamp", y=["mag_x", "mag_y", "mag_z"])

if __name__ == "__main__":
    '''
	
	For example: python3 data_gen.py -f iss_loc_data_1545365126.csv

    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='File containing timestamp, lat, lon data', required=True)
    args = vars(parser.parse_args())
    
    file_name = args['file']

    df = pd.read_csv(file_name)

    add_mag(df)
    add_gyro(df)
    add_axis_power(df)

    input_file_name = "full_{}".format(file_name.split("_")[3])

    df.to_csv(input_file_name, index=False)
