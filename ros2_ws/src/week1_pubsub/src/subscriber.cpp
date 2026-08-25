#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>("week1_subscriber");

    auto subscription =
        node->create_subscription<std_msgs::msg::String>(
            "week1_topic",
            10,
            [node](const std_msgs::msg::String::SharedPtr message) {
                RCLCPP_INFO(
                    node->get_logger(),
                    "Received: %s",
                    message->data.c_str());
            });

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}