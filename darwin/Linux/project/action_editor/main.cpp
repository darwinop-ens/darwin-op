#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <term.h>
#include <ncurses.h>
#include <libgen.h>
#include <signal.h>
#include "cmd_process.h"


using namespace Robot;

LinuxCM730 linux_cm730("/dev/ttyUSB0");
CM730 cm730(&linux_cm730);


void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}

void sighandler(int sig)
{
	cm730.Disconnect();

	struct termios term;
	tcgetattr( STDIN_FILENO, &term );
	term.c_lflag |= ICANON | ECHO;
	tcsetattr( STDIN_FILENO, TCSANOW, &term );

	exit(0);
}

int main(int argc, char *argv[])
{
	signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGINT, &sighandler);

	int ch;
	char filename[128];

	change_current_dir();
	if(argc < 2)
		strcpy(filename, "../../../Data/motion.bin"); // Set default motion file path
	else
		strcpy(filename, argv[1]);

	/////////////// Load/Create Action File //////////////////
	if(Action::GetInstance()->LoadFile(filename) == false)
	{
		printf("Can not open %s\n", filename);
		printf("Do you want to make a new action file? (y/n) ");
		ch = _getch();
		if(ch != 'y')
		{
			printf("\n");
			return 0;
		}

		if(Action::GetInstance()->CreateFile(filename) == false)
		{
			printf("Fail to create %s\n", filename);
			return 0;
		}
	}
	////////////////////////////////////////////////////////////

	//////////////////// Framework Initialize ////////////////////////////	
	if(MotionManager::GetInstance()->Initialize(&cm730) == false)
	{
		printf("Fail to initialize Motion Manager!\n");
			return 0;
	}
	MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());	
	LinuxMotionTimer::Initialize(MotionManager::GetInstance());
	LinuxMotionTimer::Stop();
	/////////////////////////////////////////////////////////////////////

	DrawIntro(&cm730);

	while(1)
	{
		ch = _getch();
		if(ch == 0x1b)
		{
			ch = _getch();
			if(ch == 0x5b)
			{
				ch = _getch();
				if(ch == 0x41) // Up arrow key
					MoveUpCursor();
				else if(ch == 0x42) // Down arrow key
					MoveDownCursor();
				else if(ch == 0x44) // Left arrow key
					MoveLeftCursor();
				else if(ch == 0x43)
					MoveRightCursor();
			}
		}
		else if( ch == '[' )
			UpDownValue(&cm730, -1);
		else if( ch == ']' )
			UpDownValue(&cm730, 1);
		else if( ch == '{' )
			UpDownValue(&cm730, -10);
		else if( ch == '}' )
			UpDownValue(&cm730, 10);
		else if( ch == ' ' )
			ToggleTorque(&cm730);
		else if( ch >= 'A' && ch <= 'z' )
		{
			char input[128];
			char *token;
			int input_len;
			char cmd[80];
			int num_param;
			int iparam[30];

			BeginCommandMode();

			printf("%c", ch);
			input[0] = (char)ch;

			gets(&input[1]);
			fflush(stdin);
			input_len = strlen(input);
			if(input_len > 0)
			{
				token = strtok( input, " " );
				if(token != 0)
				{
					strcpy( cmd, token );
					token = strtok( 0, " " );
					num_param = 0;
					while(token != 0)
					{
						iparam[num_param++] = atoi(token);
						token = strtok( 0, " " );
					}

					if(strcmp(cmd, "exit") == 0)
					{
						if(AskSave() == false)
							break;
					}
					else if(strcmp(cmd, "re") == 0)
						DrawPage();
					else if(strcmp(cmd, "help") == 0)
						HelpCmd();
					else if(strcmp(cmd, "n") == 0)
						NextCmd();
					else if(strcmp(cmd, "b") == 0)
						BeforeCmd();						
					else if(strcmp(cmd, "time") == 0)
						TimeCmd();
					else if(strcmp(cmd, "speed") == 0)
						SpeedCmd();
					else if(strcmp(cmd, "page") == 0)
					{
						if(num_param > 0)
							PageCmd(iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "play") == 0)
					{
						PlayCmd(&cm730);
					}
					else if(strcmp(cmd, "set") == 0)
					{
						if(num_param > 0)
							SetValue(&cm730, iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "list") == 0)
						ListCmd();
					else if(strcmp(cmd, "on") == 0)
						OnOffCmd(&cm730, true, num_param, iparam);
					else if(strcmp(cmd, "off") == 0)
						OnOffCmd(&cm730, false, num_param, iparam);
					else if(strcmp(cmd, "w") == 0)
					{
						if(num_param > 0)
							WriteStepCmd(iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "d") == 0)
					{
						if(num_param > 0)
							DeleteStepCmd(iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "i") == 0)
					{
						if(num_param == 0)
							InsertStepCmd(0);
						else
							InsertStepCmd(iparam[0]);
					}
					else if(strcmp(cmd, "m") == 0)
					{
						if(num_param > 1)
							MoveStepCmd(iparam[0], iparam[1]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "copy") == 0)
					{
						if(num_param > 0)
							CopyCmd(iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "new") == 0)
						NewCmd();
					else if(strcmp(cmd, "g") == 0)
					{
						if(num_param > 0)
							GoCmd(&cm730, iparam[0]);
						else
							PrintCmd("Need parameter");
					}
					else if(strcmp(cmd, "save") == 0)
						SaveCmd();
					else if(strcmp(cmd, "name") == 0)
						NameCmd();
					else
						PrintCmd("Bad command! please input 'help'");
				}
			}

			EndCommandMode();
		}
	}

	DrawEnding();

	return 0;
}