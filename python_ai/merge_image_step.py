import cv2
import numpy as np
import sys

def match_features(image1, image2):
    sift = cv2.SIFT_create()  # 更新这里以使用cv2.SIFT_create()
    bf = cv2.BFMatcher()

    kp1, des1 = sift.detectAndCompute(image1, None)
    kp2, des2 = sift.detectAndCompute(image2, None)

    matches = bf.knnMatch(des1, des2, k=2)
    good_matches = []
    for m, n in matches:
        if m.distance < 0.75 * n.distance:
            good_matches.append(m)

    src_pts = np.float32([kp1[m.queryIdx].pt for m in good_matches]).reshape(-1, 1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good_matches]).reshape(-1, 1, 2)

    M, _ = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)

    return M, good_matches

# 其余代码保持不变...

def warp_and_stitch(image1, image2, M):
    height, width, _ = image1.shape
    img2_warped = cv2.warpPerspective(image2, M, (width, height))

    # 获取图像1和图像2的重叠区域
    overlap_width = max(min(width, img2.shape[1]), 1)
    overlap_height = height

    # 创建一个大图像，用于存放结果
    result = np.zeros((height, img2.shape[1] + width, 3), dtype=np.uint8)
    
    # 将图像2的变形结果放在左边
    result[:, :img2_warped.shape[1], :] = img2_warped

    # 将image1放置在右边，注意这里需要调整索引以适应新的图像宽度
    result[:, img2_warped.shape[1]:, :] = image1

    # 在重叠区域中，根据加权平均融合两部分
    alpha = 0.5  # 控制融合程度的权重
    for y in range(overlap_height):
        for x in range(overlap_width):
            # 注意：x坐标需要转换为相对于img2_warped的位置
            x_warp = x + img2_warped.shape[1] - width
            color1 = image1[y, x]
            color2 = img2_warped[y, x_warp]
            result[y, x_warp] = (1 - alpha) * color1 + alpha * color2

    return result

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python script.py image1.jpg image2.jpg")
        sys.exit(1)

    # 从命令行读取图像
    img1_path = sys.argv[1]
    img2_path = sys.argv[2]

    # 读取图像
    img1 = cv2.imread(img1_path, cv2.IMREAD_COLOR)
    img2 = cv2.imread(img2_path, cv2.IMREAD_COLOR)

    # 匹配特征并计算变换
    M, _ = match_features(img1, img2)

    # 拼接图像
    result = warp_and_stitch(img1, img2, M)

    # 显示结果
    cv2.imshow('Stitched Image', result)
    cv2.waitKey(0)
    cv2.destroyAllWindows()