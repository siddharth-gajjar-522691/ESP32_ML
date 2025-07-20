import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

time = np.arange(0,1000,1)
current = np.sin(time/50) + np.random.normal(0,0.1,len(time))
voltage = np.cos(time/50) + np.random.normal(0,0.1,len(time))

label = [0 if t<700 else 1 for t in time]

df = pd.DataFrame({'time': time, 'current': current, 'voltage': voltage, 'label':label})

df.to_csv('predictive_data.csv', index=False)

print(df.head())
print(df.tail())

plt.figure(figsize=(10,4))
plt.plot(time, current, label='Current')
plt.plot(time, voltage, label='Voltage')
plt.axvline(x=700, color='r', linestyle='--', label='Fault region start')
plt.legend()
plt.xlabel('Time')
plt.ylabel('SIgnal')
plt.title('Simulated Sensor Data')
plt.show()