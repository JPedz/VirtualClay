#ifndef MENU_UI_H
#define MENU_UI_H
#include "stdafx.h"


namespace mb = mudbox;
class menuUI : public mb::Node {
  DECLARE_CLASS
  Q_DECLARE_TR_FUNCTIONS(menuUI);
  size_t sizeer;
  std::vector<int> ID_List;
public:
  menuUI();
  int staticClass();
	void Cleanup(void);
	void Execute(void);   // Execute 
  void addToIDList(int ID);
  std::vector<int> getIDList(void);


};

#endif