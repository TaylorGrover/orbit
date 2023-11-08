#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H
#include <iostream>
#include <map>
#include <memory>
#include <qt5/QtWidgets/QWidget>

/**
 * @brief The ParameterManager class
 * Singleton class containing the simulation parameters
 */
class ParameterManager
{
    ParameterManager();

    // Gravitational constant
    float G;

    // Density of spheres
    float density;

    // Relative sun scaling
    float sunScale;

    // Standard deviations for normally distribution locations and velocity
    //  centered at 0
    float velocitySD;
    float locationSD;
    float radiiLower, radiiUpper;

    // What fraction of spheres are lights
    float lightFraction;

    int randSeed;
    int sphereCount;

    bool fullScreenChecked;

    QColor ambientColorPalette;

public:
    static ParameterManager& getInstance();

    ParameterManager(const ParameterManager&) = delete;
    ParameterManager& operator=(const ParameterManager&) = delete;

    // Setgetters
    void setGravitationalConstant(float);
    void setVelocitySD(float);
    void setLocationSD(float);
    void setRandSeed(int);
    void setSphereCount(int);
    void setSphereDensity(float);
    void setLightFraction(float);
    void setRadiiLower(float);
    void setRadiiUpper(float);
    void setDensity(float);
    void setSunScale(float);
    void setAmbientPalette(QColor);
    void setFullscreenChecked(bool);

    // Prints all parameters
    void printParameters() const;

    float getGravitationalConstant() const;
    float getVelocitySD() const;
    float getLocationSD() const;
    float getLightFraction() const;
    float getRadiiLowerBound() const;
    float getRadiiUpperBound() const;
    float getSphereDensity() const;
    float getDensity() const;
    float getSunScale() const;
    bool getFullscreenChecked() const;
    int   getRandSeed() const;
    int   getSphereCount() const;
    QColor getAmbientPalette() const;
};

#endif // PARAMETERMANAGER_H
