# 用户目录生命周期约束

本文只记录用户存储操作的时序约束、隔离规则和回滚要求。密钥激活时序见 `crypto-key-management.md`，卷状态约束见 `disk-volume-lifecycle.md`，线程死锁见 `constraints-and-traps.md`。

## 时序约束

用户存储操作必须严格按以下顺序执行，不可跳步或乱序：

| 步骤 | 触发 | 不可跳过的原因 |
|------|------|----------------|
| PrepareAddUser | OsAccount READY（Account SA 直接 IPC） | 创建目录并生成 EL1–EL5 全部密钥（flags 强制补齐 EL3/EL4/EL5），后续步骤依赖目录/密钥存在 |
| CompleteAddUser | OsAccount UNLOCKED（Account SA 直接 IPC） | 清除 `need_restore` 迁移标记（不激活密钥） |
| ActiveUserKey | 用户解锁（Account SA 直接 IPC） | 激活 EL2→EL3→EL4→EL5（`ActiveUserKey4Single`）；跳过则这些等级加密数据不可访问 |
| PrepareStartUser | OsAccount（启动前，Account SA 直接 IPC） | 准备启动存储（优先级提升 + 转发 Daemon `PrepareStartUser`→`StartUser`）；**必须在 `ActiveUserKey` 之后**，否则加密目录密钥未激活、不可访问 |
| StartUser（Daemon 内部） | PrepareStartUser 触发 | 创建 EL2–EL5 service 目录 + `MountManager::MountByUser`；**不激活密钥**（EL1 在 PrepareAddUser 已生成，EL2–EL5 已由 `ActiveUserKey` 激活，此处仅建目录+挂载） |
| StopUser | OsAccount 停止 | `MountManager::UmountByUser` 卸载存储 |
| RemoveUser | OsAccount 移除 | `DestroyUserDirs` 销毁目录 + `DeleteUserKeys` 删除 EL1–EL5 密钥 |

`PrepareAddUser`/`CompleteAddUser`/`ActiveUserKey`/`PrepareStartUser`/`StopUser`/`RemoveUser` 由 OsAccount SA 通过 IPC 直接调用 `StorageManagerProvider`（调用方 UID 校验为 `ACCOUNT_UID`）。`AccountSubscriber` 仅监听 `USER_UNLOCKED`/`USER_SWITCHED`，负责二次挂载（`MountCryptoPathAgain`）与媒体库连接，**不**驱动用户创建时序，不要绕过 Account SA 直接调用 Daemon。

## 失败回滚

`PrepareUserDirs` 顺序：`GenerateUserKeys` →（失败直接返回，不创建目录；返回 `-EEXIST` 走 `RestoreUserKey`）→ 预清理 `DestroyUserDirs` → 创建目录 → `PrepareAppdataDir`。目录创建失败仅返回错误码（如 `E_PREPARE_DIR`），**未实现链式回滚**（已建目录保留，无 `RollbackUserDirs`）。`need_restore` 是密钥迁移标记，由 `CompleteAddUser` 清除，非创建失败回滚机制。如需回滚须由调用方保证。

## EL 等级隔离

| 等级 | 路径 | 密钥激活时机 | 不可访问条件 |
|------|------|--------------|--------------|
| EL1 | `/data/service/el1/{userId}`、`/data/app/el1/{userId}`、`/data/chipset/el1/{userId}` | PrepareAddUser（生成）；设备启动(全局 user 0) | 密钥未生成时 |
| EL2 | `/data/service/el2/{userId}`、`/data/app/el2/{userId}`、`/data/chipset/el2/{userId}` | `ActiveUserKey`（解锁） | `ActiveUserKey` 未完成时（**锁屏不移除**） |
| EL3 | `/data/service/el3/{userId}`、`/data/app/el3/{userId}` | `ActiveUserKeyAndPrepareElX`（解锁） | 密钥未激活时 |
| EL4 | `/data/service/el4/{userId}`、`/data/app/el4/{userId}` | `ActiveUserKeyAndPrepareElX`（解锁） | **锁屏时不可访问**（EL4 延迟 10s 移除） |
| EL5 | `/data/service/el5/{userId}`、`/data/app/el5/{userId}` | `ActiveUserKeyAndPrepareElX(EL5_KEY)` 激活后 `NotifyUeceActivation` 回调 | **锁屏时不可访问**（EL5 立即移除） |

EL 目录路径由 `/etc/storage_daemon/storage_user_path.json` 配置驱动（`config_default`/`config_pc` 两套），`EL1`–`EL5` 仅为 JSON 键名，`<userId>` 在运行时由 `UserPathResolver` 替换。

不可用 EL1 密钥操作 EL2 目录，密钥和目录等级必须匹配。

## 用户隔离

不可用一个用户的密钥或挂载点操作另一个用户的数据路径。userId=100 的密钥只操作 userId=100 的目录，userId=105 的密钥只操作 userId=105 的目录。

## NATO 和 AppClone

- NATO 用户有独立的恢复密钥目录 EL2/EL3/EL4（`/data/service/el1/public/storage_daemon/sd/el2_NATO/{userId}`、`el3_NATO/{userId}`、`el4_NATO/{userId}`），不要与普通用户密钥路径混用。`ActiveUserKey` 在存在 `natoRestore` 时分发到 `ActiveUserKey4Nato`。
- AppClone 用户 ID 范围独立，密钥管理使用 `AppCloneKeyManager` 而非 `KeyManager`，不要用 `KeyManager::GetInstance()` 操作 AppClone 用户。

## 修改前检查

- 当前步骤处于时序哪个位置？前置步骤是否已完成？
- 失败时是否回滚了全部已创建状态？
- 密钥等级和目录等级是否匹配？
- 用户 ID 是否与操作目标匹配？

## 测试指引

- 时序覆盖：`UserManagerTest`、`StorageManagerProviderTest`
- 多用户隔离：至少覆盖跨用户密钥/目录操作拒绝场景
- 涉及真实挂载/加密：需要板侧验证
