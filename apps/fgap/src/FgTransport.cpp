/*!
 * @file FgTransport.cpp
 *
 * @brief Transport implementation for interacting with FlightGear
 *
 * @author Oleksii Aliakin (alex@nls.la)
 * @date Created Jan 04, 2015
 * @date Modified Feb 18, 2015
 */

#include "FgTransport.h"
#include "FgGenericProtocol.h"

#include <QUdpSocket>
#include <QDebug>

FgTransport::FgTransport(QObject *parent) :
    QObject(parent),
    m_Socket(nullptr),
    m_SocketOut(nullptr),
    m_Protocol(nullptr),
    m_Ip("127.0.0.1"),
    m_Port(5556),
    m_Buffer()
{
    m_Protocol = new FgGenericProtocol(this);

    m_Socket = new QUdpSocket(this);
    m_SocketOut = new QUdpSocket(this);
    m_Socket->bind(QHostAddress::Any, 5555);

    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(onSocketRead()));

    qDebug() << "FgTransport ready [" << m_Ip.toString() << ":" << m_Port << "]";
}

FgTransport::~FgTransport()
{
    qDebug() << "FgTransport destroyed.";
}

void FgTransport::onSocketRead()
{
    while (m_Socket->hasPendingDatagrams())
    {
        {
            QByteArray datagram;
            datagram.resize(m_Socket->pendingDatagramSize());

            QHostAddress sender;
            quint16 senderPort;
            m_Socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

            // we will process only whole line
            // add datagram to buffer
            m_Buffer.append(datagram);
        }

        int newLineIndex = m_Buffer.indexOf('\n', 0);
        if (newLineIndex < 0)
        {
            return;
        }

        // we have the whole line
        QString line = QString::fromLocal8Bit(m_Buffer.data(), newLineIndex);
        m_FdmData = line.split("\t");
        m_Buffer.remove(0, newLineIndex + 1);

        emit fgDataReceived();
    }
}

bool FgTransport::writeData(const QString &data)
{
//    qDebug() << "Write: " << data;
    m_SocketOut->writeDatagram(data.toLocal8Bit(), m_Ip, m_Port);
    return true;
}

/*

# How to run multiplayer

/usr/games/fgfs \
  --airport=KSFO \
  --runway=10L \
  --aircraft=c172p \
  --console \
  --bpp=32 \
  --disable-random-objects \
  --disable-ai-models \
  --disable-ai-traffic \
  --disable-real-weather-fetch \
  --geometry=1366x768 \
  --timeofday=morning \
  --enable-terrasync \
  --enable-clouds3d \
  --enable-horizon-effect \
  --enable-enhanced-lighting \
  --callsign=App1 \
  --multiplay=out,10,mpserver02.flightgear.org,5000 \
  --multiplay=in,10,,5000 \
  --httpd=5050 \
  --generic=socket,out,40,localhost,5555,udp,FgaProtocol \
  --generic=socket,in,40,localhost,5555,udp,FgaProtocol

  --prop:/engines/engine/running=true
  --prop:/engines/engine/rpm=1000
*/

/* /usr/games/fgfs   --airport=KSFO   --runway=10L   --aircraft=c172p   --console   --bpp=32   --disable-random-objects   --disable-ai-models   --disable-ai-traffic   --disable-real-weather-fetch   --geometry=800x600   --timeofday=morning   --enable-terrasync   --enable-clouds3d   --enable-horizon-effect   --enable-enhanced-lighting   --callsign=App1   --multiplay=out,10,mpserver02.flightgear.org,5000   --multiplay=in,10,,5000   --httpd=5050   --generic=socket,out,40,localhost,5555,udp,FgaProtocol   --generic=socket,in,40,localhost,5556,udp,FgaProtocol --prop:/engines/engine/running=true --prop:/engines/engine/rpm=2000 --in-air --altitude=2000

*/
