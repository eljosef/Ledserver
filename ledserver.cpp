#include "ledserver.h"
#include "config.h"

LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_gpio = new Gpio(this);
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, m_port);
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    qDebug() << "Connected";
    qDebug() << "Bitte Zahl von 0 bis 15 eingeben: ";
    m_socket = m_server->nextPendingConnection();

    // When data received --> executes myServerRead()
    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);
    // When client disconnects --> executes myClientDisconnect()
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    // Turns all 4 LEDs off
    for (int i = 0; i < 4; i++)
    {
        m_gpio->set(LEDS[i], 0);
    }
    qDebug() << "Disconnected";
    m_socket->close();
}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{
    while (m_socket->bytesAvailable()) {
        QString msg = m_socket->readAll(); // reads Input and saves it as a QString in msg
        int LedCount = msg.toInt();
        qDebug() << LedCount;
        if (LedCount >= 0 && LedCount <= 15)
        {
            qDebug() << "OK";
            m_gpio->set(LedCount);
        }
        qDebug() << "\nBitte Zahl von 0 bis 15 eingeben: ";
    }
}
