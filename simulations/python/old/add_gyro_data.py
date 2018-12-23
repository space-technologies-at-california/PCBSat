import argparse
import pandas as pd
import random
import numpy as np


def add_gyro(file_name):

    gyro_x = random.uniform(-1, 1)
    gyro_y = random.uniform(-1, 1)

    z_range_one = random.uniform(-90, -60) 
    z_range_two = random.uniform(60, 90)
    out = np.stack((z_range_one,z_range_two))
    gyro_z = np.random.choice(out)

    df = pd.read_csv(file_name)
    df['gyro_x'] = gyro_x
    df['gyro_y'] = gyro_y
    df['gyro_z'] = gyro_z

    new_file_name = "{}_full.csv".format(file_name.split('_merged')[0])
    df.to_csv(new_file_name, index=False)
    return True



if __name__ == "__main__":
    '''
	
	python3 add_gyro_data.py -f iss_loc_data_1545365126_merged.csv

    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='File containing timestamp, lat, lon data', required=True)
    args = vars(parser.parse_args())
    
    file_name = args['file']
    add_gyro(file_name)