#ifndef SIMSETTINGS_H
#define SIMSETTINGS_H
#include <qt5/QtWidgets/QDialog>
#include <qt5/QtWidgets/QDoubleSpinBox>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtWidgets/QPushButton>
#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QColorDialog>

class SettingsDialog : public QDialog {
public:
    QDoubleSpinBox* gravConstantSpin;

    SettingsDialog();
    float getGravitationalConstant();
};

#endif // SIMSETTINGS_H
