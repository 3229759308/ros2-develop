#include "structuring_element.hpp"

#include <stdexcept>

StructuringElement::StructuringElement(int size)
{
    if (size <= 0)
    {
        throw std::invalid_argument("Size must be positive");
    }
    if (size % 2 == 0)
    {
        throw std::invalid_argument("Size must be odd");
    }
    size_ = static_cast<std::size_t>(size);
    mask_.resize(size_, std::vector<bool>(size_, true));
}

std::size_t StructuringElement::getSize() const
{
    return size_;
}

std::size_t StructuringElement::getCenter() const
{
    return size_ / 2;
}

bool StructuringElement::isActive(int x, int y) const
{
    if (x < 0 || x >= static_cast<int>(size_) || y < 0 || y >= static_cast<int>(size_))
    {
        throw std::out_of_range("Coordinates are out of bounds");
    }
    return mask_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}
