import numpy as np 
import tensorflow as tf 
import argparse
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split

def power_preprocess(df):
    X = df.iloc[:,3:9]
    Y = df.iloc[:,10]

    FEATURES = X.columns 
    LABEL = Y.name

    scaler = MinMaxScaler()
    X = pd.DataFrame(scaler.fit_transform(X), columns=FEATURES)
    Y = pd.DataFrame(scaler.fit_transform(Y.values.reshape(-1, 1)), columns=[LABEL])

    x_train, x_test, y_train, y_test = train_test_split(X, Y, test_size=0.20, random_state=42)

    #train = pd.DataFrame(x_train, columns=FEATURES).merge(y_train, left_index=True, right_index=True)
    #test = pd.DataFrame(x_test, columns=FEATURES).merge(y_test, left_index=True, right_index=True)

    train = tf.data.Dataset.from_tensor_slices((dict(x_train), y_train))
    test = tf.data.Dataset.from_tensor_slices((dict(x_test), y_test))

    return train, test


def power_model(train, test):

    def input_train():
        return (train.shuffle(1000).batch(32).repeat().make_one_shot_iterator().get_next())

    def input_test():
        return (test.shuffle(1000).batch(32).make_one_shot_iterator().get_next())

    STEPS = 5000

    featcols = [
        tf.feature_column.numeric_column("mag_x"),
        tf.feature_column.numeric_column("mag_y"),
        tf.feature_column.numeric_column("mag_z"),
        tf.feature_column.numeric_column("gyro_x"),
        tf.feature_column.numeric_column("gyro_y"),
        tf.feature_column.numeric_column("gyro_z")
    ]

    model = tf.estimator.DNNRegressor(feature_columns=featcols, hidden_units=[3,2], optimizer='Adam')

    model.train(input_fn=input_train, steps=STEPS)

    eval_result = model.evaluate(input_fn=input_test)

    average_loss = eval_result["average_loss"]

    print("Average loss is {}".format(average_loss))

if __name__ == "__main__":
    '''
	
	For example: python3 model.py -f fullunique_1545365126.csv

    '''
    tf.logging.set_verbosity(tf.logging.INFO)

    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='File containing timestamp, lat, lon data', required=True)
    args = vars(parser.parse_args())
    file_name = args['file']

    df = pd.read_csv(file_name)
    #train_x, test_x, train_y, test_y = preprocess(df)
    #train_model(train_x, train_y, test_x, test_y)
    train, test = power_preprocess(df)
    power_model(train, test)







