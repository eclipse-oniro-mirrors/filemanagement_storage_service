# 代码放置与单例陷阱

本文只记录新增代码的放置位置规则和单例获取方式的易错点。IPC 新增流程见 [[ipc-interface-guide]]。

## 新增代码放置位置

| 新功能类型 | 放置位置 | 必须修改的文件 |
|------------|----------|----------------|
| 新增 IPC 接口 | Manager + Daemon 双侧 | `IStorageManager.idl`/`IStorageDaemon.idl`（IDL 生成 Stub/Proxy） + Provider 实现 + 核心类 |
| 修改挂载逻辑 | Daemon 侧 | `MountManager`（挂载协调）或 `IVolumeOperator` 子类（具体 FS 操作） |
| 新增密钥操作 | Daemon KeyManager | `key_manager.h/.cpp`，可选新增 BaseKey 子类 |
| 新增用户操作 | Daemon + Manager | Daemon `UserManager` + Manager Provider + Stub |
| 新增空间统计 | Manager 侧 | `StorageStatusManager` 或 `StorageTotalStatusService` |
| 新增配额功能 | Daemon 侧 | `QuotaManager` |
| 新增文件系统支持 | Daemon disk_manager/volume | 新增 Operator `.h/.cpp` + `volume_operator_factory.cpp` 注册 + **`DiskMgrApi` 表**（见下「磁盘能力按需加载」） |
| 新增磁盘事件处理 | Daemon 侧 | `DiskManager` + `NetlinkHandler` |
| 新增监控阈值 | Manager 侧 | `StorageMonitorService` |
| 新增错误码 | 共享定义 | `storage_service_errno.h` |

不可只改一侧。Manager 和 Daemon 必须同时修改。

## 磁盘能力按需加载（常驻内存优化后新增）

`enable_disk_manager` 下 13 个磁盘文件（disk_common_utils/disk_utils/disk_mgr_api/smart_reader/volume_utils/scan_device/dm_device/各 fs operator 等）**不再编入 storage_daemon 可执行文件**，打包为独立 `libstorage_disk_mgr_ext.z.so`（BUILD.gn target `storage_disk_mgr_ext`，bundle.json service_group 注册安装），首次磁盘操作时由 `DiskManagerLibLoader` 单例 dlopen(RTLD_LAZY) 按需加载。

| 部件 | 位置 | 规则 |
|------|------|------|
| API 表 | `include/disk_manager/disk_mgr_api.h`（33 函数指针） | SO 唯一导出入口 `extern "C" DiskMgrGetApi()`（map 文件仅导出该符号） |
| 加载器 | `disk_manager/src/disk_manager_lib_loader.cpp`（`GetInstance()` 单例，NoCopyable） | mutex 串行 + 失败 1s 冷却自愈重试；加载失败统一返回 `E_NOT_SUPPORT`，进程不崩溃 |
| 表初始化 | `disk_manager/src/disk/disk_mgr_api.cpp`（静态表） | 33 项与头文件字段顺序**逐位一致**，错位即静默调用错函数 |
| 通用磁盘工具 | `disk_manager/src/disk/disk_common_utils.cpp/.h` | 原 `utils/disk_utils.cpp/.h` 改名迁移（R099 纯搬迁），勿在 utils/ 下新建 |
| IPC 调用 | `storage_daemon_provider.cpp` 经 Loader 专有方法分发（33 处，每方法一处）+ 4 处查询接口 `EnsureLoaded()` 门控 | 可执行文件保留 `disk_manager_innerkits` 条件依赖（netlink_handler 使用 DiskManagerClient） |

**新增磁盘函数必改 4 处**：`disk_mgr_api.h` 表字段 → `disk_mgr_api.cpp` 表初始化 → `disk_manager_lib_loader.h/.cpp` 封装方法 → provider 调用点。少一处即编译失败或漏分发。

未启用部件的产品用空 group 占位，不产出 SO；`EXTERNAL_STORAGE_QOS_TRANS` define 由 SO 目标条件携带（见 external-dependencies.md）。

## 单例获取易错点

| 类名 | 获取方式 | 易错点 |
|------|----------|--------|
| KeyManager | `GetInstance()` | 不要用 `Instance()` |
| UserManager | `GetInstance()` | 不要用 `Instance()` |
| QuotaManager | `GetInstance()` | 不要用 `Instance()` |
| HuksMaster | `GetInstance()` | 不要用 `Instance()` |
| StorageRadar | `GetInstance()` | 不要用 `Instance()` |
| DiskManager | **`Instance()`** | **不是 GetInstance**，最容易出错 |
| MountManager | `GetInstance()` | 挂载协调单例（取代旧的 `VolumeManager`，后者已不存在） |
| DiskManagerLibLoader | `GetInstance()` | 磁盘能力 dlopen 加载单例（磁盘函数统一经它分发，不要直接调 DiskUtils/operator） |
| NetlinkManager | **`Instance()`** | **不是 GetInstance** |
| StorageDaemonCommunication | `GetInstance()` | 普通 static-local 单例（函数内 static），**不是** `DelayedSingleton` |
| BundleMgrConnector | `GetInstance()` | 类名是 **BundleMgrConnector** 不是 BundleManagerConnector |

使用单例前必须确认对应类的获取方式，不要默认全部用 `GetInstance()`。

## 头文件路径（新增代码时查找定义）

- Daemon IPC 入口：`services/storage_daemon/include/ipc/storage_daemon_provider.h`
- Manager IPC 入口：`services/storage_manager/include/ipc/storage_manager_provider.h`
- Manager Stub（IDL 生成）：由 `services/storage_manager/IStorageManager.idl` 生成，源码目录下无 stub 头文件（`storage_manager_provider.h` include 生成的 `storage_manager_stub.h`）
- Daemon Stub（IDL 生成）：由 `services/storage_daemon/IStorageDaemon.idl` 生成
- Daemon 通信代理：`services/storage_manager/include/storage_daemon_communication/storage_daemon_communication.h`
- 共享常量：`services/common/include/storage_service_constant.h`
- 错误码：`interfaces/innerkits/storage_manager/native/storage_service_errno.h`

## 修改前检查

- 新代码放在了正确的侧（Manager/Daemon）？
- 对应双侧是否同时修改？
- 单例获取方式是否用了正确的方法名？
