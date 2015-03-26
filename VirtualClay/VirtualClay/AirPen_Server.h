#pragma once
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>


class AirPen_Server
{
  //#define IP "10.0.0.15"
  #define IP "192.168.11.102"
  bool isConnected;
  QTcpSocket *server;
  int lastSent;
  static const int port = 5000;

public:
  AirPen_Server(void);
  void SendMsg(int stage);
  ~AirPen_Server(void);
};

