import pandas as pd
import matplotlib.pyplot as plt

# 读取文件并提取需要的数据
filename = "/mnt/c/Users/qinziwen/Downloads/ninavi_log_sd-69.log"
data = {'timestamp': [], 'travel_dist': []}

date_format = '%Y/%m/%d %H:%M:%S:%f'
start_time = pd.to_datetime('2024/06/19 16:27:00:004', format=date_format)
end_time = pd.to_datetime('2024/06/19 16:28:10:004', format=date_format)

with open(filename, 'r', errors='replace') as file:
    for line in file:
        parts = line.strip().split()
        # print(parts)
        if (len(parts) > 6 and "navi_session.cpp" in parts[5] and "246" in parts[5]):
            print(parts)
            timestamp = parts[2][1:] + " " + parts[3][:-1]
            travel_dist = int(parts[8].split(':')[1])
            print("time: ", timestamp, ", dist: ", travel_dist)
            
            time = pd.to_datetime(timestamp, format=date_format)
            if start_time <= time <= end_time:
                data['timestamp'].append(time)
                data['travel_dist'].append(travel_dist)
                print("pd time: ", data['timestamp'], ", dist: ", data['travel_dist'])

# 创建 DataFrame
df = pd.DataFrame(data)
df = df.set_index('timestamp')

# 绘制折线图
plt.figure(figsize=(10, 6))
plt.plot(df.index, df['travel_dist'], marker='o', color='b', linestyle='-')
plt.xlabel('Time')
plt.ylabel('Travel Distance')
plt.title('Travel Distance over Time')
plt.xticks(rotation=45)
plt.grid(True)
plt.savefig('/mnt/c/Users/qinziwen/Downloads/ninavi_log_sd-69.png')
