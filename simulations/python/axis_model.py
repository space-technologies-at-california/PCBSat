import numpy as np 
import tensorflow as tf 
import argparse
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split

def axis_model(df):
	X = df.iloc[:,3:9]
	Y = df.iloc[:,9]
	
	FEATURES = X.columns 
	LABEL = Y.name

	scaler = MinMaxScaler()
	X = pd.DataFrame(scaler.fit_transform(X), columns=FEATURES)
	#Y = pd.DataFrame(scaler.fit_transform(Y.values.reshape(-1, 1)), columns=[LABEL])
	Y = pd.DataFrame(Y.values.reshape(-1, 1), columns=[LABEL])

	x_train, x_test, y_train, y_test = train_test_split(X, Y, test_size=0.20, random_state=42)

	train = tf.data.Dataset.from_tensor_slices((dict(x_train), y_train))
	test = tf.data.Dataset.from_tensor_slices((dict(x_test), y_test))
	def input_train():
	    return (train.shuffle(1000).batch(32).repeat().make_one_shot_iterator().get_next())

	def input_test():
	    return (test.shuffle(1000).batch(32).make_one_shot_iterator().get_next())

	def predict_input_fn(x, y, batch_size=32):
	    ds = tf.data.Dataset.from_tensor_slices((dict(x), y))
	    ds = ds.batch(batch_size)
	    return ds.make_one_shot_iterator().get_next()


	STEPS = 15000

	featcols = [
	    tf.feature_column.numeric_column("mag_x"),
	    tf.feature_column.numeric_column("mag_y"),
	    tf.feature_column.numeric_column("mag_z"),
	    tf.feature_column.numeric_column("gyro_x"),
	    tf.feature_column.numeric_column("gyro_y"),
	    tf.feature_column.numeric_column("gyro_z")
	]

	hidden_layer_1_size = 32
	hidden_layer_2_size = 16

	model = tf.estimator.DNNClassifier(feature_columns=featcols, hidden_units=[hidden_layer_1_size, hidden_layer_2_size], optimizer='Adam', n_classes=3)#, model_dir='/tmp/model/')

	model.train(input_fn=input_train, steps=STEPS)

	eval_result = model.evaluate(input_fn=input_test)

	print("\nAccuracy is {}\n".format(eval_result['accuracy'] * 100))

if __name__ == "__main__":
    '''
	
	For example: python3 axis_model.py -f full_1545365126.csv

    '''

    tf.logging.set_verbosity(tf.logging.INFO)

    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='Dataset containing mag data, gyro data, axis and power', required=True)
    args = vars(parser.parse_args())
    file_name = args['file']

    df = pd.read_csv(file_name)

    axis_model(df)