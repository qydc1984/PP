// pp/hypervisor/common/hypervisor_base.h
class HypervisorBase {
public:
    virtual bool Initialize() = 0;
    virtual bool Enable() = 0;
    virtual bool Disable() = 0;
    virtual bool IsEnabled() const = 0;
    virtual ~HypervisorBase() = default;
};