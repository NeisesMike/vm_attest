# vm_measure

This app is meant to implement a simple linux kernel module for the linux instance in the sel4 hypervisor.

This page serves as the starting point:
https://docs.sel4.systems/Tutorials/camkes-vm-linux.html

The current trouble is that the linux kernel, which is checked out and built as part of the sel4 build process, is not recognized as a compatible linux kernel at run time. My best guess is that it isn't cross compiling correctly.
