#include <SettingsDialog.h>


SettingsDialog::SettingsDialog()
{
    QGridLayout* settingsLayout = new QGridLayout(this);

    QWidget* paramWidget = new QWidget;
    QFormLayout* paramLayout = new QFormLayout(paramWidget);

    // Start and Close buttons
    QPushButton* closeButton = new QPushButton;
    closeButton->setText("Close");
    QPushButton* startButton = new QPushButton;
    startButton->setText("Start Simulation");
    connect(closeButton, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(startButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    QWidget* lowerWidget = new QWidget;
    QVBoxLayout* lowerLayout = new QVBoxLayout(lowerWidget);
    QWidget* startCloseWidget = new QWidget;
    QHBoxLayout* startCloseLayout = new QHBoxLayout(startCloseWidget);
    startCloseLayout->addWidget(closeButton);
    startCloseLayout->addWidget(startButton);

    gravConstantSpin = new QDoubleSpinBox;
    gravConstantSpin->setValue(6.674f);
    gravConstantSpin->setRange(-100, 100);

    // Standard deviation of normally distributed locations
    initialLocationSpin = new QDoubleSpinBox;
    initialLocationSpin->setValue(55);
    initialLocationSpin->setRange(0, 1000);

    // "                                        " velocities
    initialVelocitySpin = new QDoubleSpinBox;
    initialVelocitySpin->setValue(2.5f);
    initialVelocitySpin->setRange(0, 100);

    // Uniform distribution of sphere radii
    QWidget* radiiWidget = new QWidget;
    QHBoxLayout* radiiLayout = new QHBoxLayout(radiiWidget);
    radiiLower = new QDoubleSpinBox;
    radiiUpper = new QDoubleSpinBox;
    radiiLower->setValue(1.0);
    radiiLower->setRange(1.0, 50.0);
    radiiUpper->setValue(5.0);
    radiiUpper->setRange(1.0, 50.0);
    radiiLayout->addWidget(radiiLower);
    radiiLayout->addWidget(radiiUpper);

    // Light percentage

    // Color picker button
    QPushButton* colorButton = new QPushButton;
    connect(colorButton, &QPushButton::clicked, this, &SettingsDialog::selectColorPalette);
    colorButton->setText("Color palette");

    paramLayout->addRow("Gravitational constant (G): ", gravConstantSpin);
    paramLayout->addRow("Standard dev. of initial |distance|: ", initialLocationSpin);
    paramLayout->addRow("Standard dev. of initial |velocity|: ", initialVelocitySpin);
    paramLayout->addRow("Uniform distribution of radii", radiiWidget);

    lowerLayout->addWidget(colorButton);
    lowerLayout->addWidget(startCloseWidget);

    settingsLayout->addWidget(paramWidget, 0, 0);
    settingsLayout->addWidget(lowerWidget, 1, 0);


    setLayout(settingsLayout);
}

/*void SettingsDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) {
        event->ignore();
    } else {
        QDialog::keyPressEvent(event);
    }
}*/

float SettingsDialog::getGravitationalConstant()
{
    return gravConstantSpin->value();
}

template <typename SpinBox, typename numType>
SpinBox* SettingsDialog::getSpin(QVBoxLayout* vboxLayout, std::string labelText, numType leftRange, numType rightRange, numType initValue)
{
    SpinBox* spin = new SpinBox;
    spin->setValue(initValue);
    spin->setRange(leftRange, rightRange);
    auto* widget = new QWidget;
    auto* hBox = new QHBoxLayout(widget);
    hBox->addWidget(new QLabel(labelText.c_str()));
    hBox->addWidget(spin);
    vboxLayout->addWidget(widget);
    return spin;
}

void SettingsDialog::selectColorPalette()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        colorPalette = color;
    }
}
