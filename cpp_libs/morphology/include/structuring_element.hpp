#pragma once

#include <cstddef>
#include <vector>

class StructuringElement
{
public:
    explicit StructuringElement(int size);

    std::size_t getSize() const;
    std::size_t getCenter() const;

    bool isActive(int x, int y) const;

private:
    std::size_t size_;
    std::vector<std::vector<bool>> mask_;
};
