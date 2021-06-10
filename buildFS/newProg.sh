#!/bin/bash

# pass the path to the new prog as an argument

cp $1 ./fsJuly29/usr/bin/ &&
cd fsJuly29 && 
find . -print0 | fakeroot cpio --null -H newc -o > ../rootfs.cpio && 
cd .. && 
gzip rootfs.cpio && 
cp rootfs.cpio.gz ~/Documents/projects/sel4/april1/camkesvmJuly29/projects/camkes-vm-images/exynos5422
