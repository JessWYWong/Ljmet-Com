#ifndef LJMet_Com_interface_LjmetFactory_h
#define LJMet_Com_interface_LjmetFactory_h

/*
 Singleton manager class for all calculators
 
 Author: Gena Kukartsev, 2012
 */

#include <iostream>
#include <map>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/BaseEventSelector.h"
#include "LJMet/Com/interface/LjmetEventContent.h"

class LjmetFactory {
public:
    virtual ~LjmetFactory();
    static LjmetFactory * GetInstance() {
        if (!instance) instance = new LjmetFactory();
        return instance;
    }
    int Register(BaseCalc * calc, std::string name);
    int Register(BaseEventSelector * calc, std::string name);
    
    /// Return pointer to registered event selector. Exit if not found
    BaseEventSelector * GetEventSelector(std::string name);
    
    /// Loop over all registered calculators and compute implemented variables
    void RunAllCalculators(edm::EventBase const & event, BaseEventSelector * selector, LjmetEventContent & ec);
    
    /// Loop over all registered calculators and run all producer methods (comes before selection)
    void RunAllProducers(edm::EventBase const & event, BaseEventSelector * selector);
    
    /// Set each calc's parameter set, if present
    void SetAllCalcConfig(std::map<std::string, edm::ParameterSet const> mPar);
    void SetExcludedCalcs(std::vector<std::string> vExcl);
    
    /// Run all BeginJob()'s
    void BeginJobAllCalc();
    
    /// Run all EndJob()'s
    void EndJobAllCalc();
    void RunBeginEvent(edm::EventBase const & event, LjmetEventContent & ec);
    void RunEndEvent(edm::EventBase const & event, LjmetEventContent & ec);
    
private:
    LjmetFactory();
    LjmetFactory(const LjmetFactory &); // stop default
    std::string mLegend;
    std::map<std::string, BaseCalc * > mpCalculators;
    std::map<std::string, BaseEventSelector * > mpSelectors;
    BaseEventSelector * theSelector;
    std::vector<std::string> mvExcludedCalcs;
    static LjmetFactory * instance;
};

#endif
