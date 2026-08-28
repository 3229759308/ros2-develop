#include "morphology.hpp"
#include <cstddef>

namespace
{

// Helper function to determine whether a pixel
// should become foreground after dilation.
bool shouldBeForegroundAfterDilation(
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
            const int input_x = x - dx;
            const int input_y = y - dy;

            if (input.isInside(input_x, input_y) && input.getPixel(input_x, input_y))
            {
                return true;
            }
        }
    }

    return false;
}

bool shouldBeForegroundAfterErosion(
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
            const int input_x = x + dx;
            const int input_y = y + dy;

            if (!input.isInside(input_x, input_y) || !input.getPixel(input_x, input_y))
            {
                return false;
            }
        }
    }

    return true;
}

void stampStructuringElement(
    const StructuringElement& element,
    Image& output,
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
            const int output_x = x + dx;
            const int output_y = y + dy;

            if (output.isInside(output_x, output_y  ))
            {
                output.setPixel(output_x, output_y, 1);
            }
        }
    }
}
}  // namespace

Image morphology::dilateOutputDriven(
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
            if (shouldBeForegroundAfterDilation(input, element, static_cast<int>(x), static_cast<int>(y)))
            {
                output.setPixel(static_cast<int>(x), static_cast<int>(y), 1);
            }
        }
    }
    return output;
}

Image morphology::erodeOutputDriven(
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
            if (shouldBeForegroundAfterErosion(input, element, static_cast<int>(x), static_cast<int>(y)))
            {
                output.setPixel(static_cast<int>(x), static_cast<int>(y), 1);
            }
        }
    }
    return output;
}

Image morphology::dilateInputDriven(
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
            if (input.getPixel(static_cast<int>(x), static_cast<int>(y)))
            {
                stampStructuringElement(element, output, static_cast<int>(x), static_cast<int>(y));
            }
        }
    }
    return output;
}
