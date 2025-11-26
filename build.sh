#!/bin/bash

if [ -z "$1" ]; then
echo "Usage: $0 <build-dir> [-C] [-t target]"
exit 1
fi


BUILD_DIR="$1"
shift


TARGET="core-image-minimal"
CLEAN=0


while getopts ":Ct:" opt; do
case ${opt} in
C ) CLEAN=1 ;;
t ) TARGET="$OPTARG" ;;
* )
echo "Invalid option"
exit 1
;;
esac
done


echo "Using build directory: $BUILD_DIR"
echo "Target image: $TARGET"


# Source Yocto environment (this will cd automatically)
source poky/oe-init-build-env "$BUILD_DIR"



if [ $CLEAN -eq 1 ]; then
echo "Running cleanall for $TARGET..."
bitbake -c cleanall $TARGET
fi


echo "Building $TARGET..."
bitbake $TARGET
