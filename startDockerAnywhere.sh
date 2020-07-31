# script to start the docker session with the appropriate build environment

THIS_DIR=`pwd`

echo $THIS_DIR;

cd /home/michael/Documents/projects/sel4/camkes_vm_attest/dockerStuff &&
make user HOST_DIR=$THIS_DIR

