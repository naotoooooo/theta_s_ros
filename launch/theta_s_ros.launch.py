# # launch/theta_s_ros.launch.py
# import os
# from launch import LaunchDescription
# from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
# from launch.substitutions import LaunchConfiguration
# from launch_ros.actions import PushRosNamespace, Node
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# from ament_index_python.packages import get_package_share_directory

# def generate_launch_description():
#     pkg_share = get_package_share_directory('theta_s_ros')

#     image_raw_topic = LaunchConfiguration('image_raw_topic')
#     arg_image_raw_topic = DeclareLaunchArgument(
#         'image_raw_topic', default_value='/theta_s/image_raw'
#     )

#     # 先に namespace を押し込む（これ以降のノードに適用される）
#     ns = PushRosNamespace('theta_s')

#     include_camera = IncludeLaunchDescription(
#         PythonLaunchDescriptionSource(
#             os.path.join(pkg_share, 'launch', 'run_theta_s.launch.py')
#         )
#     )

#     converter = Node(
#         package='theta_s_ros',
#         executable='theta_s_ros',
#         name='converter',
#         output='screen',
#         parameters=[{
#             'crop_y': [145, 475],
#             'unmerge_top_and_bottom': True,
#         }],
#         # 変換ノードは /camera/image_raw を購読する想定なので、そこを /theta_s/image_raw に差し替え
#         remappings=[
#             ('/camera/image_raw', image_raw_topic),
#         ],
#     )

#     return LaunchDescription([
#         arg_image_raw_topic,
#         ns,                 # ← ここが先頭
#         include_camera,     # ← NS配下で v4l2_camera 起動（/theta_s/image_raw を出す）
#         converter
#     ])


import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch_ros.actions import PushRosNamespace, Node
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_share = get_package_share_directory('theta_s_ros')

    ns = PushRosNamespace('theta_s')

    include_camera = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_share, 'launch', 'run_theta_s.launch.py')
        )
    )

    converter = Node(
        package='theta_s_ros',
        executable='theta_s_ros',
        name='converter',
        output='screen',
        parameters=[{
            'crop_y': [145, 475],
            'unmerge_top_and_bottom': True,
        }],
        # remappings は不要（購読名が "image_raw" なら /theta_s/image_raw を自動購読）
    )

    return LaunchDescription([ns, include_camera, converter])