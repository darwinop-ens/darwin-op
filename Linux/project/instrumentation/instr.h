#ifndef __INSTR_H__
#define __INSTR_H__

#include <string>
#include "LinuxNetwork.h"
#include "CM730.h"
#include "time.h"

#include "task.h"
#include "webcam.h"

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
		bool m_print_debug;

		void ProcessData(string &data, string &result);

		void ProcessRawReadCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessRawWriteCommand(string::iterator &iterator, string::iterator &end,
string &result);

		void ProcessTextPrintCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextReadCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextStartCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextTimeCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextWriteCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextBallPositionXCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextBallPositionYCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextDebugCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextControllerCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextControllerOnCommand(string::iterator &iterator, string::iterator &end, string &result);
		void ProcessTextControllerOffCommand(string::iterator &iterator, string::iterator &end, string &result);
	public:
		InstrServer(CM730 &cm730);
		~InstrServer();
		virtual void Initialize(void);
		virtual void Execute(void);
	public:
		WebcamThreadArg* webcam;
};

#endif // __INSTR_H__

