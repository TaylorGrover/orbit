#include <SettingsDialog.h>

SettingsDialog::SettingsDialog(ParameterManager& paramManager) : paramManager(paramManager)
{
    // Initialize color palette

    QGridLayout* settingsLayout = new QGridLayout(this);

    QWidget* paramWidget = new QWidget;
    QFormLayout* paramLayout = new QFormLayout(paramWidget);

    countSpin = new QSpinBox;
    countSpin->setRange(1, 1 << 9);
    countSpin->setValue(1 << 4);

    // Start and Close buttons
    QPushButton* closeButton = new QPushButton;
    closeButton->setText("Close");
    QPushButton* startButton = new QPushButton;
    startButton->setText("Start Simulation");
    connect(closeButton, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(startButton, &QPushButton::clicked, this, &SettingsDialog::setParameters);
    QWidget* lowerWidget = new QWidget;
    QVBoxLayout* lowerLayout = new QVBoxLayout(lowerWidget);
    QWidget* startCloseWidget = new QWidget;
    QHBoxLayout* startCloseLayout = new QHBoxLayout(startCloseWidget);
    startCloseLayout->addWidget(closeButton);
    startCloseLayout->addWidget(startButton);

    gravConstantSpin = new QDoubleSpinBox;
    gravConstantSpin->setRange(-100, 100);
    gravConstantSpin->setValue(6.674f);

    // Standard deviation of normally distributed locations
    initialLocationSpin = new QDoubleSpinBox;
    initialLocationSpin->setRange(0, 1000);
    initialLocationSpin->setValue(200);

    // "                                        " velocities
    initialVelocitySpin = new QDoubleSpinBox;
    initialVelocitySpin->setRange(0, 100);
    initialVelocitySpin->setValue(2.5f);

    // Uniform distribution of sphere radii
    QWidget* radiiWidget = new QWidget;
    QHBoxLayout* radiiLayout = new QHBoxLayout(radiiWidget);
    radiiLower = new QDoubleSpinBox;
    radiiUpper = new QDoubleSpinBox;
    float lowerBound = 1.0, upperBound = 50.0;
    radiiLower->setRange(lowerBound, upperBound - 1);
    radiiLower->setValue(1.0);
    radiiUpper->setRange(lowerBound + 1, upperBound);
    radiiUpper->setValue(5.0);
    radiiLayout->addWidget(radiiLower);
    radiiLayout->addWidget(radiiUpper);
    connect(radiiLower, SIGNAL(valueChanged(double)), this, SLOT(updateRadiiUpper()));
    connect(radiiUpper, SIGNAL(valueChanged(double)), this, SLOT(updateRadiiLower()));

    // Light percentage
    lightFractionSpin = new QDoubleSpinBox;
    lightFractionSpin->setRange(0, 1);
    lightFractionSpin->setValue(.1);
    lightFractionSpin->setSingleStep(.05);

    // Color picker button
    colorSelectButton = new QPushButton;
    connect(colorSelectButton, &QPushButton::clicked, this, &SettingsDialog::selectColorPalette);

    // Random seed
    randomSeedSpin = new QSpinBox;
    randomSeedSpin->setRange(0, INT_MAX);
    randomSeedSpin->setValue(23);

    paramLayout->addRow("Number of spheres: ", countSpin);
    paramLayout->addRow("Gravitational constant (G): ", gravConstantSpin);
    paramLayout->addRow("Standard dev. of initial |distance|: ", initialLocationSpin);
    paramLayout->addRow("Standard dev. of initial |velocity|: ", initialVelocitySpin);
    paramLayout->addRow("Uniform distribution of radii", radiiWidget);
    paramLayout->addRow("Light fraction spin: ", lightFractionSpin);
    paramLayout->addRow("Color palette: ", colorSelectButton);
    paramLayout->addRow("Seed: ", randomSeedSpin);

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
        QString s("background: #"
          + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16)
          + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16)
          + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";");
        colorSelectButton->setStyleSheet(s);
        colorSelectButton->update();
    }
}

/**
 * Slot to ensure that radiiUpper value is > radiiLower value
 */
void SettingsDialog::updateRadiiUpper()
{
    if(radiiUpper->value() <= radiiLower->value()) {
        radiiUpper->setValue(radiiLower->value() + 1);
    }
}

/**
 * Slot does rougly the opposite of the above
 */
void SettingsDialog::updateRadiiLower()
{
    if(radiiUpper->value() <= radiiLower->value()) {
        radiiLower->setValue(radiiLower->value() - 1);
    }
}

/**
 * Slot to run on select
 * @brief SettingsDialog::setParameters
 * Set all parameters
 */
void SettingsDialog::setParameters()
{
    paramManager.setGravitationalConstant(gravConstantSpin->value());
    paramManager.setLocationSD(initialLocationSpin->value());
    paramManager.setVelocitySD(initialVelocitySpin->value());
    paramManager.setLightFraction(lightFractionSpin->value());
    paramManager.setRadiiLower(radiiLower->value());
    paramManager.setRadiiUpper(radiiUpper->value());
    paramManager.setRandSeed(randomSeedSpin->value());
    paramManager.setAmbientPalette(colorPalette);
    paramManager.setSphereCount(countSpin->value());
    this->accept();
}
