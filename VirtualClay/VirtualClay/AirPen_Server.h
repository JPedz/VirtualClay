#pragma once
#include <QtNetwork\qtcpserver.h>
#include <QtNetwork\qtcpsocket.h>
class AirPen_Server
{
  bool isConnected;
  QTcpSocket *server;
  int lastSent;
  static const int port = 5000;

public:
  AirPen_Server(void);
  void SendMsg(bool onOff);
  ~AirPen_Server(void);
};

