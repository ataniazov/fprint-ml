
from __future__ import print_function

import numpy as np
np.random.seed(1337)  # for reproducibility

from PIL import Image
import pandas as pd

import keras

from keras.models import load_model
from keras.preprocessing.image import img_to_array, load_img
from keras import backend as K

batch_size = 10
num_classes = 2
num_epochs = 20

fp_path = "./bigfp/"
dataset_count = 240

# To make the data ready for CNN, pictures are named with indexes, like '1.jpg', '2.jpg', etc..
def dir_to_dataset(dirpath):
    dataset = []

    #for file_count, file_name in enumerate( sorted(glob(glob_files),key=len) ):
    for i in range(1,dataset_count+1):
        #image = Image.open(mypath + str(i)+'.pgm')
        img = Image.open(dirpath + str(i) + '.pgm').convert('LA') #tograyscale
        pixels = [f[0] for f in list(img.getdata())]
        dataset.append(pixels)

    return np.array(dataset)


# To make the data ready for CNN, pictures are named with indexes, like '1.jpg', '2.jpg', etc..
def img_to_dataset(filepath):
    dataset = []
    #image = Image.open(mypath + str(i)+'.pgm')
    img = Image.open(filepath).convert('LA') #tograyscale
    pixels = [f[0] for f in list(img.getdata())]
    dataset.append(pixels)
    return np.array(dataset)

if __name__ == '__main__':

    filepath = "0.pgm"

    #X_validate = img_to_dataset(filepath)
    X_validate = dir_to_dataset("./bigfp/")

    # input image dimensions
    img_rows, img_cols = 384, 290

    X_validate = X_validate.reshape(X_validate.shape[0], img_rows, img_cols, 1)
    X_validate = X_validate.astype('float32')
    X_validate /= 255
    print('X_validate shape:', X_validate.shape)
    print(X_validate.shape[0], 'validate samples')

    model = load_model(fp_path+"fp-cnn-model.h5")
    preds = model.predict_classes(X_validate)
    prob = model.predict_proba(X_validate)
    print(preds, prob)