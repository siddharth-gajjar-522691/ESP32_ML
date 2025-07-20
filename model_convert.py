import pathlib

# Read the tflite file
tflite_model = pathlib.Path("predictive_maintenance.tflite").read_bytes()

# Write as Carray
with open("model_data.h","w") as f:
    f.write("const unsigned char model_tfilte[] = {")
    f.write(",".join(str(b) for b in tflite_model))
    f.write("};\n")
    f.write(f"const int model_tflite_len = {len(tflite_model)};")
