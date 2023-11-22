#include <SettingsDialog.h>

SettingsDialog::SettingsDialog(ParameterManager& paramManager) : paramManager(paramManager)
{
    // Initialize colorPalette
    colorPalette.setRed(255);
    colorPalette.setGreen(255);
    colorPalette.setBlue(255);
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
    gravConstantSpin->setRange(-10000, 10000);
    gravConstantSpin->setValue(6.674e-11);
    gravConstantSpin->setDecimals(14);

    densitySpin = new QDoubleSpinBox;
    densitySpin->setRange(-1000000, 1000000);
    densitySpin->setValue(0.8);
    densitySpin->setSingleStep(.1);

    // Radii scaling for light sources (suns)
    sunRadiusScaleSpin = new QDoubleSpinBox;
    sunRadiusScaleSpin->setRange(0.001, 10000);
    sunRadiusScaleSpin->setValue(4.0);

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
    float lowerBound = 1.0, upperBound = 100.0;
    radiiLower->setRange(lowerBound, upperBound);
    radiiLower->setValue(1.0);
    radiiUpper->setRange(lowerBound, upperBound);
    radiiUpper->setValue(5.0);
    radiiLayout->addWidget(radiiLower);
    radiiLayout->addWidget(radiiUpper);
    connect(radiiLower, SIGNAL(valueChanged(double)), this, SLOT(updateRadiiUpper()));
    connect(radiiUpper, SIGNAL(valueChanged(double)), this, SLOT(updateRadiiLower()));

    // Light percentage
    lightFractionSpin = new QDoubleSpinBox;
    lightFractionSpin->setRange(0, 1);
    lightFractionSpin->setValue(.1);
    lightFractionSpin->setSingleStep(.0001);
    lightFractionSpin->setDecimals(4);

    // Color picker button
    colorSelectButton = new QPushButton;
    connect(colorSelectButton, &QPushButton::clicked, this, &SettingsDialog::selectColorPalette);

    // Random seed
    randomSeedSpin = new QSpinBox;
    randomSeedSpin->setRange(0, INT_MAX);
    randomSeedSpin->setValue(23);

    // Fullscreen checkbox
    fullscreenCheckBox = new QCheckBox;

    paramLayout->addRow("Number of spheres: ", countSpin);
    paramLayout->addRow("Gravitational constant (G): ", gravConstantSpin);
    paramLayout->addRow("Sphere density: ", densitySpin);
    paramLayout->addRow("Sun radius relative scaling: ", sunRadiusScaleSpin);
    paramLayout->addRow("Standard dev. of initial |distance|: ", initialLocationSpin);
    paramLayout->addRow("Standard dev. of initial |velocity|: ", initialVelocitySpin);
    paramLayout->addRow("Uniform distribution of radii", radiiWidget);
    paramLayout->addRow("Light fraction: ", lightFractionSpin);
    paramLayout->addRow("Ambient color: ", colorSelectButton);
    paramLayout->addRow("Seed: ", randomSeedSpin);
    paramLayout->addRow("Fullscreen: ", fullscreenCheckBox);

    lowerLayout->addWidget(startCloseWidget);

    settingsLayout->addWidget(paramWidget, 0, 0);
    settingsLayout->addWidget(lowerWidget, 1, 0);

    setLayout(settingsLayout);
    readSettings();
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
        updateColorSelect();
    }
}

void SettingsDialog::updateColorSelect()
{
    QString s("background: #"
      + QString(colorPalette.red() < 16? "0" : "") + QString::number(colorPalette.red(),16)
      + QString(colorPalette.green() < 16? "0" : "") + QString::number(colorPalette.green(),16)
      + QString(colorPalette.blue() < 16? "0" : "") + QString::number(colorPalette.blue(),16) + ";");
    colorSelectButton->setStyleSheet(s);
    colorSelectButton->update();
}

/**
 * Slot to ensure that radiiUpper value is > radiiLower value
 */
void SettingsDialog::updateRadiiUpper()
{
    if(radiiUpper->value() < radiiLower->value()) {
        radiiUpper->setValue(radiiLower->value());
    }
}

/**
 * Slot does rougly the opposite of the above
 */
void SettingsDialog::updateRadiiLower()
{
    if(radiiUpper->value() < radiiLower->value()) {
        radiiLower->setValue(radiiUpper->value());
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
    paramManager.setDensity(densitySpin->value());
    paramManager.setLocationSD(initialLocationSpin->value());
    paramManager.setVelocitySD(initialVelocitySpin->value());
    paramManager.setLightFraction(lightFractionSpin->value());
    paramManager.setRadiiLower(radiiLower->value());
    paramManager.setRadiiUpper(radiiUpper->value());
    paramManager.setRandSeed(randomSeedSpin->value());
    paramManager.setAmbientPalette(colorPalette);
    paramManager.setSphereCount(countSpin->value());
    paramManager.setSunScale(sunRadiusScaleSpin->value());
    paramManager.setFullscreenChecked(fullscreenCheckBox->isChecked());
    writeSettings();
    paramManager.printParameters();
    this->accept();
}

/**
 * Can only be called after the widgets have been initialized
 * @brief readSettings
 */
void SettingsDialog::readSettings()
{
    QSettings settings("Organization", "Gravity");

    settings.beginGroup("SettingsDialog");
    gravConstantSpin->setValue(settings.value("gravitationalConstant", 6.674).toDouble());
    densitySpin->setValue(settings.value("sphereDensity", 0.8).toDouble());
    sunRadiusScaleSpin->setValue(settings.value("sunScale", 4.0).toDouble());
    initialVelocitySpin->setValue(settings.value("velocitySD", 2.5).toDouble());
    initialLocationSpin->setValue(settings.value("locationSD", 200).toDouble());
    radiiLower->setValue(settings.value("radiiLower", 1).toDouble());
    radiiUpper->setValue(settings.value("radiiUpper", 5).toDouble());
    lightFractionSpin->setValue(settings.value("lightFraction", .1).toDouble());
    randomSeedSpin->setValue(settings.value("randomSeed", 23).toInt());
    countSpin->setValue(settings.value("sphereCount", 16).toInt());
    colorPalette = settings.value("ambientColor", QColor(255, 255, 255)).value<QColor>();
    fullscreenCheckBox->setChecked(settings.value("fullScreenChecked", true).toBool());
    //std::cout << colorPalette.red() << " " << colorPalette.green() << " " << colorPalette.blue() << std::endl;
    settings.endGroup();
    updateColorSelect();
}

void SettingsDialog::writeSettings()
{
    QSettings settings("Organization", "Gravity");

    settings.beginGroup("SettingsDialog");
    settings.setValue("gravitationalConstant", gravConstantSpin->value());
    settings.setValue("sphereDensity", densitySpin->value());
    settings.setValue("sunScale", sunRadiusScaleSpin->value());
    settings.setValue("velocitySD", initialVelocitySpin->value());
    settings.setValue("locationSD", initialLocationSpin->value());
    settings.setValue("radiiLower", radiiLower->value());
    settings.setValue("radiiUpper", radiiUpper->value());
    settings.setValue("lightFraction", lightFractionSpin->value());
    settings.setValue("randomSeed", randomSeedSpin->value());
    settings.setValue("sphereCount", countSpin->value());
    settings.setValue("ambientColor", colorPalette);
    settings.setValue("fullScreenChecked", fullscreenCheckBox->isChecked());
    settings.endGroup();
}
