@echo off
g++ -std=c++20 -O2 -Wall -Wextra -o search_lab.exe main.cpp
if %errorlevel% neq 0 (echo Build FAILED & exit /b %errorlevel%)
echo Build OK -^> search_lab.exe