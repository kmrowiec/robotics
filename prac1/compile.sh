g++ -o simulator `pkg-config --cflags playerc++` -I . robot.cpp simulator.cpp `pkg-config --libs playerc++`

