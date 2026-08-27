#include "structuring_element.hpp"

#include <stdexcept>

StructuringElement::StructuringElement(int size)
{
    if (size <= 0)
    {
        throw std::invalid_argument("Size must be positive");
    }

    width_ = static_cast<std::size_t>(size);
    height_ = static_cast<std::size_t>(size);
    anchor_x_ = width_ / 2;
    anchor_y_ = height_ / 2;
    mask_.resize(height_, std::vector<bool>(width_, true));
}

StructuringElement::StructuringElement(
    const std::vector<std::vector<bool>>& mask,
    int anchor_x,
    int anchor_y)
{
    if (mask.empty())
    {
        throw std::invalid_argument("Mask must not be empty");
    }
    if (mask.front().empty())
    {
        throw std::invalid_argument("Mask rows must not be empty");
    }

    const std::size_t width = mask.front().size();
    bool has_active_cell = false;
    for (const auto& row : mask)
    {
        if (row.empty())
        {
            throw std::invalid_argument("Mask rows must not be empty");
        }
        if (row.size() != width)
        {
            throw std::invalid_argument("Mask rows must have equal width");
        }
        for (const bool cell : row)
        {
            has_active_cell = has_active_cell || cell;
        }
    }
    if (!has_active_cell)
    {
        throw std::invalid_argument("Mask must contain at least one active cell");
    }

    const std::size_t height = mask.size();
    if (anchor_x < 0 || anchor_x >= static_cast<int>(width))
    {
        throw std::invalid_argument("Anchor x-coordinate is out of bounds");
    }
    if (anchor_y < 0 || anchor_y >= static_cast<int>(height))
    {
        throw std::invalid_argument("Anchor y-coordinate is out of bounds");
    }

    width_ = width;
    height_ = height;
    anchor_x_ = static_cast<std::size_t>(anchor_x);
    anchor_y_ = static_cast<std::size_t>(anchor_y);
    mask_ = mask;
}

std::size_t StructuringElement::getWidth() const
{
    return width_;
}

std::size_t StructuringElement::getHeight() const
{
    return height_;
}

std::size_t StructuringElement::getAnchorX() const
{
    return anchor_x_;
}

std::size_t StructuringElement::getAnchorY() const
{
    return anchor_y_;
}

bool StructuringElement::isActive(int x, int y) const
{
    if (x < 0 || x >= static_cast<int>(width_) || y < 0 || y >= static_cast<int>(height_))
    {
        throw std::out_of_range("Coordinates are out of bounds");
    }
    return mask_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}
