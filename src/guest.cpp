#include "guest.h"

#include <stdexcept>
#include <iostream>
#include <string>

extern "C" {
    #include <asm/bootparam.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <linux/kvm.h>
    #include <linux/kvm_para.h>
    #include <stdarg.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/ioctl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
}

Guest::Guest(size_t ram_size_) : ram_size{ram_size_}, state{0}, user_region{} {
    kvm_fd = open("/dev/kvm", O_RDWR);
    if (kvm_fd) {
        throw std::runtime_error{"KVM is inaccessible"};
    }

    vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);
    if (vm_fd) {
        throw std::runtime_error{"Could not create a VM"};
    }

    if (ioctl(vm_fd, KVM_SET_TSS_ADDR, 0xffffd000)) {
        throw std::runtime_error{"Could not set TSS address"};
    }

    size_t map_addr = 0xffffc000;
    if (ioctl(vm_fd, KVM_SET_IDENTITY_MAP_ADDR, &map_addr)) {
        throw std::runtime_error{"Could not set identity map address"};
    }

    if (ioctl(vm_fd, KVM_CREATE_IRQCHIP, 0)) {
        throw std::runtime_error{"Could not create an interrupt controller"};
    }

    if (ram_size < KVM_MMIO_START) {
        ram_start = mmap(nullptr, ram_size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    } else {
        throw std::runtime_error{"Could not allocate 64 bit memory"};
//        ram_size = ram_size + KVM_MMIO_GAP_SIZE;
//        ram_start = mmap(nullptr, ram_size, PROT_READ | PROT_WRITE,
//                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
//        if (ram_start != MAP_FAILED) {
//            mprotect(ram_start + KVM_MMIO_START, KVM_MMIO_GAP_SIZE, PROT_NONE);
//        }
    }
    if (ram_start == MAP_FAILED) {
        throw std::runtime_error{"Could not allocate guest memory"};
    }

    madvise(ram_start, ram_size, MADV_MERGEABLE);


    user_region = {
            .slot = 0,
            .flags = 0,
            .guest_phys_addr = 0,
            .memory_size = ram_size,
            .userspace_addr = (__u64)ram_start,
    };

    if (ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &user_region)) {
        throw std::runtime_error{"Could not set user space region"};
    }


}

Guest::~Guest() {
    close(kvm_fd);
    close(vm_fd);
    munmap(ram_start, ram_size);
}


void Guest::boot(const std::string &kernel_path, const std::string &initrd_path) {
    size_t datasz;
    void *data;
    int fd = open(kernel_path.c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Could not open kernel image");
    }
    struct stat st;
    fstat(fd, &st);
    data = mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    datasz = st.st_size;
    close(fd);

    struct boot_params *boot =
            (struct boot_params *)(((uint8_t *)ram_start) + 0x10000);
    void *cmdline = (void *)(((uint8_t *)ram_start) + 0x20000);
    void *kernel = (void *)(((uint8_t *)ram_start) + 0x100000);

    memset(boot, 0, sizeof(struct boot_params));
    memmove(boot, data, sizeof(struct boot_params));
    size_t setup_sectors = boot->hdr.setup_sects;
    size_t setupsz = (setup_sectors + 1) * 512;
    boot->hdr.vid_mode = 0xFFFF; // VGA
    boot->hdr.type_of_loader = 0xFF;
    boot->hdr.ramdisk_image = 0x0;
    boot->hdr.ramdisk_size = 0x0;
    boot->hdr.loadflags |= CAN_USE_HEAP | 0x01 | KEEP_SEGMENTS;
    boot->hdr.heap_end_ptr = 0xFE00;
    boot->hdr.ext_loader_ver = 0x0;
    boot->hdr.cmd_line_ptr = 0x20000;
    memset(cmdline, 0, boot->hdr.cmdline_size);
    memcpy(cmdline, "console=ttyS0", 14);
    memmove(kernel, (char *)data + setupsz, datasz - setupsz);
}

void Guest::run() {
    int run_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    struct kvm_run *run = (kvm_run*) mmap(nullptr, run_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
    for (;;) {
        if(ioctl(vcpu_fd, KVM_RUN, 0)) {
            throw std::runtime_error("Could not run vcpu");
        }

        switch (run->exit_reason) {
            case KVM_EXIT_IO:
                if (run->io.port == 0x3f8 && run->io.direction == KVM_EXIT_IO_OUT) {
                    uint32_t size = run->io.size;
                    uint64_t offset = run->io.data_offset;
                    printf("%.*s", size * run->io.count, (char *) run + offset);
                } else if (run->io.port == 0x3f8 + 5 &&
                           run->io.direction == KVM_EXIT_IO_IN) {
                    char *value = (char *) run + run->io.data_offset;
                    *value = 0x20;
                }
                break;
            case KVM_EXIT_SHUTDOWN:
                printf("shutdown\n");
                return;
            default:
                std::cout << run->exit_reason << std::endl;
                return;
        }
    }
}
