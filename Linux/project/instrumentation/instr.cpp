#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#include "instr.h"
#include "LinuxDARwIn.h"

using namespace std;

// written by webcam.cpp
extern int BallPositionX;
extern int BallPositionY;

InstrServer::InstrServer(CM730 &cm730):
	m_server(),
	m_socket(),
	m_connected(false),
	m_cm730(cm730),
	m_print_debug(false)
{

}

InstrServer::~InstrServer()
{

}

void InstrServer::Initialize(void)
{
	cout << "instr: initializing" << endl;

	// create server socket
	m_server.listen(1234);
	m_server.set_non_blocking(true);
	m_connected = false;

	// get current time as time reference
	gettimeofday(&m_start_time, NULL);

	// change current thread priority to reduce latency
	struct sched_param params;
	params.sched_priority = 99;
	if (pthread_setschedparam(pthread_self(), SCHED_RR, &params))
		cout << "instr: failed to change current thread priority" << endl;

	cout << "instr: initialized" << endl;
}

void InstrServer::Execute(void)
{
	if (!m_connected)
	{
		m_connected = m_server.accept ( m_socket );
		if (m_connected)
		{
			cout << "instr: connected" << endl;
			m_socket.set_non_blocking(true);
			m_print_debug = false;
		}
	}
	else // connected
	{
		try
		{
			string data_in;
			m_socket >> data_in;
			if ( data_in != "" )
			{
				string data_out;
				ProcessData(data_in, data_out);
				if (data_out != "")
					m_socket << data_out;
			}
		}
		catch ( LinuxSocketException& )
		{
			cout << "instr: disconnected" << endl;
			m_connected = false;
		}
	}
}

void InstrServer::ProcessBallPositionXCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "x"
	iterator++;

	// print result
	char buf[10];
	sprintf(buf, "%d", BallPositionX);
	result += string(buf);

	if (m_print_debug)
		cout << "instr: X Command = " << string(buf) << endl;
}

void InstrServer::ProcessBallPositionYCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "y"
	iterator++;

	// print result
	char buf[10];
	sprintf(buf, "%d", BallPositionY);
	result += string(buf);

	if (m_print_debug)
		cout << "instr: Y Command = " << string(buf) << endl;
}

void InstrServer::ProcessData(string &data, string &result)
{
	string::iterator iterator = data.begin();
	string::iterator end = data.end();

	if (m_print_debug)
		cout << "instr: processing data" << endl;

	while ( iterator < end )
	{
                // one command per line
		// command format
                // "t" print current time in ms
                // "r 12 36 2" with r = read, 12 = id, 36 = start address, 2 = length
                // "w 12 36 2 1534" with w = write, 12 = id, 36 = start address, 2 = length, 1534 = value
		// "p "blah"" with p = print
		switch(*iterator) {
			case 'r':
				ProcessReadCommand(iterator, end, result);
				break;
			case 's':
				ProcessStartCommand(iterator, end, result);
				break;
			case 't':
				ProcessTimeCommand(iterator, end, result);
				break;
			case 'w':
				ProcessWriteCommand(iterator, end, result);
				break;
			case 'p':
				ProcessPrintCommand(iterator, end, result);
				break;
			case 'x':
				ProcessBallPositionXCommand(iterator, end, result);
				break;
			case 'y':
				ProcessBallPositionYCommand(iterator, end, result);
				break;
			case 'd':
				ProcessDebugCommand(iterator, end, result);
				break;
			case '\r':
				result += '\r';
				iterator++;
				break;
			case '\n':
				result += '\n';
				iterator++;
				break;
			case ' ':
				iterator++;
				break;
			default:
				// affiche un message
				cout << "instr: unknown command \"" << *iterator << "\"" << endl;
				// skip current line
				while ((iterator < end) && (*iterator != '\n'))
					iterator++;
				break;
		}
	}

	if (m_print_debug)
		cout << "instr: processed data" << endl;
}

void InstrServer::ProcessDebugCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip 'd'
	iterator++;

	m_print_debug = true;

	cout << "instr: debug enabled" << endl;
}

void InstrServer::ProcessPrintCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip 'p'
	iterator++;

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse the data
	if (iterator < end)
	{
		if ((*iterator) == '\"')
		{
			// data is quoted
			//cout << "quoted" << endl;

			// skip heading quote
			iterator++;
			while ((iterator < end) && (*iterator != '\"'))
			{
				result += *iterator;
				iterator++;
			}

			// skip trailing quote
			if (*iterator == '\"')
				iterator++;
		}
		else
		{
			// data is not quoted
			//cout << "not quoted" << endl;

			// copy until first space
			while ((iterator < end) && !isspace(*iterator))
			{
				result += *iterator;
				iterator++;
			}
		}
	}
}

void InstrServer::ProcessReadCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	string s_id(""), s_start(""), s_length("");
	int id = -1, start = -1, length = -1;

	// skip 'r'
	iterator++;

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse id
	while ((iterator < end) && isdigit(*iterator))
	{
		s_id += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse start
	while ((iterator < end) && isdigit(*iterator))
	{
		s_start += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse length
	while ((iterator < end) && isdigit(*iterator))
	{
		s_length += *iterator;
		iterator++;
	}

	// convert string values to integer
	id = atoi(s_id.c_str());
	start = atoi(s_start.c_str());
	length = atoi(s_length.c_str());

	if (m_print_debug)
		cout << "instr: read id = " << id << " start = " << start << " length = " << length << endl;

	// check if conversions were successful
	if ((id >= 0) && (start >= 0) && (length >= 0))
	{
		// so far, so good
		if (length == 2)
		{
			// read 2 bytes
			int v = 0;
			char buf[10];
			m_cm730.ReadWord(id, start, &v, 0);
			sprintf(buf, "%d", v);
			result += string(buf);
		}
		else if (length == 1)
		{
			// read 1 byte
			int v = 0;
			char buf[10];
			m_cm730.ReadByte(id, start, &v, 0);
			sprintf(buf, "%d", v);
			result += string(buf);
		}
		else
		{
			cout << "instr: read: unsupported length" << endl;
		}
	}
	else
	{
		cout << "instr: read: syntax error" << endl;
	}
}

void InstrServer::ProcessStartCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "s"
	iterator++;

	// get current time
	gettimeofday(&m_start_time, NULL);

	if (m_print_debug)
		cout << "instr: start Command" << endl;
}

// the following function is taken from http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */
     
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec)
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait. tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
     
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void InstrServer::ProcessTimeCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	struct timeval current_time, diff;
	char buf[10];

	// skip "t"
	iterator++;

	// get current time
	gettimeofday(&current_time, NULL);
	// compute time difference
	timeval_subtract(&diff, &current_time, &m_start_time);

	// print result
	sprintf(buf, "%d", (int)(diff.tv_sec*1000 + diff.tv_usec/1000));
	result += string(buf);

	if (m_print_debug)
		cout << "instr: Time Command = " << string(buf) << endl;
}

void InstrServer::ProcessWriteCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	string s_id(""), s_start(""), s_length(""), s_value("");
	int id = -1, start = -1, length = -1, value = -1;

	// skip 'w'
	iterator++;

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse id
	while ((iterator < end) && isdigit(*iterator))
	{
		s_id += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse start
	while ((iterator < end) && isdigit(*iterator))
	{
		s_start += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse length
	while ((iterator < end) && isdigit(*iterator))
	{
		s_length += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse value
	while ((iterator < end) && isdigit(*iterator))
	{
		s_value += *iterator;
		iterator++;
	}

	// convert string values to integer
	id = atoi(s_id.c_str());
	start = atoi(s_start.c_str());
	length = atoi(s_length.c_str());
	value = atoi(s_value.c_str());

	if (m_print_debug)
		cout << "instr: write id = " << id << " start = " << start << " length = " << length << " value = " << value << endl;

	// check if conversions were successful
	if ((id >= 0) && (start >= 0) && (length >= 0))
	{
		// so far, so good
		if (length == 2)
		{
			// write 2 bytes
			m_cm730.WriteWord(id, start, value, 0);
		}
		else if (length == 1)
		{
			// write 1 byte
			m_cm730.WriteByte(id, start, value, 0);
		}
		else
		{
			cout << "instr: write: unsupported length" << endl;
		}
	}
	else
	{
		cout << "instr: write: syntax error" << endl;
	}
}

