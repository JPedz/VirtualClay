#include "stdafx.h"
#include "Leap_Hand.h"


Leap_Hand::Leap_Hand(ID_List *idl,LR lOrR)
{
  lr = lOrR;
  fingers.resize(5);
  fings.resize(5);
  for(int i = 0; i < 5;i++) {
    fings.at(i).resize(3);
  }
  idList = idl;
  AddHand();
  AddLeap_Fingers();
}

void Leap_Hand::AddHand(void) {
  palm = new Leap_Fingers();
  int id = palm->ImportGeo();
  idList->storeHandID(id,lr);
  TNode = palm->getTNode();
  TNode->SetName("Palm"+QString::number(id));
  palm->SetScale(mb::Vector(0.07f,0.02f,0.07f));
}

void Leap_Hand::AddLeap_Fingers(void) {
  mblog("Leap_Fingers size:"+QString::number(fingers.size()));
  QString lOrR = "L_";
  if(lr == r) {
    lOrR = "R_";
  }
  QString fingerType;
  QString fingerJoint;
  for(int i = 0; i < 5 ; i++) {
    switch(i) {
      case(0):
        fingerType = "Thumb";
        break;
      case(1):
        fingerType = "Index";
        break;
      case(2):
        fingerType = "Middle";
        break;
      case(3):
        fingerType = "Ring";
        break;
      case(4):
        fingerType = "Pinky";
        break;

    }
    for(int j = 0; j < 3 ; j++) {

      switch(j) {
      case(0):
        fingerJoint = "Tip";
        break;
      case(1):
        fingerJoint = "MIP";
        break;
      case(2):
        fingerJoint = "PIP";
        break;
      }
      fings.at(fingerEnum(i)).at(jointEnum(j)) = new Leap_Fingers(lOrR+fingerType+"_"+fingerJoint);
        
      if(j == 0) {
        //TODO: REmove this limitation to only tip
        //#Code:111
        int id;
        id = fings.at(i).at(j)->ImportGeo();
        idList->storeFingerID(id,fingerEnum(i),lr);
        //TODO: Store ID's
        fings.at(i).at(j)->SetScale(mb::Vector(0.03f,0.03f,0.03f));
      }
    }
  }
  /*fingers.at(INDEX) = new Leap_Fingers(lOrR+"Index");
  fingers.at(MIDDLE) = new Leap_Fingers(lOrR+"Middle");
  fingers.at(RING) = new Leap_Fingers(lOrR+"Ring");
  fingers.at(PINKY) = new Leap_Fingers(lOrR+"Pinky");
  fingers.at(THUMB) = new Leap_Fingers(lOrR+"Thumb");
  int id = -1;
  for(int i = 0; i < 5; i++) {
    id = fingers.at(i)->ImportGeo();
    idList->storeFingerID(id,fingerEnum(i),lr);
    fingers.at(i)->SetPos(mb::Vector(i*20.0f,0.0f,0.0f));
    fingers.at(i)->SetScale(mb::Vector(0.01f,0.01f,0.01f));
  }*/
}
void Leap_Hand::SetFingerPos(jointEnum j, fingerEnum f, mb::Vector v) {
  fings.at(f).at(j)->SetPos(v);
}



void Leap_Hand::SetFingerPos(fingerEnum f, mb::Vector v) {
  fingers.at(f)->SetPos(v);
}

void Leap_Hand::SetFingerRot(fingerEnum f, mb::Vector v) {
  fingers.at(f)->SetRot(v);
}

void Leap_Hand::SetRot(mb::Vector v) {
  TNode->SetRotation(v);
}

void Leap_Hand::SetPos(mb::Vector v) {
  TNode->SetPosition(v);
}

void Leap_Hand::SetVisi(bool vis) {
  TNode->SetVisible(vis);
  for(int i = 0 ; i < 5 ; i++) {
    for(int j = 0 ; j < 3 ; j++) {
      //TODO: Remove finger tip limitation #Code:111
      if(j == 0) {
        fings.at(i).at(j)->SetVisi(vis);
      }
    }
  }
}

void Leap_Hand::RotateAroundPivot(jointEnum j,fingerEnum f,mb::Vector a,mb::Vector pivot) {
  //mblog("Finger: "+QString::number(f)+"\n");
  fings.at(f).at(j)->RotateAroundPivot(a,pivot);
}

void Leap_Hand::RotateAroundPivot(fingerEnum f,mb::Vector a,mb::Vector pivot) {
  //mblog("Finger: "+QString::number(f)+"\n");
  fingers.at(f)->RotateAroundPivot(a,pivot);
}

void Leap_Hand::RotateAroundPivot(mb::Vector a,mb::Vector pivot) {
  palm->RotateAroundPivot(a,pivot);
}

mb::Vector Leap_Hand::GetRot() {
  return palm->GetRot();
}

//
//mb::Vector Leap_Hand::GetFingerPos(fingerEnum fn) {
//  return fingers.at(fn)->GetPos();
//}

mb::AxisAlignedBoundingBox Leap_Hand::GetFingerBoundingBox(fingerEnum f, jointEnum j) {
  return fings.at(f).at(j)->GetBoundingBox();
}

mb::Vector Leap_Hand::GetPos() {
  return TNode->Position();
}

mb::Vector Leap_Hand::GetFingerPos(fingerEnum f, jointEnum j) {
  return fings.at(f).at(j)->GetPos();
}
Leap_Hand::~Leap_Hand(void)
{
}
