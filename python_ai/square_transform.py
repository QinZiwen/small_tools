import cv2
import numpy as np

# 打印opencv版本信息
print(cv2.__version__)

# 读取图片
original_image = cv2.imread("/Users/qinziwen/Downloads/feipan/feipan_1/feipan_1_180.jpg")
# 获取图片的宽度和高度
img_width, img_height = original_image.shape[1], original_image.shape[0]
new_img_width = 1920
new_img_height = 1080

# 初始化保存坐标列表
src_points = []
# 通过鼠标点击获取源四边形
def get_src_points(event, x, y, flags, param):
    global src_points
    if event == cv2.EVENT_LBUTTONDOWN:
        # 将坐标添加到列表中
        src_points.append((x, y))
        print(f"Clicked point: ({x}, {y})")
        # 将坐标绘制到图片上
        cv2.circle(original_image, (x, y), 5, (0, 0, 255), -1)

cv2.namedWindow('Image')  # 创建一个窗口
cv2.setMouseCallback('Image', get_src_points)  # 注册鼠标回调函数
while True:
    if len(src_points) >= 4:  # 如果已经收集了4个点，结束循环
        break
    cv2.imshow('Image', original_image)  # 显示图片
    key = cv2.waitKey(1) & 0xFF  # 等待按键事件
    if key == 27:  # 如果按下ESC键，退出程序
        break

cv2.destroyAllWindows()  # 关闭所有窗口
# 将src_points转成numpy array
src_points = np.float32(src_points)

# 定义目标四边形，通常为矩形
padding = 50
dst_points = np.float32([
    [0 + padding, 0 + padding],  # 新左上角
    [new_img_width - padding, 0 + padding],  # 新右上角
    [new_img_width - padding, new_img_height - padding],  # 新右下角
    [0 + padding, new_img_height - padding]  # 新左下角
])

print("收集的坐标:", src_points)
print("目标坐标:", dst_points)

# 计算透视单应矩阵
M = cv2.getPerspectiveTransform(src_points, dst_points)

# 应用透视变换
warped_image = cv2.warpPerspective(original_image, M, (new_img_width, new_img_height))
# 在warped_image上画出目标四边形
cv2.polylines(warped_image, [np.int32(dst_points)], True, (0, 255, 0), 3)
# 在warped_image上画一条直线
first_point = dst_points[0] + [100, 0]
last_point = dst_points[-1] + [100, 0]
first_point_int = first_point.astype(int)
last_point_int = last_point.astype(int)
cv2.line(warped_image, first_point_int, last_point_int, (0, 255, 0), 3)
first_point = dst_points[1] - [100, 0]
last_point = dst_points[2] - [100, 0]
first_point_int = first_point.astype(int)
last_point_int = last_point.astype(int)
cv2.line(warped_image, first_point_int, last_point_int, (0, 255, 0), 3)

# 将original_image的图像中心点利用M投影到warped_image，并将投影结果画在warped_image上，使用红色三角形
center_point = (img_width // 2, img_height // 2)
# 将中心点转为numpy float32格式
center_point_float32 = np.float32(center_point)
# 对中心点进行reshape，以便于传递给perspectiveTransform
projected_center = cv2.perspectiveTransform(np.float32([center_point_float32]).reshape(-1, 1, 2), M)[0][0]
# 以projected_center为中心画一个圆形在warped_image上，圆形的半径为30个像素
cv2.circle(warped_image, tuple(projected_center.astype(int)), 30, (0, 0, 255), -1)

# 显示原图和变换后的图
cv2.imshow("Original Image", original_image)
cv2.imshow("Warped Image", warped_image)
cv2.waitKey(0)
cv2.destroyAllWindows()