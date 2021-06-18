# What is this?
This is a workbench for building and testing camkes apps.
There are two apps currently: vm_attest and vm_measure.

## vm_attest
This app is meant to become the final product. It should, by the end, allow one machine to attest to another by generating evidence about itself. There are many parts. A camkes component should generate runtime evidence about the linux kernel running in the sel4 hypervisor. That evidence should be signed and given to the linux instance. From there, a userspace app should generate evidence about the userspace. The whole evidence bundle should be signed and transmitted to another machine, where it will be appraised.

## vm_measure
This app is meant to teach me how to build linux kernel modules using the sel4 build system. In time it will be integrated into vm_attest.


# How do I use this workbench?
0. clone this repo with `git clone --recurse-submodules`.
1. `genImage.sh` to grab the latest camkes-arm-vm project.
2. `getDocker.sh` if you don't already have Docker.
3. `startDocker.sh` to enter a docker instance.
4. cd into the `camkesvm[date]` directory and the `build` subdirectory.
5. `myattest.sh` to init, build, and compile the vm_attest project.

### Build Scripts
> genImage.sh
>> This first grabs the latest camkes-arm-vm project files, then copies build
>> scripts into the build directory. It also generates a build script which
>> needs to know the name of the project directory.

> mybuild.sh
>> This first grabs the vm-attest appand adds it to the camkes-arm-vm project
>> directory. Then it builds the project, trying to build the cake files
>> separately if at first it fails.

> myload.sh
>> Use this from the `build` directory to load the generated image onto an sd
>> card.

> startDocker.sh
>> Use this from the `build` directory to start a docker instance with the
>> appropriate build environment.

> getDocker.sh
>> Use this in the top-level to grab the docker program. This is useful if
>> Docker isn't behaving well for unknown reasons :shrug:

### Notes
> If you don't have the `repo` tool
>> https://source.android.com/setup/build/downloading#installing-repo

