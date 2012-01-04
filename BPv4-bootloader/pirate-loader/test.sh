#!/bin/bash

OS=`uname`

echo -n "Building for $OS..."

case $OS in

 Darwin )
	./pirate-loader_mac --dev=/dev/ttyACM0 --hello
	;;
 Linux )
	./pirate-loader_lnx --dev=/dev/ttyACM0 --hello
	;;
 FreeBSD )
	./pirate-loader_fbsd --dev=/dev/ttyACM0 --hello
 	;;
 *)
	echo "ERROR"
	echo "Unsupported operating system $OS"
	exit -1
	;;
esac

echo "DONE"
exit 0
