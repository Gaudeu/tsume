# 3DS Controller Project (Work in Progress)

A low-level input streaming application designed to turn a Nintendo 3DS into a wireless PC controller using C++ and UDP networking.

## Current State & Features

The project is currently **under active development**. In its current state, it features:

* **3DS Client:** Reads hardware inputs from the console and transmits the raw button bitmasks over the local network via UDP packets.
* **PC Server:** A lightweight background server that listens for incoming UDP packets from the 3DS, decodes the bitmask data, and emulates a virtual Xbox Controller

## Project Structure

* `/controler`: Visual Studio solution and source files for the Windows server application.
* `/PC_server_win`: backend components for packet processing in linux.
   `/PC_server_win`: Alternative backend components for packet processing on windows.

