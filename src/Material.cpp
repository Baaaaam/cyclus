// Material.cpp
#include "Material.h"

#include "BookKeeper.h"
#include "CycException.h"
#include "MassTable.h"
#include "Timer.h"
#include "Env.h"
#include "UniformTaylor.h"
#include "InputXML.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;

// Static variables to be initialized.
int Material::nextID_ = 0;

std::vector<Material*> Material::materials_;

bool Material::decay_wanted_ = false;

int Material::decay_interval_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material() {
  ID_ = nextID_++;
  last_update_time_ = TI->getTime();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(IsoVector comp) {
  ID_ = nextID_++;
  last_update_time_ = TI->getTime();
  iso_vector_ = comp;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(Material* matToAdd) {
  // Get the given Material's composition.
  iso_vector_ = iso_vector_ + matToAdd->isoVector();

  delete matToAdd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extract(double mass) {
  IsoVector new_comp = iso_vector_;
  new_comp.setMass(mass);

  iso_vector_ = iso_vector_ - new_comp;
  
  return new Material(new_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extract(IsoVector rem_comp) {
  iso_vector_ = iso_vector_ - rem_comp;
  return new Material(rem_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material* Material::clone() {
  return new Material(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuality(Resource* other){
  // This will be true until proven false
  bool toRet = true;

  // Make sure the other is a material
  try{
    IsoVector second_comp = dynamic_cast<Material*>(other)->isoVector();
    // We need to check the recipe, isotope by isotope
    CompMap::const_iterator iso_iter = iso_vector_.comp().begin();
    while (iso_iter != iso_vector_.comp().end()){
      if( second_comp.atomCount(iso_iter->first) != iso_iter->second){
        toRet = false;
        break;
      }
      iso_iter++;
    }
  } catch (Exception& e) {
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityEqual(Resource* other){
  // This will be true until proven false
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = dynamic_cast<Material*>(other)->getQuantity();
    toRet = getQuantity() - second_qty < EPS_KG;
  } catch (Exception& e) {
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityGT(Resource* other){
  // true if the total atoms in the other is greater than in the base.
  // This will be true until proven false
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = dynamic_cast<Material*>(other)->getQuantity();
    toRet = second_qty - getQuantity() > EPS_KG;
  } catch (Exception& e){
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
  int curr_time = TI->getTime();
  int delta_time = curr_time - last_update_time_;
  
  iso_vector_.executeDecay(delta_time);

  last_update_time_ = curr_time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decayMaterials(int time) {
  // if decay is on
  if (decay_wanted_) {
    // and if (time(mod interval)==0)
    if (time % decay_interval_ == 0) {
      // acquire a list of all materials
      for (vector<Material*>::iterator mat = materials_.begin();
          mat != materials_.end();
          mat++){
         // and decay each of them
         (*mat)->decay();
      }
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::setDecay(int dec) {
  if ( dec <= 0 ) {
    decay_wanted_ = false;
  } else if ( dec > 0 ) {
    decay_wanted_ = true;
    decay_interval_ = dec;
  }
}
