#include "stdafx.h"
#include "util.h"
namespace mb = mudbox;

QString VectorToQStringLine(mudbox::Vector v) {
  return QString::number(v.x)+" "+QString::number(v.y)+" "+QString::number(v.z)+"\n";
}

QString VectorToQString(mudbox::Vector v) {
  return QString::number(v.x)+" "+QString::number(v.y)+" "+QString::number(v.z)+" ";
}

mb::Matrix createRotateXMatrix(float a) {
  a = a * 3.14159265 /180;
  return mb::Matrix(1,0,0,0,0,cos(a),-sin(a),0,0,sin(a),cos(a),0,0,0,0,1);
}

mb::Matrix createRotateYMatrix(float b) {
  b = b * 3.14159265 /180;
  return mb::Matrix(cos(b),0,sin(b),0,0,1,0,0,-sin(b),0,cos(b),0,0,0,0,1);
}

mb::Matrix createRotateZMatrix(float g) {
  g = g * 3.14159265 /180;
  return mb::Matrix(cos(g),-sin(g),0,0,sin(g),cos(g),0,0,0,0,1,0,0,0,0,1);
}

mb::Matrix createTranslation(float x,float y, float z) {
  return mb::Matrix(1,0,0,x,0,1,0,y,0,0,1,z,0,0,0,1);
}


mb::Vector RotateVectorAroundPivot(mb::Vector &pos,mb::Vector &pivot, mb::Vector &rotation) {
  mb::Matrix t = createTranslation(-pivot.x,-pivot.y,-pivot.z);
  mb::Matrix tinv = createTranslation(pivot.x,pivot.y,pivot.z);
  mb::Vector pos_t = t*pos;
  mb::Matrix rX = createRotateXMatrix(rotation.x);
  mb::Matrix rY = createRotateYMatrix(rotation.y);
  mb::Matrix rZ = createRotateZMatrix(rotation.z);
  mb::Matrix rotationMatrix = rZ*rY*rX;
  mb::Vector newPos = rotationMatrix*pos_t;
  newPos = tinv*newPos;
  return newPos;
}

mb::Vector ScreenSpaceToPixels(mb::Vector v) {
  int midW = mb::Kernel()->ViewPort()->Width()/2;
  int midH = mb::Kernel()->ViewPort()->Height()/2;
  mb::Vector pixels = mb::Vector();
  pixels.x = midW+v.x*midW;
  pixels.y = midH+v.y*midH;
  return pixels;
}

mb::Vector leapVecToMBVec(Leap::Vector v) {
  return mb::Vector(v.x,v.y,v.z);
}