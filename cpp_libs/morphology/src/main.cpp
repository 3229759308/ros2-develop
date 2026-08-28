#include "image.hpp"
#include "morphology.hpp"
#include "structuring_element.hpp"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void requirePixel(const Image& image, int x, int y, bool expected, const std::string& test_name)
{
    require(image.getPixel(x, y) == expected, test_name + " produced an unexpected pixel");
}

void expectInvalidArgument(const std::function<void()>& operation, const std::string& test_name)
{
    try
    {
        operation();
    }
    catch (const std::invalid_argument&)
    {
        return;
    }
    throw std::runtime_error(test_name + " did not reject invalid input");
}

void testDilationImplementations(
    const Image& input,
    const StructuringElement& element,
    const std::string& test_name,
    const std::function<void(const Image&, const std::string&)>& verify)
{
    verify(morphology::dilateOutputDriven(input, element), test_name + " (output-driven)");
    verify(morphology::dilateInputDriven(input, element), test_name + " (input-driven)");
}

void testV1Regression()
{
    Image input(7, 7);
    input.setPixel(3, 3, 1);
    const StructuringElement element(5);
    testDilationImplementations(
        input, element, "V1 regression", [](const Image& output, const std::string& test_name) {
        for (int y = 0; y < 7; ++y)
        {
            for (int x = 0; x < 7; ++x)
            {
                const bool expected = x >= 1 && x <= 5 && y >= 1 && y <= 5;
                requirePixel(output, x, y, expected, test_name);
            }
        }
    });
}

void testCustomMask()
{
    const std::vector<std::vector<bool>> mask{
        {false, true, true, true, false},
        {true, true, true, true, true},
        {true, true, true, true, true},
        {true, true, true, true, true},
        {false, true, true, true, false}};
    const StructuringElement element(mask, 2, 2);
    Image input(7, 7);
    input.setPixel(3, 3, 1);
    testDilationImplementations(
        input, element, "Custom mask", [](const Image& output, const std::string& test_name) {
        requirePixel(output, 1, 1, false, test_name);
        requirePixel(output, 5, 1, false, test_name);
        requirePixel(output, 1, 5, false, test_name);
        requirePixel(output, 5, 5, false, test_name);
        requirePixel(output, 3, 1, true, test_name);
        requirePixel(output, 1, 3, true, test_name);
    });

    const StructuringElement inactive_anchor({{true, false}}, 1, 0);
    require(!inactive_anchor.isActive(1, 0), "Inactive anchor was not preserved");
}

void testRectangularMask()
{
    const std::vector<std::vector<bool>> mask(3, std::vector<bool>(4, true));
    const StructuringElement element(mask, 1, 1);
    Image input(7, 7);
    input.setPixel(3, 3, 1);

    require(element.getWidth() == 4 && element.getHeight() == 3, "Rectangular dimensions failed");
    testDilationImplementations(
        input, element, "Rectangular mask", [](const Image& output, const std::string& test_name) {
        for (int y = 0; y < 7; ++y)
        {
            for (int x = 0; x < 7; ++x)
            {
                const bool expected = x >= 2 && x <= 5 && y >= 2 && y <= 4;
                requirePixel(output, x, y, expected, test_name);
            }
        }
    });
}

void testAsymmetricMaskDirection()
{
    const StructuringElement element({{true, true, true}}, 0, 0);
    Image input(7, 7);
    input.setPixel(3, 3, 1);
    testDilationImplementations(
        input, element, "Asymmetric mask direction", [](const Image& output, const std::string& test_name) {
        for (int y = 0; y < 7; ++y)
        {
            for (int x = 0; x < 7; ++x)
            {
                const bool expected = y == 3 && x >= 3 && x <= 5;
                requirePixel(output, x, y, expected, test_name);
            }
        }
    });
}

void testBasicSquareErosion()
{
    Image input(7, 7);
    for (int y = 1; y <= 5; ++y)
    {
        for (int x = 1; x <= 5; ++x)
        {
            input.setPixel(x, y, 1);
        }
    }
    const StructuringElement element(3);
    const Image output = morphology::erodeOutputDriven(input, element);

    for (int y = 0; y < 7; ++y)
    {
        for (int x = 0; x < 7; ++x)
        {
            const bool expected = x >= 2 && x <= 4 && y >= 2 && y <= 4;
            requirePixel(output, x, y, expected, "Basic square erosion");
        }
    }
}

void testErosionBoundaryBehavior()
{
    Image input(5, 5);
    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            input.setPixel(x, y, 1);
        }
    }
    const StructuringElement element(3);
    const Image output = morphology::erodeOutputDriven(input, element);

    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            const bool expected = x >= 1 && x <= 3 && y >= 1 && y <= 3;
            requirePixel(output, x, y, expected, "Erosion boundary behavior");
        }
    }
}

void testAsymmetricErosionDirection()
{
    Image input(7, 7);
    input.setPixel(3, 3, 1);
    input.setPixel(4, 3, 1);
    input.setPixel(5, 3, 1);
    const StructuringElement element({{true, true, true}}, 0, 0);
    const Image output = morphology::erodeOutputDriven(input, element);

    for (int y = 0; y < 7; ++y)
    {
        for (int x = 0; x < 7; ++x)
        {
            const bool expected = x == 3 && y == 3;
            requirePixel(output, x, y, expected, "Asymmetric erosion direction");
        }
    }
}

void testInactiveErosionMaskUnit()
{
    Image input(7, 3);
    input.setPixel(2, 1, 1);
    input.setPixel(4, 1, 1);
    const StructuringElement element({{true, false, true}}, 1, 0);
    const Image output = morphology::erodeOutputDriven(input, element);

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 7; ++x)
        {
            const bool expected = x == 3 && y == 1;
            requirePixel(output, x, y, expected, "Inactive erosion mask unit");
        }
    }
}

void testInvalidInputs()
{
    expectInvalidArgument([] { StructuringElement({}, 0, 0); }, "Empty mask");
    expectInvalidArgument(
        [] { StructuringElement({{true}, {}}, 0, 0); }, "Empty mask row");
    expectInvalidArgument(
        [] { StructuringElement({{true}, {true, false}}, 0, 0); }, "Unequal row widths");
    expectInvalidArgument(
        [] { StructuringElement({{false, false}, {false, false}}, 0, 0); }, "Inactive mask");
    expectInvalidArgument(
        [] { StructuringElement({{true, true}}, 2, 0); }, "Anchor x out of bounds");
    expectInvalidArgument(
        [] { StructuringElement({{true}, {true}}, 0, 2); }, "Anchor y out of bounds");
}

}  // namespace

int main()
{
    testV1Regression();
    testCustomMask();
    testRectangularMask();
    testAsymmetricMaskDirection();
    testBasicSquareErosion();
    testErosionBoundaryBehavior();
    testAsymmetricErosionDirection();
    testInactiveErosionMaskUnit();
    testInvalidInputs();

    std::cout << "All morphology V2 tests passed.\n";
    return 0;
}
