#!/bin/bash

CPP_Prog="main.cpp  wind_rend.cpp  pendulum.cpp ode_solver.cpp";


g++ -o main $CPP_Prog -lsfml-graphics -lsfml-window -lsfml-system;
./main

