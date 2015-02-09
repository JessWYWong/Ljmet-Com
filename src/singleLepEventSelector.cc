// -*- C++ -*-
//
// FWLite PAT analyzer-selector for a generic single lepton analysis
//
// Adapted from WprimeEventSelector
// Joshua Swanson, April 2014
//
#ifndef LJMet_Com_interface_singleLepEventSelector_h
#define LJMet_Com_interface_singleLepEventSelector_h

#include <cmath>
#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h"
//#include "PhysicsTools/SelectorUtils/interface/PFElectronSelector.h"
#include "LJMet/Com/interface/TopElectronSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
//#include "LJMet/Com/interface/PFMuonSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PFMuonSelector.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PVObjectSelector.h"
#include "LJMet/Com/interface/BaseEventSelector.h"
#include "LJMet/Com/interface/LjmetFactory.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "PhysicsTools/CondLiteIO/interface/RecordWriter.h"
#include "Cintex/Cintex.h"
#include "PhysicsTools/JetMCUtils/interface/combination.h"


using trigger::TriggerObject;


class singleLepEventSelector : public BaseEventSelector {

public:

  
    singleLepEventSelector();  
    ~singleLepEventSelector();
  
  
    // executes before loop over events
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);

    // main method where the cuts are applied
    virtual bool operator()( edm::EventBase const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );
    
    // executes after loop over events
    virtual void EndJob();

    boost::shared_ptr<PFJetIDSelectionFunctor> const & jetSel()        const { return jetSel_;}
    boost::shared_ptr<PVSelector>              const & pvSel()         const { return pvSel_;}
    boost::shared_ptr<PFMuonSelector>          const & muonSel()       const { return muonSel_;}
    boost::shared_ptr<TopElectronSelector>     const & electronSel()   const { return electronSel_;}

protected:
    std::string legend;
    bool bFirstEntry;

    // containers for config parameter values
    std::map<std::string,bool>           mbPar;
    std::map<std::string,int>            miPar;
    std::map<std::string,double>         mdPar;
    std::map<std::string,std::string>    msPar;
    std::map<std::string, edm::InputTag> mtPar;
    std::map<std::string,std::vector<std::string> > mvsPar;

    boost::shared_ptr<PFJetIDSelectionFunctor> jetSel_;
    boost::shared_ptr<PVSelector>              pvSel_;
    boost::shared_ptr<PFMuonSelector>          muonSel_;
    boost::shared_ptr<TopElectronSelector>     electronSel_;

    edm::Handle<edm::TriggerResults >           mhEdmTriggerResults;
    edm::Handle<std::vector<pat::Jet> >         mhJets;
    edm::Handle<std::vector<pat::Muon> >        mhMuons;
    edm::Handle<std::vector<pat::Electron> >    mhElectrons;
    edm::Handle<std::vector<pat::Tau> >			mhTaus;
    edm::Handle<std::vector<pat::MET> >         mhMet;
    edm::Handle<std::vector<reco::PFMET> >      mhType1CorrMet;
    edm::Handle<double>                         h_rho;
    edm::Handle<std::vector<reco::Vertex> >     h_primVtx;

    std::vector<edm::Ptr<reco::Vertex> >  good_pvs_;



private:
  
    void initialize(std::map<std::string, edm::ParameterSet const> par);

};



static int reg = LjmetFactory::GetInstance()->Register(new singleLepEventSelector(), "singleLepSelector");

singleLepEventSelector::singleLepEventSelector()
{
}

singleLepEventSelector::~singleLepEventSelector()
{
}

void singleLepEventSelector::BeginJob( std::map<std::string, edm::ParameterSet const> par)
{
    BaseEventSelector::BeginJob(par);

    std::string _key;

        _key = "pfJetIDSelector";
    if ( par.find(_key)!=par.end() ){
        jetSel_ = boost::shared_ptr<PFJetIDSelectionFunctor>( new PFJetIDSelectionFunctor(par[_key]) );
        std::cout << mLegend << "jet ID selector configured!"
                  << std::endl;
    }
    else {
        std::cout << mLegend << "jet ID selector not configured, exiting"
                  << std::endl;
        std::exit(-1);
    }
    
    _key = "pvSelector";
    if ( par.find(_key)!=par.end() ){
        pvSel_ = boost::shared_ptr<PVSelector>( new PVSelector(par[_key]) );
        std::cout << mLegend << "pv selector configured!"
                  << std::endl;
    }
    else {
        std::cout << mLegend << "PV selector not configured, exiting"
                  << std::endl;
        std::exit(-1);
    }
    _key = "pfMuonSelector";
    if ( par.find(_key)!=par.end() ){
        muonSel_ = boost::shared_ptr<PFMuonSelector>( new PFMuonSelector(par[_key]) );
        std::cout << mLegend << "muon selector configured!"
        << std::endl;
    }
    else {
        std::cout << mLegend << "muon selector not configured, exiting"
        << std::endl;
        std::exit(-1);
    }

    _key = "TopElectronSelector";
    if ( par.find(_key)!=par.end() ){
        electronSel_ = boost::shared_ptr<TopElectronSelector>( new TopElectronSelector(par[_key]) );
        std::cout << mLegend << "top electron selector configured!"
        << std::endl;
    }
    else {
        std::cout << mLegend << "electron selector not configured, exiting"
        << std::endl;
        std::exit(-1);
    }

      _key = "event_selector";
    if ( par.find(_key)!=par.end() ){

        mbPar["debug"]                    = par[_key].getParameter<bool>         ("debug");
        mbPar["isMc"]                     = par[_key].getParameter<bool>         ("isMc");
        mbPar["keepFullMChistory"]        = par[_key].getParameter<bool>         ("keepFullMChistory");
        
        mbPar["trigger_cut"]              = par[_key].getParameter<bool>         ("trigger_cut");
        mbPar["dump_trigger"]             = par[_key].getParameter<bool>         ("dump_trigger");
        mvsPar["trigger_path_el"]         = par[_key].getParameter<std::vector<std::string>>  ("trigger_path_el");
        mvsPar["trigger_path_mu"]         = par[_key].getParameter<std::vector<std::string>>  ("trigger_path_mu");
        msPar["mctrigger_path_el"]        = par[_key].getParameter<std::string>  ("mctrigger_path_el");
        msPar["mctrigger_path_mu"]        = par[_key].getParameter<std::string>  ("mctrigger_path_mu");

        mbPar["pv_cut"]                   = par[_key].getParameter<bool>         ("pv_cut");
        mbPar["hbhe_cut"]                 = par[_key].getParameter<bool>         ("hbhe_cut");

        mbPar["jet_cuts"]                 = par[_key].getParameter<bool>         ("jet_cuts");
        mdPar["jet_minpt"]                = par[_key].getParameter<double>       ("jet_minpt");
        mdPar["jet_maxeta"]               = par[_key].getParameter<double>       ("jet_maxeta");
        miPar["min_jet"]                  = par[_key].getParameter<int>          ("min_jet");
        miPar["max_jet"]                  = par[_key].getParameter<int>          ("max_jet");
        mdPar["leading_jet_pt"]           = par[_key].getParameter<double>       ("leading_jet_pt");
        mbPar["removeJetLepOverlap"]      = par[_key].getParameter<bool>         ("removeJetLepOverlap");

        mbPar["muon_cuts"]                = par[_key].getParameter<bool>         ("muon_cuts");
        mdPar["muon_minpt"]         = par[_key].getParameter<double>       ("muon_minpt");
        mdPar["muon_maxeta"]        = par[_key].getParameter<double>       ("muon_maxeta");
        miPar["min_muon"]           = par[_key].getParameter<int>          ("min_muon");

        mbPar["electron_cuts"]            = par[_key].getParameter<bool>         ("electron_cuts");
        mdPar["electron_minpt"]     = par[_key].getParameter<double>       ("electron_minpt");
        mdPar["electron_maxeta"]    = par[_key].getParameter<double>       ("electron_maxeta");
        miPar["min_electron"]       = par[_key].getParameter<int>          ("min_electron");

        miPar["min_lepton"]         = par[_key].getParameter<int>          ("min_lepton");
        miPar["max_lepton"]         = par[_key].getParameter<int>          ("max_lepton");
        mbPar["second_lepton_veto"]       = par[_key].getParameter<bool>         ("second_lepton_veto");
        mbPar["tau_veto"]                 = par[_key].getParameter<bool>         ("tau_veto");

        mbPar["met_cuts"]                 = par[_key].getParameter<bool>         ("met_cuts");
        mdPar["min_met"]                  = par[_key].getParameter<double>       ("min_met");

        mbPar["btag_cuts"]                = par[_key].getParameter<bool>         ("btag_cuts");
        mbPar["btag_1"]                   = par[_key].getParameter<bool>         ("btag_1");
        mbPar["btag_2"]                   = par[_key].getParameter<bool>         ("btag_2");
        mbPar["btag_3"]                   = par[_key].getParameter<bool>         ("btag_3");

        mtPar["trigger_collection"]       = par[_key].getParameter<edm::InputTag>("trigger_collection");
        mtPar["pv_collection"]            = par[_key].getParameter<edm::InputTag>("pv_collection");
        mtPar["jet_collection"]           = par[_key].getParameter<edm::InputTag>("jet_collection");
        mtPar["muon_collection"]          = par[_key].getParameter<edm::InputTag>("muon_collection");
        mtPar["electron_collection"]      = par[_key].getParameter<edm::InputTag>("electron_collection");
        mtPar["tau_collection"]           = par[_key].getParameter<edm::InputTag>("tau_collection");
        mtPar["met_collection"]           = par[_key].getParameter<edm::InputTag>("met_collection");

        mbPar["BTagUncertUp"]             = par[_key].getParameter<bool>         ("BTagUncertUp");
        mbPar["BTagUncertDown"]           = par[_key].getParameter<bool>         ("BTagUncertDown");
        mbPar["JECup"]                    = par[_key].getParameter<bool>         ("JECup");
        mbPar["JECdown"]                  = par[_key].getParameter<bool>         ("JECdown");
        mbPar["JERup"]                    = par[_key].getParameter<bool>         ("JERup");
        mbPar["JERdown"]                  = par[_key].getParameter<bool>         ("JERdown");
        msPar["JEC_txtfile"]              = par[_key].getParameter<std::string>  ("JEC_txtfile");
        mbPar["doNewJEC"]                 = par[_key].getParameter<bool>         ("doNewJEC");
      

        std::cout << mLegend << "config parameters loaded..."
                  << std::endl;
    }   
    else {
        std::cout << mLegend << "event selector not configured, exiting"
                  << std::endl;
        std::exit(-1);
    }
  
    std::cout << mLegend << "initializing singleLep selection" << std::endl;

    bFirstEntry = true;
  
    push_back("No selection");
    set("No selection");
  
    push_back("Trigger");
    push_back("Primary vertex");
    push_back("HBHE noise and scraping filter");
    push_back("One jet or more");
    push_back("Two jets or more");
    push_back("Three jets or more");
    push_back("Min jet multiplicity");
    push_back("Max jet multiplicity");
    push_back("Leading jet pt");
    push_back("Min MET");
    push_back("Min muon");
    push_back("Min electron");
    push_back("Min lepton");
    push_back("Max lepton");
    //push_back("Trigger consistent");
    push_back("Second lepton veto");
    push_back("Tau veto");
    push_back("1 btag or more");
    push_back("2 btag or more");
    push_back("3 btag or more");
    push_back("All cuts");          // sanity check

  
    // TOP PAG sync selection v3

    set("Trigger", mbPar["trigger_cut"]); 
    set("Primary vertex", mbPar["pv_cut"]);
    set("HBHE noise and scraping filter", mbPar["hbhe_cut"]); 
 
    if (mbPar["jet_cuts"]){
        set("One jet or more", false);
        set("Two jets or more", false);
        set("Three jets or more", false);
        set("Min jet multiplicity", miPar["min_jet"]);
        set("Max jet multiplicity", miPar["max_jet"]);
        set("Leading jet pt", mdPar["leading_jet_pt"]);
    }
    else{
        set("One jet or more", false);
        set("Two jets or more", false);
        set("Three jets or more", false);
        set("Min jet multiplicity", false);
        set("Max jet multiplicity", false);
        set("Leading jet pt", false);
    }

    if (mbPar["met_cuts"]) set("Min MET", mdPar["min_met"]);

    set("Min muon", miPar["min_muon"]);  
    set("Min electron", miPar["min_electron"]);  
    set("Min lepton", miPar["min_lepton"]);  
    set("Max lepton", miPar["max_lepton"]);  
    //set("Trigger consistent", mbPar["trigger_consistent"]);  
    set("Second lepton veto", mbPar["second_lepton_veto"]);
    set("Tau veto", mbPar["tau_veto"]);
     
    if (mbPar["btag_cuts"]){
        set("1 btag or more", mbPar["btag_1"]);
        set("2 btag or more", mbPar["btag_2"]);
        set("3 btag or more", mbPar["btag_3"]);
    }
    else{
        set("1 btag or more", false);
        set("2 btag or more", false);
        set("3 btag or more", false);
    }

    set("All cuts", true);
    
   
} // end of BeginJob() 

bool singleLepEventSelector::operator()( edm::EventBase const & event, pat::strbitset & ret)
{
    pat::strbitset retJet            = jetSel_->getBitTemplate();
    pat::strbitset retMuon           = muonSel_->getBitTemplate();
  //  pat::strbitset retLooseMuon      = looseMuonSel_->getBitTemplate();
    pat::strbitset retElectron       = electronSel_->getBitTemplate();
  //  pat::strbitset retLooseElectron  = looseElectronSel_->getBitTemplate();
    
    while(1){ // standard infinite while loop trick to avoid nested ifs
    
        passCut(ret, "No selection");
    
        //
        //_____ Trigger cuts __________________________________
        //

        bool passTrigElMC = false;
        bool passTrigMuMC = false;
        bool passTrigElData = false;
        bool passTrigMuData = false;

        if ( considerCut("Trigger") ) {

            if (mbPar["debug"]) std::cout<<"trigger cuts..."<<std::endl;

            event.getByLabel( mtPar["trigger_collection"], mhEdmTriggerResults );
            //const edm::ParameterSetID ps = mhEdmTriggerResults->parameterSetID();
            const edm::TriggerNames trigNames = event.triggerNames(*mhEdmTriggerResults);

            bool passTrig = false;
            unsigned int _tSize = mhEdmTriggerResults->size();


            // dump trigger names
            if (bFirstEntry && mbPar["dump_trigger"]){
                for (unsigned int i=0; i<_tSize; i++){
                    std::string trigName = trigNames.triggerName(i);
                    std::cout << i << "   " << trigName;
                    bool fired = mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName));
                    std::cout <<", FIRED = "<<fired<<std::endl;
                } 
            }

            unsigned int _tElMCIndex = trigNames.triggerIndex(msPar["mctrigger_path_el"]);
            if ( _tElMCIndex<_tSize){
                passTrigElMC = mhEdmTriggerResults->accept(_tElMCIndex);
            }

            unsigned int _tMuMCIndex = trigNames.triggerIndex(msPar["mctrigger_path_mu"]);
            if ( _tMuMCIndex<_tSize){
                passTrigMuMC = mhEdmTriggerResults->accept(_tMuMCIndex);
            }

            //Loop over each data channel separately
            int passTrigEl = 0;
            for (unsigned int ipath = 0; ipath < mvsPar["trigger_path_el"].size(); ipath++){
                unsigned int _tIndex = trigNames.triggerIndex(mvsPar["trigger_path_el"].at(ipath));
                if ( _tIndex<_tSize){
                    if (mhEdmTriggerResults->accept(_tIndex)){
                        passTrigEl++;
                        break;
                    }
                }
            }
            if (passTrigEl>0) passTrigElData = true;

            int passTrigMu = 0;
            for (unsigned int ipath = 0; ipath < mvsPar["trigger_path_mu"].size(); ipath++){
                unsigned int _tIndex = trigNames.triggerIndex(mvsPar["trigger_path_mu"].at(ipath));
                if ( _tIndex<_tSize){
                    if (mhEdmTriggerResults->accept(_tIndex)){
                        passTrigMu++;
                        break;
                    }
                }
            }
            if (passTrigMu>0) passTrigMuData = true;

            if (mbPar["isMc"] && (passTrigMuMC||passTrigElMC) ) passTrig = true;
            if (!mbPar["isMc"] && (passTrigMuData||passTrigElData) ) passTrig = true;
            mvSelTriggers.clear();
            mvSelTriggers.push_back(passTrigEl);
            mvSelTriggers.push_back(passTrigMu);


            if ( ignoreCut("Trigger") || passTrig ) passCut(ret, "Trigger");
            else break;

        } // end of trigger cuts
    
    

        //
        //_____ Primary vertex cuts __________________________________
        //
        mvSelPVs.clear();
        if ( considerCut("Primary vertex") ) {
            if (mbPar["debug"]) std::cout<<"pv cuts..."<<std::endl;

            if ( (*pvSel_)(event) ){
                passCut(ret, "Primary vertex"); // PV cuts total
            }

            event.getByLabel( mtPar["pv_collection"], h_primVtx );
            int _n_pvs = 0;
            for (std::vector<reco::Vertex>::const_iterator _ipv = h_primVtx->begin();
                 _ipv != h_primVtx->end(); ++_ipv){
                mvSelPVs.push_back(edm::Ptr<reco::Vertex>(h_primVtx, _n_pvs));
                ++_n_pvs;
            }
          
        } // end of PV cuts


   
        //
        //_____ HBHE noise and scraping filter________________________
        //
        if ( considerCut("HBHE noise and scraping filter") ) {
            if (mbPar["debug"]) std::cout<<"HBHE cuts..."<<std::endl;

            passCut(ret, "HBHE noise and scraping filter"); // PV cuts total

        } // end of PV cuts


        //======================================================
        //
        // jet loop
        //
        //
        if (mbPar["debug"]) std::cout<<"start jet cuts..."<<std::endl;

        event.getByLabel( mtPar["jet_collection"], mhJets );

        int _n_good_jets = 0;
        int _n_jets = 0;
        int nBtagJets = 0;
        double _leading_jet_pt = 0.0;

        mvSelJets.clear();
        mvAllJets.clear();
        mvCorrJetsWithBTags.clear();
        mvSelBtagJets.clear();

        // try to get earlier produced data (in a calc)
        //std::cout << "Must be 2.34: " << GetTestValue() << std::endl;

        for (std::vector<pat::Jet>::const_iterator _ijet = mhJets->begin();
             _ijet != mhJets->end(); ++_ijet){
      

            retJet.set(false);

            bool _pass = false;
            bool _passpf = false;
            bool _isTagged = false;

            TLorentzVector jetP4 = correctJet(*_ijet, event);
            _isTagged = isJetTagged(*_ijet, event);

            // jet cuts
            while(1){ 

                // quality cuts
                if ( (*jetSel_)( *_ijet, retJet ) ){ } 
                else break; // fail 
	
                _passpf = true;

                if ( jetP4.Pt() > mdPar["jet_minpt"] ){ }
                else break; // fail 
	
                if ( fabs(jetP4.Eta()) < mdPar["jet_maxeta"] ){ }
                else break; // fail
	
                _pass = true;
                break;
            }

            pair<TLorentzVector,bool> jetwithtag;
            jetwithtag.first = jetP4;
            jetwithtag.second = _isTagged;

            if ( _pass ){


                // save all the good jets
                ++_n_good_jets;
                mvSelJets.push_back(edm::Ptr<pat::Jet>(mhJets, _n_jets)); 
                mvCorrJetsWithBTags.push_back(jetwithtag);

                if (jetP4.Pt() > _leading_jet_pt) _leading_jet_pt = jetP4.Pt();                         
            
                if (_isTagged) {
                    ++nBtagJets;
                    // save all the good b-tagged jets
                    mvSelBtagJets.push_back(edm::Ptr<pat::Jet>(mhJets, _n_jets)); 
                }
            }
      
            // save all the pf jets regardless of pt or eta
            // needed for MET corrections with JER/JES uncertainty
            if (_passpf) mvAllJets.push_back(edm::Ptr<pat::Jet>(mhJets, _n_jets)); 

            ++_n_jets; 
      
        } // end of loop over jets

        //
        if ( mbPar["jet_cuts"] ) {

            if ( ignoreCut("One jet or more") || _n_good_jets >= 1 ) passCut(ret, "One jet or more");
            else break; 
	
            if ( ignoreCut("Two jets or more") || _n_good_jets >= 2 ) passCut(ret, "Two jets or more");
            else break; 
	
            if ( ignoreCut("Three jets or more") || _n_good_jets >= 3 ) passCut(ret, "Three jets or more");
            else break; 
	
            if ( ignoreCut("Min jet multiplicity") || _n_good_jets >= cut("Min jet multiplicity",int()) ) passCut(ret, "Min jet multiplicity");
            else break; 
	
            if ( ignoreCut("Max jet multiplicity") || _n_good_jets <= cut("Max jet multiplicity",int()) ) passCut(ret, "Max jet multiplicity");
            else break; 
            if ( ignoreCut("Leading jet pt") ||  _leading_jet_pt >= cut("Leading jet pt",double()) ) passCut(ret, "Leading jet pt");
            else break;

        } // end of jet cuts
        if (mbPar["debug"]) std::cout<<"finish jet cuts..."<<std::endl;

        //
        //_____ MET cuts __________________________________
        //   
        if (mbPar["debug"]) std::cout<<"start met cuts..."<<std::endl;

        event.getByLabel( mtPar["met_collection"], mhMet );
        mpMet = edm::Ptr<pat::MET>( mhMet, 0);

        if ( mbPar["met_cuts"] ) {

            // pfMet
            //if ( mpType1CorrMet.isNonnull() && mpType1CorrMet.isAvailable() ) {
            if ( mpMet.isNonnull() && mpMet.isAvailable() ) {
                pat::MET const & met = mhMet->at(0);
                if ( ignoreCut("Min MET") ||met.et()>cut("Min MET", double()) ) passCut(ret, "Min MET");
            }
        } // end of MET cuts
        if (mbPar["debug"]) std::cout<<"finish met cuts..."<<std::endl;
        
        //
        //_____ Muon cuts ________________________________
        //      
        // loop over muons

        int _n_muons  = 0;
        int nSelMuons = 0;
        if (mbPar["debug"]) std::cout<<"start muon cuts..."<<std::endl;

        if ( mbPar["muon_cuts"] ) {

            //get muons
            event.getByLabel( mtPar["muon_collection"], mhMuons );      

            mvSelMuons.clear();
            for (std::vector<pat::Muon>::const_iterator _imu = mhMuons->begin(); _imu != mhMuons->end(); _imu++){
                retMuon.set(false);	
                bool pass = false;

                //muon cuts
                while(1){

                    if ( (*muonSel_)( *_imu, retMuon ) ){ }
                    else break; // fail
                    if (_imu->pt()>mdPar["muon_minpt"]){ }
                    else break;

                    if ( fabs(_imu->eta())<mdPar["muon_maxeta"] ){ }
                    else break;

                    pass = true; // success
                    break;
                }

                if ( pass ){
                    ++nSelMuons; 

                    // save every good muon
                    mvSelMuons.push_back( edm::Ptr<pat::Muon>( mhMuons, _n_muons) );
                }	
                	
                _n_muons++;
            } // end of the muon loop

        } // end of muon cuts
        if (mbPar["debug"]) std::cout<<"finish muon cuts..."<<std::endl;

        //
        //_____ Electron cuts __________________________________
        //      
        // loop over electrons

        int _n_electrons  = 0;
        int nSelElectrons = 0;
        if (mbPar["debug"]) std::cout<<"start electron cuts..."<<std::endl;

        if ( mbPar["electron_cuts"] ) {
            //get electrons
            event.getByLabel( mtPar["electron_collection"], mhElectrons );      

            mvSelElectrons.clear();
	
            for (std::vector<pat::Electron>::const_iterator _iel = mhElectrons->begin(); _iel != mhElectrons->end(); _iel++){
	        retElectron.set(false);
                bool pass = false;

                //electron cuts
                while(1){

                    if ( (*electronSel_)( *_iel, event, retElectron ) ){ }
                    else break; // fail
                    if (_iel->pt()>mdPar["electron_minpt"]){ }
                    else break;
	  
                    if ( fabs(_iel->eta())<mdPar["electron_maxeta"] ){ }
                    else break;

                    pass = true; // success
                    break;
                }

                if ( pass ){
                     ++nSelElectrons;

                   
                    // save every good electron
                    mvSelElectrons.push_back( edm::Ptr<pat::Electron>( mhElectrons, _n_electrons) );
                }	
                 	
                _n_electrons++;
            } // end of the electron loop

        } // end of electron cuts
        if (mbPar["debug"]) std::cout<<"finish electron cuts..."<<std::endl;

        //
        //_____ Tau cuts __________________________________
        //      
        // loop over taus

        int _n_taus  = 0;
        if (mbPar["debug"]) std::cout<<"start tau cuts..."<<std::endl;

        if ( mbPar["tau_veto"] ) {
            //get electrons
            event.getByLabel( mtPar["tau_collection"], mhTaus );      

            for (std::vector<pat::Tau>::const_iterator _itau = mhTaus->begin(); _itau != mhTaus->end(); _itau++){

                while(1){

					//Tau cuts hardcoded here	
					if(_itau->tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits")){}
					else break;
					
					if(_itau->tauID("againstElectronTight")){}
					else break;
					
					if(_itau->tauID("againstMuonTight2")){}
					else break;
					
					if(_itau->pt() > 20 && fabs(_itau->eta()) < 2.4 ){}
					else break;
					
					++_n_taus;
					break;
 
				}
			}

		}
        if (mbPar["debug"]) std::cout<<"finish tau cuts..."<<std::endl;
		
		

        if (mbPar["debug"]) std::cout<<"start lepton cuts..."<<std::endl;

        int nLeptons = nSelElectrons + nSelMuons;

        if( nSelMuons >= cut("Min muon", int()) || ignoreCut("Min muon") ) passCut(ret, "Min muon");
        else break;
        if( nSelElectrons >= cut("Min electron", int()) || ignoreCut("Min electron") ) passCut(ret, "Min electron");
        else break;
        if( nLeptons >= cut("Min lepton", int()) || ignoreCut("Min lepton") ) passCut(ret, "Min lepton");
        else break;
        if( nLeptons <= cut("Max lepton", int()) || ignoreCut("Max lepton") ) passCut(ret, "Max lepton");
        else break;

        
        bool NoSecondLepton = true;
        if ( _n_muons>0 &&  _n_electrons >0 ) NoSecondLepton = false;
       
        if( NoSecondLepton || ignoreCut("Second lepton veto") ) passCut(ret, "Second lepton veto");
        else break;
        
        if( _n_taus == 0 ) passCut(ret, "Tau veto");
        else break;
        
        if (mbPar["debug"]) std::cout<<"finish lepton cuts..."<<std::endl;
    
        //
        //_____ Btagging cuts _____________________
        //
        if (mbPar["debug"]) std::cout<<"start btag cuts..."<<std::endl;

        if ( mbPar["btag_cuts"] ) {
              
            if ( nBtagJets >= 1 || ignoreCut("1 btag or more") )  passCut(ret, "1 btag or more");
            else break;
            if ( nBtagJets >= 2 || ignoreCut("2 btag or more") )  passCut(ret, "2 btag or more");
            else break;
            if ( nBtagJets >= 3 || ignoreCut("3 btag or more") )  passCut(ret, "3 btag or more");
            else break;

        }
        if (mbPar["debug"]) std::cout<<"finish btag cuts..."<<std::endl;
    
        passCut(ret, "All cuts");
        break;

    } // end of while loop


    bFirstEntry = false;
    
    return (bool)ret;
  
    setIgnored(ret);

    return false;
}// end of operator()


void singleLepEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
    //
    // Compute analysis-specific quantities in the event,
    // return via event content
    //

//   ec.SetValue("pi", 3.14);
  

    return;
}

void singleLepEventSelector::EndJob()
{
}

#endif
