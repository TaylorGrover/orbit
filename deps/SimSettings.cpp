#include <SimSettings.h>

SettingsDialog::SettingsDialog()
{
    QFormLayout* settingsLayout = new QFormLayout(this);
    gravConstantSpin = new QDoubleSpinBox;
    gravConstantSpin->setValue(6.674f);
    gravConstantSpin->setRange(-100, 100);

    // Standard deviation of normally distributed locations
    initialLocationSpin = new QDoubleSpinBox;
    initialLocationSpin->setValue(200);
    initialLocationSpin->setRange(0, 1000);

    // "                                        " velocities
    initialVelocitySpin = new QDoubleSpinBox;
    initialVelocitySpin->setValue(2.5f);
    initialVelocitySpin->setRange(0, 100);

    // Color picker button

    // Start and Close buttons
    QPushButton* closeButton = new QPushButton;
    closeButton->setText("Close");
    QPushButton* startButton = new QPushButton;
    startButton->setText("Start Simulation");
    connect(closeButton, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(startButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    QWidget* startCloseWidget = new QWidget;
    QHBoxLayout* startCloseLayout = new QHBoxLayout(startCloseWidget);
    startCloseLayout->addWidget(closeButton);
    startCloseLayout->addWidget(startButton);

    settingsLayout->addRow("Gravitational constant: ", gravConstantSpin);
    settingsLayout->addRow("Standard dev. of initial |distance|: ", initialLocationSpin);
    settingsLayout->addRow("Standard dev. of initial |velocity|: ", initialVelocitySpin);
    settingsLayout->addRow("", startCloseWidget);

    QPushButton* colorButton = new QPushButton;
    connect(colorButton, &QPushButton::clicked, this, this->getColorPalette);
    //settingsLayout->addRow("Color palette: ", new QPush);

    setLayout(settingsLayout);
}

void SettingsDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) {
        event->ignore();
    } else {
        QDialog::keyPressEvent(event);
    }
}

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

void SettingsDialog::getColorPalette()
{

}
