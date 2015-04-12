#pragma once
#include "stdafx.h"
#include <Cg/cg.h>
#include <cmath>
#include <Cg/cgGL.h>
#include <QtCore/QDir>
class GestureHUD : public mb::ViewPortFilter {
public:
  DECLARE_CLASS
  GestureHUD(void);
  void SetVisible(bool vis);
  RequirementValue Requirement( void ) const { return eHDR | ePosition32 | eNormal16; };
	void Process( mb::ViewPortState & );
	virtual QString Name( const mb::ClassDesc * ) const { return "HUD Filter"; };
	void OnNodeEvent(const mb::Attribute &cAttribute, mb::NodeEventType eType);
  mb::aenum menuChoice;
  int currentMenuChoice;
  bool refresh;
  QTime *timeout;
  ~GestureHUD(void);
  private:
    mb::Texture *grabGesture;
    mb::Texture *rotateGesture;
    mb::Texture *panGesture;
    mb::Texture *toolGesture;
    mb::Texture *selectGesture;
    mb::Texture *undoGesture;
};

