from comet_ml import Experiment
import numpy as np 
import tensorflow as tf 
import argparse
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split

def power_model(df):

    #experiment = Experiment(api_key="",
    #                    project_name="", workspace="")

    X = df.iloc[:,3:9]
    Y = df.iloc[:,10]

    FEATURES = X.columns 
    LABEL = Y.name

    scaler = MinMaxScaler()
    X = pd.DataFrame(scaler.fit_transform(X), columns=FEATURES)
    Y = pd.DataFrame(scaler.fit_transform(Y.values.reshape(-1, 1)), columns=[LABEL])
    Y['power'] = Y['power'].apply(lambda x: x*100)

    #Y = pd.DataFrame(Y.values.reshape(-1, 1), columns=[LABEL])

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


    STEPS = 150000
    experiment.log_parameter("steps", STEPS)

    featcols = [
        tf.feature_column.numeric_column("mag_x"),
        tf.feature_column.numeric_column("mag_y"),
        tf.feature_column.numeric_column("mag_z"),
        tf.feature_column.numeric_column("gyro_x"),
        tf.feature_column.numeric_column("gyro_y"),
        tf.feature_column.numeric_column("gyro_z")
    ]

    hidden_layer_1_size = 256
    hidden_layer_2_size = 128
    experiment.log_multiple_params({"hidden_layer_1":hidden_layer_1_size, "hidden_layer_2":hidden_layer_2_size})

    model = tf.estimator.DNNRegressor(feature_columns=featcols, hidden_units=[hidden_layer_1_size, hidden_layer_2_size], optimizer='Adam')#, model_dir='/tmp/model/')

    model.train(input_fn=input_train, steps=STEPS)

    eval_result = model.evaluate(input_fn=input_test)

    predictions = model.predict(input_fn=lambda: predict_input_fn(x_test[0:32], y_test[0:32]))

    for i, el in enumerate(list(predictions)):
        prediction_val = el['predictions'][0]
        actual_val = y_test.iloc[i]['power']
        print("I: {}".format(i))
        print("Predicted: {}".format(prediction_val))
        print("Actual: {}".format(actual_val))
        print("Difference: {}\n".format(prediction_val - actual_val))
        print("")

    print("\nAverage loss is {}\n".format(eval_result['average_loss']))
    experiment.log_metric("average_loss", eval_result['average_loss'])

if __name__ == "__main__":
    '''
	
	For example: python3 power_model.py -f fullunique_1545365126.csv

    '''
    tf.logging.set_verbosity(tf.logging.INFO)

    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file', help='Dataset containing mag data, gyro data, axis and power', required=True)
    args = vars(parser.parse_args())
    file_name = args['file']

    df = pd.read_csv(file_name)
    power_model(df)

    #Refer to https://www.tensorflow.org/guide/custom_estimators#running_the_abalone_model - when making axis model 







