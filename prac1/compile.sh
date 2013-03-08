g++ -o simulator `pkg-config --cflags playerc++` -I . robot.cpp simulator.cpp utils.cpp `pkg-config --libs playerc++`

