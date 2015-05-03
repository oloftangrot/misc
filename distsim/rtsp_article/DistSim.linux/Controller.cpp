#include <dssl.h>
#include <RTSP.h>

#include <stdio.h>

#include "ControllerSend.h"
#include "ControllerRcv.h"

RTSP* rtsp;

ControllerSend::Update drive_update;
ControllerRcv::CommandGeneratorUpdate command_update;
ControllerRcv::PlantUpdate plant_update;

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

void command_callback()
{
    printf("New command from CommandGenerator: %.2f\n", 
        command_update.command);
}

void plant_callback()
{
}

int main()
{
    printf("Controller Federate\n");
    printf("Jim Ledin    August, 2000\n\n");
    // Determine the number of frames per second
    int frames_per_sec = 100;
    printf("\nUpdate rate: %d frames/sec\n\n", frames_per_sec);

    rtsp = new RTSP("Controller", 0);
    rtsp->SetPingCallback(ping_callback);
    rtsp->SetInitializeCallback(init_callback);
    rtsp->SetStartCallback(start_callback);
    rtsp->SetHaltCallback(halt_callback);

    command_update.SetCallback(*rtsp, command_callback);
    plant_update.SetCallback(*rtsp, plant_callback);

    // Declare a list of state variables and put the state variables in it
    StateList state_list;
    State<> state(&state_list, StateBase::RK4);

    double step_time = 1.0 / frames_per_sec;

    for (;;)
    {
        end_run = false;
        printf("Waiting for Initialize\n");
        rtsp->WaitForInit();

        // State initial value
        state.ic = 0;

        // Set the initial value of the states
        state_list.Initialize(step_time);

        command_update.command = 0;

        printf("Waiting for Start\n");
        rtsp->WaitForStart();
        printf("Running\n");

        rtsp->StartTimer();
        uint frame = 0;
        while (!end_run)
        {
            // Time since start of run
            double Time = frame * step_time;

            // Compensator transfer function
            double input = command_update.command - plant_update.output;
            state.der = 500*input - 40*state;
            double drive = state.der + 2*state;

            drive_update.drive = drive;
            drive_update.Send(*rtsp);

            if (frame % frames_per_sec == 0)
                printf("Time: %.2lf; Controller output: %.3lf\n",
                Time, drive);

            state_list.Integrate();

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
