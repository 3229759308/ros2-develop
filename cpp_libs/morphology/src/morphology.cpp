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
    const int element_width = static_cast<int>(element.getWidth());
    const int element_height = static_cast<int>(element.getHeight());
    const int anchor_x = static_cast<int>(element.getAnchorX());
    const int anchor_y = static_cast<int>(element.getAnchorY());

    for (int mask_y = 0; mask_y < element_height; ++mask_y)
    {
        for (int mask_x = 0; mask_x < element_width; ++mask_x)
        {
            if (!element.isActive(mask_x, mask_y))
            {
                continue;
            }

            const int dx = mask_x - anchor_x;
            const int dy = mask_y - anchor_y;
            const int nx = x - dx;
            const int ny = y - dy;

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
