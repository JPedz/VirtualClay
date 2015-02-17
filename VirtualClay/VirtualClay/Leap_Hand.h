#pragma once
#include "Leap_Fingers.h"
namespace mb = mudbox;
class Leap_Hand
{
  LR lr;
  std::vector<Leap_Fingers *> fingers;
  Leap_Fingers *palm;
  mb::Vector handRot;
  mb::Vector handPos;
  mb::Node *Node;
  mb::Transformation *TNode;
  mb::GroupNode *GNode;
  mb::Geometry *GeoNode;
  mb::Mesh *MNode;
  ID_List *idList;
public:
  Leap_Hand(ID_List *idl,LR lOrR);
  ~Leap_Hand(void);
  void AddLeap_Fingers(void);
  void AddHand(void);
  void SetRot(mb::Vector v);
  void SetPos(mb::Vector v);
  void SetVisi(bool vis);
  void SetFingerPos(fingerEnum fn,mb::Vector v);
  void SetFingerRot(fingerEnum fn,mb::Vector v);
  void RotateAroundPivot(fingerEnum fn,mb::Vector a,mb::Vector pivot);
  void RotateAroundPivot(mb::Vector a,mb::Vector pivot);
/*
  mb::Vector GetFingerPos(fingerEnum fn);*/
//TODO: 
//Set IndexFinger to Thumb Rot and pos

};

