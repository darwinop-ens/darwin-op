#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <term.h>
#include <ncurses.h>
#include <libgen.h>
#include "hex2bin.h"



int _getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

struct termios oldterm, new_term;
void set_stdin(void)
{
	tcgetattr(0,&oldterm);
	new_term = oldterm;
	new_term.c_lflag &= ~(ICANON | ECHO | ISIG); // 의미는 struct termios를 찾으면 됨.
	new_term.c_cc[VMIN] = 1;
	new_term.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_term);
}

void reset_stdin(void)
{
	tcsetattr(0, TCSANOW, &oldterm);
}

void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}


int main(int argc, char *argv[])
{
	printf("\n[F/W Installer for CM-730]\n");
	
	char filename[128];
	char ttyName[32];
	if(argc == 1)
	{
		change_current_dir();
		strcpy(ttyName, "/dev/ttyUSB0");
		strcpy(filename, "cm730_fw.hex");
	}
	else if(argc == 2)
	{
		strcpy(ttyName, "/dev/ttyUSB0");
		strcpy(filename, argv[1]);
	}
	else if(argc == 3)
	{
		strcpy(ttyName, argv[1]);
		strcpy(filename, argv[2]);
	}
	else
	{
		printf("Error parameter!\n");
		return 0;
	}

	////// Firmware file open //////////
	unsigned char binMem[MEMORY_MAXSIZE] = { 0xff, };
	long startAddr;
	long binSize = 0;
	if(hex2bin(filename, binMem, &startAddr, &binSize) == false)
	{
		printf("Fail to load %s\n", filename);
		return 0;
	}
	printf("Success to load %s \nBinary size: %dbyte\n", filename, binSize);

	// Open port
	int fd;
	struct termios oldtio, newtio;
	char TxCh;
	char RcvBuff[256] = { 0, };
	int RcvNum;
    
    if((fd = open(ttyName, O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0)
    {
		printf("Fail to open port %s\n", ttyName);
		return 0;
	}

	printf("ESC:Quit program   ~:Download hex file\n");
	
	tcgetattr(fd, &oldtio);
	memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag      = B57600|CS8|CLOCAL|CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0;
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN]   = 0;
    tcsetattr(fd, TCSANOW, &newtio);
	tcflush(fd, TCIFLUSH);

	// Enter bootloader of CM-730
	int status;
	ioctl(fd, TIOCMGET, &status);
	status |= TIOCM_DTR;
    ioctl(fd, TIOCMSET, &status); // Set DTR low
	usleep(50000); // delay about 50msec
	status &= ~TIOCM_DTR;  
    ioctl(fd, TIOCMSET, &status); // Set DTR high
	write(fd, "#####\r", 6); // Send key for entering bootloader
	
	set_stdin();
	while(1)
	{
		if(kbhit())
		{
			TxCh = _getch();
			if(TxCh == 0x1b)
				break;
			else if(TxCh == '~')
			{
				unsigned char bytesum = 0x00;
				for(long n=0; n<binSize; n++)
					bytesum += binMem[startAddr + n];

				printf("\n");
				long size = 0;
				long max_unit = 64;
				long unit;
				int res;
				while(size < binSize)
				{					
					unit = binSize - size;
					if(unit > max_unit)
						unit = max_unit;

					res = write(fd, &binMem[startAddr + size], unit);
					if(res > 0)
					{
						size += res;
						printf("\rDownloading Firmware (%d/%dbyte)", size, binSize);
					}
				}
				do
				{
					res = write(fd, &bytesum, 1);
				}while(res < 0);

				printf("\nDownloading Bytesum:%2X\n", bytesum);
				continue;
			}
			else if(TxCh == 127) // backspace
				TxCh = 0x08; // replace backspace value

			write(fd, &TxCh, 1);			
		}

		RcvNum = read(fd, RcvBuff, 256);
		if(RcvNum > 0)
		{
			RcvBuff[RcvNum] = 0;
			printf("%s", RcvBuff);
		}

		usleep(10000);
	}
	reset_stdin();

	write(fd, "\rgo\r", 4); // Exit bootloader
	tcsetattr(fd, TCSANOW, &oldtio);
	printf("\nTerminated program\n\n");
	return 0;
}