import cv2
import mediapipe as mp
import numpy as np

# 打开摄像头进行实时视频流的捕获，并实时显示出来
def video_capture():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("无法打开摄像头")
        exit()

    while True:
        ret, frame = cap.read()

        if not ret:
            print("无法接收帧(视频流已结束？)")
            break
        
        frame = cv2.flip(frame, 1)

        # 调用detect_and_draw_hand_landmarks函数
        hand_landmarks = detect_and_draw_hand_landmarks(frame) 

        # 调用draw_palm_points函数
        draw_palm_points(frame, hand_landmarks)

        cv2.imshow('Frame', frame)

        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# 测量手部区域上的关键点和边缘，并呈现关键点
def detect_and_draw_hand_landmarks(frame):
    mp_hands = mp.solutions.hands
    hands = mp_hands.Hands(static_image_mode=False, 
                           max_num_hands=2, 
                           min_detection_confidence=0.5)

    mp_drawing = mp.solutions.drawing_utils
    rgb_image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_image)

    hand_landmarks = None
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_drawing.draw_landmarks(frame, hand_landmarks)
    hands.close()
    return hand_landmarks

# 描绘手指捏合时的轨迹点跟踪与显示
def draw_palm_points(frame, hand_landmarks, history_palm_points=[]):
    if not hand_landmarks:
        history_palm_points.clear()
        return
    thumb_index = 4
    indexfinger_index = 8

    indexfinger_point = hand_landmarks.landmark[indexfinger_index]
    thumb_point = hand_landmarks.landmark[thumb_index]

    finger_distance = np.sqrt((thumb_point.x - indexfinger_point.x) ** 2 
                + (thumb_point.y - indexfinger_point.y) ** 2)
    if finger_distance < 0.1:
        palm_point = [int((thumb_point.x + indexfinger_point.x) * frame.shape[1] // 2),
                                    int((thumb_point.y + indexfinger_point.y) * frame.shape[0] // 2)]
        history_palm_points.append(palm_point)
    
    if len(history_palm_points) < 2:
        pass
    else:
        for i in range(1, len(history_palm_points)):
            cv2.line(frame, tuple(history_palm_points[i-1][:2]), tuple(history_palm_points[i][:2]), (0, 255, 255), 5) 


# 调用video_capture函数
if __name__ == '__main__':
    video_capture()
