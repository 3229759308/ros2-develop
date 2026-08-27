#include "image.hpp"
#include "morphology.hpp"
#include "structuring_element.hpp"

#include <iostream>

int main()
{
    Image input(7, 7);

    // 在图像正中心放一个前景点
    input.setPixel(3, 3, 1);

    StructuringElement element(5);

    std::cout << "Input image:\n";
    input.print();

    Image output = morphology::dilate(input, element);

    std::cout << "\nDilated image:\n";
    output.print();

    return 0;
}
