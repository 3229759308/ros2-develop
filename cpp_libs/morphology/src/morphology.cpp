#include "morphology.hpp"
#include <cstddef>

namespace
{

// Helper function to determine whether a pixel
// should become foreground after dilation.
bool shouldBeForeground(
    const Image& input,
    const StructuringElement& element,
    int x,
    int y)
{
    const int center = static_cast<int>(element.getCenter());

    for (int dy = -center; dy <= center; ++dy)
    {
        for (int dx = -center; dx <= center; ++dx)
        {
            const int mask_x = dx + center;
            const int mask_y = dy + center;

            if (!element.isActive(mask_x, mask_y))
            {
                continue;
            }

            const int nx = x + dx;
            const int ny = y + dy;

            if (input.isInside(nx, ny) && input.getPixel(nx, ny))
            {
                return true;
            }
        }
    }

    return false;
}

}  // namespace

Image morphology::dilate(
    const Image& input,
    const StructuringElement& element)
{
    const std::size_t width = input.getWidth();
    const std::size_t height = input.getHeight();
    Image output(static_cast<int>(width), static_cast<int>(height));
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            if (shouldBeForeground(input, element, static_cast<int>(x), static_cast<int>(y)))
            {
                output.setPixel(static_cast<int>(x), static_cast<int>(y), 1);
            }
        }
    }
    return output;
}
