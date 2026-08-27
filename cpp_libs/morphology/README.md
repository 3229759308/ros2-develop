# Morphology

当前模块提供独立于 ROS2 的 C++ 二值图像形态学实现。本文记录当前 dilation V2
所使用的坐标约定和 output-driven（逐输出像素反查）算法。

## 坐标定义

- `input`：输入二值图像。输入前景像素的位置记为 `p = (p_x, p_y)`。
- `output`：与输入图像尺寸相同的输出图像。当前正在判断的输出位置记为
  `q = (q_x, q_y)`，在代码中对应传给 `shouldBeForeground()` 的 `x`、`y`。
- `mask`：`StructuringElement` 内部的二维布尔矩形。mask 坐标记为
  `m = (mask_x, mask_y)`；`isActive(mask_x, mask_y)` 表示该位置是否参与膨胀。
- `anchor`：结构元素与图像像素对齐的参考位置，记为
  `a = (anchor_x, anchor_y)`，对应 `getAnchorX()` 和 `getAnchorY()`。

anchor 只定义对齐原点，其所在的 mask 单元不要求为 active。mask 可以是矩形或
非对称形状，anchor 可以是其范围内的任意合法位置。

## Dilation 坐标关系

mask 位置相对 anchor 的 offset 向量定义为：

```text
d = m - a
dx = mask_x - anchor_x
dy = mask_y - anchor_y
```

从输入前景像素出发看，正向膨胀关系是：

```text
q = p + d
```

即输入位置 `p` 通过 active mask 位置对应的 offset `d`，在输出位置 `q` 产生前景。

当前 `morphology::dilate()` 不是逐输入前景像素向输出“盖章”，而是逐个判断输出
像素 `q`。因此 `shouldBeForeground()` 需要反推出可能产生当前输出的输入位置：

```text
p = q - d
input_x = output_x - dx
input_y = output_y - dy
```

在当前代码中，`output_x`、`output_y` 是 `shouldBeForeground()` 的 `x`、`y`，
反查得到的输入坐标保存在 `nx`、`ny` 中：

```cpp
const int dx = mask_x - anchor_x;
const int dy = mask_y - anchor_y;
const int nx = x - dx;
const int ny = y - dy;
```

减号是非对称结构元素方向正确的关键。对称 mask 即使误用相反方向，镜像后的形状
也可能不变，因此不能只依靠对称结构元素验证坐标关系。

## `shouldBeForeground()` 执行流程

对于输出位置 `(x, y)`，当前实现按以下顺序判断：

1. 通过 `element.getWidth()`、`getHeight()`、`getAnchorX()` 和
   `getAnchorY()` 取得结构元素尺寸及 anchor。
2. 使用 `mask_y`、`mask_x` 遍历整个矩形 mask。
3. 若 `element.isActive(mask_x, mask_y)` 为 false，跳过该 mask 位置。
4. 对 active 位置计算 `dx = mask_x - anchor_x` 和
   `dy = mask_y - anchor_y`。
5. 使用 `nx = x - dx`、`ny = y - dy` 反查对应的 input 坐标。
6. 先调用 `input.isInside(nx, ny)` 检查边界。越界位置被忽略，等价于图像外部
   恒为背景 0，不使用异常处理正常的边界扫描。
7. 如果任意一个合法的对应输入位置满足 `input.getPixel(nx, ny) == true`，立即
   early return `true`，当前输出像素被设为前景。
8. 所有 active mask 位置均未找到输入前景时返回 `false`，输出像素保持背景 0。

该流程只通过 `StructuringElement` 的只读接口访问结构元素，支持任意合法的矩形
mask、非对称 mask 和 anchor。

## 复杂度

设图像宽高为 `W`、`H`，结构元素宽高为 `M_w`、`M_h`。最坏情况下，每个输出
像素都要扫描完整 mask，因此时间复杂度为：

```text
O(W * H * M_w * M_h)
```

输出图像本身占用 `O(W * H)` 空间；除输出图像外，算法只使用常量级辅助状态。

实际运行时，如果某个 active mask 位置很早就对应到输入前景，early return 会
停止当前输出像素剩余的 mask 扫描。因此前景分布、active mask 的分布及遍历顺序
都会影响实际检查次数，但不会改变上述最坏时间复杂度。

## 后续：input-driven 盖章版

后续计划另写一份 input-driven（逐输入前景像素“盖章”）算法说明。该版本将从每个
输入前景位置 `p` 出发，对所有 active offset 直接使用正向关系：

```text
q = p + d
```

当前阶段仅记录这一方向，不实现 input-driven 版本，也不改变现有 output-driven
实现。
