#include <chrono>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>("week1_task2_signal_generator");

    auto sin_publisher =
        node->create_publisher<std_msgs::msg::Float32>("sin_o", 10);

    auto rec_publisher =
        node->create_publisher<std_msgs::msg::Int32>("rec_o", 10);
    
    auto start_time = std::chrono::steady_clock::now();

    auto timer = node->create_wall_timer(
        1ms,
        [sin_publisher, rec_publisher,start_time]() {
            std_msgs::msg::Float32 message_sin;
            std_msgs::msg::Int32 message_rec;
            
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_time = 
            std::chrono::duration_cast<std::chrono::duration<double>>(
                current_time - start_time
                ).count();
            
            message_sin.data = static_cast<float>(std::sin(20 * M_PI * elapsed_time));
            if(static_cast<int>(elapsed_time*1000) % 1000 < 500)
            {
                message_rec.data = 1;
            }
            else
            {
                message_rec.data = -1;
            }
            sin_publisher->publish(message_sin);
            rec_publisher->publish(message_rec);

        });
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
