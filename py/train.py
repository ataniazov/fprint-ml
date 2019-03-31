
from __future__ import print_function

import numpy as np
np.random.seed(1337)  # for reproducibility

import pandas as pd
from PIL import Image

import keras
from keras.models import Sequential
#from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
#from keras.layers import Convolution2D, MaxPooling2D
from keras.utils import to_categorical
from keras import backend as K

batch_size = 10
num_classes = 2
num_epochs = 20

fp_path = "./bigfp/"
dataset_count = 240
dataset_train = 200

# To make the data ready for CNN, pictures are named with indexes, like '1.jpg', '2.jpg', etc..
def dir_to_dataset(mypath, loc_train_labels=""):
    dataset = []
    
    gbr = pd.read_csv(loc_train_labels, sep="\t")

    #for file_count, file_name in enumerate( sorted(glob(glob_files),key=len) ):
    for i in range(1,dataset_count+1):
        #image = Image.open(mypath + str(i)+'.pgm')
        img = Image.open(mypath + str(i) + '.pgm').convert('LA') #tograyscale
        pixels = [f[0] for f in list(img.getdata())]
        dataset.append(pixels)
    # outfile = glob_files+"out"
    # np.save(outfile, dataset)
    if len(loc_train_labels) > 0:
        df = pd.read_csv(loc_train_labels)
        return np.array(dataset), gbr["Class"].values
    else:
        return np.array(dataset)

if __name__ == '__main__':
    Data, y = dir_to_dataset(fp_path, fp_path+"label.csv")
    
    #Split the train set and validation set
    train_set_x = Data[:dataset_train]
    val_set_x = Data[dataset_train:]
    train_set_y = y[:dataset_train]
    val_set_y = y[dataset_train:]

    # the data, split between train and test sets
    (X_train, y_train), (X_test, y_test) = (train_set_x,train_set_y),(val_set_x,val_set_y)

    # input image dimensions
    img_rows, img_cols = 384, 290
    # number of convolutional filters to use
    num_filters = 32
    # size of pooling area for max pooling
    pool_size = (2, 2)

    # convolution kernel size
    kernel_size = (3, 3)

    # Checking if the backend is Theano or Tensorflow
    #if K.image_dim_ordering() == 'th':
    if K.image_data_format() == 'channels_first':
        X_train = X_train.reshape(X_train.shape[0], 1, img_rows, img_cols)
        X_test = X_test.reshape(X_test.shape[0], 1, img_rows, img_cols)
        input_shape = (1, img_rows, img_cols)
    else:
        X_train = X_train.reshape(X_train.shape[0], img_rows, img_cols, 1)
        X_test = X_test.reshape(X_test.shape[0], img_rows, img_cols, 1)
        input_shape = (img_rows, img_cols, 1) 

    X_train = X_train.astype('float32')
    X_test = X_test.astype('float32')
    X_train /= 255
    X_test /= 255
    print('X_train shape:', X_train.shape)
    print(X_train.shape[0], 'train samples')
    print(X_test.shape[0], 'test samples')

    # convert class vectors to binary class matrices
    Y_train = keras.utils.to_categorical(y_train, num_classes)
    Y_test = keras.utils.to_categorical(y_test, num_classes)
    
    model = Sequential()
    model.add(Conv2D(num_filters, kernel_size=(kernel_size[0], kernel_size[1]), padding='valid', activation='relu', input_shape=input_shape))
    model.add(Conv2D(num_filters, kernel_size=(kernel_size[0], kernel_size[1]), activation='relu'))
    model.add(MaxPooling2D(pool_size=pool_size))
    model.add(Dropout(0.25))

    model.add(Flatten())
    model.add(Dense(128, activation ='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(num_classes, activation='softmax'))

    model.compile(loss=keras.losses.categorical_crossentropy, optimizer='sgd', metrics=['accuracy'])

    model.fit(X_train, Y_train, batch_size=batch_size, epochs=num_epochs, verbose=1, validation_data=(X_test, Y_test))
    score = model.evaluate(X_test, Y_test, verbose=1)
    print('Test (score) loss:', score[0])
    print('Test accuracy:', score[1])
    
    model.save(fp_path+"fp-cnn-model.h5")