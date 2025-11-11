cmake_minimum_required(VERSION 3.8)
project(theta_s_ros)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_executable(${PROJECT_NAME}
  src/image_converter.cpp
  include/opencv_theta_s/EquirectangularConversion/ThetaConversion.cpp
  include/panorama2cubemap/src/Panorama2Cubemap.cpp
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

find_package(Boost REQUIRED COMPONENTS)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)

# 依存関係を追加
ament_target_dependencies(${PROJECT_NAME}
  OpenCV
  cv_bridge
)

install(FILES nodelet_description.xml
  DESTINATION share/${PROJECT_NAME}
)

# コード品質を自動的にチェック
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_package()