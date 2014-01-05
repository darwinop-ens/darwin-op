#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#include "timeutils.h"
#include "instr.h"
#include "LinuxDARwIn.h"

using namespace std;

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
			case 2:
				ProcessRawReadCommand(false, iterator, end, result);
				break;
			case 3:
				ProcessRawWriteCommand(false, iterator, end, result);
				break;
			case 17:
				ProcessRawReadCommand(true, iterator, end, result);
				break;
			case 18:
				ProcessRawWriteCommand(true, iterator, end, result);
				break;
			case 'r':
				ProcessTextReadCommand(iterator, end, result);
				break;
			case 's':
				ProcessTextStartCommand(iterator, end, result);
				break;
			case 't':
				ProcessTextTimeCommand(iterator, end, result);
				break;
			case 'w':
				ProcessTextWriteCommand(iterator, end, result);
				break;
			case 'p':
				ProcessTextPrintCommand(iterator, end, result);
				break;
			case 'x':
				ProcessTextBallPositionXCommand(iterator, end, result);
				break;
			case 'y':
				ProcessTextBallPositionYCommand(iterator, end, result);
				break;
			case 'd':
				ProcessTextDebugCommand(iterator, end, result);
				break;
			case 'c':
				ProcessTextControllerCommand(iterator, end, result);
				break;
			case 'n':
				ProcessTextControllerOnCommand(iterator, end, result);
				break;
			case 'f':
				ProcessTextControllerOffCommand(iterator, end, result);
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

void InstrServer::ProcessRawReadCommand(bool internal, string::iterator &iterator, string::iterator &end, string &result)
{
	unsigned char id, start, length;
	unsigned char buf[256];

	// skip 2
	iterator++;
	// get id, start and length
	id = *iterator;
	iterator++;
	start = *iterator;
	iterator++;
	length = *iterator;
	iterator++;

	if (internal)
	{
		if (m_print_debug)
			cout << "instr: internal read id = " << id << " start = " << start << " length = " << length << endl;
		switch (id)
		{
			case 1:
				WebcamReadTable(buf);
				break;
			default:
				cout << "invalid internal id = " << id << endl;
				memset(buf, 0, 256);
				break;
		}
	}
	else
	{
		if (m_print_debug)
			cout << "instr: read id = " << id << " start = " << start << " length = " << length << endl;
		m_cm730.ReadTable(id, start, start+length-1, buf, NULL);
	}
	for (int i=0;i<length;i++)
		result += buf[start+i];
}

void InstrServer::ProcessRawWriteCommand(bool internal, string::iterator &iterator, string::iterator &end, string &result)
{
	unsigned char id, start, length;
	unsigned char buf[256];

	// skip 2
	iterator++;
	// get id, start and length
	id = *iterator;
	iterator++;
	start = *iterator;
	iterator++;
	length = *iterator;
	iterator++;

	for (int i=0;i<length;i++)
	{
		buf[start+i] = *iterator;
		iterator++;
	}
	if (internal)
	{
		if (m_print_debug)
			cout << "instr: internal write id = " << id << " start = " << start << " length = " << length << endl;
		switch (id)
		{
			case 1:
				WebcamWriteTable(start, start+length-1, buf);
				break;
			default:
				cout << "invalid internal id = " << id << endl;
				break;
		}
	}
	else
	{
		if (m_print_debug)
			cout << "instr: write id = " << id << " start = " << start << " length = " << length << endl;
		m_cm730.WriteTable(id, start, start+length-1, buf, NULL);
	}
}

void InstrServer::ProcessTextBallPositionXCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "x"
	iterator++;

	// print result
	char buf[10];
	sprintf(buf, "%d", webcam->BallPositionX);
	result += string(buf);

	if (m_print_debug)
		cout << "instr: X Command = " << string(buf) << endl;
}

void InstrServer::ProcessTextBallPositionYCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "y"
	iterator++;

	// print result
	char buf[10];
	sprintf(buf, "%d", webcam->BallPositionY);
	result += string(buf);

	if (m_print_debug)
		cout << "instr: Y Command = " << string(buf) << endl;
}

void InstrServer::ProcessTextControllerCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	string s_te(""), s_rx(""), s_px(""), s_ix(""), s_ry(""), s_py(""), s_iy("");

	// skip "c"
	iterator++;

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse sampling time
	while ((iterator < end) && !isspace(*iterator))
	{
		s_te += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse reference x
	while ((iterator < end) && !isspace(*iterator))
	{
		s_rx += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse proportional x
	while ((iterator < end) && !isspace(*iterator))
	{
		s_px += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse integral x
	while ((iterator < end) && !isspace(*iterator))
	{
		s_ix += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse reference y
	while ((iterator < end) && !isspace(*iterator))
	{
		s_ry += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse proportional y
	while ((iterator < end) && !isspace(*iterator))
	{
		s_py += *iterator;
		iterator++;
	}

	// skip spaces
	while ((iterator < end) && isspace(*iterator))
		iterator++;

	// parse integral y
	while ((iterator < end) && !isspace(*iterator))
	{
		s_iy += *iterator;
		iterator++;
	}

	// convert string values to integer
	controllersettings->ControllerSamplingTime = atoi(s_te.c_str());
	controllersettings->ControllerReferenceX = atoi(s_rx.c_str());
	controllersettings->ControllerProportionalX = atof(s_px.c_str());
	controllersettings->ControllerIntegralX = atof(s_ix.c_str());
	controllersettings->ControllerReferenceY = atoi(s_ry.c_str());
	controllersettings->ControllerProportionalY = atof(s_py.c_str());
	controllersettings->ControllerIntegralY = atof(s_iy.c_str());

	if (m_print_debug)
		cout << "instr: controller command te=" << s_te
                     << " refx=" << s_rx << " propx=" << s_px << " intx=" << s_ix
                     << " refy=" << s_ry << " propy=" << s_py << " inty=" << s_iy << endl;
}

void InstrServer::ProcessTextControllerOnCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "n"
	iterator++;

	controllersettings->ControllerEnable = true;

	if (m_print_debug)
		cout << "instr: controller on" << endl;
}

void InstrServer::ProcessTextControllerOffCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "f"
	iterator++;

	controllersettings->ControllerEnable = false;

	if (m_print_debug)
		cout << "instr: controller off" << endl;
}

void InstrServer::ProcessTextDebugCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip 'd'
	iterator++;

	m_print_debug = true;

	cout << "instr: debug enabled" << endl;
}

void InstrServer::ProcessTextPrintCommand(string::iterator &iterator, string::iterator &end, string &result)
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

void InstrServer::ProcessTextReadCommand(string::iterator &iterator, string::iterator &end, string &result)
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

void InstrServer::ProcessTextStartCommand(string::iterator &iterator, string::iterator &end, string &result)
{
	// skip "s"
	iterator++;

	// get current time
	gettimeofday(&m_start_time, NULL);

	if (m_print_debug)
		cout << "instr: start Command" << endl;
}

void InstrServer::ProcessTextTimeCommand(string::iterator &iterator, string::iterator &end, string &result)
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

void InstrServer::ProcessTextWriteCommand(string::iterator &iterator, string::iterator &end, string &result)
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

void InstrServer::WebcamReadTable(unsigned char *buf)
{
	buf[1] = webcam->Enable;
	*((int*)&buf[2]) = Camera::WIDTH;
	*((int*)&buf[4]) = Camera::HEIGHT;
	buf[6] = webcam->BallEnable;
	buf[7] = webcam->RedEnable;
	buf[8] = webcam->YellowEnable;
	buf[9] = webcam->BlueEnable;

	*((int*)&buf[20]) = webcam->BallFinder->m_hue;
	*((int*)&buf[22]) = webcam->BallFinder->m_hue_tolerance;
	buf[24] = webcam->BallFinder->m_min_saturation;
	buf[25] = webcam->BallFinder->m_max_saturation;
	buf[26] = webcam->BallFinder->m_min_value;
	buf[27] = webcam->BallFinder->m_max_value;
	buf[28] = webcam->BallFinder->m_min_percent;
	buf[29] = webcam->BallFinder->m_min_percent;
	*((int*)&buf[30]) = webcam->BallPositionX;
	*((int*)&buf[32]) = webcam->BallPositionY;

	*((int*)&buf[40]) = webcam->RedFinder->m_hue;
	*((int*)&buf[42]) = webcam->RedFinder->m_hue_tolerance;
	buf[44] = webcam->RedFinder->m_min_saturation;
	buf[45] = webcam->RedFinder->m_max_saturation;
	buf[46] = webcam->RedFinder->m_min_value;
	buf[47] = webcam->RedFinder->m_max_value;
	buf[48] = webcam->RedFinder->m_min_percent;
	buf[49] = webcam->RedFinder->m_min_percent;
	*((int*)&buf[50]) = webcam->RedPositionX;
	*((int*)&buf[52]) = webcam->RedPositionY;

	*((int*)&buf[60]) = webcam->YellowFinder->m_hue;
	*((int*)&buf[62]) = webcam->YellowFinder->m_hue_tolerance;
	buf[64] = webcam->YellowFinder->m_min_saturation;
	buf[65] = webcam->YellowFinder->m_max_saturation;
	buf[66] = webcam->YellowFinder->m_min_value;
	buf[67] = webcam->YellowFinder->m_max_value;
	buf[68] = webcam->YellowFinder->m_min_percent;
	buf[69] = webcam->YellowFinder->m_min_percent;
	*((int*)&buf[70]) = webcam->YellowPositionX;
	*((int*)&buf[72]) = webcam->YellowPositionY;

	*((int*)&buf[80]) = webcam->BlueFinder->m_hue;
	*((int*)&buf[82]) = webcam->BlueFinder->m_hue_tolerance;
	buf[84] = webcam->BlueFinder->m_min_saturation;
	buf[85] = webcam->BlueFinder->m_max_saturation;
	buf[86] = webcam->BlueFinder->m_min_value;
	buf[87] = webcam->BlueFinder->m_max_value;
	buf[88] = webcam->BlueFinder->m_min_percent;
	buf[89] = webcam->BlueFinder->m_min_percent;
	*((int*)&buf[90]) = webcam->BluePositionX;
	*((int*)&buf[92]) = webcam->BluePositionY;
}

void InstrServer::WebcamWriteTable(unsigned char start, unsigned char end, unsigned char *buf)
{
	if (start <= 1 && end >= 1)
		webcam->Enable = buf[1];
	if (start <= 6 && end >= 6)
		webcam->BallEnable = buf[6];
	if (start <= 7 && end >= 7)
		webcam->RedEnable = buf[7];
	if (start <= 8 && end >= 8)
		webcam->YellowEnable = buf[8];
	if (start <= 9 && end >= 9)
		webcam->BlueEnable = buf[9];

	if (start <= 20 && end >= 21)
		webcam->BallFinder->m_hue = *((int*)&buf[20]);
	if (start <= 22 && end >= 23)
		webcam->BallFinder->m_hue_tolerance = *((int*)&buf[22]);
	if (start <= 24 && end >= 24)
		webcam->BallFinder->m_min_saturation = buf[24];
	if (start <= 25 && end >= 25)
		webcam->BallFinder->m_max_saturation = buf[25];
	if (start <= 26 && end >= 26)
		webcam->BallFinder->m_min_value = buf[26];
	if (start <= 27 && end >= 27)
		webcam->BallFinder->m_max_value = buf[27];
	if (start <= 28 && end >= 28)
		webcam->BallFinder->m_min_percent = buf[28];
	if (start <= 29 && end >= 29)
		webcam->BallFinder->m_min_percent = buf[29];

	if (start <= 40 && end >= 41)
		webcam->RedFinder->m_hue = *((int*)&buf[40]);
	if (start <= 42 && end >= 43)
		webcam->RedFinder->m_hue_tolerance = *((int*)&buf[42]);
	if (start <= 44 && end >= 44)
		webcam->RedFinder->m_min_saturation = buf[44];
	if (start <= 45 && end >= 45)
		webcam->RedFinder->m_max_saturation = buf[45];
	if (start <= 46 && end >= 46)
		webcam->RedFinder->m_min_value = buf[46];
	if (start <= 47 && end >= 47)
		webcam->RedFinder->m_max_value = buf[47];
	if (start <= 48 && end >= 48)
		webcam->RedFinder->m_min_percent = buf[48];
	if (start <= 49 && end >= 49)
		webcam->RedFinder->m_min_percent = buf[49];

	if (start <= 60 && end >= 61)
		webcam->YellowFinder->m_hue = *((int*)&buf[60]);
	if (start <= 62 && end >= 63)
		webcam->YellowFinder->m_hue_tolerance = *((int*)&buf[62]);
	if (start <= 64 && end >= 64)
		webcam->YellowFinder->m_min_saturation = buf[64];
	if (start <= 65 && end >= 65)
		webcam->YellowFinder->m_max_saturation = buf[65];
	if (start <= 66 && end >= 66)
		webcam->YellowFinder->m_min_value = buf[66];
	if (start <= 67 && end >= 67)
		webcam->YellowFinder->m_max_value = buf[67];
	if (start <= 68 && end >= 68)
		webcam->YellowFinder->m_min_percent = buf[68];
	if (start <= 69 && end >= 69)
		webcam->YellowFinder->m_min_percent = buf[69];

	if (start <= 80 && end >= 81)
		webcam->BlueFinder->m_hue = *((int*)&buf[80]);
	if (start <= 82 && end >= 83)
		webcam->BlueFinder->m_hue_tolerance = *((int*)&buf[82]);
	if (start <= 84 && end >= 84)
		webcam->BlueFinder->m_min_saturation = buf[84];
	if (start <= 85 && end >= 85)
		webcam->BlueFinder->m_max_saturation = buf[85];
	if (start <= 86 && end >= 86)
		webcam->BlueFinder->m_min_value = buf[86];
	if (start <= 87 && end >= 87)
		webcam->BlueFinder->m_max_value = buf[87];
	if (start <= 88 && end >= 88)
		webcam->BlueFinder->m_min_percent = buf[88];
	if (start <= 89 && end >= 89)
		webcam->BlueFinder->m_min_percent = buf[89];
}

