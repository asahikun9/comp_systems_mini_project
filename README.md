# comp_systems_mini_project
This is my mini project for computer systems core unit in Semester 1 2024.

Objectives:
To simulate wafer microfabrication process using NIOS® II microprocessor
To minimise latency of the microfabrication process via pipelining using MicroC/OS-II RTOS
To store some information about wafer status from NIOS II to Firebase via UART
To stream the microfabrication process in real time using ESP32-CAM

The 10 stages of wafer microfabrication processs:
1    Cleaning: digit segment circling on HEX0 for 10 s
2    Drying and pre-baking: LED0 turned on for 13 s
3    Photoresist coating: stepper motor spinning for 8 s
4    Pre-exposure baking: LED1-LED4 turned on sequentially for 5 s 
5    Lithography exposure: HEX1 showing figure 8 for 5 s
6    Post-exposure baking: LED5-LED9 turned on sequentially for 5 s 
7    Development: digit segment circling on HEX2 for 3 s
8    Etching: digit segment circling on HEX3 for 3 s
9    Photoresist cleaning and finalising: servo motor turned on for 12 s
10   Idle: ‘IDLE’ shown on HEX0-HEX3 when done
