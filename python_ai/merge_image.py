import cv2
import numpy as np

def stitch_images(image1, image2):
    # 初始化Stitcher对象，设置模式为自动
    stitcher = cv2.Stitcher_create(cv2.STITCHER_PANORAMA)

    # 将图片转换为numpy数组
    images = [image1, image2]

    # 对每张图片进行预处理
    for i in range(len(images)):
        images[i] = cv2.cvtColor(images[i], cv2.COLOR_BGR2RGB)
        images[i] = cv2.resize(images[i], None, fx=0.5, fy=0.5)

    # 使用Stitcher拼接图片
    status, panorama = stitcher.stitch(images)

    if status == cv2.STITCHER_OK:
        # 如果拼接成功，恢复原始大小并转换回BGR颜色空间
        panorama = cv2.resize(panorama, None, fx=2, fy=2)
        panorama = cv2.cvtColor(panorama, cv2.COLOR_RGB2BGR)
    else:
        print("Error: Could not stitch the images")

    return panorama

# 从命令行读取两个图片的路径
import sys
if len(sys.argv) != 3:
    print("Usage: python script_name.py image1_path image2_path")
    sys.exit(1)

# 读取两张图片
image1 = cv2.imread(sys.argv[1])
image2 = cv2.imread(sys.argv[2])

# 拼接图片
panorama = stitch_images(image1, image2)

# 显示拼接后的全景图
if panorama is not None:
    cv2.imshow('Panoramic Image', panorama)
    cv2.waitKey(0)  # 等待用户按键后关闭窗口
    cv2.destroyAllWindows()
else:
    print("No image to display")