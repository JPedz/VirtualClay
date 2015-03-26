#include "stdafx.h"
#include "AirPen_Server.h"


AirPen_Server::AirPen_Server(void)
{
  lastSent = 0;
  server = new QTcpSocket();
  server->connectToHost(IP, port);
  server->write("0");
  server->waitForBytesWritten(200);
  server->close();
  isConnected = false;
}


AirPen_Server::~AirPen_Server(void)
{
  if(isConnected)
    server->close();
  delete server;
}

void AirPen_Server::SendMsg(int stage) {
  //LastSent ensures we are not spamming the network.
  //mblog("Conecting\n");
  //mblog("Server socket is valid\n");
  if(isConnected) 
    if((lastSent != 1) && (stage == 1)) {
      server->connectToHost(IP, port);
      server->write("1");
      server->waitForBytesWritten(200);
      server->flush();
      server->close();
      mblog("Wrote 1\n");
      lastSent = 1;
    } else if((lastSent != 0) && (stage == 0)) {
      server->connectToHost(IP, port);
      server->write("0");
      server->waitForBytesWritten(200);
      server->flush();
      server->close();
      mblog("Wrote 0\n");
      lastSent = 0;
    } else if((lastSent != 2) && (stage == 2)) {
      server->connectToHost("10.0.0.15", port);
      server->write("2");
      server->waitForBytesWritten(200);
      server->flush();
      server->close();
      mblog("Wrote 2\n");
      lastSent = 2;
    }// else {
    //  //Ensure something is sent
    //  switch(lastSent) {
    //    case 1:
    //      server->write("1");
    //      break;
    //    case 2:
    //      server->write("2");
    //      break;
    //    default:
    //      server->write("0");
    //      break;
    //  }
    //  server->waitForBytesWritten(200);
    //}
}