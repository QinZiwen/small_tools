import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.dates as mdates  

filename = "/mnt/c/Users/qinziwen/Downloads/20250327155613/20250327155613/smart_log/logic.log"
data = {'timestamp': [], 'travel_dist': []}

# date_format = '%Y/%m/%d %H:%M:%S:%f'
date_format = '%H:%M:%S.%f'
# start_time = pd.to_datetime('2024/06/19 16:27:00:004', format=date_format)
# end_time = pd.to_datetime('2024/06/19 16:28:10:004', format=date_format)
start_time = pd.to_datetime('15:51:00.265', format=date_format)
end_time = pd.to_datetime('15:59:24.265', format=date_format)

with open(filename, 'r', errors='replace') as file:
    for line in file:
        parts = line.strip().split()
        # print(parts)
        if (len(parts) >= 6 and "dispSpeed" in parts[-2]):
            # print(parts)
            timestamp = parts[0][1:13]
            travel_dist = int(parts[-1])
            
            time = pd.to_datetime(timestamp, format=date_format)
            # data['timestamp'].append(time)
            # data['travel_dist'].append(travel_dist)
            if start_time <= time <= end_time:
                print("time: ", timestamp, ", dist: ", travel_dist)
                data['timestamp'].append(time)
                data['travel_dist'].append(travel_dist)
                # print("pd time: ", data['timestamp'], ", dist: ", data['travel_dist'])

Dlen = len(data['timestamp'])
df = pd.DataFrame(data)
df = df.set_index('timestamp')

print("Dlen: " + str(Dlen))

plt.figure(figsize=(10, 6))
plt.plot(df.index, df['travel_dist'], marker='o', color='b', linestyle='-')
plt.xlabel('Time')
plt.ylabel('speed(km/h)')
# plt.ylim(0, 10)
plt.title('speed over time')
plt.xticks(rotation=30)
# plt.xticks(np.arange(0, Dlen, 1)) 
plt.grid(True)
plt.savefig(filename + ".png")
