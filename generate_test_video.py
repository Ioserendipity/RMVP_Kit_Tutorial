import cv2
import numpy as np
import os

# --- 视频参数 ---
VIDEO_FILENAME = "test_armor.avi"
VIDEO_WIDTH = 1280
VIDEO_HEIGHT = 720
FPS = 30
DURATION_SECONDS = 5

# ==================== 修改部分 开始 ====================

# --- 模拟装甲板参数 ---
# 我们现在定义单个灯条的尺寸
LIGHT_BAR_WIDTH = 20
LIGHT_BAR_HEIGHT = 80
# 两个灯条之间的间隙
ARMOR_GAP = 60
# 整个装甲板（包括间隙）的总宽度
ARMOR_WIDTH = 2 * LIGHT_BAR_WIDTH + ARMOR_GAP
ARMOR_COLOR = (255, 0, 0) # BGR格式的蓝色
START_X = 100
END_X = VIDEO_WIDTH - 100 - ARMOR_WIDTH
Y_POS = VIDEO_HEIGHT // 2

# ==================== 修改部分 结束 ====================


def generate_video():
    output_dir = os.path.join("data", "videos")
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, VIDEO_FILENAME)
    
    fourcc = cv2.VideoWriter_fourcc(*'MJPG')
    out = cv2.VideoWriter(output_path, fourcc, FPS, (VIDEO_WIDTH, VIDEO_HEIGHT))

    if not out.isOpened():
        print(f"Error: Could not open video writer for path {output_path}")
        return

    total_frames = int(FPS * DURATION_SECONDS)
    print(f"Generating realistic video with {total_frames} frames...")

    for i in range(total_frames):
        frame = np.zeros((VIDEO_HEIGHT, VIDEO_WIDTH, 3), dtype=np.uint8)
        
        progress = i / (total_frames - 1)
        # current_x 现在是整个装甲板的左上角x坐标
        current_x = int(START_X + (END_X - START_X) * progress)

        # ==================== 修改部分 开始 ====================
        
        # 绘制左边的灯条
        left_bar_tl = (current_x, Y_POS)
        left_bar_br = (current_x + LIGHT_BAR_WIDTH, Y_POS + LIGHT_BAR_HEIGHT)
        cv2.rectangle(frame, left_bar_tl, left_bar_br, ARMOR_COLOR, -1)
        
        # 绘制右边的灯条
        right_bar_x = current_x + LIGHT_BAR_WIDTH + ARMOR_GAP
        right_bar_tl = (right_bar_x, Y_POS)
        right_bar_br = (right_bar_x + LIGHT_BAR_WIDTH, Y_POS + LIGHT_BAR_HEIGHT)
        cv2.rectangle(frame, right_bar_tl, right_bar_br, ARMOR_COLOR, -1)

        # ==================== 修改部分 结束 ====================

        out.write(frame)

        if (i + 1) % FPS == 0:
            print(f"  ... {i + 1} / {total_frames} frames written.")
            
    out.release()
    print(f"Video saved successfully to {output_path}")

if __name__ == "__main__":
    generate_video()