#include "stdafx.h"
#include "ID_List.h"
#include "cameraWrapper.h"

ID_List::ID_List(void) {
  HandCamList.resize(2,-1);
  FingerList.resize(2);
  BoneList.resize(2);
  for(int i = 0 ; i < 2 ; i++) {
    FingerList.at(i).resize(5);
    BoneList.at(i).resize(5);
    for(int j = 0 ; j < 5 ; j++) {
      FingerList.at(i).at(j).resize(4);
      BoneList.at(i).at(j).resize(4);
    }
  }
  HandList.resize(2,-1);
  viewCam = mudbox::Kernel()->Scene()->ActiveCamera()->ID();
  cameraWrapper *viewCam_tmp = new cameraWrapper(viewCam);
  viewCam_tmp->getTNode()->SetRotation(mb::Vector(0,0,0));
  viewCam_tmp->getTNode()->SetPosition(mb::Vector(0,0,800));
}

int ID_List::getFinger(fingerEnum fn, jointEnum j, LR lr) {
  return FingerList.at(lr).at(fn).at(j);
}

int ID_List::getBone(fingerEnum fn, boneEnum j, LR lr) {
  return BoneList.at(lr).at(fn).at(j);
}

int ID_List::getHand(LR lr) {
  return HandList.at(lr);
}
int ID_List::getViewCam(void) {
  return viewCam;
 // return mudbox::Kernel()->Scene()->ActiveCamera()->ID();
}
int ID_List::getCam(LR lr) {
  return HandCamList.at(lr);
}
  
std::vector<int> ID_List::getCurrentTagets(void) {
  return currentTargets;
}
  
void ID_List::storeHandID(int ID,LR lr) {
  HandList.at(lr) = ID;
}

void ID_List::storeFingerID(int ID,fingerEnum fn,jointEnum j, LR lr) {
  FingerList.at(lr).at(fn).at(j) = ID;
}

void ID_List::storeBoneID(int ID,fingerEnum fn,boneEnum j, LR lr) {
  BoneList.at(lr).at(fn).at(j) = ID;
}

void ID_List::storeHandCamID(int ID,LR lr) {
  HandCamList.at(lr) = ID;
}

void ID_List::addTargetID(int ID) {
  currentTargets.push_back(ID);
}

void ID_List::storeTargetID(std::vector<int> ID) {
  currentTargets.resize(ID.size());
  for(int i = 0; i < ID.size() ; i++) {
    currentTargets.at(i) = ID.at(i);
  }
}


ID_List::~ID_List(void)
{
}
