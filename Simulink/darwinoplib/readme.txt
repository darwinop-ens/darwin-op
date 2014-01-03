This directory and its subdirectories contain Matlab code and Simulink libraries to communicate with the DarwinOP humanoid robot.

About this library:

At this time, only the "Software In Loop (SIL)" mode is supported and the robot must execute the program /darwin/Linux/project/instrumentation/instrumentation from the darwinop-ens repository located at https://github.com/darwinop-ens/darwin-op

This project is still under heavy development and might support embedded development "Hardware In Loop" (HIL) in the future.

Installation steps:

1. Open Matlab
2. Browse to darwinoplib/darwinoplib
3. Execute script darwinoplib_setup.m

You should get "DarwinOP Target Path Setup Complete."

4. Restart Matlab

You should now have a Simulink library named "DarwinOP Lib", demos are located in darwinoplib/darwinoplibdemos