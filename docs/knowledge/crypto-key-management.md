# 加密密钥管理约束

本文只记录密钥操作的时序约束、激活规则和版本兼容边界。用户目录时序见 `user-directory-lifecycle.md`，编译宏行为差异见 `external-dependencies.md`。

## 密钥激活时序约束

| KeyType | 激活时机 | 不可访问条件 | 不可跳过的依赖 |
|---------|----------|--------------|----------------|
| EL0 | 设备启动 | 无 | 无 |
| EL1 | 设备启动(全局 user 0) / `PrepareUserDirs`(PrepareAddUser) | `PrepareUserDirs` 未完成时 | 依赖 EL 存储目录已创建 |
| EL2 | `ActiveUserKey`（解锁/用户激活流程） | `ActiveUserKey` 未完成时 | 依赖 EL1 已激活 |
| EL3 | `ActiveUserKeyAndPrepareElX`（解锁流程） | `ActiveUserKey` 未完成时 | 依赖 EL2 已激活 |
| EL4 | `ActiveUserKeyAndPrepareElX`（解锁流程） | `ActiveUserKey` 未完成时 | 依赖 EL2 已激活 |
| EL5 | `ActiveUserKeyAndPrepareElX(EL5_KEY)` 激活后由 `NotifyUeceActivation` 触发回调 | EL5 密钥未激活时 | 依赖 EL2/EL4 已激活 + UECE 支持(`IsUeceSupport`) |

密钥生成在 `PrepareAddUser`（`GenerateUserKeys` 一次性生成 EL1–EL5，flags 强制补齐 EL3/EL4/EL5），激活在解锁时的 `ActiveUserKey`（`ActiveUserKey4Single` 依次激活 EL2→EL3→EL4→EL5）。`StartUser`/`CompleteAddUser` 不激活密钥。激活是链式依赖：EL1 未激活则 EL2 不可激活，EL2 未激活则 EL3/EL4 不可激活。不要跳步激活。

## EL5 解密窗口期密钥销毁禁令

用户新增流程（`PrepareAddUser`→`CompleteAddUser`）结束后、EL5 完全解密之前的窗口期内，**禁止以非删除用户为目的调用 `InactiveKey(USER_DESTROY)`** 销毁用户密钥，否则 EL5 密钥被从内核驱逐后 UECE 无法解密，EL5 加密数据永久不可访问。

- `InactiveKey(USER_DESTROY)` 由 `BaseKey::ClearKey` 调用（`USER_DESTROY=0x1`，`base_key.cpp:975`），是密钥从内核驱逐 + 材料擦除的销毁路径。
- "EL5 完全解密" 指 `ActiveUserKey4Single` 中 EL5 经 `ActiveUserKeyAndPrepare(EL5_KEY)` 激活、且 `NotifyUeceActivation` UECE 回调完成。
- 合法调用 `InactiveKey(USER_DESTROY)`/`ClearKey` 的场景仅限：
  - 删除用户：`DestroyUserDirs`→`DeleteUserKeys`→`ClearKey`（`storage_daemon.cpp:371`）
  - 激活流程中**非 EL5** 等级的 `-ENOENT` 重建：`ActiveUserKeyAndPrepare` 在 `type != EL5_KEY` 时才调 `DoDeleteUserCeEceSeceKeys`→`ClearKey`（`storage_daemon.cpp:999`、`key_manager.cpp:991`）
- 代码已有守卫（**必须保持**）：`ActiveUserKeyAndPrepare` 的 `-ENOENT` 分支以 `if (type != EL5_KEY)` 显式跳过 `DoDeleteUserCeEceSeceKeys`，EL5 在解密前不走销毁路径。修改激活/恢复流程时必须保持该守卫，不得对未完成解密的 EL5 调用 `InactiveKey(USER_DESTROY)`/`ClearKey`/`DoDeleteUserCeEceSeceKeys`，除非整个用户被删除。

## 锁屏/解锁约束

- 锁屏（`LockUserScreen`）：EL5(UECE) 立即从内核移除，EL4(SECE) 经 10 秒延迟任务（`DelayHandler::StartDelayTask`，`DEFAULT_CHECK_INTERVAL=10s`）移除。此后不可访问 `/data/service/el4`、`/data/service/el5` 下加密数据。**EL2 不随锁屏移除**。
- 解锁（`UnlockUserScreen`）：EL4(ECE/SECE) 与 EL5(UECE) 重新安装到内核（`UnlockEceSece`/`UnlockUece`），并 `CancelDelayTask` 取消延迟锁定。恢复对 EL4/EL5 目录的访问。
- 不要在 `LockUserScreen` 后尝试读取 EL4/EL5 加密数据，必须等 `UnlockUserScreen` 完成（EL4 还有 10s 延迟锁定窗口）。

## 版本兼容边界

| 特性 | V1 | V2 |
|------|----|----|
| 密钥描述符 | `fscrypt_policy_v1`（8 字节 descriptor，`keyring add_key`） | `fscrypt_policy_v2`（16 字节 identifier+密钥引用，`FS_IOC_ADD_ENCRYPTION_KEY`，需内核 5.10+） |
| 实现类 | `FscryptKeyV1` | `FscryptKeyV2` |

V1/V2 是运行时 fscrypt 策略/描述符版本（`GetBaseKey` 按 `GetFscryptVersionFromPolicy` 与内核版本选择），与编译宏 `HUKS_IDL_ENVIRONMENT`（HDI 调用）、`RECOVER_KEY_TEE_ENVIRONMENT`（TEE 恢复）相互独立——后两者见 `external-dependencies.md`，不要与 V1/V2 混为一谈。

新增密钥操作时必须同时考虑 V1 和 V2 的行为差异（`FscryptKeyV1`/`FscryptKeyV2` 均需实现同一 `BaseKey` 虚接口），不要只测试当前版本。

## App 密钥约束

App 密钥（`GenerateAppkey`）依赖对应用户的 **EL5 密钥**已激活且 UECE 支持（`IsUeceSupport`，不支持返回 `-ENOTSUP`）。EL5 未激活或不支持 UECE 时生成 App 密钥会失败，不要在 `ActiveUserKeyAndPrepareElX(EL5_KEY)` 之前调用。

## need_restore / ClearKey 删除约束

- `BaseKey::ClearKey` 在 `USER_CRYPTO_MIGRATE_KEY` 下检查 `{USER_EL1_DIR}/{userId}/latest/need_restore`；存在则 `needClearFlag=false`，**不**擦除密钥材料、**不** `ForceRemoveDirectory` 密钥目录与备份目录。
- `CompleteAddUser` 清除该 `need_restore` 标记（迁移用），非创建失败回滚机制。
- `GenerateUserKeys` 返回 `-EEXIST` 时走 `RestoreUserKey`（迁移恢复），不要当作新建失败。
- **`RestoreUserKey` 流程禁止删除 EL1 的 `need_restore`**：`PrepareAddUser` 走到迁移恢复分支时，`RestoreOneUserKey`（`storage_daemon.cpp:180`）逐级恢复 EL1–EL5，每级成功后 `remove(elNeedRestorePath)` 删除该级 `need_restore`，但 EL1 由 `if (type != EL1_KEY)` 守卫（`storage_daemon.cpp:234`）显式跳过删除。EL1 的 `need_restore` 须保留至 `CompleteAddUser`（`storage_daemon.cpp:424`，`USER_CRYPTO_MIGRATE_KEY` 下）清除。该守卫必须保持——若在此流程删除 EL1 `need_restore`，一旦恢复流程异常中断，`PrepareAddUser` 重入时 `RestoreUserKey` 入口 `IsNeedRestorePathExist`（`storage_daemon.cpp:252`）为假、直接返回 `-EEXIST`，无法再进入解密恢复流程，EL1–EL5 密钥将无法恢复解密。
- 高危约束见 `AGENTS.md`：EL1~EL5 任一等级密钥删除不可逆，链式依赖会令高等级数据一并无法激活。

## 双层 KeyManager

`PrepareUserDirs` 同时调用 `KeyManager::GetInstance().GenerateUserKeys`（核心 fscrypt）与 `KeyManagerExt::GetInstance().GenerateUserKeys`（ext-so 委托，独立单例 `GetInstance()`）。AppClone 用户密钥由 `AppCloneKeyManager`（独立单例）管理，不要用 `KeyManager`。

## 密钥流程可靠性加固约束（PR #3277）

- **密钥完整性校验必须检查密钥文件是否完整，不得仅校验目录是否存在**。`CheckDir` 校验 EL1–EL4 时须检查 `latest` 目录内密钥文件完整性（`CheckLatestDirFilesExist`），不能退化为"目录存在即放行"。
- **升级/恢复异常走版本回退，禁止删除密钥材料**。检测到 `need_restore` 版本不匹配时，向 `need_restore` 写入版本标记（`DEFAULT_NEED_RESTORE_UPDATE_VERSION`）触发回退，不得调用 `ClearKeyFilesForPath`/`ClearKey` 销毁密钥文件；密钥材料不可逆，异常只标记、交由恢复流程处理。
- **开机批量恢复业务规格：仅 0 用户（全局设备用户）恢复失败使接口返回失败；其他用户恢复失败接口不返回失败**，仅上报大数据打点（StorageRadar）并重试，不阻断整体开机流程。
- **双升单与开机密钥流程关键节点强制 HiAudit 审计 + 超时保护**：用户级操作超时 30s、密钥级操作超时 60s，统一 `XCOLLIE_FLAG_DEFAULT`。审计节点覆盖 `InitGlobalDeviceKey`/`InitGlobalUserKeys`/`LoadAllUsersEl1Key`/双升单（`ActiveUserKey4Nato`/`ActiveUserKey4Update` 等）及用户目录流程关键节点。

## 密钥操作失败上报

所有密钥操作失败必须通过 `StorageRadar` 上报，不要吞掉错误。按场景选用上报 API：`ReportUserKeyResult`（通用密钥结果）、`ReportActiveUserKey`（激活结果）、`ReportUpdateUserAuth`（鉴权/锁屏）、`ReportEl5KeyMgrResult`（EL5/AppKey）、`ReportFucBehavior`（行为统计）、`ReportHuksResult`（HUKS）、`ReportKeyRingResult`（keyring）。无 `ErrorReport` 符号。

## 修改前检查

- 当前密钥等级是什么？前置等级是否已激活？
- 锁屏状态？EL4/EL5 密钥是否在内核（EL2 锁屏不移除）？
- V1/V2 版本是否都考虑了？编译宏（HUKS_IDL_ENVIRONMENT / RECOVER_KEY_TEE_ENVIRONMENT）是否与 V1/V2 区分？
- 失败是否上报了 StorageRadar？

## 测试指引

- 密钥激活/停用：`KeyManagerTest`，必须覆盖锁屏/解锁场景
- 版本兼容：`FscryptKeyV1Test`、`FscryptKeyV2Test`
- 涉及真实 HUKS/TEE：需要板侧验证
