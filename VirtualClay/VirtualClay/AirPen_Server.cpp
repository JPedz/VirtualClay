#include "stdafx.h"
#include "AirPen_Server.h"


AirPen_Server::AirPen_Server(void)
{
  lastSent = 0;
  server = new QTcpSocket();
}


AirPen_Server::~AirPen_Server(void)
{
  server->close();
  delete server;
}


void AirPen_Server::SendMsg(bool onOff) {
  //LastSent ensures we are not spamming the network.
  mblog("Conecting\n");
  server->connectToHost("10.0.0.15", port);
  mblog("Server socket is valid\n");
    if((lastSent != 1) && (onOff)) {
      server->write("1");
      mblog("Wrote 1\n");
      lastSent = 1;
    } else if((lastSent != 0) && !onOff) {
      server->write("0");
      mblog("Wrote 0\n");
      lastSent = 0;
    }
    server->close();
}