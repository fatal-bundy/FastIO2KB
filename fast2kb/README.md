# FastIO2KB

This repository contains source code for a Windows console application, `fast2kb.exe`, which reads input via `iDmacDrv32.dll` or `iDmacDrv64.dll` from a Fast I/O Direct Memory Access Controller (DMAC) and remaps this input to DirectInput keyboard events. Any DirectInput-compatible application can receive the remapped input.

Orginally designed as a testing tool. this Repo as morphed in to a more user friendly tool to assist in controlling games and applications with a FastIO. EG a front end or MAME

an upstream fork by Joseph Howse (<josephhowse@nummist.com>) can be found at https://github.com/JoeHowse/FastIO2KB

I have since reverted to a prefork state and added config based button mapping.
At this stage further progression for this new fork will be maintained by Myself.