# 3DS Controller Project (Work in Progress)

A low-level input streaming application designed to turn a Nintendo 3DS into a wireless PC controller using C++ and UDP networking.

## Current State & Features

The project is currently **under active development**. In its current state, it features:

* **3DS Client:** Reads hardware inputs from the console and transmits the raw button bitmasks over the local network via UDP packets.
* **PC Server:** A lightweight background server that listens for incoming UDP packets from the 3DS, decodes the bitmask data, and emulates a virtual Xbox Controller on Windows using the ViGEmBus API.

## Project Structure

* `/controler`: Visual Studio solution and source files for the Windows server application.
* `/memoria`: Core modules and shared memory logic.
* `/PC_server_win`: Alternative lightweight backend components for packet processing.

## Roadmap & Upcoming Features

- [ ] Complete UI overhaul for the 3DS client application using Citro2D.
- [ ] Integration of the Circle Pad and C-Stick analog input mapping.
- [ ] Cross-platform Linux support for the PC server workstation.
- [ ] Configuration UI on the PC server to customize button mappings.

## Requirements (Current Setup)

* A Nintendo 3DS with Homebrew access.
* Windows PC with the **ViGEmBus** driver installed.
* Both devices connected to the same local area network (LAN).

---
Any help is highly appreciated S2
