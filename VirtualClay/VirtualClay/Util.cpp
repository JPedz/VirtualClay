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
//  mb::Matrix rotationMatrix = rX*rZ*rY;
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


mb::Vector findDisplacementUV(mb::Base &base, mb::Vector &p0, mb::Vector &p1) {
  Leap::Vector vX = mbVecToLeapVec(base.Axis(0).Normalize());
  Leap::Vector vY = mbVecToLeapVec(base.Axis(1).Normalize());
  Leap::Vector vZ = mbVecToLeapVec(base.Axis(2).Normalize());
  Leap::Vector p2 = mbVecToLeapVec(p1-p0);
  Leap::Matrix *m = new Leap::Matrix(vX,vY,vZ);
  return -1*leapVecToMBVec(m->transformPoint(p2));
  
}
mb::Vector leapVecToMBVec(Leap::Vector v) {
  return mb::Vector(v.x,v.y,v.z);
}

Leap::Vector mbVecToLeapVec(mb::Vector v) {
  return Leap::Vector(v.x,v.y,v.z);
}

//mb::Vector AngleBetweenTwoPoints(mb::Vector p1, mb::Vector p2) {
//
//  
//  return mb::Vector(0,0,0);
//}

__inline mb::Matrix matrixFromVectors(mb::Vector v1,mb::Vector v2,mb::Vector v3) {
  return mb::Matrix(v1.x,v1.y,v1.z,0.0f,v2.x,v2.y,v2.z,0,v3.x,v3.y,v3.z,0.0f,0.0f,0.0f,0.0f,0.0f);
}


mb::Vector GetAimRotation(mb::Vector constrained,mb::Vector target) {
  mb::Matrix m = AimConstraint(constrained,target);
  float RD =Leap::RAD_TO_DEG;;
  mb::Vector rotation;
  rotation.x = asinf(-m._23)* RD;
  rotation.y = atan2f(m._13,m._33)*RD;
  rotation.z = atan2f(m._21,m._22)*RD;
  return mb::Vector(rotation.z,rotation.y,rotation.x);
}

mb::Matrix AimConstraint(mb::Vector constrained, mb::Vector target) {
  mb::Vector PassThoughVect = constrained - target;
  PassThoughVect.Normalize();
  mb::Vector upVect = mb::Vector(0,1,0);
  mb::Vector unconst = upVect&PassThoughVect;
  unconst.Normalize();
  upVect = unconst&PassThoughVect;
  upVect.Normalize();
  return matrixFromVectors(unconst,upVect,PassThoughVect);
}

#ifdef Q_OS_WIN
 #include <windows.h> // for Sleep
#else
 #include <unistd.h>
#endif
void LeapSleep(unsigned int ms) {
  #ifdef Q_OS_WIN
    Sleep(ms);
  #else
    sleep((unsigned int)(ms/1000));
  #endif
}
