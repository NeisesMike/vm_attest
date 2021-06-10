vmdir=camkesvm`date +"%B%d"`
fsdir=fs`date +"%B%d"`
mkdir ${fsdir} &&
cd ${fsdir} &&
cp /home/michael/Documents/projects/sel4/camkes_vm_attest/${vmdir}/projects/camkes-vm-images/exynos5422/rootfs.cpio.gz . &&
gunzip rootfs.cpio.gz &&
fakeroot cpio -id --no-preserve-owner --preserve-modification-time < rootfs.cpio &&
rm rootfs.cpio

