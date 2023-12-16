# tigcc-drmario

"DrMario" game I wrote in C, for TI92+ calculators, back in 2002.

![](./drmario.gif)

## Install and build

- Download and run [TIGCC](http://tigcc.ticalc.org/download.html)
- Clone the repository
- Open `drmario.tpr`
- Build the binary (--> `drmario.9xz` file)

## Useful tips for TI92+

### How to upload the game onto a TI92+ Calculator

Note: I failed to upload anything using TI Connect on MacOS, despite following [this guide](https://tout82.forumactif.org/t292-tuto-connectez-votre-ti-a-votre-mac)...

From a PC:
1. Make sure that the calculator is ON, and currently idle on the Home screen.
2. Connect the calculator to the computer, using Graphlink cable.
3. From TI Connect or TI Graphlink software, copy the `drmario.9xz` file into the calculator.
2. On the calculator, press the `2nd` and `varlink` keys to find `drmario.9xz`.
3. Start it by pressing enter on it.

## Troubleshooting

- Helpful tips in case of connection issues: [TI-89 USB Connection Issues](https://www.cemetech.net/projects/uti/viewtopic.php?t=7446&start=0)
- [Firmware update process](https://education.ti.com/en/customer-support/knowledge-base/other-graphing/troubleshooting-messages-unexpected-results/12047)
- A cool GUI to run games, including from the archive/Flash memory: [Doors](http://doors.ticalc.org/DoorsOSdoc.htm)

### What to do in case of crash

1. First, try pressing standard keys to try to quit the program: ESC, CLEAR, Q, APPS...
2. If nothing happens, press the following keys together: ON, Hand and ESC, during 5 seconds. 
3. To reset the calculator, press the following keys together: ON, 2nd and Hand, several times, for ~10 seconds. Warning: all files that were not archived in the Flash memory will be deleted!
4. Still no luck? Remove one battery for ~30 seconds, then try to turn the calculator on.

After a reset, if you use Doors, you may need to:
1. Press VAR-LINK
2. Run KERNEL (to install Doors' shell/GUI)
3. Copy its contents to the MAIN directory
