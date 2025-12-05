#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wiringPi.h>

#define ON 23
#define RIGHT 15
#define LEFT 14
#define PAUSE 25

void send_cmd(const char* cmd)
{
        sleep(1);
        FILE* fifo = fopen("/tmp/mplayer_fifo", "w");
        fprintf(fifo, cmd);
        fflush(fifo);
        fclose(fifo);
        return;
}

int main()
{
        // setup
        wiringPiSetupGpio();
        pinMode(ON, INPUT);
        pinMode(RIGHT, INPUT);
        pinMode(LEFT, INPUT);
        pinMode(PAUSE, INPUT);
        pullUpDnControl(PAUSE, PUD_DOWN);
        bool running = false;

        // main loop
        while(true){
                if(digitalRead(ON)==HIGH){
                        sleep(1);
                        if(!running){
                                system("mkfifo /tmp/mplayer_fifo 2>/dev/null");
                                system("mplayer -slave -input file=/tmp/mplayer_fifo ./files/* &"); /**/
                                running = true;
                        } else {
                                FILE* fifo = fopen("/tmp/mplayer_fifo", "w");
                                fprintf(fifo, "quit\n");
                                fflush(fifo);
                                fclose(fifo);
                                sleep(1);
                                system("rm /tmp/mplayer_fifo");
                                running = false;
                        }
                } else if(digitalRead(RIGHT)==HIGH && running){
                        send_cmd("pt_step 1\n");
                } else if(digitalRead(LEFT)==HIGH && running) {
                        send_cmd("pt_step -1\n");
                } else if(digitalRead(PAUSE)==HIGH && running) {
                        send_cmd("pause\n");
                }
        }

        return 0;
}
