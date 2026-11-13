# 存储管理服务指引

## 项目定位

本仓库对应 OpenHarmony `foundation/filemanagement/storage_service`。优先按这些目录定位问题：

- `services/storage_daemon/`：底层守护进程，负责设备热插拔、分区/挂载/格式化、fscrypt 密钥管理、用户目录创建/销毁、配额管理。
- `services/storage_manager/`：系统 SA（ID 5003），负责卷/磁盘状态维护、用户生命周期、空间统计与监控、IPC 桥接到 Daemon（ID 5004）。
- `interfaces/innerkits/`、`interfaces/kits/`：Native 内部 API 和 JS/ETS 对外 API。
- `services/common/`：共享常量、日志宏、错误码定义。
- `services/storage_space_manager/`：独立 SA，负责空间统计/配额/监控（自有 IDL `IStorageSpaceManager.idl`，自有权限 `STORAGE_MANAGER`/`MOUNT_UNMOUNT_MANAGER`/`MOUNT_FORMAT_MANAGER`）。
- `interfaces/kits/js/`、`interfaces/kits/taihe/`（key_manager、storage_statistics）、`interfaces/kits/cj/`：JS/ETS、Taihe/ANI、CJ FFI 对外 kits。
- `tools/ohos-storage-manager/`：`ohos-storageManager` CLI 工具。
- `services/storage_daemon/mtpfs/`、`gphotofs/`：MTP/gphoto FUSE 挂载（由 `support_open_source_libmtp`/`support_open_source_libgphoto2` 门控）。
- `test/`、`test/fuzztest/`：单元测试和 fuzz 目标。

### 高频改动 / 高风险路径

- `services/storage_daemon/user/src/user_manager.cpp`（`StartUser`/`StopUser`，按用户挂载 EL2-EL5）。
- `services/storage_daemon/libfscrypt/src/key_control.c`（fscrypt 密钥 add/remove）。
- `services/storage_daemon/crypto/src/key_manager.cpp`（`GenerateUserKeys`/`ActiveUserKey`/`ClearKey`）与 `crypto/src/base_key.cpp`（`ClearKey` 流程，`ForceRemoveDirectory` 调用点）。
- `services/storage_daemon/utils/file_utils.cpp::RmDirRecurse`（递归删除，见「高危删除约束」）。
- `services/storage_manager/ipc/src/storage_manager_provider.cpp`（`StorageManagerProvider : SystemAbility, StorageManagerStub`；`CheckClientPermission`；白名单 UID；`REGISTER_SYSTEM_ABILITY_BY_ID(..., 5003)`）。
- `interfaces/innerkits/storage_manager/native/storage_service_errno.h`（错误码唯一来源，`STORAGE_SERVICE_SYS_CAP_TAG=13600000`）。
- IDL 文件 `services/storage_manager/IStorageManager.idl`、`services/storage_daemon/IStorageDaemon.idl`、`services/storage_space_manager/interfaces/innerkits/IStorageSpaceManager.idl`（经 `idl_gen_interface` 生成 stub/proxy）。

## 构建和验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

```sh
./build.sh --product-name rk3568 --build-target storage_daemon --ccache
./build.sh --product-name rk3568 --build-target storage_manager --ccache
./build.sh --product-name rk3568 --build-target //foundation/filemanagement/storage_service/services/storage_daemon:storage_daemon_unit_test --ccache
./build.sh --product-name rk3568 --build-target //foundation/filemanagement/storage_service/services/storage_manager:storage_manager_unit_test --ccache
./build.sh --product-name rk3568 --build-target //foundation/filemanagement/storage_service/test/fuzztest:storage_service_fuzztest --ccache
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 StorageDaemonProviderTest
```

测试目标共三个（见 `bundle.json` 的 `test` 数组）：`storage_daemon_unit_test`、`storage_manager_unit_test`、`storage_service_fuzztest`，须按改动范围选择至少一个相关目标。

涉及真实磁盘、加密、多用户或配额的行为，需要补充板侧证据。提交使用 `git commit -s`，并保留 `Co-Authored-By: Agent`。变更需通过单元测试和 fuzz 测试。

### 静态分析 / 风格

无仓库级 cpplint/cpucodecheck 目标；`.clang-format` 为风格基准；`branch_protector_ret="pac_ret"`、`cfi`/`cfi_cross_dso`、`integer_overflow`、`ubsan`、`boundary_sanitize` 按目标在 BUILD.gn 中启用（如 `interfaces/innerkits/storage_manager/native/BUILD.gn`），不得剥离。

### API / 契约检查

公共/innerkits 改动须核对 `bundle.json` `inner_kits` 声明的导出头与 `interfaces/innerkits/storage_manager/native/storage_service_errno.h` 错误码一致；无仓库级 `api_check`，须人工对比导出头与 `inner_kits` 声明，确认签名/枚举/错误码无意外变更。

### 完成定义

完成 = 受影响 target 构建通过 + 相关单元测试通过 + fuzz 构建/运行通过（fuzz-sensitive 改动）+ 高危改动附板侧证据 + 无新增 `E_ERR` + 日志必打节点齐全 + 约束披露完整。回复须含：变更文件、执行的命令与结果、板侧证据 Y/N、权限/兼容/DFX/多用户/加密影响、剩余风险。

### 无法本地验证时

若无法在本地构建或板侧验证，须列出未执行命令、原因与风险，不得将未执行项描述为已通过。

## 错误码与代码规范

### 命名规则

| 类型 | 规则 | 注意 |
|------|------|------|
| 类名 | 大驼峰 | `KeyManager`、`MountManager` |
| 函数名 | 大驼峰 | `GenerateUserKeys`、`ActiveUserKey` |
| 变量名 | 小驼峰 | `userId`、`volumeId` |
| 成员变量 | 下划线结尾或小驼峰 | 源码中两种并存，修改时保持同文件一致 |
| 常量 | `extern const` | `MAX_USER_ID` 类型是 `int` 不是 `int32_t`；`EL1` 类型是 `std::string` 不是 `const char*` |
| 枚举 | 大驼峰类型 + 大写值 | `VolumeState`、`DiskInfo::DiskType`、`KeyType` 是 `enum`（非 `enum class`）；`BizStage`、`KeyEncryptType` 是 `enum class`（引用需带作用域） |

### 错误码选用

禁止使用 `E_ERR`（太笼统），必须使用对应类别的具体错误码：

| 场景 | 必须使用 | 禁止使用 |
|------|----------|----------|
| userId 范围无效 | `E_USERID_RANGE` | E_ERR |
| volumeId/diskId 不存在 | `E_NON_EXIST` | E_ERR |
| volume 状态不允许 | `E_VOL_STATE` | E_ERR |
| 目录创建失败 | `E_CREATE_DIR_RECURSIVE_FAILED` | E_ERR |
| 目录销毁失败 | `E_DESTROY_DIR` | E_ERR |
| 密钥存储失败 | `E_ELX_KEY_STORE_ERROR` | E_ERR |
| 密钥激活失败 | `E_ELX_KEY_ACTIVE_ERROR` | E_ERR |
| 权限不足 | `E_PERMISSION_DENIED` | E_ERR |
| 参数无效 | `E_PARAMS_INVALID` | E_ERR |

### 错误码范围

基础值 `STORAGE_SERVICE_SYS_CAP_TAG = 13600000`（定义于 `interfaces/innerkits/storage_manager/native/storage_service_errno.h`）：

| 范围 | 类别 | 示例 |
|------|------|------|
| 0 | 成功（`E_OK`） | — |
| 13600001-13600025 | 通用错误 | `E_PERMISSION_DENIED`、`E_NON_EXIST`、`E_PARAMS_INVALID` |
| 13600201-13600245 | 密钥管理 | `E_USERID_RANGE`、`E_ELX_KEY_STORE_ERROR`、`E_ELX_KEY_ACTIVE_ERROR` |
| 13600350-13600417 | 密钥完整性/密钥操作 | `E_DIR_INTEGRITY_ERR`、`E_KEY_SIZE_ERROR`、`E_INSTALL_KEYDESC_KEYRING_ERR` |
| 13600701-13600765 | 用户管理/挂载 | `E_USER_MOUNT_ERR`、`E_MOUNT_HMDFS`、`E_UMOUNT_SHARE_FILE` |
| 13601201-13601233 | 空间统计（含 BundleManager） | `E_BUNDLEMGR_ERROR`、`E_STATVFS`、`E_GET_INODE_ERROR` |
| 13601701-13601750 | 外卡管理（卷状态/挂载/分区） | `E_VOL_STATE`、`E_TIMEOUT_MOUNT`、`E_UDF_MOUNT` |
| 13602201-13602205 | 文件分享/配额内核 | `E_CREATE_SHARE_FILE_FAILED`、`E_QUOTA_CTL_KERNEL_ERR` |

### 日志必打节点

| 流程 | 必打节点 | 级别 |
|------|----------|------|
| 用户管理（Prepare/Start/Complete/Stop/Remove） | 开始、成功、失败 | LOGI/LOGE |
| 密钥管理（Generate/Active/Inactive/Lock/Unlock/Update） | 成功、失败 | LOGI/LOGE |
| 卷操作（Mount/Unmount/Format/TryToFix） | 开始、成功、失败 | LOGI/LOGE |
| 磁盘事件（添加/移除） | 设备信息、正常/异常移除 | LOGI/LOGW |
| IPC 连接 | 连接成功/失败、断开 | LOGI/LOGE/LOGW |

敏感信息禁止打印：不可在日志中出现密码、密钥数据、token 内容。只打印长度或状态。

### 编码约束

| 约束项 | 限制 |
|--------|------|
| 单个函数 | ≤ 50 行，超过则拆分 |
| 单个文件 | ≤ 2000 行，超过则按职责拆分 |

## 知识索引

稳定背景知识放在 `docs/knowledge/`。改动前按场景读取对应文件。改动前先在工作说明中确认：(1) 任务分类；(2) 已读取的 `docs/knowledge/*` 文件；(3) 本次触达的「项目约束」中的高风险约束，若未触达也明确说明。

| 场景 | 先读 |
|------|------|
| 用户创建/删除、PrepareAddUser/StartUser/CompleteAddUser、EL 目录、多用户隔离、回滚 | `docs/knowledge/user-directory-lifecycle.md` |
| 卷 Mount/Unmount、EJECTING、磁盘热插拔、SD/USB/光驱/MTP 差异、挂载超时 | `docs/knowledge/disk-volume-lifecycle.md` |
| 密钥生成/激活、锁屏/解锁 EL2、V1/V2 兼容、App 密钥 | `docs/knowledge/crypto-key-management.md` |
| Manager-Daemon 职责、懒连接、崩溃恢复、IPC 新增流程 | `docs/knowledge/ipc-interface-guide.md` |
| 配额设置/查询、存储监控阈值、共享文件 ACL | `docs/knowledge/storage-quota-monitoring.md` |
| VolumeState 跳转、DiskType/KeyType 作用域、位掩码 | `docs/knowledge/enums-and-state-machine.md` |
| 代码放置位置、Instance/GetInstance 差异、头文件路径 | `docs/knowledge/architecture.md` |
| 实现必做步骤、各场景操作必做项 | `docs/knowledge/implementation-templates.md` |
| 测试覆盖要求、Mock 选择、Fuzz | `docs/knowledge/testing-guide.md` |
| 时序跳步、状态跳转、锁屏访问、跨用户、IPC 死锁 | `docs/knowledge/constraints-and-traps.md` |
| 日志查看、SA 状态检查、问题定位 | `docs/knowledge/debugging-guide.md` |
| 编译宏差异、HUKS/OsAccount 不可用 | `docs/knowledge/external-dependencies.md` |

### 路径 → 文档

| 改动路径 | 先读 |
| --- | --- |
| `services/storage_daemon/user/` | `docs/knowledge/user-directory-lifecycle.md` |
| `services/storage_daemon/libfscrypt/`、`crypto/` | `docs/knowledge/crypto-key-management.md` |
| `services/storage_manager/ipc/` | `docs/knowledge/ipc-interface-guide.md` |
| `services/storage_space_manager/`、配额/监控 | `docs/knowledge/storage-quota-monitoring.md` |
| `services/storage_daemon/disk/`、卷状态 | `docs/knowledge/disk-volume-lifecycle.md` |
| `interfaces/kits/{js,taihe,cj}/`、`services/storage_manager/{kits_impl,cj_impl}/` | 本文件「错误码范围」「项目约束·`JsErrCode` 双枚举约束」；`interfaces/innerkits/storage_manager/native/storage_service_errno.h` |

### 词汇 → 文档

`need_restore`、`EL5_KEY`、`ActiveUserKeyAndPrepare`、`-ENOENT`、`NotifyUeceActivation` → `crypto-key-management.md`；`EJECTING`、`MOUNTED→REMOVED` → `disk-volume-lifecycle.md` + `enums-and-state-machine.md`；`ForceRemoveDirectory`、`RmDirRecurse` → `constraints-and-traps.md`；`PrepareAddUser→CompleteAddUser→ActiveUserKey→PrepareStartUser` → `user-directory-lifecycle.md`；`JsErrCode`、`E_IPCSS`、`E_SUPPORTEDFS`、JS/CJ/Taihe 错误码映射 → 本文件「项目约束·`JsErrCode` 双枚举约束」+ `storage_service_errno.h`（`JsErrCode` 隐式自增，禁中间插入）。

## 项目约束

- 用户时序不可跳步：`PrepareAddUser`（生成 EL1-EL5 密钥+建目录）→ `CompleteAddUser`（清 need_restore）→ `ActiveUserKey`（解锁时激活 EL2-EL5）→ `PrepareStartUser`（触发 Daemon `StartUser`：建 EL2-EL5 服务目录+挂载，须在密钥激活后）。缺步会导致 EL2-EL5 服务目录/挂载未建立或密钥未激活。
- 卷状态不可跳转：`MOUNTED → EJECTING → REMOVED`，必须经过 EJECTING，不可直接置 REMOVED。
- EL4/EL5 密钥锁屏移除、解锁安装：`LockUserScreen` 锁 EL5（立即）+ EL4（延迟 10s），锁屏时不可访问 EL4/EL5 加密数据；EL2 不随锁屏移除，必须在解锁后才操作 EL4/EL5。
- 用户隔离：不可用 userId=100 的密钥操作 userId=105 的目录，密钥和目录必须匹配。
- IPC 死锁预防：不要在持有 Manager 锁时调用 Daemon IPC，先释放锁再发起 IPC。
- 高危删除约束：对路径执行不可逆删除（递归删除、`RmDirRecurse`/`ForceRemoveDirectory`、`unlink`、`rmdir` 等）前，必须确认目标真实后端即预期对象——挂载类间接层（设备挂载/绑定挂载/重复或多层挂载等）会使挂载点目录的真实后端≠表面，对残留挂载层的挂载点递归删会穿透到源数据导致数据丢失。删挂载点目录前必须逐层彻底卸载、确认无残留挂载层，否则禁止执行、需人工确认。
- ELX 密钥删除约束：删除 EL1~EL5 任一等级密钥是不可逆操作——`ClearKey` 会擦除密钥材料并递归删除密钥目录与备份目录（`ForceRemoveDirectory`），密钥一旦删除则对应等级加密数据永久无法解密；且低等级（如 EL1）删除会令 EL2~EL5 链式依赖断裂、高等级数据一并无法激活。删除前必须确认已无回滚/恢复需要：存在 `need_restore` 标志或半成品/待恢复态时禁止销毁密钥材料（须先恢复或重建），并在用户/设备销毁流程确认完成后再执行，否则禁止删除、需人工确认。
- EL5 解密窗口期禁令：`PrepareAddUser`/`CompleteAddUser` 完成后到 EL5 完全解密（`ActiveUserKey4Single` 中 EL5 经 `ActiveUserKeyAndPrepare(EL5_KEY)` 激活 + `NotifyUeceActivation` 回调完成）前，除删除用户外禁止 `InactiveKey(USER_DESTROY)`/`ClearKey`/`DoDeleteUserCeEceSeceKeys` 销毁用户密钥，否则 EL5 密钥被驱逐后 UECE 无法解密。`ActiveUserKeyAndPrepare` 的 `-ENOENT` 分支以 `if (type != EL5_KEY)` 守卫跳过销毁，该守卫必须保持（详见 `docs/knowledge/crypto-key-management.md`）。
- 公共 API 与错误码兼容：`bundle.json` 声明 4 个 inner_kits（2 个在 `interfaces/innerkits/`：`storage_manager/native`、`acl/native`；另含 `services/storage_daemon/libfscrypt` 与 `interfaces/kits/cj/storage_manager`）与 `interfaces/kits/{js,taihe,cj}/*` 为公共契约；不得无意改变签名、参数、返回值、错误码语义或枚举值顺序；新增错误码须落入 `interfaces/innerkits/storage_manager/native/storage_service_errno.h` 已声明范围并更新该文件。
- `JsErrCode` 双枚举约束：`storage_service_errno.h` 含两个枚举——`ErrNo`（13600000+，C/Native 面向）与 `JsErrCode`（JS/CJ/Taihe 面向，行 334+）。`JsErrCode` 存在隐式自增成员（`E_IPCSS = TAG+1` 之后 `E_SUPPORTEDFS` 等逐个 +1），禁止在中间插入值；新增必须 append 到末尾并显式赋值，否则会平移所有下游 JS 错误码、破坏公共契约。错误码范围表（见「错误码范围」）为非穷举，其余范围以 `storage_service_errno.h` 全表为准。
- 生成代码边界：`idl_gen_interface` 生成的 `StorageManagerStub`/`StorageDaemonStub`/`OnRemoteRequest` 派发（输出到 `${target_gen_dir}`）与 `taihe_ffi_gen` 生成的 ANI/CJ 绑定为生成物；修改前先改 IDL（`IStorageManager.idl`/`IStorageDaemon.idl`/`IStorageSpaceManager.idl` 或 `.taihe`）再重新生成，不得直接编辑生成文件。
- 第三方依赖：`libfuse`/`libusb`/`libmtp`/`ntfs-3g`/`libgphoto2` 升级或裁剪须验证板侧挂载/热插拔/MTP/光驱行为；`bounds_checking_function` 提供的 `memcpy_s`/`strcpy_s` 为强制安全函数，禁止裸 `memcpy`/`strcpy`。
- IPC 权限校验：新增 IPC stub 须在 `services/storage_manager/ipc/src/storage_manager_provider.cpp::CheckClientPermission` 注册对应权限字符串；现有权限 `ohos.permission.STORAGE_MANAGER`/`STORAGE_MANAGER_CRYPT`/`MOUNT_UNMOUNT_MANAGER`/`MOUNT_FORMAT_MANAGER`/`PUBLISH_SYSTEM_COMMON_EVENT`；`ACCOUNT_UID`(3058)/`BACKUP_SA_UID`(1089)/`FOUNDATION_UID`(5523) 等白名单 UID 自动放行，改动白名单须取得确认。
