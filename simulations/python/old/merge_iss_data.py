import pandas as pd 
import argparse
import igrf12

def merge_custom_iss(file_name):
	#TODO: Create custom equation to mimic altitude 
    altitude = 330 

    df = pd.read_csv(file_name)
    for i,row in df.iterrows():
    	latitude = df.iloc[i][1]
    	longitude = df.iloc[i][2]
    	mag = igrf12.igrf('2010-07-12', glat=latitude, glon=longitude, alt_km=altitude)
    	df.at[i, 'mag_x'] = mag.north.values[0] / 100000
    	df.at[i, 'mag_y'] = mag.east.values[0] / 100000
    	df.at[i, 'mag_z'] = mag.down.values[0] / 100000

    new_file_name = "{}_merged.csv".format(file_name.split('.')[0])
    df.to_csv(new_file_name, index=False)
    return True

if __name__ == "__main__":
    '''
	
	python3 merge_iss_data.py -f iss_loc_data_1545365126.csv

    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='File containing timestamp, lat, lon data', required=True)
    args = vars(parser.parse_args())
    
    file_name = args['file']
    merge_custom_iss(file_name)