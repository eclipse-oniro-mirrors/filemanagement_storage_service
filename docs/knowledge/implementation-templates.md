# 实现必做清单

本文只记录实现新功能时的必做步骤，不包含代码模板。IPC 新增流程见 [[ipc-interface-guide]]，代码放置见 [[architecture]]。

## 所有公开接口的必做步骤

| 步骤 | 要求 | 不做的后果 |
|------|------|------------|
| 参数校验 | 所有公开接口必须有 | 调用方传空或越界导致内核 crash |
| 权限校验 | Manager 入口必须有 | 未授权调用操作密钥或挂载 |
| 状态检查 | 操作前检查当前状态 | 非法状态操作导致数据损坏 |
| 状态预更新 | 操作前先置中间状态 | 操作中途查询到不一致状态 |
| 失败回退 | 操作失败回退到之前状态 | 停留在中间状态导致死锁 |
| 成功日志 | 成功必须 LOGI | 无法确认操作是否完成 |
| 失败日志 | 失败必须 LOGE | 无法定位问题 |
| 失败上报 | 关键操作失败必须 StorageRadar 上报 | 故障统计缺失 |

## 各场景必做项

### 卷操作

- 卷挂载/卸载通过 `IVolumeOperator`（`VolumeOperatorFactory::CreateOperator(fsType)` 分发）执行；`VolumeState` 枚举仍定义于 `volume_core.h`，但当前生产路径不再由 `VolumeInfo` 状态机驱动（`VolumeCore::SetState` 无生产调用方）。
- 新增卷操作继承 `IVolumeOperator` 并在 `VolumeOperatorFactory::RegisterOperators` 注册；Mount 前仍应遵循 CHECKING、Unmount 前 EJECTING 的状态语义。

### 密钥操作

- 操作前：确认 fscrypt/加密已启用（`KeyCtrlHasFscryptSyspara` / `IsUeceSupport`），HUKS 通过 `HuksMaster::GenerateKey` 返回值判断失败（无 `IsHuksAvailable` 方法）。
- userId 校验：范围 [0, 10738]（`START_USER_ID=0`，含 ZERO_USER），超出返回 `E_USERID_RANGE`
- 失败必须 `StorageRadar::ReportUserKeyResult` 或 `ReportActiveUserKey`（按场景选用对应 `Report*`）

### 用户目录操作

- 创建失败直接返回错误码（如 `E_PREPARE_DIR`），当前 `UserManager::PrepareUserDirs` 不做已建目录递归回滚（无 `RollbackUserDirs`）；如需回滚须由调用方（`StorageDaemon::PrepareUserDirs`）保证 `GenerateUserKeys` 与目录创建的失败清理
- 密钥生成失败直接返回，不创建目录（返回 `-EEXIST` 走 `RestoreUserKey`）
- 权限设置：SELinux context 必须设置（`RestoreconRecurse`/`RestoreconDir`）

### 配额操作

- 设置前：初始化配额挂载表 `InitialiseQuotaMounts()` 并查 `/data` quota 设备（无 `IsQuotaSupported`）
- 查询失败无自动降级：`quotactl` 失败直接返回错误码，如需目录级统计显式调 `GetDirListSpace`

### IPC 层

- Stub 由 IDL 自动生成（`IStorageManager.idl`/`IStorageDaemon.idl`），无需手写 MessageParcel 解析；参数校验在 Provider 实现层完成
- Manager Provider：必须权限校验 + 状态检查 + 调用 Daemon
- Daemon Provider：必须路径校验（`ValidateBlockDevicePath`/`ValidateMountPath`）+ 调用核心类

## 新增文件系统 Operator

1. 新增 `xxx_operator.h/.cpp` 继承 `IVolumeOperator`
2. 实现 `Mount`/`Unmount`（注意是 `Unmount` 非 `UMount`）/`Format`/`Check`（及按需 `ReadMetadata`/`Repair`/`SetLabel`/`CreateIsoImage`/`Burn`）；`fsType` 由 `CreateOperator` 入参提供，**不需要** `GetFsType` 方法
3. 在 `volume_operator_factory.cpp` 的 `RegisterOperators()` 中向 operators map 注册（`CreateOperator` 经 `EnsureOperatorsInit`/`call_once` 触发一次性注册）

## 修改前检查

- 必做8步骤是否全部覆盖？
- 失败时是否上报了 StorageRadar？
- 回滚逻辑是否覆盖了半成品状态？
