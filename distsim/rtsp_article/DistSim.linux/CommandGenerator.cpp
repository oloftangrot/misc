#include <RTSP.h>

#include <stdio.h>
#include "CommandGeneratorSend.h"

RTSP* rtsp;

CommandGeneratorSend::Update command_update;

bool ping_callback()
{
    printf("Ping callback\n");
    return true;
}

bool init_callback()
{
    printf("Initialize callback\n");
    return true;
}

bool start_callback()
{
    printf("Start callback\n");
    return true;
}

bool end_run = false;
bool halt_callback()
{
    printf("Halt callback\n");
    end_run = true;
    return true;
}

int main()
{
    printf("CommandGenerator Federate\n");
    printf("Jim Ledin    August, 2000\n\n");

    // Determine the number of frames per second
    int frames_per_sec = 100;
    printf("\nUpdate rate: %d frames/sec\n\n", frames_per_sec);

    rtsp = new RTSP("CommandGenerator", 0);
    rtsp->SetPingCallback(ping_callback);
    rtsp->SetInitializeCallback(init_callback);
    rtsp->SetStartCallback(start_callback);
    rtsp->SetHaltCallback(halt_callback);

    for (;;)
    {
        end_run = false;
        printf("Waiting for Initialize\n");
        rtsp->WaitForInit();

        printf("Waiting for Start\n");
        rtsp->WaitForStart();
        printf("Running\n");

        double step_time = 1.0 / frames_per_sec;

        rtsp->StartTimer();
        uint frame = 0;
        while (!end_run)
        {
            // Time since start of run
            double Time = frame * step_time;

            // Output a new random command every 10 seconds
            if (frame % (10*frames_per_sec) == 0)
            {
                double command = 10.0 * double(rand()) / RAND_MAX;
                command_update.command = command;
                command_update.Send(*rtsp);

                printf("Time: %.1lf; New command: %.3lf\n", Time, command);
                fflush(stdout);
            }

            uint end_of_frame = uint(1e6 * ++frame * step_time);
            uint cur_time = rtsp->ReadTimer();
            while (cur_time < end_of_frame)
            {
                rtsp->PollRcv(true, end_of_frame - cur_time);
                cur_time = rtsp->ReadTimer();
            }
        }
        printf("End of run\n");
    }

    return 0;
}
