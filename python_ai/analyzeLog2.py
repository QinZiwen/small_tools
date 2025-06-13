import pandas as pd
import matplotlib.pyplot as plt
import re

filename = "/mnt/c/Users/qinziwen/Downloads/analysisLog.txt"

timeSec = 0
with open(filename, 'r', errors='replace') as file:
    for line in file:
        length_match = re.search(r'length:\s*(\d+)', line)
        speed_match = re.search(r'speed:\s*([\d\.]+)', line)
        if length_match and speed_match:  
            length = int(length_match.group(1))  # length in meters  
            speed = float(speed_match.group(1))   # speed in km/h  
            
            # 计算行驶时间 (时间 = 距离 / 速度)  
            # 速度需要转换成米每秒  
            speed_mps = speed * 1000 / 3600  # km/h to m/s  
            time_seconds = length / speed_mps if speed_mps > 0 else float('inf')  # avoid division by zero
            timeSec += time_seconds

print("timeSec: ", timeSec)
