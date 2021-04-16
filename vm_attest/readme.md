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

#### Instructions to build the app are included in the parent directory's readme.md file.

## To do:

* Implement an example attestation.
* Write more convenient interfaces for shell interaction within the hypervisor.

### Notes:

It appears as though there can be at most 1 component which communicates with the linux vm. That is, a ping to any address on the subnet for which eth0 is configured will be delivered irrespective of the destination address. It is not clear to me whether another ethX connection can be established with another component.

It appears as though the ID for a virtqueue can be no greater than 100.

When communicating from within the hypervisor, it is best to null terminate \0 strings when sending them to stdout.
