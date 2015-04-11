#pragma once
#include "Leap_Fingers.h"
namespace mb = mudbox;
class Leap_Hand
{
  LR lr;
  std::vector<Leap_Fingers *> fingers;
  std::vector<std::vector<Leap_Fingers *> > fings;
  std::vector<std::vector<Leap_Fingers *> > bones;
  Leap_Fingers *palm;
  mb::Vector handRot;
  mb::Vector handPos;
  mb::Node *Node;
  mb::Transformation *TNode;
  mb::GroupNode *GNode;
  mb::Geometry *GeoNode;
  mb::Mesh *MNode;
  ID_List *idList;
  
  bool curVis;
  bool palmOnly;
  bool fingertips;
  bool fingertipsandPalm;
  bool FullHands;
public:
  Leap_Hand(ID_List *idl,LR lOrR);
  Leap_Hand(ID_List *idl,LR lOrR,Leap_Hand *copyHand);
  ~Leap_Hand(void);
  void AddLeap_Bones(void);
  void AddLeap_Fingers(void);
  void AddHand(void);
  Leap_Fingers *GetPalm();
  void CopyHand(Leap_Hand *copyHand);
  float AvgDistFromThumb();
  void SetRot(mb::Vector v);
  void SetRotMatrix(mb::Vector &v);
  void AddRot(mb::Vector v);
  void SetPos(mb::Vector &v);
  mb::Transformation *GetTNode() { return TNode; }
  mb::Vector GetPos();
  void SetVisi(bool vis);
  void UpdateBone(boneEnum b, fingerEnum f, mb::Vector orient);
  void SetFingerPos(fingerEnum fn,mb::Vector &v);
  void SetFingerPos(jointEnum j, fingerEnum f, mb::Vector &v);
  void SetFingerRot(fingerEnum fn,mb::Vector &v);
  void AddFingerRot(fingerEnum fn,mb::Vector v);
  void RotateAroundPivot(jointEnum j,fingerEnum f,mb::Vector a,mb::Vector pivot);
  void RotateAroundPivot(fingerEnum fn,mb::Vector a,mb::Vector pivot);
  void RotateAroundPivot(mb::Vector a,mb::Vector pivot);
  mb::AxisAlignedBoundingBox GetFingerBoundingBox(fingerEnum f, jointEnum j = jointEnum(0));
  mb::Vector GetFingerPos(fingerEnum f, jointEnum j = jointEnum(0));
  mb::Vector GetRot();

  //Collision Detection
  void UpdateCollisionPos(fingerEnum f, mb::Vector &pos, mb::Vector &pivot, mb::Vector &a);
  void UpdateCollisionPos(mb::Vector &pos, mb::Vector &pivot, mb::Vector &a);
  mb::AxisAlignedBoundingBox getCollisionBox(mb::Vector &pos, mb::Vector &pivot, mb::Vector &a);
  mb::AxisAlignedBoundingBox getCollisionBox(fingerEnum f);
  mb::AxisAlignedBoundingBox getCollisionBox();

  
void SetAllFingerJointRots();
/*
  mb::Vector GetFingerPos(fingerEnum fn);*/
//TODO: 
//Set IndexFinger to Thumb Rot and pos

};

