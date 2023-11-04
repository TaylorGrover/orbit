#include <SimSettings.h>

SettingsDialog::SettingsDialog()
{
    resize(800, 600);
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    auto* gravConstantSpin = new QDoubleSpinBox(this);
    gravConstantSpin->setRange(0, 100); // Set appropriate range
    gravConstantSpin->setValue(6.67430); // Example default value

    auto* initialVelocitySpin = new QDoubleSpinBox(this);
    initialVelocitySpin->setRange(0, 100); // Set appropriate range

    auto* randomSeedSpin = new QSpinBox(this);
    randomSeedSpin->setRange(0, INT_MAX); // Set appropriate range

    auto* colorButton = new QPushButton("Choose Color", this);

    layout->addWidget(new QLabel("Gravitational Constant:"));
    layout->addWidget(gravConstantSpin);
    layout->addWidget(new QLabel("Average Initial Velocity:"));
    layout->addWidget(initialVelocitySpin);
    layout->addWidget(new QLabel("Random Seed:"));
    layout->addWidget(randomSeedSpin);
    layout->addWidget(colorButton);

    connect(colorButton, &QPushButton::clicked, this, []() {
        QColorDialog::getColor(); // Add handling for color selection
    });

    // Add your other parameters here

    setLayout(layout);
}

float SettingsDialog::getGravitationalConstant()
{
    return gravConstantSpin->value();
}
