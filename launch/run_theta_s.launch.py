# from launch import LaunchDescription
# from launch_ros.actions import Node

# def generate_launch_description():
#     cam = Node(
#         package='v4l2_camera',
#         executable='v4l2_camera_node',
#         name='theta_s_camera',
#         output='screen',
#         parameters=[{
#             'video_device': '/dev/video0',
#             'image_size': [1280, 720],
#             'frame_rate': 14,
#             'pixel_format': 'MJPG',      # MJPG に修正（4文字FOURCC
#             'output_encoding': 'rgb8',       # 追加：出力エンコーディングを明示
#             'camera_info_url': '',       # とりあえず空でOK（警告回避）
#         }]
#     )
#     return LaunchDescription([cam])

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    cam = Node(
        package='usb_cam',
        executable='usb_cam_node_exe',
        name='theta_s_camera',
        output='screen',
        parameters=[{
            'video_device': '/dev/video0',    # or /dev/video1 or by-id
            'framerate': 14.0,
            'image_width': 1280,
            'image_height': 720,
            'io_method': 'mmap',
            'camera_name': 'theta_s',
            'camera_info_url': '',
            'pixel_format': 'mjpeg2rgb',
        }]
    )
    return LaunchDescription([cam])


