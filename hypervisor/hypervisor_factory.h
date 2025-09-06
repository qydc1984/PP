// pp/hypervisor/hypervisor_factory.h
class HypervisorFactory {
public:
    static std::unique_ptr<HypervisorBase> CreateHypervisor() {
        CPUVendor vendor = DetectCPUVendor();
        switch (vendor) {
        case CPUVendor::Intel:
            return std::make_unique<IntelHypervisor>();
        case CPUVendor::AMD:
            return std::make_unique<AMDHypervisor>();
        default:
            return nullptr;
        }
    }
};