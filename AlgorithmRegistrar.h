#ifndef ALGORITHMREGISTRAR_H_
#define ALGORITHMREGISTRAR_H_

#include <functional>
#include <memory>
#include <list>
#include <dlfcn.h>
#include "AbstractAlgorithm.h"

class AlgorithmRegistrar {
	static AlgorithmRegistrar instance;
    list<string> algorithmNames;
    list<function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;

public:
    friend class AlgorithmRegistration;
    
	enum { ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };

    void RegisterAlgorithm(function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) { instance.algorithmFactories.push_back(algorithmFactory); }

    int LoadAlgorithm(string& path, const string& algorithmName, void*& dlib);
    
    list<unique_ptr<AbstractAlgorithm>> getNewAlgorithmInstances() const;

    const list<std::string>& getAlgorithmNames() const { return algorithmNames; }
    
    size_t getNumRegisteredAlgorithms() const { return algorithmFactories.size(); }
    
    static AlgorithmRegistrar& getInstance() { return instance; }
    
    void RemoveAlgorithms() { algorithmFactories.clear(); }
    
private:
    void setLastAlgorithmName(const string& algorithmName) { algorithmNames.push_back(algorithmName); }
};

#endif /* ALGORITHMREGISTRAR_H_ */
