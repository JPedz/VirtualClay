#pragma once
#include "stdafx.h"
#include "Leap_Fingers.h"

class Leap_Tool
{
  std::vector<Leap_Fingers *> tools;
  mb::Image *stamp;
public:
  //Constructor
  Leap_Tool(void);
  ~Leap_Tool(void);
  
  //Stamp
  void SetStamp(QString &fullPath);
  mb::Image* GetStamp();
  void Leap_Tool::ResizeStamp(float x, float y);

  //Tool GUI
  void SetVisi(bool vis);
  void SetPos(int i, mb::Vector &pos);
  void SetPos(std::vector<mb::Vector> &pos);
  void SetRot(int i, mb::Vector &pos);
  void SetRot(std::vector<mb::Vector> &pos);
  void RotateAroundPivot(int i, mb::Vector &a, mb::Vector &pivot);
  void Leap_Tool::RotateAroundPivot(mb::Vector &a, mb::Vector &pivot);
  mb::Vector GetPos(int i);
  mb::AxisAlignedBoundingBox GetBoundingBox(int i);
};

