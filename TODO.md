combined-hv/
├── hypervisor/
│   ├── airhv/              # 原始airhv代码（Intel VT-x）
│   │   ├── Source/
│   │   └── Include/
│   ├── memhv-driver/       # memhv驱动部分（AMD-V概念）
│   │   ├── svm/
│   │   ├── vmcb/
│   │   ├── npt/
│   │   └── ...
│   ├── common/             # 共享组件
│   │   ├── cpu_detection.h
│   │   ├── unified_interface.h
│   │   └── communication.h
│   └── CombinedHv.vcxproj
├── client/
│   ├── memhv-client/       # 原始memhv客户端
│   │   ├── Library/
│   │   │   └── memhv.h
│   │   └── Examples/
│   └── unified_client.h    # 统一客户端接口
└── build/

hypervisor/airhv/Source/
├── main.cpp              # 驱动入口点和主要逻辑
├── vmm.cpp               # 虚拟机监控器核心实现
├── vmcs.cpp              # VMCS（虚拟机控制结构）操作
├── ept.cpp               # EPT（扩展页表）管理
├── vmexit.cpp            # VM退出处理
├── invept.cpp            # INVEPT指令操作
├── msr.cpp               # MSR（模型特定寄存器）操作
├── idt.cpp               # IDT（中断描述符表）管理
├── hook.cpp              # 函数挂钩机制
├── pool.cpp              # 内存池管理
├── utils.cpp             # 工具函数集合
├── ia32.cpp              # IA-32架构相关定义和操作
├── arch.cpp              # 架构相关操作
├── communication.cpp     # 用户态通信接口
├── unified_wrapper.cpp   # 统一接口包装器
└── log.cpp               # 日志系统

hypervisor/airhv/Include/
├── main.h                # 主要头文件
├── vmm.h                 # VMM相关定义
├── vmcs.h                # VMCS操作定义（与ia32/vmcs.h不同）
├── ept.h                 # EPT相关定义
├── vmexit.h              # VM退出处理定义
├── invept.h              # INVEPT相关定义
├── msr.h                 # MSR操作定义（与ia32/msr.h不同）
├── idt.h                 # IDT相关定义
├── hook.h                # 函数挂钩定义
├── pool.h                # 内存池定义
├── utils.h               # 工具函数声明
├── arch.hpp              # 架构相关定义
├── communication.h        # 通信接口定义
├── common.h              # 通用定义
├── log.h                 # 日志系统定义
└── ia32/                 # IA-32架构定义子目录
hypervisor/airhv/Include/ia32/
├── cr.h                  # 控制寄存器定义
├── dr.h                  # 调试寄存器定义
├── msr.h                 # 模型特定寄存器定义
├── segment.h             # 段寄存器定义
├── cpuid.h               # CPUID相关定义
├── vmcs.h                # VMCS字段定义
├── exit_reason.h         # VM退出原因定义
├── exit_qualification.h  # VM退出限定符定义
├── instruction_info.h    # 指令信息定义
├── io_info.h             # IO信息定义
├── msr_info.h            # MSR信息定义
├── paging.h              # 分页相关定义
├── mtrr.h                # MTRR相关定义
└── performance.h         # 性能监控定义