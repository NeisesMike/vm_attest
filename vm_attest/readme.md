# vm_attest

#### This project builds upon several apps:

* the camkes-arm-vm app called vm_cross_connector
* the camkes-arm-vm app called vm_virtio_net (see virtio_net.md)
* the cakeml attestation manager (see components/CakeServer/cakeml_am/README.md)
* the camkes-introspect-app provided by Oliver

#### This app implements a communication architecture between three CAmkES components:

* A hypervisor which runs a linux OS
* The "IntrospectManager" component, which acts as an interface to communication with the linux OS instance
* The "CakeServer" component, which implements the cakeml attestation manager

See images/arch\ image.pdf for a visual.

#### Instructions to build the app are included in the parent directory's readme.md file.

## Future Work

* ~~Implement a measurement interface, allowing the introspection of linux userspace memory.~~
* Find a way to measure kernel-only memory.
* Write a paper arguing we've achieved the assumptions set out in the Confining paper by Paul.

