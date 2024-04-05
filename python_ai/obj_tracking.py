import cv2
import numpy as np

def draw_trajectory(frame, points):
    for i in range(1, len(points)):
        prev_pt = tuple(points[i - 1])
        curr_pt = tuple(points[i])
        cv2.line(frame, prev_pt, curr_pt, color=(0, 255, 0), thickness=2)

def main():
    video_path = "/Users/qinziwen/Downloads/feipan/1.mov"
    cap = cv2.VideoCapture(video_path)

    # 初始化追踪器
    tracker = cv2.TrackerCSRT_create()

    # 获取第一帧并确保成功读取
    success, frame = cap.read()
    if not success:
        raise ValueError("Failed to read video.")

    # 通过鼠标点击方式选择要追踪的目标区域
    x, y, w, h = cv2.selectROI("Select Object", frame, fromCenter=False, showCrosshair=True)
    bbox = (x, y, w, h)  # 坐标(x, y)，宽度w，高度h
    tracker.init(frame, bbox)

    points = []  # 存储追踪点坐标

    out_path = video_path.replace(".mov", "_tracked.mp4")
    out = cv2.VideoWriter(out_path, cv2.VideoWriter_fourcc(*'mp4v'), 30, (frame.shape[1], frame.shape[0]))

    while cap.isOpened():
        success, frame = cap.read()
        if not success:
            break

        # 更新追踪器并获取当前目标位置
        success, bbox = tracker.update(frame)

        if success:
            x, y, w, h = [int(v) for v in bbox]
            points.append((x + w // 2, y + h // 2))  # 记录中心点作为轨迹点

            # 绘制当前追踪框
            cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)

        # 绘制轨迹
        if len(points) > 1:
            draw_trajectory(frame, points)

        out.write(frame)

        # 显示结果
        cv2.imshow("Object Tracking with Trajectory", frame)
        key = cv2.waitKey(1) & 0xFF

        # 按'q'键退出
        if key == ord('q'):
            break

    # 清理资源
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()