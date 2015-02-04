#ifndef FINGERS_H_
#define FINGERS_H_
#include "stdafx.h"
namespace mb = mudbox;

class Fingers {
  private:
    mb::Node *Node;
    mb::Transformation *TNode;
    mb::GroupNode *GNode;
    void Fingers::BuildGeo(void);

  public:
    Fingers(menuUI *mu);
    mb::Vector Fingers::GetPos(void);
    void Fingers::SetPos(mb::Vector v);
    void Fingers::SetRot(mb::Vector r);
    
};


#endif