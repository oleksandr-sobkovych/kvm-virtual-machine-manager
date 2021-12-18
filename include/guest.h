#ifndef KBM_VMM_GUEST_H
#define KBM_VMM_GUEST_H

#include <vector>

extern "C" {
#include <linux/kvm.h>
}

#include "vcpu.h"

class Guest {
    static constexpr size_t KVM_32BIT_MAX_MEM_SIZE = 1ULL << 32;
    static constexpr size_t KVM_MMIO_GAP_SIZE = 768 << 20;
    static constexpr size_t KVM_MMIO_START = KVM_32BIT_MAX_MEM_SIZE - KVM_MMIO_GAP_SIZE;

    int kvm_fd;
    int vm_fd;
    std::vector<Vcpu> vcpu_vctr;
    size_t ram_size;
    void* ram_start;
    int state;

    struct kvm_userspace_memory_region user_region;
public:
    explicit Guest(size_t ram_size_ = 1 << 30);
    ~Guest();
    void boot(const std::string& kernel_path, const std::string& initrd_path);
    void run();
};


#endif //KBM_VMM_GUEST_H
