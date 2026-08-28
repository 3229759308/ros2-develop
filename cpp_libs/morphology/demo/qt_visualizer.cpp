#include "image.hpp"
#include "morphology.hpp"
#include "structuring_element.hpp"

#include <QApplication>
#include <QColor>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <vector>

namespace
{

constexpr int image_cell_size = 20;
constexpr int element_cell_size = 48;

class ImageGridWidget : public QWidget
{
public:
    explicit ImageGridWidget(const Image& image, QWidget* parent = nullptr)
        : QWidget(parent), image_(image)
    {
        setFixedSize(
            static_cast<int>(image_.getWidth()) * image_cell_size,
            static_cast<int>(image_.getHeight()) * image_cell_size);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        const QPen grid_pen(QColor(205, 211, 219));

        for (int y = 0; y < static_cast<int>(image_.getHeight()); ++y)
        {
            for (int x = 0; x < static_cast<int>(image_.getWidth()); ++x)
            {
                const QRect cell(
                    x * image_cell_size,
                    y * image_cell_size,
                    image_cell_size,
                    image_cell_size);
                painter.fillRect(cell, image_.getPixel(x, y) ? QColor(42, 92, 140) : Qt::white);
                painter.setPen(grid_pen);
                painter.drawRect(cell.adjusted(0, 0, -1, -1));
            }
        }
    }

private:
    Image image_;
};

class StructuringElementWidget : public QWidget
{
public:
    explicit StructuringElementWidget(
        const StructuringElement& element,
        QWidget* parent = nullptr)
        : QWidget(parent), element_(element)
    {
        setFixedSize(
            static_cast<int>(element_.getWidth()) * element_cell_size,
            static_cast<int>(element_.getHeight()) * element_cell_size);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        const QPen grid_pen(QColor(180, 186, 194));

        for (int y = 0; y < static_cast<int>(element_.getHeight()); ++y)
        {
            for (int x = 0; x < static_cast<int>(element_.getWidth()); ++x)
            {
                const QRect cell(
                    x * element_cell_size,
                    y * element_cell_size,
                    element_cell_size,
                    element_cell_size);
                const bool active = element_.isActive(x, y);
                painter.fillRect(cell, active ? QColor(72, 156, 132) : QColor(245, 247, 249));
                painter.setBrush(Qt::NoBrush);
                painter.setPen(grid_pen);
                painter.drawRect(cell.adjusted(0, 0, -1, -1));

                if (x == static_cast<int>(element_.getAnchorX())
                    && y == static_cast<int>(element_.getAnchorY()))
                {
                    painter.save();
                    painter.setPen(QPen(QColor(210, 45, 45), 3));
                    painter.drawRect(cell.adjusted(2, 2, -3, -3));
                    painter.setBrush(QColor(210, 45, 45));
                    painter.setPen(Qt::NoPen);
                    painter.drawEllipse(cell.center(), 4, 4);
                    painter.restore();
                }
            }
        }
    }

private:
    StructuringElement element_;
};

Image createInputImage()
{
    Image input(16, 12);

    for (int y = 2; y <= 9; ++y)
    {
        for (int x = 2; x <= 7; ++x)
        {
            if (x <= 3 || y <= 3 || y >= 8)
            {
                input.setPixel(x, y, 1);
            }
        }
    }

    for (int y = 4; y <= 7; ++y)
    {
        for (int x = 10; x <= 13; ++x)
        {
            input.setPixel(x, y, 1);
        }
    }

    return input;
}

QGroupBox* createPanel(const QString& title, QWidget* content)
{
    auto* panel = new QGroupBox(title);
    auto* layout = new QVBoxLayout(panel);
    layout->addWidget(content, 0, Qt::AlignCenter);
    return panel;
}

class VisualizerWindow : public QWidget
{
public:
    VisualizerWindow()
        : input_(createInputImage()),
          element_(
              {{false, true, false}, {true, true, true}, {false, true, false}},
              1,
              1),
          dilation_(morphology::dilateOutputDriven(input_, element_)),
          erosion_(morphology::erodeOutputDriven(input_, element_))
    {
        setWindowTitle("Morphology Visualizer");

        auto* layout = new QGridLayout(this);
        layout->addWidget(createPanel("Input Image", new ImageGridWidget(input_)), 0, 0);
        layout->addWidget(
            createPanel("Structuring Element", new StructuringElementWidget(element_)), 0, 1);
        layout->addWidget(createPanel("Dilation Output", new ImageGridWidget(dilation_)), 1, 0);
        layout->addWidget(createPanel("Erosion Output", new ImageGridWidget(erosion_)), 1, 1);

        const QString information = QString(
            "Image Size: %1 x %2 | Structuring Element Size: %3 x %4 | Anchor: (%5, %6)")
                                        .arg(input_.getWidth())
                                        .arg(input_.getHeight())
                                        .arg(element_.getWidth())
                                        .arg(element_.getHeight())
                                        .arg(element_.getAnchorX())
                                        .arg(element_.getAnchorY());
        auto* information_label = new QLabel(information);
        information_label->setAlignment(Qt::AlignCenter);
        layout->addWidget(information_label, 2, 0, 1, 2);
    }

private:
    Image input_;
    StructuringElement element_;
    Image dilation_;
    Image erosion_;
};

}  // namespace

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    VisualizerWindow window;
    window.show();
    return application.exec();
}
