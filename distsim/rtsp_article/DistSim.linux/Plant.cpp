#include <dssl.h>
#include <RTSP.h>

#include <stdio.h>

#include "PlantSend.h"
#include "PlantRcv.h"

RTSP* rtsp;

PlantSend::Update plant_update;
PlantRcv::ControllerUpdate drive_update;

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

void drive_callback()
{
}

int main()
{
    printf("Plant Federate\n");
    printf("Jim Ledin    August, 2000\n\n");
    // Determine the number of frames per second
    int frames_per_sec = 100;
    printf("\nUpdate rate: %d frames/sec\n\n", frames_per_sec);

    rtsp = new RTSP("Plant", 0);
    rtsp->SetPingCallback(ping_callback);
    rtsp->SetInitializeCallback(init_callback);
    rtsp->SetStartCallback(start_callback);
    rtsp->SetHaltCallback(halt_callback);

    drive_update.SetCallback(*rtsp, drive_callback);

    // Declare a list of state variables and put the state variables in it
    StateList state_list;
    State<> pos(&state_list);
    State<> vel(&state_list);

    double step_time = 1.0 / frames_per_sec;

    for (;;)
    {
        end_run = false;
        printf("Waiting for Initialize\n");
        rtsp->WaitForInit();

        // State initial values
        pos.ic = 0;
        vel.ic = 0;

        // Set the initial value of the states
        state_list.Initialize(step_time);

        drive_update.drive = 0;

        printf("Waiting for Start\n");
        rtsp->WaitForStart();
        printf("Running\n");

        rtsp->StartTimer();
        uint frame = 0;
        while (!end_run)
        {
            // Time since start of run
            double Time = frame * step_time;

            // Use the most recent value of the controller drive signal
            vel.der = drive_update.drive;
            pos.der = vel;

            plant_update.output = pos;
            plant_update.Send(*rtsp);

            state_list.Integrate();

            if (frame % frames_per_sec == 0)
                printf("Time: %.2lf; Plant output: %.3lf\n", Time, double(pos));

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
