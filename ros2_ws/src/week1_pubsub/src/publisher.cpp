#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>("week1_publisher");

    auto publisher =
        node->create_publisher<std_msgs::msg::String>("week1_topic", 10);

    auto timer = node->create_wall_timer(
        1s,
        [publisher]() {
            std_msgs::msg::String message;
            message.data = "Hello ROS2";

            publisher->publish(message);
        });

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}