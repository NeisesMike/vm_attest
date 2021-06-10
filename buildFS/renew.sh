#!/bin/bash
vmdir=camkesvm`date +"%B%d"`
fsdir=fs`date +"%B%d"`

cd ${fsdir}  && 
find . -print0 | fakeroot cpio --null -H newc -o > ../rootfs.cpio && 
cd .. && 
gzip rootfs.cpio && 
cp rootfs.cpio.gz ~/Documents/projects/sel4/camkes_vm_attest/${vmdir}/projects/camkes-vm-images/exynos5422

