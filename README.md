combined-hv/
├── hypervisor/
│   ├── airhv/              # 原始airhv代码（Intel VT-x）
│   │   ├── Source/
│   │   │   ├── main.cpp
│   │   │   ├── vmm.cpp
│   │   │   ├── vmcs.cpp
│   │   │   ├── ept.cpp
│   │   │   └── ...
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
