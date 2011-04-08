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

#include <getopt.h>
#include <iostream>

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
    int r = 0;
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        r = chdir(dirname(exepath));
}

void help(char *progname)
{
    fprintf(stderr, "-----------------------------------------------------------------------\n");
    fprintf(stderr, "Usage: %s\n" \
                    " [-h | --help]........: display this help\n" \
                    " [-d | --device]......: port to open                    (/dev/ttyUSB0)\n" \
                    " [-f | --firmware]....: firmware file name      (cm730_rx28m_4096.hex)\n", progname);
    fprintf(stderr, "-----------------------------------------------------------------------\n");
    fprintf(stderr, "Example #1:\n" \
                    " To open a default port and install with firmware file \"cm730.hex\":\n" \
                    "  %s -f cm730.hex\n", progname);
    fprintf(stderr, "-----------------------------------------------------------------------\n");
    fprintf(stderr, "Example #2:\n" \
                    " To open a /dev/ttyUSB1 and install with default firmware file name:\n" \
                    "  %s -d /dev/ttyUSB1 \n", progname);
    fprintf(stderr, "-----------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
    int r = 0;

    fprintf(stderr, "\n***********************************************************************\n");
    fprintf(stderr,   "*                CM-730 & Actuator Firmware Installer                 *\n");
    fprintf(stderr,   "***********************************************************************\n\n");

    char *filename = (char*)"cm730_rx28m_4096.hex";
    char *dev = (char*)"/dev/ttyUSB0";

    /* parameter parsing */
    while(1)
    {
        int option_index = 0, c = 0;
        static struct option long_options[] = {
                {"h", no_argument, 0, 0},
                {"help", no_argument, 0, 0},
                {"d", required_argument, 0, 0},
                {"device", required_argument, 0, 0},
                {"f", required_argument, 0, 0},
                {"firmware", required_argument, 0, 0},
                {0, 0, 0, 0}
        };

        /* parsing all parameters according to the list above is sufficent */
        c = getopt_long_only(argc, argv, "", long_options, &option_index);

        /* no more options to parse */
        if(c == -1) break;

        /* unrecognized option */
        if(c == '?') {
            help(argv[0]);
            return 0;
        }

        /* dispatch the given options */
        switch(option_index) {
        /* h, help */
        case 0:
        case 1:
            help(argv[0]);
            return 0;
            break;

            /* d, device */
        case 2:
        case 3:
            dev = strdup(optarg);
            break;

            /* c, controller */
        case 4:
        case 5:
            filename = strdup(optarg);
            break;

        default:
            help(argv[0]);
            return 0;
        }
    }

    fprintf(stderr, "You can choose to: \n\n"
                    "  (1) CM-730 Firmware install from PC to CM-730\n" \
                    "  (2) Actuator Firmware install from CM-730 to actuator\n\n");
    char choice = 0;
    do{
        fprintf(stderr, "Enter your choice: ");
        std::cin >> choice;
    }while(choice != '1' && choice != '2');
    int mode = atoi(&choice);

    unsigned char binMem[MEMORY_MAXSIZE] = { 0xff, };
    long startAddr;
    long binSize = 0;

    if(mode == 1)
    {
        fprintf(stderr, "\n [ CM-730 Firmware install mode ]\n\n");
        fprintf(stderr, "Load %s... ", filename);

        if(hex2bin(filename, binMem, &startAddr, &binSize) == false)
            return 0;
        fprintf(stderr, "Success!! \nBinary size: %ld byte\n\n", binSize);
    }
    else if(mode == 2)
    {
        fprintf(stderr, "\n [ Actuator Firmware install mode ]\n\n");
    }

    // Open port
    int fd;
    struct termios oldtio, newtio;
    char TxCh;
    char RcvBuff[256] = { 0, };
    int RcvNum;

    if((fd = open(dev, O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0)
    {
        fprintf(stderr, "Fail to open port %s\n", dev);
        return 0;
    }

    fprintf(stderr, "[ESC] : Quit program \n");

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

    set_stdin();

    fprintf(stderr, "Press DARwIn-OP's Reset button to start...\n");

    while(1)
    {
        r = write(fd, "#", 1);
        usleep(20000);
        RcvNum = read(fd, RcvBuff, 256);
        if(RcvNum > 0)
        {
            RcvBuff[RcvNum] = 0;
            if(strcmp(RcvBuff, "#") == 0)
            {
                r = write(fd, "\r", 1);
                break;
            }
//            else
//                printf("%s", RcvBuff);
        }

        if(kbhit())
        {
            TxCh = _getch();
            if(TxCh == 0x1b)
                goto EXIT;
        }
    }

    if(mode == 1)
    {
        /*+++ start download block[0] (128KB) +++*/
        r = write(fd, "l", 1);
        r = write(fd, "\r", 1);

        while(1)
        {
            usleep(135000);
            RcvNum = read(fd, RcvBuff, 256);
            if(RcvNum > 0)
            {
                RcvBuff[RcvNum] = 0;
                fprintf(stderr, "%s", RcvBuff);
            }
            else
                break;
        }

        fprintf(stderr, "\nErase block[0] complete...\n");
        usleep(100000);


        unsigned char bytesum = 0x00;
        for(long n=0; n<128*1024; n++)
            bytesum += binMem[startAddr + n];

        printf("\n");
        long size = 0;
        long max_unit = 64;
        long unit;
        int res;
        while(size < 128*1024)
        {
            unit = 128*1024 - size;
            if(unit > max_unit)
                unit = max_unit;

            res = write(fd, &binMem[startAddr + size], unit);
            if(res > 0)
            {
                size += res;
                printf("\rDownloading Firmware block[0] (%ld/%d byte)", size, 128*1024);
            }
        }
        do
        {
            res = write(fd, &bytesum, 1);
        }while(res < 0);

        printf("\nDownloading Bytesum:%2X\n", bytesum);

        for(int x = 0; x < 100; x++)
        {
            usleep(10000);
            RcvNum = read(fd, RcvBuff, 256);
            if(RcvNum > 0)
            {
                RcvBuff[RcvNum] = 0;
                fprintf(stderr, "%s", RcvBuff);
            }
        }
        /*--- end download block[0] (128KB) ---*/

        usleep(10000);

        /*+++ start download block[1] (Total - 128KB) +++*/
        r = write(fd, "l 8023000\r", 10);

        while(1)
        {
            usleep(135000);
            RcvNum = read(fd, RcvBuff, 256);
            if(RcvNum > 0)
            {
                RcvBuff[RcvNum] = 0;
                fprintf(stderr, "%s", RcvBuff);
            }
            else
                break;
        }

        fprintf(stderr, "\nErase block[1] complete...\n");
        usleep(100000);


        bytesum = 0x00;
        for(long n=128*1024; n<binSize; n++)
            bytesum += binMem[startAddr + n];

        printf("\n");
        size = 0;
        max_unit = 64;
        while(size < binSize-128*1024)
        {
            unit = binSize - 128*1024 - size;
            if(unit > max_unit)
                unit = max_unit;

            res = write(fd, &binMem[startAddr + 128*1024 + size], unit);
            if(res > 0)
            {
                size += res;
                printf("\rDownloading Firmware block[1] (%ld/%ld byte)", size, binSize - 128*1024);
            }
        }
        do
        {
            res = write(fd, &bytesum, 1);
        }while(res < 0);

        printf("\nDownloading Bytesum:%2X\n", bytesum);

        for(int x = 0; x < 100; x++)
        {
            usleep(10000);
            RcvNum = read(fd, RcvBuff, 256);
            if(RcvNum > 0)
            {
                RcvBuff[RcvNum] = 0;
                fprintf(stderr, "%s", RcvBuff);
            }
        }
        /*--- end download block[1] (Total - 128KB) ---*/


        r = write(fd, "\rgo\r", 4); // Exit bootloader
        usleep(50000);
        RcvNum = read(fd, RcvBuff, 256);
        if(RcvNum > 0)
        {
            RcvBuff[RcvNum] = 0;
            printf("%s", RcvBuff);
        }
    }
    else if(mode == 2)
    {
        r = write(fd, "go 8023000", 10);
        r = write(fd, "\r", 1);

        int wait_count = 0;
        while(1)
        {
            if(kbhit())
            {
                TxCh = _getch();
                if(TxCh == 0x1b)
                    break;
                else if(TxCh == 127) // backspace
                    TxCh = 0x08; // replace backspace value

                r = write(fd, &TxCh, 1);
            }

            RcvNum = read(fd, RcvBuff, 256);
            if(RcvNum > 0)
            {
                RcvBuff[RcvNum] = 0;
                printf("%s", RcvBuff);
                wait_count = 0;
            }
            else
                wait_count++;

            if(wait_count > 400)
                break;

            usleep(20000);
        }
    }

    EXIT:
    reset_stdin();

    tcsetattr(fd, TCSANOW, &oldtio);
    printf("\nTerminated program\n\n");
    return 0;
}
