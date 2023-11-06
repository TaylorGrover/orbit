#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H
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

    // Standard deviations for normally distribution locations and velocity
    //  centered at 0
    float velocitySD;
    float locationSD;
    float radiiLower, radiiUpper;

    // What fraction of spheres are lights
    float lightFraction;

    int randSeed;
    int sphereCount;

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
    void setLightFraction(float);
    void setRadiiLower(float);
    void setRadiiUpper(float);
    void setAmbientPalette(QColor);

    float getGravitationalConstant() const;
    float getVelocitySD() const;
    float getLocationSD() const;
    float getLightFraction() const;
    float getRadiiLowerBound() const;
    float getRadiiUpperBound() const;
    int   getRandSeed() const;
    int   getSphereCount() const;
    QColor getAmbientPalette() const;
};

#endif // PARAMETERMANAGER_H
