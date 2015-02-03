#ifndef MENU_UI_H
#define MENU_UI_H
#include "stdafx.h"


class menuUI {
    Q_DECLARE_TR_FUNCTIONS(VirtualClay);
    std::vector<int> ID_List;
public:
  menuUI(void);
	void Cleanup(void);
	void Execute(void);   // Execute 
  void addToIDList(int ID);
  std::vector<int> getIDList(void);


};

#endif