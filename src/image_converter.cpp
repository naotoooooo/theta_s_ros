// #include "theta_s_ros/image_converter.hpp"

// #include "opencv_theta_s/EquirectangularConversion/ThetaConversion.hpp"
// #include "panorama2cubemap/src/Panorama2Cubemap.hpp"


// ImageConverter::ImageConverter()
// :Node("image_converter")
// {
//     using namespace std::literals::string_literals;

//     crop_y_ = this->declare_parameter<std::vector<int64_t>>("crop_y", {});
//     unmerge_top_and_bottom_ = this->declare_parameter<bool>("unmerge_top_and_bottom", false);
//     if (crop_y_.size() != 0 && crop_y_.size() != 2) {
//         RCLCPP_ERROR(this->get_logger(), "crop_y_ must be a list of size 2");
//         rclcpp::shutdown();
//     }
    
//     image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
//         "/camera/image_raw", 1, std::bind(&ImageConverter::image_callback, this, std::placeholders::_1));
//     equirectangular_image_pub_ = this->create_publisher<sensor_msgs::msg::CompressedImage>("/equirectangular/image_raw/compressed", 1);
//     for (const auto& face : { "right"s, "left"s, "top"s, "bottom"s, "front"s, "back"s, "merged"s }) {
//         cubemap_image_pubs_.push_back(this->create_publisher<sensor_msgs::msg::CompressedImage>(
//             "/cubemap/" + face + "/image_raw/compressed", 1));
//     }
// }

// void ImageConverter::convert_to_equirectangular(cv::Mat& cv_image, cv::Mat& equirectangular_image)
// {
//     static ThetaConversion theta_conversion(cv_image.cols, cv_image.rows);
//     theta_conversion.doConversion(cv_image);
//     static auto rect = crop_y_.size() == 0 ? cv::Rect(0, 0, cv_image.cols, cv_image.rows)
//                                            : cv::Rect(0, crop_y_[0], cv_image.cols, crop_y_[1] - crop_y_[0] + 1);
//     cv::Mat cv_image_cropped(cv_image, rect);
//     equirectangular_image = std::move(cv_image_cropped);
// }

// void ImageConverter::publish_image(const cv::Mat& src_image, rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr pub, const std_msgs::msg::Header& header)
// {
//     sensor_msgs::msg::CompressedImage image_msg;
//     image_msg.header = header;
//     std::vector<unsigned char> buffer;
//     image_msg.format = "jpeg";
//     cv::imencode(".jpg", src_image, buffer);
//     image_msg.data.swap(buffer);
//     equirectangular_image_pub_->publish(image_msg);
// }

// void ImageConverter::image_callback(const std::shared_ptr<const sensor_msgs::msg::Image>& received_image)
// {
//     RCLCPP_DEBUG(this->get_logger(), "start image_callback");
//     double start_time = rclcpp::Clock().now().seconds();

//     RCLCPP_DEBUG(this->get_logger(), "ros image to cv image");
//     cv_bridge::CvImageConstPtr cv_image_ptr;
//     try {
//         cv_image_ptr = cv_bridge::toCvCopy(received_image, sensor_msgs::image_encodings::BGR8);
//     } catch (cv_bridge::Exception& ex) {
//         RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", ex.what());
//         return;
//     }
//     cv::Mat cv_image(cv_image_ptr->image.rows, cv_image_ptr->image.cols, cv_image_ptr->image.type());
//     cv_image = cv_image_ptr->image;

//     RCLCPP_DEBUG(this->get_logger(), "image conversion");
//     cv::Mat equirectangular_image;
//     std::vector<cv::Mat> cubemap_images;
//     cv::Mat merged_image;
//     convert_to_equirectangular(cv_image, equirectangular_image);
//     pano2cube(cv_image, cubemap_images, merged_image, unmerge_top_and_bottom_);
//     cubemap_images.push_back(merged_image);


//     RCLCPP_DEBUG(this->get_logger(), "publish images"); 
//     publish_image(equirectangular_image, equirectangular_image_pub_, received_image->header);
//     for (std::size_t i = 0; i < cubemap_image_pubs_.size(); ++i) {
//         publish_image(cubemap_images[i], cubemap_image_pubs_[i], received_image->header);
//     }

//     RCLCPP_INFO(this->get_logger(), "[image_converter:image_callback] elapsed time : %f [sec]", 
//             rclcpp::Clock(RCL_SYSTEM_TIME).now().seconds() - start_time);

//     return;
// }

// int main(int argc, char** argv)
// {
//     rclcpp::init(argc, argv);
//     const auto node = std::make_shared<ImageConverter>();
//     rclcpp::spin(node);
//     rclcpp::shutdown();
//     return 0;
// }



#include "theta_s_ros/image_converter.hpp"

#include "opencv_theta_s/EquirectangularConversion/ThetaConversion.hpp"
#include "panorama2cubemap/src/Panorama2Cubemap.hpp"

using std::placeholders::_1;

ImageConverter::ImageConverter()
: rclcpp::Node("image_converter")
{
  using namespace std::literals::string_literals;

  crop_y_ = this->declare_parameter<std::vector<int64_t>>("crop_y", {});
  unmerge_top_and_bottom_ = this->declare_parameter<bool>("unmerge_top_and_bottom", false);
  if (crop_y_.size() != 0 && crop_y_.size() != 2) {
    RCLCPP_ERROR(this->get_logger(), "crop_y_ must be a list of size 2");
    rclcpp::shutdown();
  }

  // 入力（相対名。必要なら launch 側で remap する）
  image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
      "image_raw", 1, std::bind(&ImageConverter::image_callback, this, _1));

  // 出力：Equirectangular（Raw & Compressed）
  equirectangular_image_pub_raw_ =
      this->create_publisher<sensor_msgs::msg::Image>("equirectangular/image_raw", 10);
  equirectangular_image_pub_ =
      this->create_publisher<sensor_msgs::msg::CompressedImage>("equirectangular/image_raw/compressed", 10);

  // 出力：Cubemap 各面（Raw & Compressed）
  for (const auto& face : { "right"s, "left"s, "top"s, "bottom"s, "front"s, "back"s, "merged"s }) {
    cubemap_image_pubs_raw_.push_back(
        this->create_publisher<sensor_msgs::msg::Image>("cubemap/" + face + "/image_raw", 1));
    cubemap_image_pubs_.push_back(
        this->create_publisher<sensor_msgs::msg::CompressedImage>("cubemap/" + face + "/image_raw/compressed", 1));
  }
}

void ImageConverter::convert_to_equirectangular(cv::Mat& cv_image, cv::Mat& equirectangular_image)
{
  static ThetaConversion theta_conversion(cv_image.cols, cv_image.rows);
  theta_conversion.doConversion(cv_image);
  static auto rect = crop_y_.empty()
      ? cv::Rect(0, 0, cv_image.cols, cv_image.rows)
      : cv::Rect(0, static_cast<int>(crop_y_[0]),
                 cv_image.cols, static_cast<int>(crop_y_[1] - crop_y_[0] + 1));
  cv::Mat cv_image_cropped(cv_image, rect);
  equirectangular_image = std::move(cv_image_cropped);
}

void ImageConverter::publish_image(const cv::Mat& src_image,
                                   rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr pub,
                                   const std_msgs::msg::Header& header)
{
  sensor_msgs::msg::CompressedImage image_msg;
  image_msg.header = header;
  image_msg.format = "jpeg";
  std::vector<unsigned char> buffer;
  cv::imencode(".jpg", src_image, buffer);
  image_msg.data.swap(buffer);
  pub->publish(image_msg);  // 引数の publisher に対して publish
}

void ImageConverter::publish_image_raw(const cv::Mat& src_image,
                                       rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub,
                                       const std_msgs::msg::Header& header)
{
  auto img_msg = cv_bridge::CvImage(header, sensor_msgs::image_encodings::BGR8, src_image).toImageMsg();
  pub->publish(*img_msg);
}

void ImageConverter::image_callback(const std::shared_ptr<const sensor_msgs::msg::Image>& received_image)
{
  RCLCPP_DEBUG(this->get_logger(), "start image_callback");
  double start_time = rclcpp::Clock().now().seconds();

  // ROS Image → cv::Mat
  cv_bridge::CvImageConstPtr cv_image_ptr;
  try {
    cv_image_ptr = cv_bridge::toCvCopy(received_image, sensor_msgs::image_encodings::BGR8);
  } catch (cv_bridge::Exception& ex) {
    RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", ex.what());
    return;
  }
  cv::Mat cv_image(cv_image_ptr->image.rows, cv_image_ptr->image.cols, cv_image_ptr->image.type());
  cv_image = cv_image_ptr->image;

  // 変換
  cv::Mat equirectangular_image;
  std::vector<cv::Mat> cubemap_images;
  cv::Mat merged_image;
  convert_to_equirectangular(cv_image, equirectangular_image);
  pano2cube(cv_image, cubemap_images, merged_image, unmerge_top_and_bottom_);
  cubemap_images.push_back(merged_image);  // faces + merged の順で計7枚

  // Publish: Equirectangular（Raw & Compressed）
  publish_image_raw(equirectangular_image, equirectangular_image_pub_raw_, received_image->header);
  publish_image(equirectangular_image, equirectangular_image_pub_, received_image->header);

  // Publish: Cubemap 各面（Raw & Compressed）
  for (std::size_t i = 0; i < cubemap_images.size() && i < cubemap_image_pubs_.size(); ++i) {
    publish_image_raw(cubemap_images[i], cubemap_image_pubs_raw_[i], received_image->header);
    publish_image(cubemap_images[i], cubemap_image_pubs_[i], received_image->header);
  }

  RCLCPP_INFO(this->get_logger(), "[image_converter:image_callback] elapsed time : %f [sec]",
              rclcpp::Clock(RCL_SYSTEM_TIME).now().seconds() - start_time);
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ImageConverter>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
