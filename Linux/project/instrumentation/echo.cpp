#include <string.h>
#include <sstream>
#include <iostream>

#include "echo.h"

using namespace std;

EchoServer::EchoServer():
	m_server(),
	m_socket(),
	m_connected(false)
{

}

EchoServer::~EchoServer()
{

}

void EchoServer::Initialize(void)
{
	cout << "echo: initializing" << endl;
	m_server.listen(7);
	m_server.set_non_blocking(true);
	m_connected = false;
	cout << "echo: initialized" << endl;
}

void EchoServer::Execute(void)
{
	if (!m_connected)
	{
		m_connected = m_server.accept ( m_socket );
		if (m_connected)
		{
			cout << "echo: connected" << endl;
			m_socket.set_non_blocking(true);
		}
	}
	else // connected
	{
		try
		{
			string data;
			m_socket >> data;
			if ( data != "" ) {
				cout << data << "\n";
				m_socket << data;
			}
		}
		catch ( LinuxSocketException& )
		{
			cout << "echo: disconnected" << endl;
			m_connected = false;
		}
	}
}

