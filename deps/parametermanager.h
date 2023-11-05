#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H
#include <map>
#include <memory>

/**
 * @brief The ParameterManager class
 * Singleton class containing the simulation parameters
 */
class ParameterManager
{
    ParameterManager();

    float G;

    // Standard deviations for normally distribution locations and velocity
    //  centered at 0
    float velocitySD;
    float locationSD;
    int randSeed;
public:
    static ParameterManager& getInstance();

    ParameterManager(const ParameterManager&) = delete;
    ParameterManager& operator=(const ParameterManager&) = delete;

    // Setgetters
    void setGravitationalConstant(float);
    void setVelocitySD(float);
    void setLocationSD(float);
    void setRandSeed(int);

    float getGravitationalConstant() const;
    float getVelocitySD() const;
    float getLocationSD() const;
    int getRandSeed() const;
};

#endif // PARAMETERMANAGER_H
