#ifndef MENU_UI_H
#define MENU_UI_H
#include "stdafx.h"
#include "MeshOps.h"


namespace mb = mudbox;
class MenuUI : public mb::Node {
  DECLARE_CLASS
  Q_DECLARE_TR_FUNCTIONS(MenuUI);
  size_t sizeer;
  ID_List *idList;
public:
  MenuUI();
	void Cleanup(void);
	void Execute(void);   // Execute 
  void addToIDList(int ID);
  QList<int> getIDList(void);
};

#endif