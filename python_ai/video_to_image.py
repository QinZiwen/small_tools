import cv2
import os

def video_to_images_opencv(input_video_path, output_image_path, frame_rate=1, rotation_angle=0):
    """
    将视频转换为图片
    :param input_video_path: 输入视频的路径
    :param output_image_path: 输出图片的目录
    :param frame_rate: 指定的帧率，每frame_rate秒保存一帧
    :param rotation_angle: 保存图片时的旋转角度（0, 90, 180, 270）
    """
    # 检查输出路径是否存在，如果存在则清空，如果不存在则创建
    if os.path.exists(output_image_path):
        for file in os.listdir(output_image_path):
            file_path = os.path.join(output_image_path, file)
            os.remove(file_path)
    if not os.path.exists(output_image_path):
        os.makedirs(output_image_path)

    # 打开视频文件
    cap = cv2.VideoCapture(input_video_path)
    
    # 获取视频的帧率
    fps = cap.get(cv2.CAP_PROP_FPS)
    
    # 计算每帧的索引，以便按指定帧率保存
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    step = int(fps / frame_rate)
    print(f"Total frames: {total_frames}, fps: {fps}, step: {step}")
    
    # 保存每一帧为图片
    for i in range(0, total_frames, step):
        ret, frame = cap.read()
        if not ret:
            break
        
        # 旋转图像
        if rotation_angle != 0:
            (h, w) = frame.shape[:2]
            center = (w // 2, h // 2)
            M = cv2.getRotationMatrix2D(center, rotation_angle, 1.0)
            
            # 计算旋转后的图像的新尺寸
            cos = abs(M[0, 0])
            sin = abs(M[0, 1])
            new_w = int((h * sin) + (w * cos))
            new_h = int((h * cos) + (w * sin))
            
            # 调整旋转矩阵以适应新的尺寸
            M[0, 2] += (new_w / 2) - center[0]
            M[1, 2] += (new_h / 2) - center[1]
            
            frame = cv2.warpAffine(frame, M, (new_w, new_h))
        
        image_filename = f"{os.path.splitext(os.path.basename(input_video_path))[0]}_{i}.jpg"  # 图片文件名
        image_path = os.path.join(output_image_path, image_filename)
        cv2.imwrite(image_path, frame)

    # 释放视频捕获对象
    cap.release()

# 使用示例
video_to_images_opencv("/Users/qinziwen/Downloads/yinhu.mp4", "/Users/qinziwen/Downloads/yinhu", 5, 90)