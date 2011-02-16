/*
 *   CM730.cpp
 *
 *   Author: ROBOTIS
 *
 */
#include <stdio.h>
#include "CM730.h"

using namespace Robot;


#define ID					(2)
#define LENGTH				(3)
#define INSTRUCTION			(4)
#define ERRBIT				(4)
#define PARAMETER			(5)
#define DEFAULT_BAUDNUMBER	(1)
#define MAXNUM_TXPARAM		(150)
#define MAXNUM_RXPARAM		(60)

#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)


CM730::CM730(PlatformCM730 *platform)
{
	m_Platform = platform;
	DEBUG_PRINT = false;
}

CM730::~CM730()
{
	Disconnect();
}

int CM730::TxRxPacket(unsigned char *txpacket, unsigned char *rxpacket, int priority)
{
	if(priority > 1)
		m_Platform->LowPriorityWait();
	if(priority > 0)
		m_Platform->MidPriorityWait();
	m_Platform->HighPriorityWait();

	int res;
	int length = txpacket[LENGTH] + 4;

	txpacket[0] = 0xFF;
    txpacket[1] = 0xFF;
	txpacket[length - 1] = CalculateChecksum(txpacket);

	if(DEBUG_PRINT == true)
	{
		fprintf(stderr, "\nTX: ");
		for(int n=0; n<length; n++)
			fprintf(stderr, "%.2X ", txpacket[n]);

		fprintf(stderr, "INST: ");
		switch(txpacket[INSTRUCTION])
		{
		case INST_PING:
			fprintf(stderr, "PING\n");
			break;

		case INST_READ:
			fprintf(stderr, "READ\n");
			break;

		case INST_WRITE:
			fprintf(stderr, "WRITE\n");
			break;

		case INST_REG_WRITE:
			fprintf(stderr, "REG_WRITE\n");
			break;

		case INST_ACTION:
			fprintf(stderr, "ACTION\n");
			break;

		case INST_RESET:
			fprintf(stderr, "RESET\n");
			break;

		case INST_SYNC_WRITE:
			fprintf(stderr, "SYNC_WRITE\n");
			break;

		default:
			fprintf(stderr, "UNKNOWN\n");
			break;
		}
	}

	if(length < (MAXNUM_TXPARAM + 6))
	{
		m_Platform->ClearPort();
		if(m_Platform->WritePort(txpacket, length) == length)
		{
			if (txpacket[ID] != ID_BROADCAST)
			{
				int to_length = 0;

				if(txpacket[INSTRUCTION] == INST_READ)
					to_length = txpacket[PARAMETER+1] + 6;
				else
					to_length = 6;

				m_Platform->SetPacketTimeout(length);

				int get_length = 0;
				if(DEBUG_PRINT == true)
					fprintf(stderr, "RX: ");
				
				while(1)
				{
					length = m_Platform->ReadPort(&rxpacket[get_length], to_length - get_length);
					if(DEBUG_PRINT == true)
					{
						for(int n=0; n<length; n++)
							fprintf(stderr, "%.2X ", rxpacket[get_length + n]);
					}
					get_length += length;

					if(get_length == to_length)
					{
						// Find packet header
						int i;
						for(i = 0; i < (get_length - 1); i++)
						{
							if(rxpacket[i] == 0xFF && rxpacket[i+1] == 0xFF)
								break;
							else if(i == (get_length - 2) && rxpacket[get_length - 1] == 0xFF)
								break;
						}

						if(i == 0)
						{
							// Check checksum
							unsigned char checksum = CalculateChecksum(rxpacket);
							if(DEBUG_PRINT == true)
								fprintf(stderr, "CHK:%.2X\n", checksum);

							if(rxpacket[get_length-1] == checksum)
								res = SUCCESS;
							else
								res = RX_CORRUPT;
							
							break;
						}
						else
						{
							for(int j = 0; j < (get_length - i); j++)
								rxpacket[j] = rxpacket[j+i];
							get_length -= i;
						}						
					}
					else
					{
						if(m_Platform->IsPacketTimeout() == true)
						{
							if(get_length == 0)
								res = RX_TIMEOUT;
							else
								res = RX_CORRUPT;
							
							break;
						}
					}
				}
			}
			else
				res = SUCCESS;			
		}
		else
			res = TX_FAIL;		
	}
	else
		res = TX_CORRUPT;

	if(DEBUG_PRINT == true)
	{
		fprintf(stderr, "Time:%.2fms  ", m_Platform->GetPacketTime());
		fprintf(stderr, "RETURN: ");
		switch(res)
		{
		case SUCCESS:
			fprintf(stderr, "SUCCESS\n");
			break;

		case TX_CORRUPT:
			fprintf(stderr, "TX_CORRUPT\n");
			break;

		case TX_FAIL:
			fprintf(stderr, "TX_FAIL\n");
			break;

		case RX_FAIL:
			fprintf(stderr, "RX_FAIL\n");
			break;

		case RX_TIMEOUT:
			fprintf(stderr, "RX_TIMEOUT\n");
			break;

		case RX_CORRUPT:
			fprintf(stderr, "RX_CORRUPT\n");
			break;

		default:
			fprintf(stderr, "UNKNOWN\n");
			break;
		}
	}

	if(priority > 1)
		m_Platform->LowPriorityRelease();
	if(priority > 0)
		m_Platform->MidPriorityRelease();
	m_Platform->HighPriorityRelease();

	return res;
}

unsigned char CM730::CalculateChecksum(unsigned char *packet)
{
	unsigned char checksum = 0x00;
	for(int i=2; i<packet[LENGTH]+3; i++ )
		checksum += packet[i];
	return (~checksum);
}

int CM730::UpdateTable(int *error)
{
	if(m_Platform->IsUpdateTimeout() == false)
		return SUCCESS;

	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;
	int address = P_DXL_POWER;
	int length = MAXNUM_ADDRESS - P_DXL_POWER;

    txpacket[ID]           = (unsigned char)ID_CM;
    txpacket[INSTRUCTION]  = INST_READ;
	txpacket[PARAMETER]    = (unsigned char)address;
    txpacket[PARAMETER+1]  = (unsigned char)length;
    txpacket[LENGTH]       = 4;

	result = TxRxPacket(txpacket, rxpacket, 1);
	if(result == SUCCESS)
	{
		for(int i=0; i<length; i++)
			m_ControlTable[address + i] = rxpacket[PARAMETER + i];

		if(error != 0)
			*error = (int)rxpacket[ERRBIT];

		m_Platform->SetUpdateTimeout(RefreshTime);
	}

	return result;
}

int CM730::SyncWrite(int start_addr, int each_length, int number, int *pParam)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int n;

    txpacket[ID]                = (unsigned char)ID_BROADCAST;
    txpacket[INSTRUCTION]       = INST_SYNC_WRITE;
    txpacket[PARAMETER]			= (unsigned char)start_addr;
    txpacket[PARAMETER + 1]		= (unsigned char)(each_length - 1);
    for(n = 0; n < (number * each_length); n++)
        txpacket[PARAMETER + 2 + n]   = (unsigned char)pParam[n];
    txpacket[LENGTH]            = n + 4;

    return TxRxPacket(txpacket, rxpacket, 0);
}

bool CM730::Connect()
{
	if(m_Platform->OpenPort() == false)
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, " Fail to open port\n");
		return false;
	}

	return DXLPowerOn();
}

bool CM730::DXLPowerOn()
{
	if(WriteByte(CM730::ID_CM, CM730::P_DXL_POWER, 1, 0) == CM730::SUCCESS)
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, " Succeed to change Dynamixel power!\n");
		
		WriteWord(CM730::ID_CM, CM730::P_LED_HEAD_L, MakeColor(0, 128, 255), 0);
		m_Platform->Sleep(300); // about 300msec
	}
	else
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, " Fail to change Dynamixel power!\n");
		return false;
	}

	return true;
}

void CM730::Disconnect()
{
	WriteWord(CM730::ID_CM, CM730::P_LED_HEAD_L, MakeColor(0, 255, 0), 0);
	m_Platform->ClosePort();
}

int CM730::ReadByte(int address, int *pValue, int *error)
{
	int result = UpdateTable(error);

	if(result == SUCCESS)
	{
		*pValue = m_ControlTable[address];
	}

	return result;
}

int CM730::ReadWord(int address, int *pValue, int *error)
{
	int result = UpdateTable(error);

	if(result == SUCCESS)
	{
		*pValue = MakeWord(m_ControlTable[address], m_ControlTable[address+1]);
	}

	return result;
}

int CM730::ReadTable(int start_addr, int end_addr, unsigned char *table, int *error)
{
	int result = UpdateTable(error);

	if(result == SUCCESS)
	{
		for(int i=start_addr; i<=end_addr; i++)
			table[i] = m_ControlTable[i];
	}

	return result;
}

int CM730::WriteByte(int address, int value, int *error)
{
	return WriteByte(ID_CM, address, value, error);
}

int CM730::WriteWord(int address, int value, int *error)
{
	return WriteWord(ID_CM, address, value, error);
}

int CM730::Ping(int id, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_PING;
    txpacket[LENGTH]       = 2;

	result = TxRxPacket(txpacket, rxpacket, 2);
	if(result == SUCCESS && txpacket[ID] != ID_BROADCAST)
	{		
		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::ReadByte(int id, int address, int *pValue, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_READ;
	txpacket[PARAMETER]    = (unsigned char)address;
    txpacket[PARAMETER+1]  = 1;
    txpacket[LENGTH]       = 4;

	result = TxRxPacket(txpacket, rxpacket, 2);
	if(result == SUCCESS)
	{
		*pValue = (int)rxpacket[PARAMETER];
		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::ReadWord(int id, int address, int *pValue, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_READ;
	txpacket[PARAMETER]    = (unsigned char)address;
    txpacket[PARAMETER+1]  = 2;
    txpacket[LENGTH]       = 4;

	result = TxRxPacket(txpacket, rxpacket, 2);
	if(result == SUCCESS)
	{
		*pValue = MakeWord((int)rxpacket[PARAMETER], (int)rxpacket[PARAMETER + 1]);

		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::ReadTable(int id, int start_addr, int end_addr, unsigned char *table, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;
	int length = end_addr - start_addr + 1;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_READ;
	txpacket[PARAMETER]    = (unsigned char)start_addr;
    txpacket[PARAMETER+1]  = (unsigned char)length;
    txpacket[LENGTH]       = 4;

	result = TxRxPacket(txpacket, rxpacket, 1);
	if(result == SUCCESS)
	{
		for(int i=0; i<length; i++)
			table[start_addr + i] = rxpacket[PARAMETER + i];

		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::WriteByte(int id, int address, int value, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_WRITE;
	txpacket[PARAMETER]    = (unsigned char)address;
    txpacket[PARAMETER+1]  = (unsigned char)value;
    txpacket[LENGTH]       = 4;

	result = TxRxPacket(txpacket, rxpacket, 2);
	if(result == SUCCESS && id != ID_BROADCAST)
	{
		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::WriteWord(int id, int address, int value, int *error)
{
	unsigned char txpacket[MAXNUM_TXPARAM + 10] = {0, };
	unsigned char rxpacket[MAXNUM_RXPARAM + 10] = {0, };
	int result;

    txpacket[ID]           = (unsigned char)id;
    txpacket[INSTRUCTION]  = INST_WRITE;
	txpacket[PARAMETER]    = (unsigned char)address;
    txpacket[PARAMETER+1]  = (unsigned char)GetLowByte(value);
	txpacket[PARAMETER+2]  = (unsigned char)GetHighByte(value);
    txpacket[LENGTH]       = 5;

	result = TxRxPacket(txpacket, rxpacket, 2);
	if(result == SUCCESS && id != ID_BROADCAST)
	{
		if(error != 0)
			*error = (int)rxpacket[ERRBIT];
	}

	return result;
}

int CM730::MakeWord(int lowbyte, int highbyte)
{
	unsigned short word;

    word = highbyte;
    word = word << 8;
    word = word + lowbyte;

    return (int)word;
}

int CM730::GetLowByte(int word)
{
	unsigned short temp;
    temp = word & 0xff;
    return (int)temp;
}

int CM730::GetHighByte(int word)
{
	unsigned short temp;
    temp = word & 0xff00;
    return (int)(temp >> 8);
}

int CM730::MakeColor(int red, int green, int blue)
{
	int r = red & 0xFF;
	int g = green & 0xFF;
	int b = blue & 0xFF;

	return (int)(((r>>3)<<10)|((g>>3)<<5)|(b>>3));
}