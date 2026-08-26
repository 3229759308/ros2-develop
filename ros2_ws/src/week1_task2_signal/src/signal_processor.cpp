#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/int32.hpp"

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>("week1_task2_signal_subscriber");

    float sin_value = 0.0f;
    int rec_value = 0;

    auto processor_publisher =
        node->create_publisher<std_msgs::msg::Float32>("processor_signal", 10);

    auto rec_subscription =
        node->create_subscription<std_msgs::msg::Int32>(
            "rec_o",
            10,
            [&rec_value](const std_msgs::msg::Int32 & message) {
        rec_value = message.data;
    });
    
    auto sin_subscription =
        node->create_subscription<std_msgs::msg::Float32>(
            "sin_o",
            10,
            [&sin_value, &rec_value, processor_publisher](const std_msgs::msg::Float32 & message) {
        sin_value = message.data;
        std_msgs::msg::Float32 result;
        if(sin_value * static_cast<float>(rec_value) > 0)
        {
            result.data = sin_value;
        }
        else
        {
            result.data = 0.0f;
        }
        processor_publisher->publish(result);
    });

    

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}