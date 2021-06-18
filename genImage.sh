# script to grab the latest version of the camkes-arm project,
# add the vm_attest app,


dir=camkesvm`date +"%B%d"`
mkdir ${dir};
cd ${dir} &&
echo "==================================" &&
echo "Grab the latest repo..." &&
echo "==================================" &&
repo init -u https://github.com/sel4/camkes-arm-vm-manifest &&
repo sync &&
echo "==================================" &&
echo "Image generated!" &&
echo "Copy vm_attest into the project..." &&
cp -r ../vm_attest projects/vm-examples/apps/Arm/ &&
echo "Copy vm_measure into the project..." &&
cp -r ../vm_measure projects/vm-examples/apps/Arm/ &&
mkdir build &&
cd build &&   
echo "vm_attest copied." &&
echo "Copy some build scripts..." &&
cp ../../buildScripts/* . &&
echo "Generate cakeml build script..." &&
cd ../.. &&
(rm ./vm_attest/components/CakeServer/buildCakeDocker.sh || echo "and that's no problem!") &&
cp ./vm_attest/components/CakeServer/buildCakeTemplate.sh vm_attest/components/CakeServer/buildCakeDocker.sh &&

echo "
echo 'move the cake assembly into position' &&
cp cakeServer.S ../../../${dir}/build/apps/Arm/vm_attest/components/CakeServer/cakeServer.S &&

echo 'done building cake files and moving libs!'
" >> vm_attest/components/CakeServer/buildCakeDocker.sh &&
echo "Generated!" &&
echo "=================================="
