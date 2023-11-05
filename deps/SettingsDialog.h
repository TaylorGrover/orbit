#ifndef SIMSETTINGS_H
#define SIMSETTINGS_H
#include <map>
#include <parametermanager.h>
#include <qt5/QtCore/QString>
#include <qt5/QtCore/QObject>
#include <qt5/QtWidgets/QDialog>
#include <qt5/QtWidgets/QDoubleSpinBox>
#include <qt5/QtWidgets/QFormLayout>
#include <qt5/QtWidgets/QGridLayout>
#include <qt5/QtWidgets/QHBoxLayout>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtWidgets/QPushButton>
#include <qt5/QtWidgets/QLabel>
#include <qt5/QtWidgets/QColorDialog>
#include <qt5/QtWidgets/QWidget>
#include <qt5/QtGui/QKeyEvent>

class SettingsDialog : public QDialog {
    Q_OBJECT

private:
    QDoubleSpinBox *gravConstantSpin;
    QDoubleSpinBox *initialVelocitySpin;
    QDoubleSpinBox *initialLocationSpin;
    QDoubleSpinBox *radiiLower, *radiiUpper;
    QSpinBox *randomSeedSpin;
    QColor colorPalette;

    // Return either a QSpinBox or QDoubleSpinBox with various numeric types
    template<typename SpinBox, typename numType>
    SpinBox* getSpin(QVBoxLayout*, std::string labelText, numType leftRange, numType rightRange, numType initValue);

public slots:
    void selectColorPalette();
public:
    // Constructor
    SettingsDialog();

    // Override keyPressEvent for escape
    //void keyPressEvent(QKeyEvent* event) override;

    // Getters for simulation parameters
    float getGravitationalConstant();
    bool getStartSimulation();
};

#endif // SIMSETTINGS_H
