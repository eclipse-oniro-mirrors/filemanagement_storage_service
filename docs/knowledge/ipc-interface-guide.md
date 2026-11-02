# IPC 接口约束

本文只记录 Manager-Daemon 双进程架构的交互约束和新增接口规则。密钥时序见 `crypto-key-management.md`，卷约束见 `disk-volume-lifecycle.md`，线程死锁见 `constraints-and-traps.md`。

## 双进程职责边界

| 进程 | 职责 | 不可越界 |
|------|------|----------|
| Manager (SA 5003) | 权限校验、状态缓存、监控通知、IPC 桥接 | 不可直接操作设备节点或密钥 |
| Daemon (SA `STORAGE_MANAGER_DAEMON_ID`=5004) | 设备操作、密钥管理、文件系统操作、用户目录 | 不可直接响应应用请求 |

Manager 不操作设备，Daemon 不响应应用，职责不可交叉。Daemon 为原生进程（非 sa_main 启动），在 `main.cpp` 中自注册 SA（无 sa_profile）；SA ID 常量 `STORAGE_MANAGER_DAEMON_ID`（值 5004）来自外部 `system_ability_definition.h`。

## 启动顺序约束

Daemon 必须先启动（内核 netlink 事件需先就绪），Manager 后启动。Manager 按需连接 Daemon（懒连接），不要假设 Manager 启动时 Daemon 已就绪。

## 死锁预防

不要在持有 Manager 锁时调用 Daemon IPC。IPC 调用是同步的，持有锁发起 IPC 可能导致双进程互锁。先释放锁再发起 IPC。

## 崩溃恢复约束

Daemon 崩溃后 Manager 会锁屏降级并重建连接。不要假设崩溃前的 IPC 状态仍有效——Daemon 重启后内存状态（卷映射、密钥缓存）为空，Manager 的缓存可能过期，必须重新同步。

## 权限校验

Manager 入口必须校验权限（`CheckClientPermission`），Daemon 入口必须校验路径合法性（`ValidateBlockDevicePath`、`ValidateMountPath`）。两层校验不可绕过。

## 新增 IPC 接口规则

Stub/Proxy 由 IDL 自动生成，无需手写 MessageParcel 解析。接口在 IDL 声明，实现在 Provider：

| 步骤 | 修改位置 | 必须包含 |
|------|----------|----------|
| 1. 定义接口 | `services/storage_manager/IStorageManager.idl` / `services/storage_daemon/IStorageDaemon.idl`（`[ipccode N]` 方法声明） | 接口签名 + ipccode |
| 2. Stub/Proxy | IDL 自动生成 `*_stub.h/.cpp`、`*_proxy.h/.cpp` | 无需手写 MessageParcel 解析 |
| 3. Manager 实现 | `StorageManagerProvider`（`services/storage_manager/ipc/src/`） | 权限校验 + 调用 Daemon |
| 4. Daemon 实现 | `StorageDaemonProvider`（`services/storage_daemon/ipc/src/`）→ 核心类 | `ValidateBlockDevicePath`/`ValidateMountPath` + 状态检查 |
| 5. 测试 | `*_provider_test` + `test/fuzztest/` | Provider/Fuzz 全覆盖 |

不可只改一侧。Manager 和 Daemon 必须同时修改（paired IDL 编辑），参数校验在 Provider 实现层完成。

## UECE / EL5 激活回调

EL5 密钥激活通过回调通知上层，IPC 面向需双侧镜像：

- Manager：`RegisterUeceActivationCallback`（ipccode 72）/ `UnregisterUeceActivationCallback`（ipccode 73），转发到 Daemon（ipccode 55/56）。
- 回调接口 `IUeceActivationCallback::OnEl5Activation(resultCode, userId, needGetAllAppKey)`（`services/storage_manager/IUeceActivationCallback.idl`）。
- 由 `EL5_FILEKEY_MANAGER` 宏守卫；Daemon 侧 `KeyManager::NotifyUeceActivation` 在 EL5 激活后触发回调。新增 EL5 相关 crypto 接口时，Manager/Daemon 回调注册必须同步。

## 修改前检查

- 当前操作是否在持有锁时发起 IPC？
- 崩溃恢复后是否重新同步了状态？
- 两层权限/路径校验是否都保留？
- 新增接口是否覆盖了双侧 Stub + 测试？

## 测试指引

- Provider：`storage_manager_provider_test`、`storage_manager_provider_permission_test`、`storage_manager_provider_communication_test`、`storage_daemon_provider_test`、`storage_daemon_test`
- Stub/Fuzz：`storagemanagerstub_fuzzer`、`storagedaemonproxy_*_fuzzer`、`storagedaemonprovider_fuzzer`（IDL 生成的 stub 无独立手写 stub 单测，覆盖经 provider/fuzz）
- 崩溃恢复：需要板侧验证（kill storage_daemon 进程）
