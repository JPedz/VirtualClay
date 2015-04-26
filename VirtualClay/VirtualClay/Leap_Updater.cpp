#include "stdafx.h"
#include "Leap_Updater.h"

namespace mb = mudbox;

Leap_Updater::Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r)
  :frameEvent(this)
{
  uniqueMiss = 0;
  uniqueMissBool = true;
  missCounter = 0;
  //To resize the image, use Image.GenerateUpscaledImage(*targetImg,factor);
  mblog("Creating texture\n");
  idList = idl;
  hand_l = l;
  hand_r = r;
  mblog("Creating tool\n");
  tool = new Leap_Tool(hand_l);
  mblog("Created tool\n");
  tool->SetStamp(RESOURCESDIR+"stamp1.png");
  meshOp = new MeshOps();
  //meshOp_R = new MeshOps();
  bigTimer = new QTime();
  //mblog("Listing Nodes\n");
  //for(mb::Node *nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
  //  mblog("Node: "+nodes->Name()+" "+QString::number(nodes->ID())+"\n");
  //}
  //mblog("Listed Nodes\n");
  meshOp->ChangeCamera(new cameraWrapper(idList->getCam(LR(0))));
  //meshOp_R->ChangeCamera(new cameraWrapper(idList->getCam(LR(1))));
  leapReader = new Leap_Reader();
  leapReader->SetScale(mb::Vector(1,1,1));
  frameEvent.Connect(mb::Kernel()->ViewPort()->FrameEvent);
  menuFilter = new Leap_HUD();
  //brushIcon = new Leap_HUD();
  gestureHUD = new GestureHUD();
  //brushIcon->menuChoice = 22;
  mb::Kernel()->ViewPort()->AddFilter(menuFilter);
  //mb::Kernel()->ViewPort()->AddFilter(brushIcon);
  mb::Kernel()->ViewPort()->AddFilter(gestureHUD);
  gestureHUD->SetVisible(false);
  menuFilter->SetVisible(false);
  //brushIcon->SetVisible(true);
  brushSize = 30.0f;
  brushStrength = 10.0f;
  inMenu_L = false;
  inMenu_R = false;
  menuLeft = false;
  menuDown = false;
  menuRight_0 = false;
  menuUp_0 = false;
  menuRight_1 = false;
  menuUp_1 = false;
  menuDown_0 = false;
  menuDown_1 = false;
  reqIntersectionForSelection = false;
  thumbGrabModeToggle = false;
  thumbDirectionBasedMovement = true;
  //stickyMovement = false;
  selectWithBrushSize = true;
  pinchGrab = true;
  toolStamp = true;
  brushSizeMenuToggle = false;
  brushStrengthMenuToggle = false;
  SceneNavigationToggle = true;
  pivotHandsOnMesh = true;
  moveObjectMode = false;
  firstPan_L = true;
  firstPan_R = true;
  savedPanHandPosition_L = mb::Vector(0,0,0);
  savedPanHandPosition_R = mb::Vector(0,0,0);
  savedHandPivotPoint = mb::Vector(0,0,0);
  brushStrengthFingerStartPos = mb::Vector(0,0,0);
  brushSizeStartFingerStartPos = mb::Vector(0,0,0);
  GimbalLockZXYMode_L = false;
  GimbalLockZXYMode_R = false;
  
  bigTimer->start();

}

mb::Vector Leap_Updater::fitToCameraSpace() {
  mb::Vector camPos = viewCam->getPosition();
  mb::Vector pO = mb::Vector(0,200,300);
  mb::Vector heightOffset = mb::Vector(0,200,0);
  mb::Vector camForward = viewCam->getForward();
  int forwardfactor = (int)(pO.Length());
  mb::Vector centrePoint = camPos - (camForward*forwardfactor) - heightOffset;
  return centrePoint;
}

void Leap_Updater::rotateCamera(mb::Vector r) {
  mb::Vector camPos = viewCam->getTNode()->Position();
  mb::Vector aimPoint = viewCam->getCamera()->Aim();
  mb::Camera *viewCam_Cam = viewCam->getCamera();
  viewCam->setPosition(RotateVectorAroundPivot(camPos,aimPoint,r));
  viewCam_Cam->SetTarget(aimPoint);
}

void Leap_Updater::ScreenTap(LR lr) {
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos;
  if(lr == l) {
    indexPos = hand_l->GetFingerPos(INDEX,TIP);
    hand_l->SetVisi(false);
  } else {
    indexPos = hand_r->GetFingerPos(INDEX,TIP);
    hand_r->SetVisi(false);
  }

  mb::Vector projPos = viewCam->getCamera()->Project(indexPos);
  projPos = projPos * mb::Vector(1,-1,1);
  mblog(VectorToQString(indexPos)+"Pos"+VectorToQStringLine(projPos));
  meshOp->SelectObject(viewCam,projPos);
  //meshOp_R->SelectObject(viewCam,projPos);
  if(lr == l) {
    hand_l->SetVisi(true);
  } else {
    hand_r->SetVisi(true);
  }
}

mb::Vector Leap_Updater::GetRelativeScreenSpaceFromWorldPos(mb::Vector wPos) {
  mb::Camera *activeCam = mb::Kernel()->Scene()->ActiveCamera();
  return activeCam->Project(wPos);
}

bool Leap_Updater::selectMeshPinch() {
  meshOp->ChangeCamera(viewCam);
  mb::Vector indexPos = hand_l->GetFingerPos(INDEX);
  mb::Vector thumbPos = hand_l->GetFingerPos(THUMB);
  if(indexPos.DistanceFrom(thumbPos) < 10) {
    mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos);
    mb::Vector indexProj = viewCam->getCamera()->Project(indexPos);
    mb::Vector midPoint = (thumbProj+indexProj)*0.5f;
    return meshOp->SelectFaces(l,midPoint,0.0f,0.0f);
  } else {
    mblog("Fingers too far apart");
    return false;
  }
}

bool Leap_Updater::selectMesh(LR lOrR) {
  int handCamID = idList->getCam(lOrR);
  cameraWrapper *handCam = new cameraWrapper(handCamID);
  bool b =false;
  float avgSize;
  meshOp->ChangeCamera(handCam);
  mb::Vector zeroVector = mb::Vector(0.0f,0.0f,0.0f);
  
  if(lOrR == l) {
    //hand_l->SetVisi(false);
    hand_l->SetPos(zeroVector);
    avgSize = hand_l->AvgDistFromThumb();
  } else {
    //hand_r->SetVisi(false);
    hand_r->SetPos(zeroVector);
    avgSize = hand_r->AvgDistFromThumb();
  }
  if(selectWithBrushSize) 
    avgSize = brushSize;
  mb::Kernel()->Scene()->SetActiveCamera(handCam->getCamera());
  mb::Kernel()->ViewPort()->Redraw();
  mblog("Brush Size = "+QString::number(avgSize));
  b = meshOp->SelectFaces(lOrR,avgSize,brushStrength);
  //if(lOrR == l) {
  //  hand_l->SetVisi(true);
  //} else {
  //  hand_r->SetVisi(true);
  //}
  mb::Kernel()->Scene()->SetActiveCamera(viewCam->getCamera());
  mb::Kernel()->ViewPort()->Redraw();
  return b;
}

void Leap_Updater::MoveMesh(LR lOrR) {
  mb::Vector currentHandPos;
  mb::Vector distanceDiff;
  if(lOrR == l) {
  currentHandPos = hand_l->GetPos();
    distanceDiff = currentHandPos - lastFrameHandPos_L;
    lastFrameHandPos_L = currentHandPos;
  } else {
    currentHandPos = hand_r->GetPos();
    distanceDiff = currentHandPos - lastFrameHandPos_R;
    lastFrameHandPos_R = currentHandPos;
  }
  if(distanceDiff.x > 10 || distanceDiff.y > 10 || distanceDiff.z > 10) {
    distanceDiff = mb::Vector(0,0,0);
  }
  //mblog("Moving Mesh currentHandPos: "+VectorToQString(currentHandPos)+
  //  "lastFrameHandPos_L: "+VectorToQString(lastFrameHandPos_L)+
  //  "DistanceDiff: "+VectorToQStringLine(distanceDiff));
  meshOp->MoveVertices(lOrR,distanceDiff);
}

int Leap_Updater::countIntersectingFingers(LR lOrR) {
  int counter = 0;
  if(lOrR == l) {
    for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  } else {
    for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  }
  return counter;
}

int Leap_Updater::countTouchingFingers(LR lOrR) {
  int counter = 0;
  if(lOrR == l) {
    for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckTouching(hand_l->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  } else {
    for(int i = 0; i < 5 ; i++) {
      if(meshOp->CheckTouching(hand_r->GetFingerBoundingBox(fingerEnum(i)))) {
        counter++;
      }
    }
  }
  return counter;
}

bool Leap_Updater::ThumbSelect() {
  mb::Vector thumbPos = hand_l->GetFingerPos(THUMB,TIP);
  mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos);
  hand_l->SetVisi(false);
  mblog("Thumb Proj Pos = "+VectorToQStringLine(thumbProj));
  mblog("Thumb Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(thumbProj)));
  //float save = thumbProj.x;
  //thumbProj.x = thumbProj.y;
  //thumbProj.y = save;
  meshOp->ChangeCamera(viewCam);
  if(!facesAreSelected_L&& meshOp->SelectFaces(l,ScreenSpaceToPixels(thumbProj),10.0f,5)) {
    facesAreSelected_L= true;
  } else {
    mb::Vector currentThumbPos = hand_l->GetFingerPos(THUMB,TIP);
    mb::Vector distanceDiff = currentThumbPos - lastFrameThumbPos;
    if(distanceDiff.x > 10 || distanceDiff.y > 10 || distanceDiff.z > 10) {
      distanceDiff = mb::Vector(0,0,0);
    }
    //mblog("Moving Mesh currentHandPos: "+VectorToQString(currentHandPos)+
    //  "lastFrameHandPos_L: "+VectorToQString(lastFrameHandPos_L)+
    //  "DistanceDiff: "+VectorToQStringLine(distanceDiff));
    meshOp->MoveVertices(l,distanceDiff);
    lastFrameThumbPos = currentThumbPos;
    mbstatus("Moving faces");
    mblog("Moving Faces");
  }
  return true;
}

void Leap_Updater::ThumbDirMove(LR lr) {
  mb::Vector thumbPos;
  if(lr == l) {
    thumbPos = hand_l->GetFingerPos(THUMB,TIP);
    hand_l->SetVisi(false);
  } else {
    thumbPos = hand_r->GetFingerPos(THUMB,TIP);
    hand_r->SetVisi(false);
  }
  mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos) * mb::Vector(1,-1,1);
  mblog("Thumb Proj Pos = "+VectorToQStringLine(thumbProj));
  mblog("Thumb Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(thumbProj)));
  meshOp->ChangeCamera(viewCam);
  if(meshOp->SelectFaces(lr,ScreenSpaceToPixels(thumbProj),10.0f,brushSize/4)) {
    mb::Vector dirNorm = leapReader->getMotionDirection(THUMB,lr);
    mblog("Normalised Direction = "+VectorToQStringLine(dirNorm));
    mblog("Brush Strength= "+QString::number(brushStrength));
    mb::Vector dist = dirNorm*3;
    meshOp->MoveVertices(lr,dist,false);
  }
  if(lr == l) 
    hand_l->SetVisi(true);
  else
    hand_r->SetVisi(true);
}

void Leap_Updater::ThumbSmoothMove(LR lr) {
  mb::Vector thumbPos;
  if(lr == l) {
    thumbPos = hand_l->GetFingerPos(THUMB,TIP);
    hand_l->SetVisi(false);
  } else {
    thumbPos = hand_r->GetFingerPos(THUMB,TIP);
    hand_r->SetVisi(false);
  }
  mb::Vector thumbProj = viewCam->getCamera()->Project(thumbPos) * mb::Vector(1,-1,1);

  meshOp->ChangeCamera(viewCam);
  
  if(meshOp->SelectFaces(lr,ScreenSpaceToPixels(thumbProj),10.0f,brushSize/4)) {
    facesAreSelected_Tool = true;
    mb::Vector dirNorm = leapReader->getToolMotionDirection();
    mblog("Normalised Direction = "+VectorToQStringLine(dirNorm));
    float dist = brushStrength*2;
    meshOp->MoveVerticesNormal(r,dist,thumbPos);
  }
  tool->SetVisi(true);
}

__inline void Leap_Updater::SetHandAndFingerPositions() {
  //TODO:
  // Do I need to actually rotate the fingers?? If so, by what metric?
  // hand_l->SetFingerRot(fingerEnum(i),leapReader->getFingerDirection_L(fingerEnum(i)));
  QTime *overall = new QTime();
  overall->start();
  QTime *t = new QTime();
  t->start();
  mb::Vector camRot = viewCam->getTNode()->Rotation();
  mb::Vector handOffset = mb::Vector(70,0,0);

  
  mb::Vector invertRoll = mb::Vector (1,1,1);
  if(viewCam->getTNode()->Position().z < 0) {
    invertRoll = mb::Vector(1,1,-1);
  }
  
  hand_l->SetVisi(leapReader->isVisible(l));
  hand_r->SetVisi(leapReader->isVisible(r));
  //mblog(" Set Visi Time: "+QString::number(t->elapsed())+"\n");
  t->restart();
  int leftCamID = idList->getCam(l);
  int rightCamID = idList->getCam(r);
  //mblog(" get Cam IDs Time: "+QString::number(t->elapsed())+"\n");
  
  // Set hand position and orientation
  mb::Vector tmp = cameraPivot + leapReader->getPosition_L();
  hand_l->SetPos(tmp);
  tmp = cameraPivot + leapReader->getPosition_R();
  hand_r->SetPos(tmp);
  
  hand_l->RotateAroundPivot(-1*camRot,cameraPivot);
  hand_r->RotateAroundPivot(-1*camRot,cameraPivot);
  //mblog(" Step 1 Time: "+QString::number(t->elapsed())+"\n");
  t->restart();
  //Rotate the hands XZY
  mb::Vector rotation = (mb::Vector(1,1,-1)*camRot) + leapReader->getDirection_L()*invertRoll;
  mb::Matrix rX = createRotateXMatrix(rotation.x-20);
  mb::Matrix rY = createRotateYMatrix(rotation.y);
  mb::Matrix rZ = createRotateZMatrix(rotation.z);
  mb::Matrix rotationMatrix;
  if(!GimbalLockZXYMode_L) {
    rotationMatrix = rX*rY*rZ;
    hand_l->GetTNode()->SetRotation(rotationMatrix);
    if((int(hand_l->GetRot().y)%180 < 135) && ( int(hand_l->GetRot().y)%180 > 45))
    {
     GimbalLockZXYMode_L = true;
     rotationMatrix = rZ*rX*rY;
    }
    else {
     rotationMatrix = rX*rY*rZ;
    }
    mbhud("GimbalLock ON");
  } else {
    rotationMatrix = rZ*rX*rY;
    hand_l->GetTNode()->SetRotation(rotationMatrix);
    if(!((int(hand_l->GetRot().x)%180 < 135) && ( int(hand_l->GetRot().x)%180 > 45)))
    {
     GimbalLockZXYMode_L = false;
     rotationMatrix = rX*rY*rZ;
    }
    else {
     rotationMatrix = rZ*rX*rY;
    }
    mbhud("GimbalLock OFF");
  }
  hand_l->GetTNode()->SetRotation(rotationMatrix);

  
  
  mb::Vector rotation_r = (mb::Vector(1,1,-1)*camRot) + leapReader->getDirection_R()*invertRoll;
  //rotation_r = GetAimRotation(hand_r->GetPos(),hand_r->GetFingerPos(INDEX))+mb::Vector(90,0,0);
  mb::Matrix rX_r = createRotateXMatrix(rotation_r.x-20);
  mb::Matrix rY_r = createRotateYMatrix(rotation_r.y);
  mb::Matrix rZ_r = createRotateZMatrix(rotation_r.z);
  mb::Matrix rotationMatrix_r;
  if(!GimbalLockZXYMode_R) {
    rotationMatrix_r = rX_r*rY_r*rZ_r;
    hand_r->GetTNode()->SetRotation(rotationMatrix_r);
    if((int(hand_r->GetRot().y)%180 < 135) && ( int(hand_r->GetRot().y)%180 > 45))
    {
     GimbalLockZXYMode_R = true;
     rotationMatrix_r = rZ_r*rX_r*rY_r;
    }
    else {
     rotationMatrix_r = rX_r*rY_r*rZ_r;
    }
    //mbhud("GimbalLock ON");
  } else {
    rotationMatrix_r = rZ_r*rX_r*rY_r;
    hand_r->GetTNode()->SetRotation(rotationMatrix_r);
    if(!((int(hand_r->GetRot().x)%180 < 135) && ( int(hand_r->GetRot().x)%180 > 45)))
    {
     GimbalLockZXYMode_R = false;
     rotationMatrix_r = rX_r*rY_r*rZ_r;
    }
    else {
     rotationMatrix_r = rZ_r*rX_r*rY_r;
    }
    //mbhud("GimbalLock OFF");
  }
  hand_r->GetTNode()->SetRotation(rotationMatrix_r);
  
  //mblog(" Rotation Matrix Calc Time: "+QString::number(t->elapsed())+"\n");
  t->restart();


  //Setting Cameras to follow Hand;
  cameraWrapper *leftHand = new cameraWrapper(leftCamID);
  leftHand->getTNode()->SetRotation(hand_l->GetRot()+handOffset);
  leftHand->getTNode()->SetPosition(hand_l->GetPos());
  leftHand->MoveForward(-10.0f);
  cameraWrapper *rightHand = new cameraWrapper(rightCamID);
  rightHand->getTNode()->SetRotation(hand_r->GetRot()+handOffset);
  rightHand->getTNode()->SetPosition(hand_r->GetPos());
  rightHand->MoveForward(-10.0f);
  //bool leftColl = false;
  //mblog("Finger IntersectCount = "+QString::number(countIntersectingFingers(l))+"\n");

  //For Coll detection and replacement:
  mb::Vector fingerPos_L = mb::Vector(0,0,0);
  mb::Vector fingerPos_R = mb::Vector(0,0,0);
  //mblog(" Hand Set Time Calc Time: "+QString::number(t->elapsed())+"\n");
  t->restart();
  for(int i = 0 ; i < 5 ; i++) {
    for(int j = 0 ; j < 4 ; j++) {
      fingerPos_L = hand_l->GetFingerPos(fingerEnum(i),jointEnum(j));
      fingerPos_R = hand_r->GetFingerPos(fingerEnum(i),jointEnum(j));
      tmp = cameraPivot + leapReader->getFingerPosition(fingerEnum(i),l).at(j);
      hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),tmp);
      hand_l->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);

      tmp = cameraPivot + leapReader->getFingerPosition(fingerEnum(i),r).at(j);
      hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),tmp);
      hand_r->RotateAroundPivot(jointEnum(j),fingerEnum(i),-1*camRot,cameraPivot);

      //if(collisionToggle) {
      //  if(meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
      //    if(j == 0) {
      //      hand_l->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_L);
      //    }
      //    //mblog("L Hand collision!\n");
      //  }
      //  if(meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(fingerEnum(i),jointEnum(j)))) {
      //    if(j == 0) {
      //      hand_r->SetFingerPos(jointEnum(j),fingerEnum(i),fingerPos_R);
      //    }
      //    //mblog("R Hand collision!\n");
      //  }
      //}
      // }
      //mblog("Updating Bone Pos :"+QString::number(j)+"\n");
      //hand_r->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),r).at(j));
      //hand_l->UpdateBone(boneEnum(j),fingerEnum(i),leapReader->getBoneOrients(fingerEnum(i),l).at(j));
      //mblog("Updated Bone Pos\n");
      }
    }
  hand_l->SetAllFingerJointRots();
  hand_r->SetAllFingerJointRots();
  //mblog(" Loop for Fingers: "+QString::number(t->elapsed())+"\n");
  t->restart();
  if(leapReader->isTool) {
    tool->SetVisi(true);
    std::vector<mb::Vector> toolLocs = leapReader->GetToolPositions();
    tmp = cameraPivot+toolLocs.at(0);
    tool->SetPos(0,tmp);
    tmp = cameraPivot+toolLocs.at(1);
    tool->SetPos(1,tmp);
    tmp = (mb::Vector(-1,1,-1)*camRot)+leapReader->GetToolDirection();
    tool->SetRot( tmp);
    tmp = GetAimRotation(tool->GetPos(0),mb::Vector(0,0,0));
    tool->SetRot(0,tmp);
    tmp = (-1*camRot)/2;
    tool->RotateAroundPivot(tmp,cameraPivot);
    tool->RotateAroundPivot(tmp,cameraPivot);
  } else {
    tool->SetVisi(false);
  }
  
//  mb::Vector worldPos = mb::Vector(0,0,0);
//  mb::Vector screenBrushPos;
//  meshOp->ChangeCamera(leftHand);
//  mb::Kernel()->Scene()->SetActiveCamera(leftHand->getCamera());
//  mb::Kernel()->ViewPort()->Redraw();
//  worldPos = meshOp->FireAtMesh(mb::Vector(0,0,0));
//  if(worldPos.x != -1) {
//    screenBrushPos = mb::Vector(-1,-1,0)*viewCam->getCamera()->Project(worldPos);
//    brushIcon->SetPoints(screenBrushPos);
//    mbhud(VectorToQStringLine(viewCam->getCamera()->Project(worldPos)));
//    mblog("HitWorldPos\n")
//  }
//  
//  mb::Kernel()->Scene()->SetActiveCamera(viewCam->getCamera());
//  mb::Kernel()->ViewPort()->Redraw();
  //mblog(" Tools Time: "+QString::number(t->elapsed())+"\n");
  //mblog("  Overall time"+QString::number(overall->elapsed())+"\n");
}

void Leap_Updater::CameraRotate(LR lOrR) {
  const float deadzone = 20.0f;
  mb::Vector handRot;
  if(lOrR == r) {
    handRot = leapReader->getDirection_R()+mb::Vector(15,0,0);
  } else {
    handRot = leapReader->getDirection_L()+mb::Vector(15,0,0);
  }
  mbstatus("HandRot: "+VectorToQString(handRot));
  if(std::abs(handRot.x) > deadzone || std::abs(handRot.y) > deadzone || std::abs(handRot.z) > deadzone) {
    if(std::abs(handRot.x) > std::abs(handRot.y) && std::abs(handRot.x) > std::abs(handRot.z)) {
      //mblog("Pitch\n");
      //mbhud("Rotate Pitch");
      float PosZ = viewCam->getTNode()->Position().z;
      if(PosZ > 0) {
        if(handRot.x > 0) {
          rotateCamera(mb::Vector(-0.5,0,0));
        } else {
          rotateCamera(mb::Vector(0.5,0,0));
        }
      } else {
        if(handRot.x > 0) {
          rotateCamera(mb::Vector(0.5,0,0));
        } else {
          rotateCamera(mb::Vector(-0.5,0,0));
        }
      }
    } else
    if(std::abs(handRot.y) > std::abs(handRot.x) && std::abs(handRot.y) > std::abs(handRot.z)) {
//      mblog("Roll\n");
//      mbhud("Rotate Roll");
      if(handRot.y > 0)
        rotateCamera(mb::Vector(0,0,0.5));
      else
        rotateCamera(mb::Vector(0,0,-0.5));
    } else if(std::abs(handRot.z) > std::abs(handRot.x) && std::abs(handRot.z) > std::abs(handRot.y)) {
      //mblog("Yaw\n");
      //mbhud("Rotate Yaw");
      if(handRot.z > 0)
        rotateCamera(mb::Vector(0,0.5,0));
      else
        rotateCamera(mb::Vector(0,-0.5,0));
    }
  }
  //viewCam->getTNode()->SetPosition(sceneRotate);
}

void Leap_Updater::CameraZoom(LR lOrR) {
  const float deadzone = 20.0f;
  mb::Vector handPos;
  if(lOrR == r) {
    handPos = leapReader->getPosition_R();
  } else {
    handPos = leapReader->getPosition_L();
  }
  mbstatus("HandRot: "+VectorToQString(handPos));
  if(std::abs(handPos.z) > deadzone) {
      if(handPos.z > 0) {
        mblog("Forward\n");
        viewCam->getCamera()->MoveForward(mb::Vector(5,0,0));
      } else {
        mbhud("Backward\n");
        viewCam->getCamera()->MoveBackward(mb::Vector(5,0,0));
      }
    }

  //viewCam->getTNode()->SetPosition(sceneRotate);
}

void Leap_Updater::CameraPan(LR lOrR) {
  const float deadzone = 20.0f;
  mb::Vector handPos;
  mb::Vector difference;

  if(lOrR == r) {
    handPos = leapReader->getPosition_R();
    difference = handPos - savedPanHandPosition_R;
  } else {
    handPos = leapReader->getPosition_L();
    difference = handPos - savedPanHandPosition_L;
  }
  mbstatus("Difference = "+VectorToQString(difference));
  if(std::abs(difference.x) > std::abs(difference.y)) {
    if(difference.x > deadzone) {

      
      viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+(0.5f*viewCam->getCamera()->Right()));
      viewCam->setPosition(viewCam->getPosition()+(0.5f*viewCam->getCamera()->Right()));

      //viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+mb::Vector(0.5,0,0));
      //viewCam->setPosition(viewCam->getPosition()+mb::Vector(0.5,0,0));
    } else if(difference.x < -deadzone) {
      //viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+mb::Vector(-0.5,0,0));
      //viewCam->setPosition(viewCam->getPosition()+mb::Vector(-0.5,0,0));
      viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+(-0.5f*viewCam->getCamera()->Right()));
      viewCam->setPosition(viewCam->getPosition()+(-0.5f*viewCam->getCamera()->Right()));
    }
  } else {  
    if(difference.y > deadzone) {
      viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+mb::Vector(0,0.5,0));
      viewCam->setPosition(viewCam->getPosition()+mb::Vector(0,0.5,0));
    } else if(difference.y < -deadzone) {
      viewCam->getCamera()->SetAim(viewCam->getCamera()->Aim()+mb::Vector(0,-0.5,0));
      viewCam->setPosition(viewCam->getPosition()+mb::Vector(0,-0.5,0));
    }
  }
  
  mblog("Aim = "+VectorToQStringLine(viewCam->getCamera()->Aim()));
  viewCam->getCamera()->SetTarget(viewCam->getCamera()->Aim());
}

void Leap_Updater::MenuSettings_R() {
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
  mbstatus(VectorToQStringLine(PosDifference));
  menuFilter->menuChoice = 5;
  if(PosDifference.y > menuDeadZone) {
    menuFilter->menuChoice = 8;
    if(PosDifference.y > menuActivateZone) {
      if(PosDifference.x > 0) {
        menuFilter->menuChoice = 19;
        if(PosDifference.y > menuActivateZone2) {
          menuDown_0 = true;
        }
      } else {
        menuFilter->menuChoice = 20;
        if(PosDifference.y > menuActivateZone2) {
          menuDown_1 = true;
        }
      }
    }
  } else if(PosDifference.y < -menuDeadZone) {
    menuFilter->menuChoice = 6;
    if(PosDifference.y < -menuActivateZone) {
      if(PosDifference.x > 0) {
        menuFilter->menuChoice = 15;
        if(PosDifference.y < -menuActivateZone2)
          menuUp_0 = true;
      } else {
        menuFilter->menuChoice = 16;
        if(PosDifference.y < -menuActivateZone2)
          menuUp_1 = true;
      }
    }
  }
  mblog("Abs pos X = "+QString::number(std::abs(PosDifference.x))+"\n");
  mblog("Abs 0.5f = "+QString::number(std::abs(float(0.5)))+"\n");
  mblog("Abs 0.5f = "+QString::number(std::abs(float(PosDifference.x)))+"\n");
  if(std::abs(PosDifference.x) > std::abs(PosDifference.y)) {
    mbhud("ABS WORKS");
    mblog("ABS WORKS");
    if(PosDifference.x > menuDeadZone) {
      menuFilter->menuChoice = 9;
      if(PosDifference.x > menuActivateZone) {
        menuLeft = true;
      }
    } else if(PosDifference.x < -menuDeadZone) {
      menuFilter->menuChoice = 7;
      if(PosDifference.x < -menuActivateZone) {
        if(PosDifference.y < 0) {
          menuFilter->menuChoice = 17;
          if(PosDifference.x < -menuActivateZone2)
            menuRight_0 = true;
        } else {
          menuFilter->menuChoice = 18;
          if(PosDifference.x < -menuActivateZone2)
            menuRight_1 = true;
        }
      }
    }
  }
  if(leapReader->isCircleCCW_R) {
      menuFilter->SetVisible(false);
      inMenu_R = false;
  }
  if(menuUp_0) {
    //GRAB MODE
    thumbGrabModeToggle = false;
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuUp_0 = false;
  }  if(menuUp_1) {
    //PUSH THUMBS MODE
    thumbGrabModeToggle = true;
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuUp_1 = false;
  } else if(menuRight_0) {
    //SUBDIVIDE MESH
    meshOp->SubDivide();
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuRight_0 = false;
  } else if(menuRight_1) {
    moveObjectMode = !moveObjectMode;
    if(moveObjectMode)
      mbhud("Move Object Mode On");
    else
      mbhud("Move Object Mode Off");
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuRight_1 = false;
  } else if(menuDown_0) {
    //LOCK CURRENT HAND PIVOT #999
    savedHandPivotPoint = fitToCameraSpace();
    pivotHandsOnMesh = false;
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuDown_0 = false;
  } else if(menuDown_1) {
    //LOCK TO CAMERA #999
    pivotHandsOnMesh = true;
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuDown_1 = false;
  } else if(menuLeft) {
    //Back
    menuFilter->SetVisible(false);
    inMenu_R = false;
    menuLeft = false;
  }
}

void Leap_Updater::MenuSettings_L() {
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
  menuFilter->menuChoice = 0;
  if(PosDifference.y > menuDeadZone) {
    menuFilter->menuChoice = 3;
    if(PosDifference.y > menuActivateZone)
      menuDown = true;
  } else if(PosDifference.y < -menuDeadZone) {
    menuFilter->menuChoice = 1;
    if(PosDifference.y < -menuActivateZone) {
      if(PosDifference.x > 0) {
        menuFilter->menuChoice = 11;
        if(PosDifference.y < -menuActivateZone2)
          menuUp_0 = true;
      } else {
        menuFilter->menuChoice = 12;
        if(PosDifference.y < -menuActivateZone2)
          menuUp_1 = true;
      }
    }
  }
  if(std::abs(PosDifference.x) > std::abs(PosDifference.y)) {
    if(PosDifference.x > menuDeadZone) {
      menuFilter->menuChoice = 4;
      if(PosDifference.x > menuActivateZone) {
        menuLeft = true;
      }
    } else if(PosDifference.x < -menuDeadZone) {
      menuFilter->menuChoice = 2;
      if(PosDifference.x < -menuActivateZone) {
        if(PosDifference.y < 0) {
          menuFilter->menuChoice = 13;
          if(PosDifference.x < -menuActivateZone2)
            menuRight_0 = true;
        } else {
          menuFilter->menuChoice = 14;
          if(PosDifference.x < -menuActivateZone2)
            menuRight_1 = true;
        }
      }
    }
    if(leapReader->isCircleCCW_L) {
      mbstatus("Got Anti-Clockwise Circle");
      menuFilter->SetVisible(false);
      inMenu_L = false;
    }
  }

  if(menuUp_0) {
    brushSizeMenuToggle = true;
    brushSizeStartFingerStartPos = GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
    menuFilter->menuChoice = 10;
    inMenu_L = false;
    menuUp_0 = false;  
  } else if(menuUp_1) {
    brushStrengthMenuToggle = true;
    brushStrengthFingerStartPos = GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
    //BRUSH STRENGTH #999
    menuFilter->menuChoice = 21;
    inMenu_L = false;
    menuUp_1 = false;
  } else if(menuRight_0) {
    //Navigation ON/OFF
    SceneNavigationToggle = !SceneNavigationToggle;
    if(SceneNavigationToggle)
      mbhud("SceneNavigation On");
    else
      mbhud("SceneNavigation Off");
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuRight_0 = false;
  } else if(menuRight_1) {
    //Collision On off
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuRight_0 = false;
  } else if(menuDown) {
    toolStamp = !toolStamp;
    if(toolStamp)
      mbhud("toolStamp On");
    else
      mbhud("toolStamp Off");
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuDown = false;
  } else if(menuLeft) {
    menuFilter->SetVisible(false);
    inMenu_L = false;
    menuLeft = false;
  }
}

void Leap_Updater::BrushSize() {
  if(leapReader->isScreenTap_L || leapReader->isScreenTap_R) {
    mblog("got tap\n");
    brushSizeMenuToggle = false;
    menuFilter->SetVisible(false);
    return;
  }
  mb::Vector PosDifference = brushStrengthFingerStartPos - GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
  //include a deadzone
  const float scalefactor = 0.5;
  if(PosDifference.y > 0.1f) {
    brushSize = MAX(brushSize - (PosDifference.y-0.05)*scalefactor,0.1);
  } else if(PosDifference.y < 0.1f) {
    brushSize = MAX(brushSize - (PosDifference.y+0.05)*scalefactor,0.1);
  } else {
    return;
  }
  mb::Kernel()->Interface()->HUDMessageHide();
  mbhud("Brush size = "+QString::number(brushSize));
  float relativeSize = brushSize/mb::Kernel()->ViewPort()->Height()*4;
  mblog("Relative size = "+QString::number(relativeSize)+"\n");
  menuFilter->SetSize(relativeSize);
}

void Leap_Updater::BrushStrength() {
  if(leapReader->isScreenTap_L || leapReader->isScreenTap_R) {
    mblog("got tap\n");
    brushSizeMenuToggle = false;
    menuFilter->SetVisible(false);
    return;
  }
  mb::Vector PosDifference = menuStartSpace - GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
  const float scalefactor = 0.5;
  //include a deadzone
  if(PosDifference.y > 0.1f) {
    brushStrength = MAX(brushStrength - (PosDifference.y-0.01)*scalefactor,0.1);
  } else if(PosDifference.y < 0.1f) {
    brushStrength = MAX(brushStrength - (PosDifference.y+0.01)*scalefactor,0.1);
  } else {
    return;
  }
  mb::Kernel()->Interface()->HUDMessageHide();
  mbhud("Brush strength = "+QString::number(brushStrength));
  float relativeSize = brushStrength/mb::Kernel()->ViewPort()->Height();
  mblog("Relative size = "+QString::number(relativeSize)+"\n");
  menuFilter->SetSize(relativeSize);
}

void Leap_Updater::Extrusion(LR LorR) {
  if(LorR == l) {
    mblog("Finger IntersectCount = "+QString::number(countIntersectingFingers(l))+"\n");
    if(!facesAreSelected_L){
      if(!reqIntersectionForSelection || (countIntersectingFingers(l) > 3)) {
        if(selectMesh(l) ) {
          uniqueMissBool = true;
          facesAreSelected_L= true;
          firstmoveswitch = true;
          mblog("Succesfully Selected\n");
        } else {
          if(uniqueMissBool) {
            uniqueMissBool = false;
            uniqueMiss++;
          }
          missCounter++;
          mbhud("Failed To Select L_Hand, Have you selected the Mesh?");
          mblog("Failed to select\n");
        }
      }
      mbstatus("Grabbing");
    } else {
      if(firstmoveswitch) {
        lastFrameHandPos_L = hand_l->GetPos();
        firstmoveswitch = false;
      } else {
        mbstatus("MovingMesh");
        mblog("Moving Mesh\n");
        MoveMesh(l);
      }
    }
  } else {
    if(!facesAreSelected_R) {
      if(!reqIntersectionForSelection || (countIntersectingFingers(r) > 3)) {
        if(selectMesh(r) ) {
          uniqueMissBool = true;
          facesAreSelected_R = true;
          firstmoveswitch_R = true;
          mblog("Succesfully Selected\n");
        } else {
          if(uniqueMissBool) {
            uniqueMissBool = false;
            uniqueMiss++;
          }
          missCounter++;
          mbhud("Failed To Select with R_Hand, Have you selected the Mesh?");
          mblog("Failed to select\n");
        }
      }
      mbstatus("Grabbing");
    } else {
      if(firstmoveswitch_R) {
        lastFrameHandPos_R = hand_r->GetPos();
        firstmoveswitch_R = false;
      } else {
        mbstatus("MovingMesh");
        mblog("Moving Mesh\n");
        MoveMesh(r);
      }
    }

  }
}

__inline void Leap_Updater::checkNavigationGestures() {
  if(SceneNavigationToggle) {
    if(leapReader->isVisible(l)) {
      if(leapReader->CheckRotateHandGesture(l)) {
        CameraRotate(l);
        gestureHUD->menuChoice = 2;
        gestureHUD->SetVisible(true);
        firstPan_L = true;
      } else if(leapReader->CheckScaleHandGesture(l)) {
        CameraZoom(l);
        if(firstPan_L) {
          savedPanHandPosition_L = leapReader->getPosition_L();
          firstPan_L = false;
        }
        gestureHUD->menuChoice = 3;
        gestureHUD->SetVisible(true);
        CameraPan(l);
      } else {
        firstPan_L = true;
      }
    }
    if(leapReader->isVisible(r)) {
      if(leapReader->CheckRotateHandGesture(r)) {
        CameraRotate(r);
        gestureHUD->menuChoice = 2;
        gestureHUD->SetVisible(true);
        firstPan_R = true;
      } else if(leapReader->CheckScaleHandGesture(r)) {
        CameraZoom(r);
        if(firstPan_R) {
          savedPanHandPosition_R = leapReader->getPosition_R();
          firstPan_R = false;
        }
        gestureHUD->menuChoice = 3;
        gestureHUD->SetVisible(true);
        CameraPan(r);
      } else {
        firstPan_R = true;
      }
    }
  }
}

__inline void Leap_Updater::checkMenuGesture() {
    //If Circle Clockwise
  if(leapReader->isCircleCW_R) {
    if(leapReader->CheckFingerExtensions(r,false,true,true,false,false)) {
      menuStartSpace = GetRelativeScreenSpaceFromWorldPos(hand_r->GetFingerPos(INDEX));
      mbhud("MenuSpace "+VectorToQStringLine(menuStartSpace));
      mbstatus("MenuSpace "+VectorToQStringLine(menuStartSpace));
      menuFilter->SetCentre(menuStartSpace);
      menuFilter->SetVisible(true);
      inMenu_R = true;
    }
  }
  if(leapReader->isCircleCW_L) {
    if(leapReader->CheckFingerExtensions(l,false,true,true,false,false)) {
      menuStartSpace = GetRelativeScreenSpaceFromWorldPos(hand_l->GetFingerPos(INDEX));
      mbhud("MenuSpace "+VectorToQStringLine(menuStartSpace));
      mbstatus("MenuSpace "+VectorToQStringLine(menuStartSpace));
      menuFilter->SetCentre(menuStartSpace);
      menuFilter->SetVisible(true);
      inMenu_L = true;
    }
  }
}

__inline void Leap_Updater::checkScreenTapGesture() {
  //If they tapped the screen
  if(leapReader->CheckFingerExtensions(l,false,true,false,false,false)) {
    if(leapReader->isScreenTap_L) {
      mblog("got tap\n");
      gestureHUD->refresh = true;
      gestureHUD->menuChoice = 5;
      gestureHUD->SetVisible(true);
      ScreenTap(l);
    }
  } else if(leapReader->CheckFingerExtensions(r,false,true,false,false,false)) {
    if(leapReader->isScreenTap_R) {
      mblog("got tap\n");
      gestureHUD->refresh = true;
      gestureHUD->menuChoice = 5;
      gestureHUD->SetVisible(true);
      ScreenTap(r);
    }
  }
}

__inline void Leap_Updater::checkUndoGesture() {
  //If they Swiped Left
  if(leapReader->isUndo) {
    mb::Kernel()->Interface()->HUDMessageShow("UNDO");
    meshOp->UndoLast();
    gestureHUD->refresh = true;
    gestureHUD->menuChoice = 6;
    gestureHUD->SetVisible(true);
    //meshOp_R->UndoLast();
  }
}

__inline void Leap_Updater::checkGrabbingGesture() {
  if(thumbGrabModeToggle) {
    bool leftThumb = (meshOp->CheckIntersection(hand_l->GetFingerBoundingBox(THUMB,TIP)));
    bool rightThumb = (meshOp->CheckIntersection(hand_r->GetFingerBoundingBox(THUMB,TIP)));
    if(leftThumb || rightThumb) {
      if(meshOp->firstUse) {
        meshOp->firstUse = false;
      }
      if(thumbDirectionBasedMovement) {
        if(leftThumb)
          ThumbDirMove(l);
        if(rightThumb)
          ThumbDirMove(r);
      } else {
        if(leftThumb)
          ThumbSmoothMove(l);
        if(rightThumb)
          ThumbSmoothMove(r);
      }
    } else {
      meshOp->firstUse = true;
      if(facesAreSelected_L){
        meshOp->DeselectAllFaces();
      }
      facesAreSelected_L= false;
    }
  } else {
  //If they are Grabbing and Thumb Toggle is Off
    if(leapReader->isGrabbing_L ) {
      Extrusion(l);
      if(leapReader->isGrabbing_R) {
        mbstatus("Right hand grabbing too\n");
        Extrusion(r);
      }
      gestureHUD->menuChoice = 1;
      gestureHUD->SetVisible(true);
    } else if(leapReader->isGrabbing_R) {
      Extrusion(r);
      gestureHUD->menuChoice = 1;
      gestureHUD->SetVisible(true);
    } else {
      if(facesAreSelected_R || facesAreSelected_L){
        meshOp->DeselectAllFaces();
        //meshOp->FindTesselationFaces(l);
        mblog("going to deselect faces");
        //meshOp_R->DeselectAllFaces();
      }
      gestureHUD->refresh = true;
      uniqueMissBool = true;
      firstmoveswitch = true;
      facesAreSelected_R = false;
      facesAreSelected_L= false;
    }
    /*
    } else {
      if(facesAreSelected_L){
        meshOp->DeselectAllFaces();
      }
      firstmoveswitch_R = true;
      facesAreSelected_L= false;
    }*/
    lastFrameHandPos_L = hand_l->GetPos();
    lastFrameHandPos_R = hand_r->GetPos();
  }
}

void Leap_Updater::ToolStampMove() {
  int toolCamID = idList->getToolCam();
  cameraWrapper *toolCam= new cameraWrapper(toolCamID);
  toolCam->setPosition(tool->GetPos(1));
  toolCam->setAim(tool->GetPos(0));
  toolCam->MoveForward(-50);
  meshOp->ChangeCamera(viewCam);
  tool->SetVisi(false);
  mblog("ToolStamp\n");
  int dist = 5;
  mb::Vector toolPos = tool->GetPos(0);
  mb::Vector toolProj = viewCam->getCamera()->Project(toolPos);
  mblog("Tool Proj Pos = "+VectorToQStringLine(toolProj));
  toolProj = toolProj * mb::Vector(1,-1,1);
  mblog("Tool Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(toolProj)));
  tool->ResizeStamp(100,100);
  if(meshOp->ToolManip(ScreenSpaceToPixels(toolProj),brushSize,tool)) {
    facesAreSelected_Tool = true;
    mblog("Moving vertices maybe?\n");
    meshOp->MoveVertices(r,dist);
  }
  tool->SetVisi(true);
}

void Leap_Updater::ToolSmoothMove() {
  int toolCamID = idList->getToolCam();
  cameraWrapper *toolCam= new cameraWrapper(toolCamID);
  toolCam->setPosition(tool->GetPos(1));
  toolCam->MoveForward(-50);
  toolCam->setAim(tool->GetPos(0));
  meshOp->ChangeCamera(viewCam);
  tool->SetVisi(false);
  mb::Vector toolPos = tool->GetPos(0);
  mb::Vector toolProj = viewCam->getCamera()->Project(toolPos);
  toolProj = toolProj * mb::Vector(1,-1,1);
  mblog("ToolSmoothMove\n");
  mblog("Tool Proj Pos = "+VectorToQStringLine(toolProj));
  mblog("Tool Proj Pos Pixels = "+VectorToQStringLine(ScreenSpaceToPixels(toolProj)));
  //meshOp->ChangeCamera(viewCam);
  if(meshOp->SelectFaces(r,ScreenSpaceToPixels(toolProj),brushStrength,brushStrength)) {
    facesAreSelected_Tool = true;
    mb::Vector dirNorm = leapReader->getToolMotionDirection();
    mblog("Normalised Direction = "+VectorToQStringLine(dirNorm));
    float dist = brushStrength*2;
    meshOp->MoveVerticesNormal(r,dist,tool->GetPos(0));
  }
  tool->SetVisi(true);
}

__inline void Leap_Updater::checkToolIntersection() {
  if(leapReader->isTool) {
    if(meshOp->CheckIntersection(tool->GetInteractionBox())) {
      tool->SendToServer(1);
      mblog("Sending 1\n");
      if(meshOp->CheckIntersection(tool->GetBoundingBox(0))) {
        tool->SendToServer(2);
        mblog("Sending 2\n");
        mblog("Tool bounding box Pos = "+VectorToQStringLine(tool->GetBoundingBox(0).Center()));
       // mblog("Test Tool bounding box Pos = "+VectorToQStringLine(mb::AxisAlignedBoundingBox(tool->GetPos(0),0.2f).Center()));
        //http://www.sciencedirect.com/science/article/pii/S0010448512002734
        //Set the undo list to iterate on.
        if(meshOp->firstUse) {
          meshOp->firstUse = false;
        }
        if(toolStamp) {
          ToolStampMove();
        } else {
          ToolSmoothMove();
        }
      } else {
        if(facesAreSelected_Tool) {
          meshOp->DeselectAllFaces();
        }
        facesAreSelected_Tool = false;
        //Set the undo list to iterate on.
        meshOp->firstUse = true;
      }
    } else {
        tool->SendToServer(0);
    }
  } else {
    tool->SendToServer(0);
  }
}

void Leap_Updater::MoveSelectedObject(LR lr) {
  mb::Vector moveDist;
  if(lr == l) {
    moveDist = hand_l->GetPos() - lastFrameHandPos_L;
    lastFrameHandPos_L = hand_l->GetPos();
  } else {
    moveDist = hand_r->GetPos() - lastFrameHandPos_R;
    lastFrameHandPos_R = hand_r->GetPos();
  }
  meshOp->MoveObject(moveDist);
}

__inline void Leap_Updater::checkMoveObjGesture() {
  if(leapReader->isGrabbing_L) {
    if(isFirstGrab) {
      lastFrameHandPos_L = hand_l->GetPos();
      isFirstGrab = false;
      meshOp->SelectWholeMesh();
    }
    MoveSelectedObject(l);
  } else if(leapReader->isGrabbing_R) {
    if(isFirstGrab) {
      lastFrameHandPos_R = hand_r->GetPos();
      isFirstGrab = false;
      meshOp->SelectWholeMesh();
    }
    MoveSelectedObject(r);
  } else {
    if(!isFirstGrab) {
      meshOp->StoreLastMoveUndoQueue();
      isFirstGrab = true;
      meshOp->DeselectAllFaces();
      mblog("Mesh Moved");
    }
  }
}

__inline void Leap_Updater::checkUndoMoveGesture() {
  //If they Swiped Left
  if(leapReader->isUndo) {
    mb::Kernel()->Interface()->HUDMessageShow("UNDO Move");
    meshOp->UndoLastMove();
    gestureHUD->refresh = true;
    gestureHUD->menuChoice = 6;
    gestureHUD->SetVisible(true);
  }
}

void Leap_Updater::OnEvent(const mb::EventGate &cEvent) {
  QTime *overall = new QTime();
  overall->start();
  QTime *t = new QTime();
  t->start();
  if(cEvent == frameEvent) {
    int viewcamID = idList->getViewCam();
    viewCam = new cameraWrapper(viewcamID);
    t->restart();
    if(leapReader->updateAll()) {
      //mblog("Update All Time "+QString::number(t->elapsed())+"\n");
      if((leapReader->ishands || leapReader->isTool) && leapReader->isConnected) {
        //mblog("Getting CameraID\n");
        if(pivotHandsOnMesh) {
          cameraPivot = fitToCameraSpace();
        } else {
          cameraPivot = savedHandPivotPoint;
        }
        t->restart();
        SetHandAndFingerPositions();
        //mblog("Set Hands and Finger Position Times: "+QString::number(t->elapsed())+"\n");
        //If Circle Anti-Clockwise
        if(inMenu_R) {
          MenuSettings_R();
        } else if(inMenu_L) {
          MenuSettings_L();
        } else if(brushSizeMenuToggle) {
          BrushSize();
        } else if(brushStrengthMenuToggle) {
          BrushStrength();
        } else if(moveObjectMode) {
          //Important to keep this ordering, menus first then other modes, ensures we can exit and
          //enter menus at any time!
          t->restart();
          checkMenuGesture();
          //mblog("Check Menu Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkNavigationGestures();
          //mblog("Check Nav Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkScreenTapGesture();
          //mblog("Check Screen Tap Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkUndoMoveGesture();
          //mblog("Check Undo Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkMoveObjGesture();
          //mblog("Check Move Obj Gesture Times: "+QString::number(t->elapsed())+"\n");
        } else {
          t->restart();
          checkNavigationGestures();
          //mblog("Check Nav Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkMenuGesture();
          //mblog("Check Menu Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkScreenTapGesture();
          //mblog("Check Screen Tap Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkUndoGesture();
          //mblog("Check Undo Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkGrabbingGesture();
          //mblog("Check Grabbing Gesture Times: "+QString::number(t->elapsed())+"\n");
          t->restart();
          checkToolIntersection();
          //mblog("Check Tool Intersection Times: "+QString::number(t->elapsed())+"\n");
        }
      }
    }
  }
  mb::Kernel()->ViewPort()->Redraw();
  mblog("Misses = "+QString::number(missCounter)+"\n");
  mblog("Unique Misses = "+QString::number(uniqueMiss)+"\n");
  mblog("Current Time = "+QString::number(bigTimer->elapsed()));
  //mblog("Full Loop Time: "+QString::number(overall->elapsed())+"\n");
}
