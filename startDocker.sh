# script to start the docker session with the appropriate build environment

THIS_DIR=`pwd`

echo $THIS_DIR;

(git clone https://github.com/seL4/seL4-CAmkES-L4v-dockerfiles.git dockerStuff ||
 echo "using old git clone" ) &&
cd dockerStuff &&
make user HOST_DIR=$THIS_DIR

