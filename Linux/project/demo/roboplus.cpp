#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
//#include <stdlib.h>
//#include <unistd.h>
//#include <termios.h>
//#include <term.h>
//#include <signal.h>
//#include <ncurses.h>
//#include <libgen.h>
#include "LinuxDARwIn.h"

using namespace Robot;
using namespace std;

#define MOTION_FILE_PATH    "../../../Data/motion_4096.bin"

#define VERSION                    "1.000"
#define TCPIP_PORT                6501
#define ROBOPLUS_JOINT_MAXNUM    26
#define ARGUMENT_NAXNUM            30

string* string_split(string str_org, string str_tok)
{
    int cutAt;
    int index = 0;

    string* str_result = new string[ARGUMENT_NAXNUM];

    while((cutAt = str_org.find_first_of(str_tok)) != str_org.npos)
    {
        if(cutAt > 0)
            str_result[index++] = str_org.substr(0, cutAt);
        str_org = str_org.substr(cutAt + 1);
    }
    
    if(str_org.length() > 0)
        str_result[index++] = str_org.substr(0, cutAt);

    return str_result;
}

void roboplus_exec(CM730 &cm730, LinuxServer &server, LinuxSocket &new_sock, bool &connected, LinuxMotionTimer &motion_timer)
{
    if (!connected)
    {
        //cout << "[Waiting..]" << endl;  
        connected = server.accept ( new_sock );
        if(connected)
        {
           cout << "[Accepted..]" << endl;
           new_sock.set_non_blocking(true);
        }
    }
    else // connected
    {
        try
            {
                string data_in;
                stringstream data_out;
                Action::PAGE page;

                new_sock >> data_in;

                if (data_in != "")
                {
                    cout << data_in << endl;

                    string* p_str_tok = string_split(data_in, " ");

                    if(p_str_tok[0] == "v")
                    {
                        data_out << "{[DARwIn:" << VERSION << "]}\n";
                    }
                    else if(p_str_tok[0] == "E")
                    {
                        data_out << "{[DARwIn:1.000]}";
                        data_out << "{[PC:TCP/IP][DXL:1000000(BPS)]}";
                        data_out << "{";
                        for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
                            data_out << "[" << id << ":029(MX-28)]";
                        data_out << "}";
                        data_out << "{[DXL:" << JointData::NUMBER_OF_JOINTS << "(PCS)]}";
                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "exit")
                    {
                        Action::GetInstance()->Stop();
                        throw LinuxSocketException("");
                    }
                    else if(p_str_tok[0] == "List")
                    {
                        data_out << "{";
                        for(int i = 0; i < 256; i++)
                        {
                            Action::GetInstance()->LoadPage(i, &page);
                            data_out << "[" << i << ":";

                            //data_out.send(page.header.name, 14);
                            string name((char *)page.header.name);
                            while( name.length() < 14 ) name += " ";
                            data_out << name;

                            data_out << ":" << (int)page.header.next << ":" << (int)page.header.exit << ":" << (int)page.header.stepnum << "]";
                        }
                        data_out << "}";
                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "Get" || p_str_tok[0] == "on" || p_str_tok[0] == "off")
                    {
                        if(p_str_tok[0] == "on" || p_str_tok[0] == "off")
                        {
                            int torque;
                            if(p_str_tok[0] == "on")
                            {
                                torque = 1;
                                
                            }
                            else //if(p_str_tok[0] == "off")
                            {
                                torque = 0;
                            }

                            int i;
                            for(i=1; i<ARGUMENT_NAXNUM; i++)
                            {
                                if(p_str_tok[i].length() > 0)
                                {
                                    cm730.WriteByte(atoi(p_str_tok[i].c_str()), MX28::P_TORQUE_ENABLE, torque, 0);
                                }
                                else
                                    break;
                            }

                            if(i == 1)
                            {
                                for(int id=1; id<JointData::NUMBER_OF_JOINTS; id++)
                                    cm730.WriteByte(id, MX28::P_TORQUE_ENABLE, torque, 0);
                            }
                        }

                        int torque, position;
                        data_out << "{";
                        for(int id=0; id<ROBOPLUS_JOINT_MAXNUM; id++)
                        {
                            data_out << "[";
                            cout << "[";
                            if(id >= 1 && id < JointData::NUMBER_OF_JOINTS)
                            {
                                if(cm730.ReadByte(id, MX28::P_TORQUE_ENABLE, &torque, 0) == CM730::SUCCESS)
                                {
                                    if(torque == 1)
                                    {
                                        if(cm730.ReadWord(id, MX28::P_GOAL_POSITION_L, &position, 0) == CM730::SUCCESS)
                                        {
                                            data_out << position;
                                            cout << position;
                                        }
                                        else
                                        {
                                            data_out << "----";
                                            cout << "Fail to read present position ID(" << id << ")";
                                        }
                                    }
                                    else
                                    {
                                        data_out << "????";
                                        cout << "????";
                                    }
                                }
                                else
                                {
                                    data_out << "----";
                                    cout << "Fail to read torque ID(" << id << ")";
                                }
                            }
                            else
                            {
                                data_out << "----";
                                cout << "----";
                            }
                            data_out << "]";
                            cout << "]";
                        }
                        cout << endl;
                        data_out << "}";
                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "go")
                    {
                        int GoalPosition, StartPosition, Distance;
                        
                        data_out << "{";
                        for(int id=0; id<ROBOPLUS_JOINT_MAXNUM; id++)
                        {
                            if(id >= 1 && id < JointData::NUMBER_OF_JOINTS)
                            {
                                if(cm730.ReadWord(id, MX28::P_PRESENT_POSITION_L, &StartPosition, 0) == CM730::SUCCESS)
                                {
                                    GoalPosition = (int)atoi(p_str_tok[id + 1].c_str());

                                    if( StartPosition > GoalPosition )
                                        Distance = StartPosition - GoalPosition;
                                    else
                                        Distance = GoalPosition - StartPosition;

                                    Distance >>= 2;
                                    if( Distance < 8 )
                                        Distance = 8;

                                    cm730.WriteWord(id, MX28::P_MOVING_SPEED_L, Distance, 0);
                                    if(cm730.WriteWord(id, MX28::P_GOAL_POSITION_L, GoalPosition, 0) == CM730::SUCCESS)
                                    {
                                        data_out << "[" << GoalPosition << "]";
                                        cout << "[" << GoalPosition << "]";
                                    }
                                    else
                                    {
                                        data_out << "[----]";
                                        cout << "[----]";
                                    }
                                }
                                else
                                {
                                    data_out << "[----]";
                                    cout << "[----]";
                                }
                            }
                            else
                            {
                                data_out << "[----]";
                                cout << "[----]";
                            }
                        }
                        cout << endl;
                        data_out << "}";
                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "set")
                    {
                        int id = (int)atoi(p_str_tok[1].c_str());
                        int position = (int)atoi(p_str_tok[2].c_str());

                        if(cm730.WriteWord(id, MX28::P_GOAL_POSITION_L, position, 0) == CM730::SUCCESS)
                        {
                            if(cm730.ReadWord(id, MX28::P_GOAL_POSITION_L, &position, 0) == CM730::SUCCESS)
                            {
                                cout << "{[" << position << "]}";
                                data_out << "{[" << position << "]}";
                            }
                            else
                            {
                                cout << "[Fail to read goal position ID(" << id << ")]";
                                data_out << "{[----]}";
                            }
                        }
                        else
                        {
                            cout << "[Fail to write goal position ID(" << id << ")]";
                            data_out << "{[----]}";
                        }
                        cout << endl;
                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "play" || p_str_tok[0] == "rplay")
                    {                        
                        int value;

                        for(int id=0; id<ROBOPLUS_JOINT_MAXNUM; id++)
                        {
                            if(id >= JointData::ID_R_SHOULDER_PITCH && id <= JointData::ID_HEAD_TILT)
                            {
                                if(cm730.ReadWord(id, MX28::P_TORQUE_ENABLE, &value, 0) == CM730::SUCCESS)
                                {
                                    if(value == 0)
                                    {
                                        if(cm730.ReadWord(id, MX28::P_PRESENT_POSITION_L, &value, 0) == CM730::SUCCESS)
                                            MotionStatus::m_CurrentJoints.SetValue(id, value);
                                        else
                                            cout << "[Fail to communication ID(" << id << ")]" << endl;
                                    }
                                    else
                                    {
                                        if(cm730.ReadWord(id, MX28::P_GOAL_POSITION_L, &value, 0) == CM730::SUCCESS)
                                            MotionStatus::m_CurrentJoints.SetValue(id, value);
                                        else
                                            cout << "[Fail to communication ID(" << id << ")]" << endl;
                                    }
                                }
                                else
                                    cout << "[Fail to communication ID(" << id << ")]" << endl;
                            }
                        }                                                        
                        
                        motion_timer.Start();
                        MotionManager::GetInstance()->SetEnable(true);
                        
                        int index = (int)atoi(p_str_tok[1].c_str());
                        if(p_str_tok[0] == "play")
                            Action::GetInstance()->Start(index);
                        else
                            Action::GetInstance()->Start(index, &page);
                    }
                    else if(p_str_tok[0] == "info")
                    {
                        int ipage, istep;
                        if(Action::GetInstance()->IsRunning(&ipage, &istep) == 1)
                        {
                            data_out << "{[" << ipage << ":" << istep << "]}\n";
                            cout << "[" << ipage << ":" << istep << "]" << endl;
                        }
                        else
                        {
                            data_out << "{[OK]}\n";
                            cout << "[END]" << endl;
                            MotionManager::GetInstance()->SetEnable(false);
                            motion_timer.Stop();
                        }
                    }
                    else if(p_str_tok[0] == "stop")
                    {
                        Action::GetInstance()->Stop();
                    }
                    else if(p_str_tok[0] == "stopinfo")
                    {
                        Action::GetInstance()->Stop();

                        int ipage, istep;
                        if(Action::GetInstance()->IsRunning(&ipage, &istep) == 1)
                        {
                            data_out << "{[" << ipage << ":" << istep << "]}\n";
                            cout << "[" << ipage << ":" << istep << "]" << endl;
                        }
                        else
                        {
                            data_out << "{[OK]}\n";
                            cout << "[END]" << endl;
                            MotionManager::GetInstance()->SetEnable(false);
                            motion_timer.Stop();
                        }
                    }
                    else if(p_str_tok[0] == "break")
                    {
                        Action::GetInstance()->Brake();
                    }
                    else if(p_str_tok[0] == "breakinfo")
                    {
                        Action::GetInstance()->Brake();

                        int ipage, istep;
                        if(Action::GetInstance()->IsRunning(&ipage, &istep) == 1)
                        {
                            data_out << "{[" << ipage << ":" << istep << "]}\n";
                            cout << "[" << ipage << ":" << istep << "]" << endl;
                        }
                        else
                        {
                            data_out << "{[OK]}\n";
                            cout << "[END]" << endl;
                            MotionManager::GetInstance()->SetEnable(false);
                            motion_timer.Stop();
                        }
                    }
                    else if(p_str_tok[0] == "RDownload")
                    {
                        int rcv_len = (int)sizeof(Action::PAGE);
                        int i_data = 0;
                        unsigned char *data = (unsigned char*)&page;

                        data_out << "{[READY]}\n";
                        new_sock << data_out.str();
                        data_out.clear();
                        while(rcv_len > 0)
                        {
                            i_data += new_sock.recv(data, rcv_len);
                            data += i_data;
                            rcv_len -= i_data;
                        }
                        data_out << "{[SUCCESS]}\n";
                        new_sock << data_out.str();
                        data_out.clear();
                        data_out << "{[ME]}\n";
                    }                    
                    else if(p_str_tok[0] == "upload")
                    {
                        // send page data
                        int index = (int)(atoi(p_str_tok[1].c_str()) / sizeof(Action::PAGE));
                        int num = (int)(atoi(p_str_tok[2].c_str()) / sizeof(Action::PAGE));
                        
                        for(int i=0; i<num; i++)
                        {
                            Action::GetInstance()->LoadPage(index + i, &page);
                            new_sock.send((unsigned char*)&page, (int)sizeof(Action::PAGE));
                        }

                        data_out << "{[ME]}\n";
                    }
                    else if(p_str_tok[0] == "ld")
                    {
                        int index = (int)(atoi(p_str_tok[1].c_str()) / sizeof(Action::PAGE));
                        int num = (int)(atoi(p_str_tok[2].c_str()) / sizeof(Action::PAGE));
                        int rcv_len;
                        int i_data;
                        unsigned char *data;

                        cout << "[READY]" << endl;
                        data_out << "{[READY]}\n";
                        new_sock << data_out.str();
                        data_out.clear();
                        // byte stream
                        for(int i=0; i<num; i++)
                        {
                            i_data = 0;
                            rcv_len = (int)sizeof(Action::PAGE);
                            data = (unsigned char*)&page;
                            while(rcv_len > 0)
                            {
                                i_data += new_sock.recv(data, rcv_len);
                                data += i_data;
                                rcv_len -= i_data;
                            }
                            Action::GetInstance()->SavePage(index + i, &page);
                            cout << "[SAVE:" << index + i << "]" << endl;
                        }
                    }
                    else
                    {
                        cout << " [Invalid:" << p_str_tok[0] << "]" << endl;
                    }
                } // if (data_in != "")

                // send the data if any
                string str_out = data_out.str();
                if (str_out != "")
                {
                    //cout << str_out;
                    new_sock << str_out;
                }
            }
            catch ( LinuxSocketException& )
            {
                cout << "[Disconnected]" << endl;
                                connected = false;

                if(Action::GetInstance()->IsRunning() == 1)
                {
                    Action::GetInstance()->Stop();
                    while(Action::GetInstance()->IsRunning() == 1)
                        usleep(1);
                    MotionManager::GetInstance()->SetEnable(false);
                    motion_timer.Stop();
                }
            }
        }
}

