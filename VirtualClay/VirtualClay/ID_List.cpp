#include "stdafx.h"
#include "ID_List.h"


ID_List::ID_List(void) {
  viewCam = -1;
  HandCamList.resize(2,-1);
  FingerList.resize(10,-1);
  HandList.resize(2,-1);
}

int ID_List::getFinger(fingerEnum fn,LR lr) {
  return FingerList.at(fn*(lr+1));
}
int ID_List::getHand(LR lr) {
  return HandList.at(lr);
}
int ID_List::getViewCam(void) {
  return viewCam;
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

void ID_List::storeFingerID(int ID,fingerEnum fn, LR lr) {
  FingerList.at(fn*(lr+1)) = ID;
}

void ID_List::storeHandCamID(int ID,LR lr) {
  HandCamList.at(lr) = ID;
}

void ID_List::storeViewCamID(int ID) {
  viewCam = ID;
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
