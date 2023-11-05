#include "parametermanager.h"

// Private constructor
ParameterManager::ParameterManager() : G(0.0)
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

float ParameterManager::getGravitationalConstant() const {
    return G;
}
