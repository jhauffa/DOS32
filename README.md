In the era of the 486 and early Pentium CPUs, DOS was platform of choice for
game developers. To circumvent the limitation to 1 MB of RAM, so-called "DOS
extenders" acted as a translation layer between the 32-bit application code and
the 16-bit DOS system calls. Emulators like DOSBox are often too slow to
properly play these games, and virtualization solutions like QEMU or VirtualBox
do not fully emulate the required legacy hardware (VGA/SVGA, SoundBlaster 16,
etc.)

This project is an attempt to run 32-bit DOS applications natively (i.e.,
without resorting to CPU emuluation or virtualization) on modern operating
systems. This is done by intercepting system calls and direct access to hardware
via MMIO or ports. It is very much a work in progress: currently, it requires
MacOS as the host system, only runs applications written for the DOS/4G family
of extenders, and only implements a small subset of DOS system calls. In its
current state, it is only capable of running small toy examples.
