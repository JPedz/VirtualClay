#include "stdafx.h"
#include "Leap_Hand.h"

Leap_Hand::Leap_Hand(ID_List *idl,LR lOrR)
{
  lr = lOrR;
  fingers.resize(5);
  fings.resize(5);
  bones.resize(5);
  for(int i = 0; i < 5;i++) {
    fings.at(i).resize(4);
    bones.at(i).resize(4);
  }

  idList = idl;
  AddHand();
  AddLeap_Fingers();
}

Leap_Hand::Leap_Hand(ID_List *idl,LR lOrR,Leap_Hand *copyHand)
{
  lr = lOrR;
  fingers.resize(5);
  fings.resize(5);
  bones.resize(5);
  for(int i = 0; i < 5;i++) {
    fings.at(i).resize(4);
    bones.at(i).resize(4);
  }

  idList = idl;
  AddHand();
  AddLeap_Fingers();
}

void Leap_Hand::CopyHand(Leap_Hand *copyHand) {
  palm = new Leap_Fingers();
  int id = palm->CopyGeo(copyHand->GetPalm());
  idList->storeHandID(id,lr);
  TNode = palm->getTNode();
  TNode->SetName("Palm"+QString::number(id));
  palm->SetScale(mb::Vector(0.07f,0.02f,0.07f));
}

void Leap_Hand::AddHand(void) {
  palm = new Leap_Fingers();
  int id = palm->ImportGeo();
  idList->storeHandID(id,lr);
  TNode = palm->getTNode();
  TNode->SetName("Palm"+QString::number(id));
  palm->SetScale(mb::Vector(0.07f,0.02f,0.07f));
}

Leap_Fingers *Leap_Hand::GetPalm() {
  return palm;
}

void Leap_Hand::AddLeap_Fingers(void) {
  mb::Kernel()->Interface()->ProgressStart("Creating Hands",20);
  QString lOrR = "L_";
  if(lr == r) {
    lOrR = "R_";
  }
  QString fingerType;
  QString fingerJoint;
  QString boneName;
  for(int i = 0; i < 5 ; i++) {
    switch(i) {
      case(0):
        fingerType = "Thumb_";
        break;
      case(1):
        fingerType = "Index_";
        break;
      case(2):
        fingerType = "Middle_";
        break;
      case(3):
        fingerType = "Ring_";
        break;
      case(4):
        fingerType = "Pinky_";
        break;
    }
    for(int j = 0; j < 4 ; j++) {
      switch(j) {
      case(0):
        fingerJoint = "Tip";
        boneName = "Distal";
        break;
      case(1):
        fingerJoint = "MIP";
        boneName = "Inter";
        break;
      case(2):
        fingerJoint = "PIP";
        boneName = "Phalanx";
        break;
      case(3):
        fingerJoint = "MCP";
        boneName = "Meta";
        break;
      }
      bones.at(fingerEnum(i)).at(jointEnum(j)) = new Leap_Fingers(lOrR+fingerType+boneName);
      fings.at(fingerEnum(i)).at(jointEnum(j)) = new Leap_Fingers(lOrR+fingerType+fingerJoint);
      //if(j == 0) {
        //TODO: REmove this limitation to only tip
        //#Code:111
        int id;
        //if(i == 0 && j == 0) {
        //  id = fings.at(i).at(j)->ImportGeo();
        //} else {
          id = fings.at(i).at(j)->CopyGeo(palm);
        //}
        idList->storeFingerID(id,fingerEnum(i),jointEnum(j),lr);
        //TODO: Store ID's
        fings.at(i).at(j)->SetScale(mb::Vector(0.03f,0.03f,0.03f));
      
        //int idBone;
        //idBone = bones.at(i).at(j)->ImportGeo();
        //mblog("Storing BoneID \n");
        //idList->storeBoneID(idBone,fingerEnum(i),boneEnum(j),lr);
        //mblog("Stored BoneID \n");
        //bones.at(i).at(j)->SetScale(mb::Vector(0.2f,0.02f,0.02f));
     // }
      mb::Kernel()->Interface()->ProgressAdd();
    }
  }
  mb::Kernel()->Interface()->ProgressEnd();
}

void Leap_Hand::UpdateBone(boneEnum b, fingerEnum f, mb::Vector orient) {
  //#Code:111
  mb::Vector tmp;
  if(b == boneEnum(0)) {
    bones.at(f).at(b)->SetRot(orient);
    if(b != 3) {
      tmp = fings.at(f).at(b+1)->GetPos();
      mblog("b+1 = :"+QString::number(b+1)+"\n");
      bones.at(f).at(b)->SetPos(tmp);
    } else {
        tmp = palm->GetPos();
        bones.at(f).at(b)->SetPos(tmp);
    }
  }
}


void Leap_Hand::SetFingerPos(jointEnum j, fingerEnum f, mb::Vector &v) {
  fings.at(f).at(j)->SetPos(v);
}

void Leap_Hand::SetFingerPos(fingerEnum f, mb::Vector &v) {
  fingers.at(f)->SetPos(v);
}

void Leap_Hand::SetFingerRot(fingerEnum f, mb::Vector &v) {
  fingers.at(f)->SetRot(v);
}

void Leap_Hand::AddFingerRot(fingerEnum f, mb::Vector v) {
  fingers.at(f)->AddRot(v);
}

void Leap_Hand::SetRot(mb::Vector v) {
  TNode->SetRotation(v);
}

void Leap_Hand::SetRotMatrix(mb::Vector &rotation) {
  if(TNode != NULL) {
    mb::Matrix rX = createRotateXMatrix(rotation.x);
    mb::Matrix rY = createRotateYMatrix(rotation.y);
    mb::Matrix rZ = createRotateZMatrix(rotation.z);
    mb::Matrix rotationMatrix = rX*rZ*rY;
    TNode->SetRotation(rotationMatrix);
  }
}

float Leap_Hand::AvgDistFromThumb() {
  float sumdist = 0;
  const mb::Vector thumb = fings.at(0).at(0)->GetPos();
  for(int f = 1 ; f < 5 ; f++) {
    sumdist += fings.at(f).at(0)->GetPos().DistanceFrom(thumb);
  }
  return sumdist*0.25;
}

void Leap_Hand::AddRot(mb::Vector v) {
  TNode->AddRotation(v,true);
}

void Leap_Hand::SetPos(mb::Vector &v) {
  TNode->SetPosition(v);
}

void Leap_Hand::SetVisi(bool vis) {
  TNode->SetVisible(vis);
  for(int i = 0 ; i < 5 ; i++) {
    for(int j = 0 ; j < 4 ; j++) {
      //TODO: Remove finger tip limitation #Code:111
//      if(j <= 1) {
        fings.at(i).at(j)->SetVisi(vis);
//      }
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

void Leap_Hand::UpdateCollisionPos(fingerEnum f, mb::Vector &pos, mb::Vector &pivot, mb::Vector &a) {
  fings.at(f).at(0)->UpdateCollisionPos(pos,pivot,a);
}

void Leap_Hand::UpdateCollisionPos(mb::Vector &pos, mb::Vector &pivot, mb::Vector &a) {
  palm->UpdateCollisionPos(pos,pivot,a);
}

mb::AxisAlignedBoundingBox Leap_Hand::getCollisionBox(fingerEnum f)  {
  return fings.at(f).at(0)->GetCollisionBox();
}

mb::AxisAlignedBoundingBox Leap_Hand::getCollisionBox(mb::Vector &pos, mb::Vector &pivot, mb::Vector &a) {
  mb::Vector fakePosVect = (RotateVectorAroundPivot(pos,pivot,a));
  mblog("FakePosVect = "+VectorToQStringLine(fakePosVect));
  return mb::AxisAlignedBoundingBox(fakePosVect,1.0f);
}

mb::AxisAlignedBoundingBox Leap_Hand::getCollisionBox() {
  return palm->GetCollisionBox();
}
Leap_Hand::~Leap_Hand(void)
{
}
