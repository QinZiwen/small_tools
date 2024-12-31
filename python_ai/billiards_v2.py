import cv2
import numpy as np

def calcHomography(pts):
    # 已知的实际世界中的点（单位可以是米或厘米等, 根据实际情况）
    real_points = np.array([
        [0, 0],       # Point 1 (X1, Y1)
        [0, 383],       # Point 2 (X2, Y2)
        [204, 383],       # Point 3 (X3, Y3)
        [204, 0]        # Point 4 (X4, Y4)
    ], dtype='float32')

    # 对应的图像中的点（以像素为单位）
    # image_points = np.array([
    #     [100, 100],   # Point 1 (u1, v1)
    #     [300, 100],   # Point 2 (u2, v2)
    #     [300, 300],   # Point 3 (u3, v3)
    #     [100, 300]    # Point 4 (u4, v4)
    # ], dtype='float32')
    image_points = np.array(pts[:4], dtype='float32')

    # 计算单应性矩阵
    H, mask = cv2.findHomography(image_points, real_points, cv2.RANSAC)

    # 图像中的点，需要映射到实际世界中的点
    # image_point_to_map = np.array([
    #     [200, 200]    # Point (u, v)
    # ], dtype='float32')
    image_point_to_map = np.array(pts[4], dtype='float32')

    # 增加一个维度，使其成为齐次坐标
    image_point_to_map = image_point_to_map.reshape(-1, 1, 2)

    # 使用单应性矩阵映射点
    real_point = cv2.perspectiveTransform(image_point_to_map, H)

    # 打印结果
    print("Image Point:", image_point_to_map[0][0])
    print("Real Point:", real_point[0][0])

def transformImageToImage(image):
    src_points = []
    def getConerpoints(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            src_points.append((x, y))
            print(f"Clicked point: ({x}, {y})")
            cv2.circle(image, (x, y), 5, (0, 0, 255), -1)

    cv2.namedWindow('ImageTransform')
    cv2.setMouseCallback('ImageTransform', getConerpoints)
    while True:
        if len(src_points) >= 4:
            break
        cv2.imshow('ImageTransform', image) 
        key = cv2.waitKey(1) & 0xFF
        if key == 27:  # ESC
            break
    cv2.destroyAllWindows()

    src_points = np.float32(src_points)

    padding = 50
    new_img_width = 1920
    new_img_height = 1080
    dst_points = np.float32([
        [0 + padding, 0 + padding],  # 新左上角
        [new_img_width - padding, 0 + padding],  # 新右上角
        [new_img_width - padding, new_img_height - padding],  # 新右下角
        [0 + padding, new_img_height - padding]  # 新左下角
    ])

    print("收集的坐标:", src_points)
    print("目标坐标:", dst_points)

    M = cv2.getPerspectiveTransform(src_points, dst_points)
    print("透视变换矩阵:", M)
    warped_image = cv2.warpPerspective(image, M, (new_img_width, new_img_height))
    return warped_image

def transformImage(image):
    src_points = []
    def getConerpoints(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            src_points.append((x, y))
            print(f"Clicked point: ({x}, {y})")
            cv2.circle(image, (x, y), 5, (255, 0, 0), -1)

    cv2.namedWindow('ImageTransform')
    cv2.setMouseCallback('ImageTransform', getConerpoints)
    while True:
        if len(src_points) >= 5:
            break
        cv2.imshow('ImageTransform', image) 
        key = cv2.waitKey(1) & 0xFF
        if key == 27:  # ESC
            break
    cv2.destroyAllWindows()

    calcHomography(src_points)

if __name__ == "__main__":
    image = cv2.imread("/Users/qinziwen/Downloads/taiqiu.png")
    #   wrap_image = transformImageToImage(image)
    transformImage(image)