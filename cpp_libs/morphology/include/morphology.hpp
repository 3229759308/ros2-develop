#pragma once

#include "image.hpp"
#include "structuring_element.hpp"

namespace morphology
{
    Image dilate(
        const Image& input,
        const StructuringElement& element
    );
}
