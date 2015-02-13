#pragma once
#include "stdafx.h"
class ID_List {
  //Stores the ID's for all of the nodes.
  //TODO: on close store these values somewhere.
  std::vector<int> HandCamList;
  std::vector<int> FingerList;
  std::vector<int> HandList;
  std::vector<int> currentTargets;
public:
  ID_List(void);
  int getFinger(fingerEnum fn,LR lr);
  int getHand(LR lr);
  int getViewCam(void);
  int getCam(LR lr);
  void storeHandID(int ID,LR lr);
  void storeFingerID(int ID,fingerEnum fing, LR lr);
  void storeHandCamID(int ID,LR lr);
  void storeViewCamID(int ID);
  void addTargetID(int ID);
  void storeTargetID(std::vector<int> ID);
  std::vector<int> getCurrentTagets(void);
  ~ID_List(void);
};

