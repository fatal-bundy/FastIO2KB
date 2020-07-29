# FastIO2KB

This repository contains source code for a Windows console application, `fast2kb.exe`, which reads input via `iDmacDrv32.dll` or `iDmacDrv64.dll` from a Fast I/O Direct Memory Access Controller (DMAC) and remaps this input to DirectInput keyboard events. Any DirectInput-compatible application can receive the remapped input.

For example, on a Windows-based arcade machine with a Fast I/O control system, `fast2kb.exe` can serve input to a DirectInput-compatible emulator such as MAME or PPSSPP.

## Credits

* **Maintainer:** Joseph Howse (<josephhowse@nummist.com>)
* **Authors:** Corey Griffith, Joseph Howse

## System Requirements

* **Operating system:** Windows XP or later. Tested successfully with:
  * Windows XP Embedded SP2
  * Windows 10 Pro, 32-bit
  * Windows 10 Pro, 64-bit
* **Fast I/O DMAC board and drivers**: Tested successfully with:
  * Taito Type X<sup>2</sup> PCIe board (K91X1209A J9100636A). This device is manufactured by Oki Information Systems and is found in NESiCAxLive and Dariusburst versions of Taito Type X<sup>2</sup> arcade machines. Device Manager was used to install a driver package containing `oem1.inf`, `iDmacDrv32.sys`, and `iDmacDrv32.dll`, all pulled from a working system.
  * Taito Type X<sup>3</sup> PCIe board (K91X1217C J9100638A) + connector board (K92X0281C J9200167C). This device is manufactured by Oki Information Systems and is found in Taito Type X<sup>3</sup> arcade machines. Device Manager was used to install a driver package containing `idmacdrv64.inf`, `iDmacDrv64.sys`, and `iDmacDrv64.dll`, `iDmacDrv64.cat`, and `idmacdrv64.PNF`, all pulled from a working system.
* **Fast I/O microcontroller board**: Taito boards with the designation J9100634A (having a JAMMA edge) or J9100633A (not having a JAMMA edge) are believed to be compatible. Tested successfully with:
  * Taito K91X1204B J9100634A board. Testing included inputs on JAMMA edge, as well as Button 6–8 inputs on 14-pin JST NH header (labelled "B"). All work.
  * Taito K91X1243A J9100633A board. Testing included inputs on 60-pin JST RA header (labelled "G1"). Button 5–8 inputs do not work; others do work.

## Troubleshooting

* Depending on how your environment is configured, you might need to copy a compatible version of `iDmacDrv32.dll` or `iDmacDrv64.dll` into the same directory as `fast2kb.exe`.
* If a COM2 port is present on your system, ensure that it is either unused or connected to a JVS board. If `fast2b.exe` is able to open COM2, it assumes that COM2 is connected to a JVS board.

## Build Configurations and Keymaps

The project has different build configurations for different keymaps:

* **MAME:** This configuration works well with the default keyboard controls in MAME (an arcade machine emulator).
* **PPSSPP:** This configuration works well with the default keyboard controls in PPSSPP (a PSP emulator). Because the PSP has many inputs and just one player, the keymap combines Fast I/O's Player 1 and 2 inputs to support just one PSP player.

The following tables show the keymaps from Fast I/O inputs to keyboard inputs and from keyboard inputs to emulated controls in MAME or PPSSPP.

### MAME Keymap

| Fast I/O Input                       | Key Received by MAME | Default Mapping in MAME |
| ------------------------------------ | -------------------- | ----------------------- |
| Player 1 Start                       | 1                    | Player 1 Start          |
| Player 1 Up                          | Up                   | Player 1 Up             |
| Player 1 Down                        | Down                 | Player 1 Down           |
| Player 1 Left                        | Left                 | Player 1 Left           |
| Player 1 Right                       | Right                | Player 1 Right          |
| Player 1 Button 1                    | Left Ctrl            | Player 1 Button 1       |
| Player 1 Button 2                    | Left Alt             | Player 1 Button 2       |
| Player 1 Button 3                    | Space                | Player 1 Button 3       |
| Player 1 Button 4                    | Left Shift           | Player 1 Button 4       |
| Player 1 Button 5                    | Z                    | Player 1 Button 5       |
| Player 1 Button 6                    | X                    | Player 1 Button 6       |
| Player 1 Button 7                    | C                    | Player 1 Button 7       |
| Player 1 Button 8                    | V                    | Player 1 Button 8       |
| Player 2 Start                       | 2                    | Player 2 Start          |
| Player 2 Up                          | R                    | Player 2 Up             |
| Player 2 Down                        | F                    | Player 2 Down           |
| Player 2 Left                        | D                    | Player 2 Left           |
| Player 2 Right                       | G                    | Player 2 Right          |
| Player 2 Button 1                    | A                    | Player 2 Button 1       |
| Player 2 Button 2                    | S                    | Player 2 Button 2       |
| Player 2 Button 3                    | Q                    | Player 2 Button 3       |
| Player 2 Button 4                    | W                    | Player 2 Button 4       |
| Player 2 Button 5                    | E                    | *Unmapped*              |
| Player 2 Button 6                    | Y                    | *Unmapped*              |
| Player 2 Button 7                    | H                    | *Unmapped*              |
| Player 2 Button 8                    | U                    | *Unmapped*              |
| Player 3 Start                       | 3                    | Player 3 Start          |
| Player 3 Up                          | I                    | Player 3 Up             |
| Player 3 Down                        | K                    | Player 3 Down           |
| Player 3 Left                        | J                    | Player 3 Left           |
| Player 3 Right                       | L                    | Player 3 Right          |
| Player 3 Button 1                    | Right Ctrl           | Player 3 Button 1       |
| Player 3 Button 2                    | Right Shift          | Player 3 Button 2       |
| Player 3 Button 3                    | Enter                | Player 3 Button 3       |
| Player 3 Button 4                    | Right Alt            | *Unmapped*              |
| Player 4 Start                       | 4                    | Player 4 Start          |
| Player 4 Up                          | Numpad 8             | Player 4 Up             |
| Player 4 Down                        | Numpad 2             | Player 4 Down           |
| Player 4 Left                        | Numpad 4             | Player 4 Left           |
| Player 4 Right                       | Numpad 6             | Player 4 Right          |
| Player 4 Button 1                    | Numpad 0             | Player 4 Button 1       |
| Player 4 Button 2                    | Numpad .             | Player 4 Button 2       |
| Player 4 Button 3                    | Numpad Enter         | Player 4 Button 3       |
| Player 4 Button 4                    | Numpad +             | *Unmapped*              |
| Coin 1                               | 5                    | Coin 1                  |
| Coin 2                               | 6                    | Coin 2                  |
| Coin 3                               | 7                    | Coin 3                  |
| Coin 4                               | 8                    | Coin 4                  |
| Test 1                               | F2                   | Test                    |
| Test 2                               | F2                   | Test                    |
| Tilt 1                               | T                    | Tilt                    |
| Tilt 2                               | T                    | Tilt                    |
| Service 1                            | 9                    | Service 1               |
| Service 2                            | 0                    | Service 2               |
| Service 3                            | -                    | Service 3, Volume Down  |
| Service 4                            | =                    | Service 4, Volume Up    |
| Player 1 Start + Button 1 + Button 3 | Esc                  | Exit                    |

### PPSSPP Keymap

| Fast I/O Input                       | Key Received By PPSSPP | Default Mapping in PPSSPP |
| ------------------------------------ | ---------------------- | ------------------------- |
| Player 1 Start                       | Space                  | Start                     |
| Player 1 Up                          | Up                     | D-pad Up                  |
| Player 1 Down                        | Down                   | D-pad Down                |
| Player 1 Left                        | Left                   | D-pad Left                |
| Player 1 Right                       | Right                  | D-pad Right               |
| Player 1 Button 1                    | Q                      | Left Shoulder             |
| Player 1 Button 2                    | W                      | Right Shoulder            |
| Player 1 Button 3                    | S                      | Triangle                  |
| Player 1 Button 4                    | X                      | Circle                    |
| Player 1 Button 5                    | A                      | Square                    |
| Player 1 Button 6                    | Z                      | X                         |
| Player 1 Button 7                    | D                      | *Unmapped*                |
| Player 1 Button 8                    | C                      | *Unmapped*                |
| Player 2 Start                       | V                      | Select                    |
| Player 2 Up                          | I                      | Analog Up                 |
| Player 2 Down                        | K                      | Analog Down               |
| Player 2 Left                        | J                      | Analog Left               |
| Player 2 Right                       | L                      | Analog Right              |
| Player 2 Button 1                    | Escape                 | Pause                     |
| Player 2 Button 2                    | 1                      | *Unmapped*                |
| Player 2 Button 3                    | 2                      | *Unmapped*                |
| Player 2 Button 4                    | 3                      | *Unmapped*                |
| Player 2 Button 5                    | 4                      | *Unmapped*                |
| Player 2 Button 6                    | 5                      | *Unmapped*                |
| Player 2 Button 7                    | [                      | *Unmapped*                |
| Player 2 Button 8                    | ]                      | *Unmapped*                |
| Coin 1                               | \`                     | Speed Toggle              |
| Coin 2                               | 6                      | *Unmapped*                |
| Test 1                               | 7                      | *Unmapped*                |
| Tilt 1                               | \                      | *Unmapped*                |
| Service 1                            | 8                      | *Unmapped*                |
| Service 2                            | 9                      | *Unmapped*                |
| Player 1 Start + Button 1 + Button 3 | 0                      | *Unmapped*                |

## To-do

* Test compatibility with other Fast I/O microcontroller boards, including:
  * Taito K91X1202A J9100633A board
  * Taito K91X1252A J9100633A board. Reportedly, Button 5–8 inputs work on this board (unlike the Taito K91X1243A J9100633A board).
* Consider adding fallback to JVS in case where Fast I/O is unavailable

## Changelog

### 2020-07-27

* Added 64-bit project configurations that use 64-bit drivers

### 2020-07-12

* Added support for submillisecond timings:
  * MAME configuration: Try polling at submillisecond interval (nominally, 0ms) with 0.5ms precision
  * PPSSPP configuration: Try polling at 1ms interval with 0.5ms precision. (PPSSPP's menu screens cannot handle input at submillisecond interval.)

### 2020-07-11

* Fixed undesirable power-cycling behaviour on Taito Type X<sup>2</sup> and similar systems, where a JVS board is connected to COM2. The fix entails periodically sending a JVS Reset message to COM2, to prevent timeout of the JVS board's watchdog timer. If `fastio2kb.exe` cannot open COM2 (e.g. if the system has no COM2 port or if another process already has an open connection to COM2), then `fastio2kb.exe` proceeds without COM2 communications. On the other hand, if `fastio2kb.exe` can open COM2 but COM2 is connected to some device other than a JVS board, then the connected device could malfunction due to incompatible communications.
* Clarified log messages about Fast I/O ports

### 2020-06-21

* Refactored code
* Added support for the following inputs:
  * Player 3 Start, Up, Down, Left, Right, Buttons 1–8
  * Player 4 Start, Up, Down, Left, Right, Buttons 1–8
  * Coins 3&4, Test 2, Tilt 2, Service 3&4
  * Escape 2 sequence: Player 2 Start + Button 1 + Button 3
  * Escape 3 sequence: Player 3 Start + Button 1 + Button 3
  * Escape 4 sequence: Player 4 Start + Button 1 + Button 3
* Enhanced MAME keymap to support Players 3&4
* Added early return in case where no I/O connection is detected

### 2020-06-16

* Configured project to better ensure support for Windows XP SP2
* Added request for precise timer resolution via Windows Multimedia API

### 2020-06-10

* Refactored code
* Added support for connecting Player 1&2 microcontroller board to either Port 1 or 2 of DMAC board
* Added support for extended keyboard scan codes
* Fixed bugs in mapping of Player 2 Up, Down, Left, Right
* Fixed type correctness issues

### 2020-06-08

* Added license based on zlib License, with permission from Corey Griffith ([fatal-bundy](https://github.com/fatal-bundy))

### 2020-06-07

* Forked from [fatal-bundy/KB2FastIO](https://github.com/fatal-bundy/FastIO2KB)
* Removed unused files
* Configured project to support Windows XP or later
* Configured project to use Warning Level 4 (instead of 3) for greater strictness at compile time
* Replaced keymap by adding separate build configurations for:
  * MAME keymap
  * PPSSPP keymap
* Refactored code
* Added control handler function for cleaner termination
* Shortened polling interval to 1ms (instead of 10ms)
* Fixed bugs that caused keypress (keydown) events to be reported multiple times
* Added support for the following inputs:
  * Player 1 Buttons 7&8
  * Player 2 Start, Up, Down, Left, Right, Buttons 1–8
  * Coins 1&2, Tilt, Service 2
  * Escape sequence: Player 1 Start + Button 1 + Button 3

