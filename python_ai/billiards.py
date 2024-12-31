import cv2
import numpy as np

print(cv2.__version__)

image = cv2.imread("/Users/qinziwen/Downloads/taiqiu.png")
img_width, img_height = image.shape[1], image.shape[0]
new_img_width = 1920
new_img_height = 1080
def getPointPosition(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(f"Clicked point: ({x}, {y})")
        cv2.circle(image, (x, y), 5, (0, 0, 255), -1)
def drawPointOnImage(image):
    cv2.namedWindow("imagePoints")
    cv2.setMouseCallback("imagePoints", getPointPosition)
    while True:
        cv2.imshow("imagePoints", image)
        if cv2.waitKey(20) & 0xFF == 27:
            break
    cv2.destroyAllWindows()

def transformImage(image):
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

    src_points = []
    cv2.imshow("Warped Image", warped_image)
    cv2.setMouseCallback('Warped Image', getConerpoints)
    while True:
        cv2.imshow('Warped Image', warped_image) 
        key = cv2.waitKey(1) & 0xFF
        if key == 27:  # ESC
            break
    cv2.destroyAllWindows()

    M_inv = np.linalg.inv(M)
    points = np.array(src_points).reshape(-1, 1, 2).astype(np.float32)
    transformed_points = np.array(cv2.perspectiveTransform(points, M_inv))
    projected_point = transformed_points.reshape(-1, 2).astype(int)
    print(f"投影点: {projected_point}")

    for point in src_points:
        cv2.circle(warped_image, tuple(point), 20, (255, 0, 0), 3)
    for point in projected_point:
        cv2.circle(image, tuple(point), 20, (255, 0, 0), 3)
    
    cv2.imshow("Warped Image with Point", warped_image)
    cv2.imshow("Original Image with Projected Point", image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def detect_billiards_template_matching(image, template_path):
    # 准备模板
    template = cv2.imread(template_path, 0)
    w, h = template.shape[::-1]

    # 转换为灰度图
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # 应用模板匹配
    res = cv2.matchTemplate(gray, template, cv2.TM_CCOEFF_NORMED)
    threshold = 0.8
    loc = np.where(res >= threshold)
    for pt in zip(*loc[::-1]):
        cv2.rectangle(image, pt, (pt[0] + w, pt[1] + h), (0, 255, 0), 2)

    # 显示结果
    cv2.imshow('Detected Billiards', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    # detect_billiards_template_matching(image, "/Users/qinziwen/Downloads/taiqiu_template.png")
    drawPointOnImage(image)
    transformImage(image)