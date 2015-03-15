#pragma once
#include "stdafx.h"

class ID_List {
  //Stores the ID's for all of the nodes.
  //TODO: on close store these values somewhere.
  std::vector<int> HandCamList;
  std::vector<std::vector<std::vector<int> > > FingerList;
  std::vector<std::vector<std::vector<int> > > BoneList;
  std::vector<int> HandList;
  std::vector<int> currentTargets;
  int viewCam;
  int ToolCam;
public:
  ID_List(void);
  int getFinger(fingerEnum fn,jointEnum j, LR lr);
  int getBone(fingerEnum fn,boneEnum j, LR lr);
  int getHand(LR lr);
  int getViewCam(void);
  int getCam(LR lr);
  int getToolCam();
  void setViewCam();
  void storeHandID(int ID,LR lr);
  void storeFingerID(int ID,fingerEnum f, jointEnum j, LR lr);
  void storeBoneID(int ID,fingerEnum f, boneEnum j, LR lr);
  void storeHandCamID(int ID,LR lr);
  void storeViewCamID(int ID);
  void addTargetID(int ID);
  void storeTargetID(std::vector<int> ID);
  void setToolCam(int camID);
  std::vector<int> getCurrentTagets(void);
  ~ID_List(void);
};

