#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"
#include "AlgorithmRegistration.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().RegisterAlgorithm(algorithmFactory);
}
