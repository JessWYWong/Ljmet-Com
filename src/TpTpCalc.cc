/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include <algorithm>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"

using namespace std;

class LjmetFactory;


class TpTpCalc : public BaseCalc{
  
public:
  
    TpTpCalc();
    virtual ~TpTpCalc(){}

    virtual int BeginJob(){
 
      if (mPset.exists("genParticles")) genParticles_it = mPset.getParameter<edm::InputTag>("genParticles");
      else                              genParticles_it = edm::InputTag("prunedGenParticles");
 
      return 0;
    }

    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;}

  
private:
  
  edm::InputTag genParticles_it;
 
};



static int reg = LjmetFactory::GetInstance()->Register(new TpTpCalc(), "TpTpCalc");



TpTpCalc::TpTpCalc(){
}

int TpTpCalc::AnalyzeEvent(edm::EventBase const & event,
                             BaseEventSelector * selector){
    //
    // compute event variables here
    //
      
    bool isTZTZ = false;
    bool isTZTH = false;
    bool isTZBW = false;
    bool isTHTH = false;
    bool isTHBW = false;
    bool isBWBW = false;

    bool isBZBZ = false;
    bool isBZBH = false;
    bool isBZTW = false;
    bool isBHBH = false;
    bool isBHTW = false;
    bool isTWTW = false;

    std::vector<int>    tPrimeStatus;
    std::vector<int>    tPrimeID;
    std::vector<int>    tPrimeNDaughters;
    std::vector<double> tPrimeMass;
    std::vector<double> tPrimePt;
    std::vector<double> tPrimeEta;
    std::vector<double> tPrimePhi;
    std::vector<double> tPrimeEnergy;

    std::vector<int>    bPrimeStatus;
    std::vector<int>    bPrimeID;
    std::vector<int>    bPrimeNDaughters;
    std::vector<double> bPrimeMass;
    std::vector<double> bPrimePt;
    std::vector<double> bPrimeEta;
    std::vector<double> bPrimePhi;
    std::vector<double> bPrimeEnergy;

    std::vector<int>    quarkID;
    std::vector<double> quarkPt;
    std::vector<double> quarkEta;
    std::vector<double> quarkPhi;
    std::vector<double> quarkEnergy;

    std::vector<int>    bosonID;
    std::vector<double> bosonPt;
    std::vector<double> bosonEta;
    std::vector<double> bosonPhi;
    std::vector<double> bosonEnergy;

    int Nlepdecays = 0;
    std::vector<int> lepID;
    std::vector<int> lepParentID;
    std::vector<double> lepPt;
    std::vector<double> lepEta;
    std::vector<double> lepPhi;
    std::vector<double> lepEnergy;

    // Get the generated particle collection
    edm::Handle<reco::GenParticleCollection> genParticles;
    event.getByLabel(genParticles_it, genParticles);

    // loop over all gen particles in event
    for(size_t i = 0; i < genParticles->size(); i++){
      const reco::GenParticle &p = (*genParticles).at(i);
      int id = p.pdgId();

      // find T' and B' particles 
      if(abs(id) == 8) cout << "saw 8" << endl;
      if(abs(id) != 8000001 && abs(id) != 8000002) continue;
      
      // get the T'/B' daughter particles
      size_t nDs = p.numberOfDaughters();
      bool hasTdaughter = false;
      for(size_t j = 0; j < nDs; j++){
	int dauId = (p.daughter(j))->pdgId();
	const reco::Candidate *d = p.daughter(j);
	if(d->pdgId() != dauId) std::cout << "making daughter GenParticle didn't work" << std::endl;
	
	// test for a T' --> T' decay
	if(abs(id) == 8000001 && abs(dauId) == 8000001) hasTdaughter = true;
	if(abs(id) == 8000002 && abs(dauId) == 8000002) hasTdaughter = true;
	if(!hasTdaughter && abs(dauId) > 8000000 && abs(dauId) < 8000009) cout << "daughter is 800000X but doesn't match mother" << endl;
      }

      // Skip T' --> T', we'll catch the second one later in the loop
      if(hasTdaughter) continue;

      // Save kinematics of the original T' in the case of a T' --> T' decay
      const reco::Candidate *m0 = p.mother();
      if(abs(id) == 8000001){
	if(abs(m0->pdgId()) == 8000001){
	  tPrimeStatus.push_back( m0->status() );
	  tPrimeID.push_back( m0->pdgId() );
	  tPrimeMass.push_back( m0->mass() );
	  tPrimePt.push_back( m0->pt() );
	  tPrimeEta.push_back( m0->eta() );
	  tPrimePhi.push_back( m0->phi() );
	  tPrimeEnergy.push_back( m0->energy() );
	  tPrimeNDaughters.push_back( m0->numberOfDaughters() );
	}else{
	  tPrimeStatus.push_back( p.status() );
	  tPrimeID.push_back( p.pdgId() );
	  tPrimeMass.push_back( p.mass() );
	  tPrimePt.push_back( p.pt() );
	  tPrimeEta.push_back( p.eta() );
	  tPrimePhi.push_back( p.phi() );
	  tPrimeEnergy.push_back( p.energy() );
	  tPrimeNDaughters.push_back( p.numberOfDaughters() );
	}
      }
      // Save kinematics of the original B' in the case of a B' --> B' decay
      if(abs(id) == 8000002){
	if(abs(m0->pdgId()) == 8000002){
	  bPrimeStatus.push_back( m0->status() );
	  bPrimeID.push_back( m0->pdgId() );
	  bPrimeMass.push_back( m0->mass() );
	  bPrimePt.push_back( m0->pt() );
	  bPrimeEta.push_back( m0->eta() );
	  bPrimePhi.push_back( m0->phi() );
	  bPrimeEnergy.push_back( m0->energy() );
	  bPrimeNDaughters.push_back( m0->numberOfDaughters() );
	}else{
	  bPrimeStatus.push_back( p.status() );
	  bPrimeID.push_back( p.pdgId() );
	  bPrimeMass.push_back( p.mass() );
	  bPrimePt.push_back( p.pt() );
	  bPrimeEta.push_back( p.eta() );
	  bPrimePhi.push_back( p.phi() );
	  bPrimeEnergy.push_back( p.energy() );
	  bPrimeNDaughters.push_back( p.numberOfDaughters() );
	}
      }

      // Find quark and boson daughters  
      for(size_t j = 0; j < nDs; j++){
	int dauId = (p.daughter(j))->pdgId();
	const reco::Candidate *d = p.daughter(j);
	if(d->pdgId() != dauId) std::cout << "making daughter GenParticle didn't work" << std::endl;
	
	if(abs(dauId) == 5 || abs(dauId) == 6){
	  quarkID.push_back(dauId);
	  quarkPt.push_back(d->pt());
	  quarkEta.push_back(d->eta());
	  quarkPhi.push_back(d->phi());
	  quarkEnergy.push_back(d->energy());
	  const reco::Candidate *mom = d->mother();
	  if(abs(mom->pdgId()) != 8000001 && abs(mom->pdgId()) != 8000002) cout << "quark mother not T' or B': " << mom->pdgId() << endl;

	  // ID lepton decays for t -> Wb. Need to deal with t -> t -> Wb and also W -> W -> lv or quarks
	  if(abs(dauId) == 6){
	    size_t nTDs = d->numberOfDaughters();
	    for(size_t k = 0; k < nTDs; k++){
	      const reco::Candidate *Tdaughter = d->daughter(k);
	      int TdaughterID = Tdaughter->pdgId();

	      // t -> t -> Wb decay
	      if(abs(TdaughterID) == 6){
		size_t nT2Ds = Tdaughter->numberOfDaughters();
		for(size_t l = 0; l < nT2Ds; l++){
		  const reco::Candidate *T2daughter = Tdaughter->daughter(l);
		  int T2daughterID = T2daughter->pdgId();
		  
		  if(abs(T2daughterID) == 24){
		    size_t nWDs = T2daughter->numberOfDaughters();
		    for(size_t m = 0; m < nWDs; m++){
		      const reco::Candidate *Wdaughter = T2daughter->daughter(m);
		      int WdaughterID = Wdaughter->pdgId();

		      // W -> W decay
		      if(abs(WdaughterID) == 24){
			size_t nW2Ds = Wdaughter->numberOfDaughters();
			for(size_t n = 0; n < nW2Ds; n++){
			  const reco::Candidate *W2daughter = Wdaughter->daughter(n);
			  int W2daughterID = W2daughter->pdgId();

			  if(abs(W2daughterID) == 11 || abs(W2daughterID) == 13 || abs(W2daughterID) == 15){
			    Nlepdecays++;
			    lepID.push_back(W2daughterID);
			    lepParentID.push_back(dauId);
			    lepPt.push_back(W2daughter->pt());
			    lepEta.push_back(W2daughter->eta());
			    lepPhi.push_back(W2daughter->phi());
			    lepEnergy.push_back(W2daughter->energy());
			  }
			}
		      }
		      // W direct decay
		      if(abs(WdaughterID) == 11 || abs(WdaughterID) == 13 || abs(WdaughterID) == 15){
			Nlepdecays++;
			lepID.push_back(WdaughterID);
			lepParentID.push_back(dauId);
			lepPt.push_back(Wdaughter->pt());
			lepEta.push_back(Wdaughter->eta());
			lepPhi.push_back(Wdaughter->phi());
			lepEnergy.push_back(Wdaughter->energy());
		      }
		    }
		  }
		}
	      }
	      // t-> Wb direct decay
	      if(abs(TdaughterID) == 24){
		size_t nWDs = Tdaughter->numberOfDaughters();
		for(size_t l = 0; l < nWDs; l++){
		  const reco::Candidate *Wdaughter = Tdaughter->daughter(l);
		  int WdaughterID = Wdaughter->pdgId();

		  // W -> W decay
		  if(abs(WdaughterID) == 24){
		    size_t nW2Ds = Wdaughter->numberOfDaughters();
		    for(size_t l = 0; l < nW2Ds; l++){
		      const reco::Candidate *W2daughter = Wdaughter->daughter(l);
		      int W2daughterID = W2daughter->pdgId();
		      if(abs(W2daughterID) == 11 || abs(W2daughterID) == 13 || abs(W2daughterID) == 15){
			Nlepdecays++;
			lepID.push_back(W2daughterID);
			lepParentID.push_back(dauId);
			lepPt.push_back(W2daughter->pt());
			lepEta.push_back(W2daughter->eta());
			lepPhi.push_back(W2daughter->phi());
			lepEnergy.push_back(W2daughter->energy());			
		      }
		    }
		  }
		  // W direct decay
		  if(abs(WdaughterID) == 11 || abs(WdaughterID) == 13 || abs(WdaughterID) == 15){
		    Nlepdecays++;
		    lepID.push_back(WdaughterID);
		    lepParentID.push_back(dauId);
		    lepPt.push_back(Wdaughter->pt());
		    lepEta.push_back(Wdaughter->eta());
		    lepPhi.push_back(Wdaughter->phi());
		    lepEnergy.push_back(Wdaughter->energy());		   
		  }
		}
	      }
	    }
	  }
	}

	else if(abs(dauId) > 22 && abs(dauId) < 26){
	  bosonID.push_back(dauId);
	  bosonPt.push_back(d->pt());
	  bosonEta.push_back(d->eta());
	  bosonPhi.push_back(d->phi());
	  bosonEnergy.push_back(d->energy());
	  const reco::Candidate *mom = d->mother();
	  if(abs(mom->pdgId()) != 8000001 && abs(mom->pdgId()) != 8000002) cout << "boson mother not T' or B': " << mom->pdgId() << endl;

	  // ID lepton decays for W's. Need to deal with W -> W -> lv or quarks
	  if(abs(dauId) == 24){
	    size_t nWDs = d->numberOfDaughters();
	    for(size_t k = 0; k < nWDs; k++){
	      const reco::Candidate *Wdaughter = d->daughter(k);
	      int WdaughterID = Wdaughter->pdgId();

	      // W -> W decay
	      if(abs(WdaughterID) == 24){
		size_t nW2Ds = Wdaughter->numberOfDaughters();
		for(size_t l = 0; l < nW2Ds; l++){
		  const reco::Candidate *W2daughter = Wdaughter->daughter(l);
		  int W2daughterID = W2daughter->pdgId();

		  if(abs(W2daughterID) == 11 || abs(W2daughterID) == 13 || abs(W2daughterID) == 15){
		    Nlepdecays++;
		    lepID.push_back(W2daughterID);
		    lepParentID.push_back(dauId);
		    lepPt.push_back(W2daughter->pt());
		    lepEta.push_back(W2daughter->eta());
		    lepPhi.push_back(W2daughter->phi());
		    lepEnergy.push_back(W2daughter->energy());
		  }
		}
	      }
	      // W direct decay
	      if(abs(WdaughterID) == 11 || abs(WdaughterID) == 13 || abs(WdaughterID) == 15){
		Nlepdecays++;
		lepID.push_back(WdaughterID);
		lepParentID.push_back(dauId);
		lepPt.push_back(Wdaughter->pt());
		lepEta.push_back(Wdaughter->eta());
		lepPhi.push_back(Wdaughter->phi());
		lepEnergy.push_back(Wdaughter->energy());
	      }
	    }
	  }
	}
	else continue;
      }
    }

    SetValue("NLeptonDecays", Nlepdecays);
    SetValue("LeptonID", lepID);
    SetValue("LeptonParentID", lepParentID);
    SetValue("LeptonPt", lepPt);
    SetValue("LeptonEta", lepEta);
    SetValue("LeptonPhi", lepPhi);
    SetValue("LeptonEnergy", lepEnergy);

    // Safeguard for non-T',B' files. Skip storage if there are no particles
    if(tPrimeID.size() == 0 && bPrimeID.size() == 0) return 0;
    if(tPrimeID.size() > 0 && bPrimeID.size() > 0) cout << "Found both T' and B' " << endl;

    // store variables into the tree
    if(tPrimeID.size() > 0 && bPrimeID.size() == 0){
      SetValue("tPrimeStatus",tPrimeStatus);
      SetValue("tPrimeID",tPrimeID);
      SetValue("tPrimeMass",tPrimeMass);
      SetValue("tPrimePt",tPrimePt);
      SetValue("tPrimeEta",tPrimeEta);
      SetValue("tPrimePhi",tPrimePhi);
      SetValue("tPrimeEnergy",tPrimeEnergy);
      SetValue("tPrimeNDaughters",tPrimeNDaughters);
    }
    if(bPrimeID.size() > 0 && tPrimeID.size() == 0){
      SetValue("bPrimeStatus",bPrimeStatus);
      SetValue("bPrimeID",bPrimeID);
      SetValue("bPrimeMass",bPrimeMass);
      SetValue("bPrimePt",bPrimePt);
      SetValue("bPrimeEta",bPrimeEta);
      SetValue("bPrimePhi",bPrimePhi);
      SetValue("bPrimeEnergy",bPrimeEnergy);
      SetValue("bPrimeNDaughters",bPrimeNDaughters);
    }
    SetValue("quarkID",quarkID);
    SetValue("quarkPt",quarkPt);
    SetValue("quarkEta",quarkEta);
    SetValue("quarkPhi",quarkPhi);
    SetValue("quarkEnergy",quarkEnergy);

    SetValue("bosonID",bosonID);
    SetValue("bosonPt",bosonPt);
    SetValue("bosonEta",bosonEta);
    SetValue("bosonPhi",bosonPhi);
    SetValue("bosonEnergy",bosonEnergy);

    // Now time for some checking -- do we have the right particles? 
    if(quarkID.size() != 2){
      std::cout << "More/less than 2 quarks stored: " << quarkID.size() << std::endl;
      for(size_t i = 0; i < quarkID.size(); i++){std::cout << "quark " << i << " = " << quarkID[i] << std::endl;}

      // First check for opposite sign particles in the first two slots
      // A b-bbar pair can sneak in between the daughters we want
      double test = quarkID[0]*quarkID[1];
      int sign = -1; if(test > 0) sign = 1;

      if(sign > 0){
	if(quarkID.size() == 4){
	  std::swap(quarkID[2],quarkID[3]);
	  std::swap(quarkPt[2],quarkPt[3]);
	  std::swap(quarkEta[2],quarkEta[3]);
	  std::swap(quarkPhi[2],quarkPhi[3]);
	  std::swap(quarkEnergy[2],quarkEnergy[3]);

	  std::cout << "Moved up an opposite sign quark: 3 --> 2" << std::endl;
	}
	std::swap(quarkID[1],quarkID[2]);
	std::swap(quarkPt[1],quarkPt[2]);
	std::swap(quarkEta[1],quarkEta[2]);
	std::swap(quarkPhi[1],quarkPhi[2]);
	std::swap(quarkEnergy[1],quarkEnergy[2]);
	
	std::cout << "Moved up an opposite sign quark: 2 --> 1" << std::endl;

	test = quarkID[0]*quarkID[1];
	sign = -1; if(test > 0) sign = 1;
	if(sign < 0) std::cout << "Signs are fixed!" << std::endl;
      }

      // Now check for t's that come after a b-bbar pair, even if signs are ok
      if(quarkID.size() > 3 && abs(quarkID[3]) == 6){
	std::swap(quarkID[2],quarkID[3]);
	std::swap(quarkPt[2],quarkPt[3]);
	std::swap(quarkEta[2],quarkEta[3]);
	std::swap(quarkPhi[2],quarkPhi[3]);
	std::swap(quarkEnergy[2],quarkEnergy[3]);

	std::cout << "Moved up a top, 3 --> 2" << std::endl;
      }
      if(quarkID.size() > 2 && abs(quarkID[2]) == 6){
	std::swap(quarkID[1],quarkID[2]);
	std::swap(quarkPt[1],quarkPt[2]);
	std::swap(quarkEta[1],quarkEta[2]);
	std::swap(quarkPhi[1],quarkPhi[2]);
	std::swap(quarkEnergy[1],quarkEnergy[2]);

	std::cout << "Moved up a top, 2 --> 1" << std::endl;
      }
    }

    if(bosonID.size() != 2) std::cout << "More/less than 2 bosons stored: " << bosonID.size() << std::endl;

    // tag the decay chains according to ID'd quarks and bosons.
    if(tPrimeID.size() != 0 && bPrimeID.size() == 0){

      // 2 b quarks, check for matching W's
      if(abs(quarkID[0]) == 5 && abs(quarkID[1]) == 5){
	if(abs(bosonID[0]) == 24 && abs(bosonID[1]) == 24) isBWBW = true;
	else std::cout << "2 b daughters didn't match bWbW: " << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // 2 t quarks, check for Z's and H's
      else if(abs(quarkID[0]) == 6 && abs(quarkID[1]) == 6){
	if(bosonID[0] == 23 && bosonID[1] == 23) isTZTZ = true;
	else if(bosonID[0] == 25 && bosonID[1] == 25) isTHTH = true;
	else if(bosonID[0] == 25 && bosonID[1] == 23) isTZTH = true;
	else if(bosonID[0] == 23 && bosonID[1] == 25) isTZTH = true;
	else std::cout << "2 t daughters didn't match tZtZ, tHtH, or tZtH" << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // t-b pairs, check for correlating bosons in the right spots
      else if(abs(quarkID[0]) == 6 && abs(quarkID[1]) == 5){
	if(bosonID[0] == 23 && abs(bosonID[1]) == 24) isTZBW = true;
	else if(bosonID[0] == 25 && abs(bosonID[1]) == 24) isTHBW = true;
	else std::cout << "t - b pair didn't match Z/H - W pair" << bosonID[0] << ", " << bosonID[1] << std::endl; 
      }
      // b-t pairs, check for correlating bosons in the right spots
      else if(abs(quarkID[1]) == 6 && abs(quarkID[0]) == 5){
	if(bosonID[1] == 23 && abs(bosonID[0]) == 24) isTZBW = true;
	else if(bosonID[1] == 25 && abs(bosonID[0]) == 24) isTHBW = true;
	else std::cout << "b - t pair didn't match W - Z/H pair" << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // error messages if we found something else entirely
      else{
	std::cout << "T' daughters didn't match a recognized pattern" << std::endl;
	for(size_t i = 0; i < quarkID.size(); i++){ std::cout << "quark " << i << " = " << quarkID[i] << std::endl; }
	for(size_t i = 0; i < bosonID.size(); i++){ std::cout << "boson " << i << " = " << bosonID[i] << std::endl; }
      }

      // store tag values in the ROOT tree
      SetValue("isTZTZ",isTZTZ);
      SetValue("isTZTH",isTZTH);
      SetValue("isTZBW",isTZBW);
      SetValue("isTHTH",isTHTH);
      SetValue("isTHBW",isTHBW);
      SetValue("isBWBW",isBWBW);

    }

    if(bPrimeID.size() != 0 && tPrimeID.size() == 0){
      // 2 t quarks, check for matching W's
      if(abs(quarkID[0]) == 6 && abs(quarkID[1]) == 6){
	if(abs(bosonID[0]) == 24 && abs(bosonID[1]) == 24) isTWTW = true;
	else std::cout << "2 t daughters didn't match tWtW: " << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // 2 b quarks, check for Z's and H's
      else if(abs(quarkID[0]) == 5 && abs(quarkID[1]) == 5){
	if(bosonID[0] == 23 && bosonID[1] == 23) isBZBZ = true;
	else if(bosonID[0] == 25 && bosonID[1] == 25) isBHBH = true;
	else if(bosonID[0] == 25 && bosonID[1] == 23) isBZBH = true;
	else if(bosonID[0] == 23 && bosonID[1] == 25) isBZBH = true;
	else std::cout << "2 b daughters didn't match bZbZ, bHbH, or bZbH" << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // b-t pairs, check for correlating bosons in the right spots
      else if(abs(quarkID[0]) == 5 && abs(quarkID[1]) == 6){
	if(bosonID[0] == 23 && abs(bosonID[1]) == 24) isBZTW = true;
	else if(bosonID[0] == 25 && abs(bosonID[1]) == 24) isBHTW = true;
	else std::cout << "b - t pair didn't match Z/H - W pair" << bosonID[0] << ", " << bosonID[1] << std::endl; 
      }
      // t-b pairs, check for correlating bosons in the right spots
      else if(abs(quarkID[1]) == 5 && abs(quarkID[0]) == 6){
	if(bosonID[1] == 23 && abs(bosonID[0]) == 24) isBZTW = true;
	else if(bosonID[1] == 25 && abs(bosonID[0]) == 24) isBHTW = true;
	else std::cout << "t - b pair didn't match W - Z/H pair" << bosonID[0] << ", " << bosonID[1] << std::endl;
      }
      // error messages if we found something else entirely
      else{
	std::cout << "B' daughters didn't match a recognized pattern" << std::endl;
	for(size_t i = 0; i < quarkID.size(); i++){ std::cout << "quark " << i << " = " << quarkID[i] << std::endl; }
	for(size_t i = 0; i < bosonID.size(); i++){ std::cout << "boson " << i << " = " << bosonID[i] << std::endl; }
      }

      // store tag values in the ROOT tree
      SetValue("isBZBZ",isBZBZ);
      SetValue("isBZBH",isBZBH);
      SetValue("isBZTW",isBZTW);
      SetValue("isBHBH",isBHBH);
      SetValue("isBHTW",isBHTW);
      SetValue("isTWTW",isTWTW);

    }

    return 0;
}

