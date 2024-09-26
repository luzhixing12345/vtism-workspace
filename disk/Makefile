

m:
	mkdir tmp
	sudo mount -o loop,offset=2097152 ubuntu.raw tmp


chu:
	sudo mount -o loop,offset=2097152 ubuntu.raw tmp
	sudo mount -o bind /proc tmp/proc
	sudo mount -o bind /dev tmp/dev
	sudo mount -o bind /dev/pts tmp/dev/pts
	sudo mount -o bind /sys tmp/sys
	sudo chroot tmp

um:
	sudo umount -l tmp
	rm -r tmp

ch:
	sudo ./chroot.sh

