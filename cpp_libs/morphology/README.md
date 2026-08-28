# Morphology

当前模块提供独立于 ROS2 的 C++ 二值图像形态学实现。当前 dilation V2 同时提供
output-driven（逐输出像素反查）和 input-driven（逐输入前景像素“盖章”）实现，
本文记录两者共用的坐标约定和算法关系。

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

`morphology::dilateOutputDriven()` 不是逐输入前景像素向输出“盖章”，而是逐个判断
输出像素 `q`。因此 `shouldBeForeground()` 需要反推出可能产生当前输出的输入位置：

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

## Output-driven 复杂度

设图像宽高为 `W`、`H`，结构元素宽高为 `M_w`、`M_h`。最坏情况下，每个输出
像素都要扫描完整 mask，因此时间复杂度为：

```text
O(W * H * M_w * M_h)
```

输出图像本身占用 `O(W * H)` 空间；除输出图像外，算法只使用常量级辅助状态。

实际运行时，如果某个 active mask 位置很早就对应到输入前景，early return 会
停止当前输出像素剩余的 mask 扫描。因此前景分布、active mask 的分布及遍历顺序
都会影响实际检查次数，但不会改变上述最坏时间复杂度。

## Input-driven 盖章版

`morphology::dilateInputDriven()` 当前已实现 input-driven（逐输入前景像素
“盖章”）算法。核心流程如下：

1. 遍历 input，寻找前景像素 `p`。
2. 只有 `p` 为前景时才遍历 `StructuringElement`。
3. 只处理 active mask 单元，并将 anchor 对准 `p`。
4. 根据坐标关系计算 output 位置，若未越界则写为前景。

对于当前 active mask 坐标，先计算它相对 anchor 的 offset：

```text
d = m - a
```

再从 input 前景点直接计算最终写入位置：

```text
q = p + d
q = p + (m - a)
```

其中：

- `p` 是 input 前景点。
- `a` 是结构元素 anchor。
- `m` 是当前 active mask 坐标。
- `q` 是最终写入的 output 坐标。

两种实现使用相同的 dilation 坐标关系，但遍历方向不同：output-driven 固定 `q`，
反查 `p`；input-driven 固定 `p`，直接计算 `q`。稀疏前景时 input-driven 通常更有
优势；密集前景时 output-driven 可能受益于 early return。

## Erosion

`morphology::erodeOutputDriven()` 使用 output-driven 方式实现腐蚀。对于当前正在
判断的 output 点 `q`，将结构元素的 anchor 放到 `q`，再检查所有 active mask
单元对应的 input 位置。只要发现一个位置为背景或越界，当前 output 点就是背景；
只有所有 active 位置都为前景时，当前 output 点才为前景。inactive mask 单元不
参与判断。

mask 位置相对 anchor 的 offset 以及需要检查的 input 坐标为：

```text
d = m - a
p = q + d
p = q + (m - a)
```

其中：

- `q` 是当前正在判断的 output 点。
- `a` 是结构元素 anchor。
- `m` 是当前 active mask 坐标。
- `p` 是需要检查的 input 坐标。

内部 `shouldBeForegroundAfterErosion()` 遍历 active mask 单元。发现对应的 `p`
越界或为背景时立即 early return `false`；所有 active 单元都通过后才返回 `true`。
图像外部按背景处理，因此结构元素延伸到图像外时不会错误保留边界前景。

erosion 会让前景区域收缩。结构元素越大，通常腐蚀越明显；更换 mask 形状会改变
腐蚀后的边缘形状。与 dilation 相比，dilation 满足任意一个覆盖条件即可成为前景，
而 erosion 必须满足所有 active 条件才保留前景。
