#ifndef KBM_VMM_VCPU_H
#define KBM_VMM_VCPU_H

#include <cstdlib>
#include <thread>

class Vcpu {
    size_t id;
    int kvm_fd;
    int vm_fd;
    int vcpu_fd;
    std::thread vcpu_thrd;

public:
    Vcpu(size_t id_, int kvm_fd_, int vm_fd_);
    ~Vcpu();
    allocate();
    run();
};


#endif //KBM_VMM_VCPU_H
