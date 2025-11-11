// #ifndef __IMAGE_CONVERTER_HPP
// #define __IMAGE_CONVERTER_HPP

// #include <cv_bridge/cv_bridge.h>
// #include <opencv2/opencv.hpp>
// #include <rclcpp/rclcpp.hpp>
// #include <sensor_msgs/msg/compressed_image.hpp>
// #include <sensor_msgs/msg/image.hpp>


// class ImageConverter : public rclcpp::Node
// {
//     public:
//         ImageConverter();

//         void convert_to_equirectangular(cv::Mat& cv_image, cv::Mat& equirectangular_image);
//         void publish_image(const cv::Mat& src_image, rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr pub, const std_msgs::msg::Header& header);
//         void image_callback(const std::shared_ptr<const sensor_msgs::msg::Image>&);

//     private:
        
//         rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr equirectangular_image_pub_;
//         std::vector<rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr> cubemap_image_pubs_;
//         rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;

//         std::vector<int64_t> crop_y_;
//         bool unmerge_top_and_bottom_;

// };


// #endif // __IMAGE_CONVERTER_HPP



#ifndef __IMAGE_CONVERTER_HPP
#define __IMAGE_CONVERTER_HPP

#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/image.hpp>

class ImageConverter : public rclcpp::Node
{
public:
  ImageConverter();

  void convert_to_equirectangular(cv::Mat& cv_image, cv::Mat& equirectangular_image);

  // Compressed(JPEG) を publish
  void publish_image(const cv::Mat& src_image,
                     rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr pub,
                     const std_msgs::msg::Header& header);

  // Raw(Image) を publish
  void publish_image_raw(const cv::Mat& src_image,
                         rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub,
                         const std_msgs::msg::Header& header);

  void image_callback(const std::shared_ptr<const sensor_msgs::msg::Image>&);

private:
  // Equirectangular（Raw & Compressed）
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr           equirectangular_image_pub_raw_;
  rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr equirectangular_image_pub_;

  // Cubemap 各面（Raw & Compressed）
  std::vector<rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr>           cubemap_image_pubs_raw_;
  std::vector<rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr> cubemap_image_pubs_;

  // 入力
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;

  // パラメータ
  std::vector<int64_t> crop_y_;
  bool unmerge_top_and_bottom_;
};

#endif // __IMAGE_CONVERTER_HPP
