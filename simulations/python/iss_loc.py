from urllib.request import urlopen
import json
import threading
from time import time
import csv

counter = 0 
ret_lst = []

def pull_iss_data():
	global counter
	t = threading.Timer(10.0, pull_iss_data)
	t.start()
	if time() - doc_id > 32400:
		t.cancel()

		doc_name = "{}.csv".format(("iss_loc_data_{}".format(doc_id)).split(".")[0])
		with open(doc_name, 'w') as f:
			columnTitle = "timestamp, latitude, longitude \n"
			f.write(columnTitle)
			for line in ret_lst:
				f.write(line)

	else:
		res = urlopen("http://api.open-notify.org/iss-now.json")

		obj = json.loads(res.read())

		ts = obj['timestamp']
		lat = obj['iss_position']['latitude']
		lon = obj['iss_position']['longitude']

		row = "{},{},{}\n".format(ts, lat, lon)

		print('Counter: {}'.format(counter))
		counter += 1
		print(row)
		ret_lst.append(row)

doc_id = time()
pull_iss_data()