#pragma once

#include <cstddef>
#include <vector>

class StructuringElement
{
public:
    explicit StructuringElement(int size);
    StructuringElement(
        const std::vector<std::vector<bool>>& mask,
        int anchor_x,
        int anchor_y);

    std::size_t getWidth() const;
    std::size_t getHeight() const;
    std::size_t getAnchorX() const;
    std::size_t getAnchorY() const;
    bool isActive(int x, int y) const;

private:
    std::size_t width_;
    std::size_t height_;
    std::size_t anchor_x_;
    std::size_t anchor_y_;
    std::vector<std::vector<bool>> mask_;
};
