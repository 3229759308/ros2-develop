#pragma once

#include "image.hpp"
#include "structuring_element.hpp"

namespace morphology
{
    Image dilateOutputDriven(
        const Image& input,
        const StructuringElement& element
    );

    Image dilateInputDriven(
        const Image& input,
        const StructuringElement& element
    );

    Image erodeOutputDriven(
        const Image& input,
        const StructuringElement& element
    );
}
