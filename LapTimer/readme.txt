Lap Timed based on Arduino


This document describes construction of a Lap Timer intended for RC cars.

Basic principal
============================
The system is based on multiple Arduino controllers and an infra-red gate. The main controller is called the base unit, and it collects, stores and displays lap times. 
Other controllers are called transcienvers and those are picking up the infrared gate singal marking start/finish.
Each transciever has its unique ID and upon receiving the IR singal will send a 433 MHz based signal to the base unit.


The transciever
============================
As the 433 MHz communication between the car and base unit is one-directional, we cannot ensure that the result was received properly.
To make the system of transmission as secure as possible, the results will be sent multiple times and the base unit will match the result to 
existing ones based on a hash-tag. If the hash-tag exists, the result is ignored.

The base unit
============================
The base unit collects signals from all cars tracking the lap time, lap record as well as order in which those are received.