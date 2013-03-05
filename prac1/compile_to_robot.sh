g++ -o simulator `pkg-config --cflags playerc++` -I . -D ROBOT robot.cpp simulator.cpp `pkg-config --libs playerc++`

