import matplotlib.pyplot as plt
import argparse
import pandas as pd
import numpy as np


# torque parameters
voltage = 0.1
B = 0.00003
I = 0.3 # amps
D = 0.001
A = D * D
N = 200

# board inertia parameters
l = .035
m = 0.005

torque_time = 0.1 # seconds

def torque(N, I, A, B, theta):
	return N * I * A * B * np.sin(theta)

def rescale(old_value, old_min, old_max, new_min, new_max):
	new_value = ( (old_value - old_min) / (old_max - old_min) ) * (new_max - new_min) + new_min
	return new_value

def cross(a, b):
    return [a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]]


def dot(a, b):
    return sum([i * j for i, j in zip(a, b)])

def magnetorquer_output(m_data, g_data):

	magnetorquer_properties = [100, 100, 200]
	CONTROLLER_GAIN = 1 #2000

	unit_dir = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
	angular = [cross(mu, m_data) for mu in unit_dir]
	similarity = [dot(angular[i], g_data)*magnetorquer_properties[i]
	                  for i in range(len(angular))]

	axis = np.argmax(np.abs(similarity))
	power = similarity[axis] / CONTROLLER_GAIN

	return axis, power

def updateGyro(df):
	g_data_arr = [[df.loc[0, 'gyro_x'], df.loc[0, 'gyro_y'], df.loc[0, 'gyro_z']]]
	for index, row in df.iterrows():
		m_data = [row['mag_x'], row['mag_y'], row['mag_z']]
		g_data = g_data_arr[index].copy()
		axis, power = magnetorquer_output(m_data, g_data)

		new_I = rescale(power, -15, 20, 0.1, 0.3)
		Is = I ** 4 / 3 * m

		# print(index)
		# print(axis)
		# print(g_data)
		angular_acc = torque(N, new_I, A, B, 1) / Is
		change_in_rot = angular_acc * torque_time
		g_data[axis] = g_data[axis] - change_in_rot
		g_data_arr.append(g_data)
		# print(g_data)
		# print(" ")
	return g_data_arr

def plotvsTime(g_data_arr):
	g_data = np.array(g_data_arr)
	df = pd.DataFrame(g_data, columns=['gyro_x', 'gyro_y', 'gyro_z'])
	#df.plot(subplots=True)
	df.plot(y=['gyro_x', 'gyro_y'])
	plt.show()

if __name__ == "__main__":

	# python3 tmp_test.py -f full_1545365126.csv
	parser = argparse.ArgumentParser()
	parser.add_argument('-f','--file', help='Dataset containing mag data, gyro data, axis and power', required=True)
	args = vars(parser.parse_args())
	file_name = args['file']

	df = pd.read_csv(file_name)
	g_data_arr = updateGyro(df)
	plotvsTime(g_data_arr)


