import numpy as np
import matplotlib.pyplot as plt

# 模拟原始速度数据 (单位 km/h)
np.random.seed(42)
num_points = 50

# true_speed 随时间慢慢增长，例如从 78.5 增长到 82
true_speed_start = 78.5
true_speed_end = 82.0
true_speed = np.linspace(true_speed_start, true_speed_end, num_points)

# 模拟微小测量波动
noise = np.random.uniform(-0.5, 0.5, size=num_points)
raw_speed = true_speed + noise

# 直接四舍五入的速度
raw_speed_int = np.round(raw_speed)

# 简单低通滤波 (EMA, alpha=0.5)
alpha = 0.2
smoothed_speed = []
s_prev = raw_speed_int[0]  # 初始值
for v in raw_speed_int:
    s_new = alpha * v + (1 - alpha) * s_prev
    smoothed_speed.append(s_new)
    s_prev = s_new

# 绘图
plt.figure(figsize=(12,6))
plt.plot(raw_speed_int, label="Raw speed (rounded)", marker='o')
plt.plot(smoothed_speed, label="Smoothed speed (low-pass)", marker='x')
plt.plot(true_speed, color='gray', linestyle='--', label="True speed (trend)")
plt.title("Comparison of Raw vs Smoothed Speed (Increasing True Speed)")
plt.xlabel("Sample index")
plt.ylabel("Speed (km/h)")
plt.legend()
plt.grid(True)
plt.show()
