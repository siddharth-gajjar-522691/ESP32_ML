from sklearn.model_selection import train_test_split
from tensorflow import keras
from tensorflow.keras import layers
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plt
import os
import numpy as np

os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

# Prepare features and target
df =pd.read_csv('predictive_data.csv') 
X = df[['current','voltage', 'time']]
y = df['label']

# Split 80% data for to train and 20% for test
X_train, X_test,y_train, y_test = train_test_split(X,y,test_size = 0.2, random_state=42)

# Simple MLP
model = keras.Sequential([
    layers.Dense(8, activation = 'relu', input_shape=(3,)),
    layers.Dense(4, activation = 'relu'),
    layers.Dense(1, activation = 'sigmoid')
])

model.compile(optimizer='adam', 
              loss='binary_crossentropy', 
              metrics=['accuracy'])

# Train the model
history = model.fit(
                    X_train, y_train, 
                    epochs = 20, 
                    batch_size=20,          # train in mini-batches of samples
                    validation_split=0.1    # use 10% of training data for validation
                )

# Evaluate
loss, accuracy = model.evaluate(X_test,y_test)
print(f"Test Accuracy: {accuracy*100:.2f}%")

# Visualize training
# plt.plot(history.history['accuracy'], label='Train accuracy')
# plt.plot(history.history['val_accuracy'], label='Val accuracy')
# plt.xlabel('Epoch')
# plt.ylabel('Accuracy')
# plt.legend()
# plt.title('Training Progress')
# plt.show()

#  conversion code
# Convert keras model in to tensorflow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)

# Optimization for size and speed
converter.optimizations = [tf.lite.Optimize.DEFAULT]

tflite_model = converter.convert()

# Save the TFlite model
with open("Predictive_maintenance.tflite", "wb") as f:
    f.write(tflite_model)
print("Model saved as predictive_maintenance.tflite")


# Test TFLite model
interpreter = tf.lite.Interpreter(model_path="predictive_maintenance.tflite")
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Prepare one sample
sample_input = np.array([X_test.iloc[0].values], dtype=np.float32)

interpreter.set_tensor(input_details[0]['index'], sample_input)
interpreter.invoke()
output_data = interpreter.get_tensor(output_details[0]['index'])

print("TFLite prediction:", output_data)
