obj-m += find_offset.o

KDIR := /home/canal/petalinux_workspace/fx_zu3eg/build/tmp/work/zynqmp_generic-xilinx-linux/linux-xlnx/5.4+gitAUTOINC+22b71b4162-r0/linux-zynqmp_generic-standard-build

CROSS := aarch64-linux-gnu-

ccflags-y += -fno-stack-protector -mno-stack-protector-guard

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=$(CROSS) modules

clean:
	make -C $(KDIR) M=$(PWD) clean