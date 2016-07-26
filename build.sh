#!/bin/sh
source /opt/powerpc-toolchain-multilib-freescale/toolchain-env.sh
rm -rf release
mkdir release
make
cp -v ./vim3u3_fpga_cli release
cp -v ./fmap1.xml release

