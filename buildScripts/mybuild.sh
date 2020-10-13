# script to build the project with the appropriate flags

echo "====================================" &&
echo "Grabbing the latest vm_attest..." &&

if rm -rf ../projects/vm/apps/vm_intro ; then 
    true
else
    echo "The vm_intro app wasn't there."
fi &&

echo "..." &&
cp -r ../../vm_intro ../projects/vm/apps/ &&
echo "vm_attest updated." &&
echo "Trying an initial build..." &&
echo "====================================" &&


../init-build.sh -DCAMKES_VM_APP=vm_intro -DPLATFORM=qemu-arm-virt &&
#if ../init-build.sh -DCAMKES_VM_APP=vm_intro -DPLATFORM=qemu-arm-virt; then
#    true
#else
#    echo "====================================" &&
#    echo "Initial build failed." &&
#    echo "Building cake and moving libs..." &&
#    echo "====================================" &&
#    cd ../../vm_attest/components/CakeServer &&
#    ./buildCakeDocker.sh &&
#    cd - &&
#    #../init-build.sh -DCAMKES_VM_APP=vm_attest -DPLATFORM=exynos5422 -DAARCH32=1
#    ../init-build.sh -DCAMKES_VM_APP=vm_intro -DPLATFORM=qemu-arm-virt
#fi &&
echo "====================================" &&
echo "Build succeeded." &&
echo "Calling ninja..." &&
echo "====================================" &&
ninja
