Example distributed simulation using the Real Time Simulation Protocol
Jim Ledin                                               November, 2000

This directory contains an example RTSP distributed simulation that consists of three
federates plus a separate federation controller application.

To build the simulations using Microsoft Visual C++ 6.0, the easiest way is to place the
 DistSim, DSSL, and RTSP directories below the directory: C:\Projects\rtsp_article

Next, open DistSim.dsw in Microsoft Visual C++. In the "Workspace" pane, right click on 
"DistSim Files" and select "Set as active project" on the popup menu. Then press F7 to build 
all three executables.

Run the three executables plus the federation controller by executing the run.bat command file.