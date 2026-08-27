#include "image.hpp"

#include <iostream>
#include <stdexcept>

Image::Image(int width, int height)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Width and height must be positive integers.");
    }
    width_ = static_cast<std::size_t>(width);
    height_ = static_cast<std::size_t>(height);
    data_.resize(height_, std::vector<int>(width_, 0));
}

std::size_t Image::getWidth() const
{
    return width_;
}

std::size_t Image::getHeight() const
{
    return height_;
}

bool Image::isInside(int x, int y) const
{
    return x >= 0 && x < static_cast<int>(width_) && y >= 0 && y < static_cast<int>(height_);
}

bool Image::getPixel(int x, int y) const
{
    if (!isInside(x, y))
    {
        throw std::out_of_range("Pixel coordinates are out of bounds.");
    }
    return data_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

void Image::setPixel(int x, int y, int value)
{
    if (!isInside(x, y))
    {
        throw std::out_of_range("Pixel coordinates are out of bounds.");
    }
    if (value != 0 && value != 1)
    {
        throw std::invalid_argument("Pixel value must be either 0 or 1.");
    }
    data_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = value;
}

void Image::print() const
{
    for (const auto& row : data_)
    {
        for (const auto& pixel : row)
        {
            std::cout << pixel << " ";
        }
        std::cout << std::endl;
    }
}
