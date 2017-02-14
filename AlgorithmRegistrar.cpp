#include "AlgorithmRegistrar.h"

// create the static field
AlgorithmRegistrar AlgorithmRegistrar::instance;

//--------------------	--------------------------------------------------------
// returns:  0 for success
//          -1 if file cannot be loaded
//          -2 if file doesn't register any algorithm
//----------------------------------------------------------------------------

int AlgorithmRegistrar::LoadAlgorithm(string& path, const string& algorithmName, void*& dlib)
{
    size_t size = instance.getNumRegisteredAlgorithms();

	dlib = dlopen(path.c_str(), RTLD_NOW);
	if (dlib == nullptr) return FILE_CANNOT_BE_LOADED;

	if (instance.getNumRegisteredAlgorithms() <= size) 
	{
		dlclose(dlib);
		return NO_ALGORITHM_REGISTERED;
	}

    instance.setLastAlgorithmName(algorithmName);
	return ALGORITHM_REGISTERED_SUCCESSFULY;
}

list<unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getNewAlgorithmInstances() const
{
	list<unique_ptr<AbstractAlgorithm>> algorithms;

	for (auto algorithmFactoryFunc : algorithmFactories) 
		algorithms.push_back(algorithmFactoryFunc());
	
	return algorithms;
}