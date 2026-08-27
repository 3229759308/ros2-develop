#pragma once

#include <cstddef>
#include <vector>

class Image
{
public:
    Image(int width, int height);
    std::size_t getWidth() const;
    std::size_t getHeight() const;
    bool isInside(int x, int y) const;
    bool getPixel(int x, int y) const;
    void setPixel(int x, int y, int value);
    void print() const;

private:
    std::size_t width_;
    std::size_t height_;
    std::vector<std::vector<int>> data_;
};
