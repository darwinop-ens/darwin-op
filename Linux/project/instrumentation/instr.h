#ifndef __INSTR_H__
#define __INSTR_H__

#include <string>
#include "LinuxNetwork.h"
#include "CM730.h"
#include "time.h"

#include "task.h"

using namespace Robot;
using namespace std;

class InstrServer: public Task
{
	private:
		LinuxServer m_server;
		LinuxSocket m_socket;
		bool m_connected;
		CM730 m_cm730;
		struct timeval m_start_time;

		void ProcessPrintCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessReadCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessStartCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTimeCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessWriteCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessBallPositionXCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessBallPositionYCommand(string::iterator &iterator, string::iterator &end, string &result);

		void ProcessData(string &data, string &result);
	public:
		InstrServer(CM730 &cm730);
		~InstrServer();
		virtual void Initialize(void);
		virtual void Execute(void);
};

#endif // __INSTR_H__

