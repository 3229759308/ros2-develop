# ROS2-develop

RoboMaster-南京理工大学Alliance战队2026赛季培训使用

本质上是RMCS项目 (https://github.com/Alliance-Algorithm/RMCS) 去除了全部的ros2包，提供一个docker启动的ROS2开发环境，供2026赛季算法组培训成员作业使用

## Week 1 Task 2 - Foxglove Visualization

使用 `foxglove_bridge` 完成了 `week1_task2_signal` 的可视化验收。Bridge 默认监听 `8765` 端口，Windows 本机 Foxglove 通过 `ws://localhost:8765` 成功连接。

运行时确认存在以下 ROS2 Topic：

- `/sin_o`
- `/rec_o`
- `/processor_signal`

Foxglove Plot 面板最终显示的数据系列为 `/processor_signal.data`，消息类型为 `std_msgs::msg::Float32`。处理后的波形能够持续稳定显示，且符合预期逻辑：`sin_o` 与 `rec_o` 同号时保留原始正弦值，异号时输出 `0`。

输入正弦波频率为 10 Hz，方波频率为 1 Hz，方波每 0.5 s 翻转一次。因此，一个方波半周期内约有 5 个完整正弦周期。实际观察到正方波期间保留正半周、负方波期间保留负半周，波形周期性重复，与该周期关系一致。

## Week 1 Task 3 - Morphology

已完成支持可替换 `StructuringElement` 的二值图像形态学项目，包括 output-driven / input-driven dilation、output-driven erosion、自定义 mask 与 anchor、手写测试，以及 Qt6 Widgets 可视化。

[详细算法说明与运行方式](cpp_libs/morphology/README.md)
