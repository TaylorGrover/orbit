#include "parametermanager.h"

// Private constructor
ParameterManager::ParameterManager()
{

}

ParameterManager& ParameterManager::getInstance()
{
    static ParameterManager instance;
    return instance;
}

void ParameterManager::setGravitationalConstant(float value) {
    G = value;
}

void ParameterManager::setVelocitySD(float velSD)
{
    velocitySD = velSD;
}

void ParameterManager::setLocationSD(float locSD)
{
    locationSD = locSD;
}

void ParameterManager::setRandSeed(int seed)
{
    randSeed = seed;
}

void ParameterManager::setLightFraction(float frac)
{
    lightFraction = frac;
}

void ParameterManager::setAmbientPalette(QColor color)
{
    ambientColorPalette = color;
}

void ParameterManager::setRadiiLower(float lower)
{
    radiiLower = lower;
}

void ParameterManager::setRadiiUpper(float upper)
{
    radiiUpper = upper;
}

void ParameterManager::setSphereCount(int count)
{
    sphereCount = count;
}

void ParameterManager::setDensity(float density)
{
    ParameterManager::density = density;
}

// Getters
float ParameterManager::getGravitationalConstant() const {
    return G;
}

float ParameterManager::getVelocitySD() const
{
    return velocitySD;
}

float ParameterManager::getLocationSD() const
{
    return locationSD;
}

int ParameterManager::getRandSeed() const
{
    return randSeed;
}

int ParameterManager::getSphereCount() const
{
    return sphereCount;
}

float ParameterManager::getLightFraction() const
{
    return lightFraction;
}

QColor ParameterManager::getAmbientPalette() const
{
    return ambientColorPalette;
}

float ParameterManager::getRadiiLowerBound() const
{
    return radiiLower;
}

float ParameterManager::getRadiiUpperBound() const
{
    return radiiUpper;
}

float ParameterManager::getDensity() const
{
    return density;
}

void ParameterManager::printParameters() const
{
    std::cout << "G: " << G << std::endl;
    std::cout << "velSD: " << velocitySD << std::endl;
    std::cout << "locSD: " << locationSD << std::endl;
    std::cout << "radLower: " << radiiLower << std::endl;
    std::cout << "radUpper: " << radiiUpper << std::endl;
    std::cout << "lightFraction: " << lightFraction << std::endl;
    std::cout << "randSeed: " << randSeed << std::endl;
    std::cout << "sphereCount: " << sphereCount << std::endl;
}
