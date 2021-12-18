#include "vcpu.h"

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

Vcpu::Vcpu(size_t id_, int vm_fd_, int kvm_fd_) : id{id_} {


    struct kvm_regs regs;
    struct kvm_sregs sregs;
    if (ioctl(vcpu_fd, KVM_GET_SREGS, &(sregs))) {
        throw std::runtime_error{"Could not retrieve special registers"};
    }

    sregs.cs.base = 0;
    sregs.cs.limit = ~0;
    sregs.cs.g = 1;

    sregs.ds.base = 0;
    sregs.ds.limit = ~0;
    sregs.ds.g = 1;

    sregs.fs.base = 0;
    sregs.fs.limit = ~0;
    sregs.fs.g = 1;

    sregs.gs.base = 0;
    sregs.gs.limit = ~0;
    sregs.gs.g = 1;

    sregs.es.base = 0;
    sregs.es.limit = ~0;
    sregs.es.g = 1;

    sregs.ss.base = 0;
    sregs.ss.limit = ~0;
    sregs.ss.g = 1;

    sregs.cs.db = 1;
    sregs.ss.db = 1;
    sregs.cr0 |= 1; /* enable protected mode */

    if (ioctl(vcpu_fd, KVM_SET_SREGS, &sregs) < 0) {
        throw std::runtime_error{"Could not set special registers"};
    }

    if (ioctl(vcpu_fd, KVM_GET_REGS, &(regs)) < 0) {
        throw std::runtime_error{"Could not retrieve registers"};
    }

    regs.rflags = 2;
    regs.rip = 0x100000;
    regs.rsi = 0x10000;

    if (ioctl(vcpu_fd, KVM_SET_REGS, &(regs)) < 0) {
        throw std::runtime_error{"Could not set special registers"};
    }

    struct {
        uint32_t nent;
        uint32_t padding;
        struct kvm_cpuid_entry2 entries[100];
    } kvm_cpuid;
    kvm_cpuid.nent = sizeof(kvm_cpuid.entries) / sizeof(kvm_cpuid.entries[0]);
    ioctl(kvm_fd, KVM_GET_SUPPORTED_CPUID, &kvm_cpuid);

    for (unsigned int i = 0; i < kvm_cpuid.nent; i++) {
        struct kvm_cpuid_entry2 *entry = &kvm_cpuid.entries[i];
        if (entry->function == KVM_CPUID_SIGNATURE) {
            entry->eax = KVM_CPUID_FEATURES;
            entry->ebx = 0x4b4d564b; // KVMK
            entry->ecx = 0x564b4d56; // VMKV
            entry->edx = 0x4d;       // M
        }
    }
    ioctl(vcpu_fd, KVM_SET_CPUID2, &kvm_cpuid);
}

Vcpu::~Vcpu() {
    close(vcpu_fd);
}
