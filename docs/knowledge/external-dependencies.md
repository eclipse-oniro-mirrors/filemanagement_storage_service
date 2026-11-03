# 外部依赖约束

本文只记录编译宏对代码行为的影响和依赖不可用时的处理规则。密钥操作依赖见 `crypto-key-management.md`，HUKS 封装见源码 `crypto/huks_master.h`。

## 编译宏行为差异

| 编译宏 | 影响范围 | 行为差异 | 不考虑的后果 |
|--------|----------|----------|--------------|
| `USER_CRYPTO_MIGRATE_KEY` | UpdateUserAuth | 增加 `needGenerateShield` 参数 | 只写2参数版本，宏定义时编译失败 |
| `RECOVER_KEY_TEE_ENVIRONMENT` | RecoveryManager | 恢复密钥使用 TEE 环境 | 非 TEE 环境下调用了 TEE 专属接口 |
| `EL5_FILEKEY_MANAGER` | KeyManager | UECE 回调注册功能 | 不处理 UECE 导致 EL5 密钥未激活 |
| `USER_AUTH_FRAMEWORK` | IamClient | 定义时引入 `user_idm_client.h` 等 UserAuth 框架头；类名是 `IamClient`（非 IAMClient） | 未定义时缺少 UserAuth 回调类型，编译失败 |
| `HUKS_IDL_ENVIRONMENT` | HuksMaster | HUKS 使用 HDI 1.1 接口 | 链接了错误的 HUKS 库 |
| `SUPPORT_RECOVERY_KEY_SERVICE` | KeyManager | 恢复密钥服务支持 | 未实现恢复密钥流程 |

新增代码涉及上述宏影响的功能时，必须同时考虑宏定义和未定义两种行为，不要只写一种。

## 依赖不可用时的处理规则

| 依赖 | 检查方法 | 不可用时返回 | 不可用时不可做 |
|------|----------|--------------|----------------|
| HUKS | 无 `IsHuksAvailable` 方法；通过 `InitGlobalDeviceKey`/`GenerateKey`/`DecryptKey` 返回值判断（失败返回 `E_GLOBAL_KEY_INIT_ERROR`） | `E_GLOBAL_KEY_INIT_ERROR` | 不要继续生成/激活密钥 |
| OsAccount | `OsAccountManager::IsOsAccountExists()` | `E_SA_IS_NULLPTR` | 不要操作不存在用户的目录 |
| AccessToken | `AccessTokenKit::GetTokenTypeFlag()` | `E_PERMISSION_DENIED` | 不要跳过权限校验 |
| BundleFramework | `BundleMgrConnector::GetInstance().GetBundleMgrProxy()` 返回 nullptr | `E_BUNDLEMGR_ERROR`（无 `E_GET_UID_ERROR`） | 不要假设应用 UID 可查 |

依赖不可用时必须返回具体错误码并上报 StorageRadar，不要静默忽略或返回 E_ERR。

## 依赖初始化顺序

1. Daemon 启动：注册 SA → 启动 Netlink/DiskManager（受 `EXTERNAL_STORAGE_MANAGER` 宏控制） → 经 Manager IPC 调用 `InitGlobalKey()` → `InitGlobalDeviceKey()` → 加载 EL1
2. Manager 启动：懒连接 Daemon → 启动监控 → 订阅用户事件

不要假设 Manager 启动时所有依赖已就绪。

## 超时保护与审计（DFX）

storage_daemon 密钥/用户流程强制依赖 HiCollie 超时保护与 HiAudit 审计（PR #3277 可靠性加固）：

- 超时阈值：用户级操作 30s（`USER_OPERATE_OUT_SECONDS`）、密钥级操作 60s（`KEY_OPERATE_OUT_SECONDS`），统一 `XCOLLIE_FLAG_DEFAULT`。
- HiAudit 审计覆盖双升单与开机密钥流程关键节点（`InitGlobalDeviceKey`/`InitGlobalUserKeys`/`LoadAllUsersEl1Key`/`ActiveUserKey4Nato` 等），详见 `crypto-key-management.md`。
- 这些节点原细粒度 `StorageRadar` 上报已被 HiAudit 替代，新增审计节点处不要重复打细粒度雷达。

## 修改前检查

- 涉及的编译宏是否考虑了定义/未定义两种行为？
- 依赖不可用时是否返回了具体错误码？
- 新增依赖是否添加了可用性检查？
