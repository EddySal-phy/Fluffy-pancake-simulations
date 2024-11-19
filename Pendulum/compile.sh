#!/bin/bash

CPP_Prog="Pendulum_main.cpp  win_rend.cpp  system.cpp ode_sol.cpp";


g++ -o main $CPP_Prog -lsfml-graphics -lsfml-window -lsfml-system;
./main
