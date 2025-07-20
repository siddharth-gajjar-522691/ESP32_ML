Installation dependencies

Install virtual Environment in conda

1. install miniconda
2. set the environmenta variable and set conda as a command promt command
3. create a virtual enviroment
	conda create -n myenv
4. install required dependencies
	pip install pandas numpy scikit-learn matplotlib tensorflow
5. write the script and run in this virtual environment


“Faced a runtime issue with Intel oneMKL when starting the project. Solved it by setting up a clean Python environment with proper MKL support. Troubleshooting is part of the journey!”

“To ensure consistent results across different CPUs, I disabled oneDNN optimizations by setting TF_ENABLE_ONEDNN_OPTS=0—a small but important tweak for stability.”

Batch size:

“Instead of training on the whole dataset at once, I split it into smaller groups—here 20 samples at a time. After each group, the model updates its weights. This makes training efficient and memory‑friendly.”

Validation split:

“I set aside 10% of the training data as validation data. The model doesn’t train on this—it only tests on it after each epoch. This helps me see if the model is generalizing well or overfitting.”

🚀 Step 3: Convert to TFLite
👉 Why TensorFlow Lite?
Microcontrollers (like ESP32) can’t run heavy TensorFlow models.
TensorFlow Lite converts the model into a lightweight format with:
✅ Smaller size
✅ Faster inference
✅ Lower memory usage

linked in
“Converted my predictive maintenance neural network into a TensorFlow Lite model optimized for microcontrollers.
File size: just a few KB! Next step: deploy on ESP32 and run real-time predictions on current and voltage patterns. #TinyML #EmbeddedAI #ESP32”