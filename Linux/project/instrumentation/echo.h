#ifndef __ECHO_H__
#define __ECHO_H__

#include "LinuxNetwork.h"

#include "task.h"

using namespace Robot;

class EchoServer: public Task
{
	private:
		LinuxServer m_server;
		LinuxSocket m_socket;
		bool m_connected;
	public:
		EchoServer();
		~EchoServer();
		virtual void Initialize(void);
		virtual void Execute(void);
};

#endif // __ECHO_H__

