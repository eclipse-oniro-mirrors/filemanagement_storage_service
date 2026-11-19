# 测试覆盖要求

本文只记录测试场景覆盖要求和 Mock 选择规则，不包含测试代码示例。状态机转换测试见 `enums-and-state-machine.md`，IPC 测试见 `ipc-interface-guide.md`。

## 覆盖要求

所有新增功能必须有对应单元测试，公开 API 必须有 fuzz 测试。

## 必测场景

### KeyManager

| 函数 | 必测场景 |
|------|----------|
| GenerateUserKeys | userId 正常(0/100)、边界(0/10738)、超范围(-1/10739)、flags 组合、密钥已存在(-EEXIST 走 RestoreUserKey)、HUKS 不可用 |
| ActiveUserKey | userId 有效/无效、密钥类型 EL2-EL5（激活链 EL2→EL3→EL4→EL5）、密钥不存在；注意激活在解锁时由 ActiveUserKey 触发，非 CompleteAddUser |
| InActiveUserKey | userId 有效/无效、密钥状态 |
| LockUserScreen | userId 有效/无效、EL5 立即锁定 + EL4 延迟 10s 锁定（DelayHandler） |
| UnlockUserScreen | userId 有效、token/secret 无效、取消 EL4 延迟任务 |
| GenerateAppkey | EL5 已激活 + UECE 支持（不支持返回 -ENOTSUP） |
| FscryptKeyV1/V2 | V1 与 V2 密钥路径分别覆盖、V1↔V2 兼容、`SUPPORT_FSCRYPT_V2` 关闭时回退 V1 |

### IVolumeOperator（取代旧的 VolumeManager）

| 函数 | 必测场景 |
|------|----------|
| Mount | devPath/fsType 正常/空/不存在、Check 失败、60s 超时（`E_TIMEOUT_MOUNT`） |
| Unmount | devPath 正常/空/不存在、未挂载 |
| Format | devPath 正常、fsType 有效/无效 |
| Check/Repair | 各 operator 的 fsck 命令路径（ext4/vfat 无 Check；ntfs `fsck.ntfs`/`ntfsfix`；hmfs `fsck.f2fs`） |
| VolumeOperatorFactory | `RegisterOperators` 注册的 fsType（vfat/ntfs/exfat/f2fs/hmfs/ext4/udf/iso9660）全覆盖 |

### UserManager

| 函数 | 必测场景 |
|------|----------|
| PrepareUserDirs | userId 正常/边界/无效、目录已存在、创建失败返回错误码（无链式回滚）、`-EEXIST` 走 RestoreUserKey |
| DestroyUserDirs | userId 正常/无效、目录不存在 |

### QuotaManager

| 函数 | 必测场景 |
|------|----------|
| SetBundleQuota | uid 正常/无效、limit 为 0/超上限、quota 不支持 |

### 状态机

| 必测 | 合法转换 + 所有非法跳转拒绝 |
|------|------|

### IPC Stub

| 必测 | 参数正常/空/超长/类型错误 |

### DiskManagerLibLoader / DiskMgrApi（磁盘能力按需加载）

| 对象 | 必测场景 |
|------|----------|
| EnsureLoaded | dlopen 失败→`E_NOT_SUPPORT` 不崩溃；冷却 1s 内不热重试；冷却期满自愈重试；成功后快路径不再 dlopen |
| Init/Deinit | dlsym 失败→dlclose 回滚；`DiskMgrGetApi` 返回空→回滚；单轮 dlopen 3 次重试（间隔 100ms） |
| 33 个封装方法 | 门控分支（库未加载）+ 分发分支（fake API 表桩函数记录调用名/哨兵值传播）全覆盖 |
| DiskMgrGetApi 表 | 33 项非空完整性；8 个 operator Wrapper 的 unknown-fsType→`E_NOT_SUPPORT` 与已知类型分发双分支 |
| ScanDevice/DmDevice 封装 | GetDataDisks/GetExternalDisks/GetDiskSize/CreateDmDevice（失败分支宿主可测） |

## Mock 选择

| Mock 文件 | 用途 |
|-----------|------|
| `huks_master_mock.h` | Mock HUKS 加密/解密 |
| `base_key_mock.h` | Mock 密钥激活/停用 |
| `iam_client_mock.h` | Mock 用户认证（类名 `IamClient`） |
| `key_manager_mock.h` | Mock 密钥生成 |
| `mount_manager_mock.h` | Mock 挂载/卸载（`MountManager`） |
| `fscrypt_control_mock.h` | Mock fscrypt 安装/移除 |
| `file_utils_mock.h` | Mock 文件操作 |
| `disk_utils_mock.h` | Mock 磁盘信息 |
| `storage_daemon_proxy_mock.h` / `storage_daemon_communication_mock.h` | Mock Manager→Daemon IPC（`services/storage_manager/include/mock/`） |
| `storage_status_manager_mock.h` / `bundle_mgr_interface_mock.h` / `datashare_helper_mock.h` / `disk_manager_client_mock.h` | Mock Manager 侧状态/Bundle/DataShare/Disk 客户端 |
| `uece_activation_callback_mock.h` | Mock UECE/EL5 激活回调 |
| `file_utils_mock.h`（含 `ExtStorageMountForkExec`） | Mock 文件操作与 QOS 挂载 fork 路径（exfat DoMount 测试必需） |
| `disk_utils_mock.h`（含 `GetUdfRev`/`SendScsiCmd`/`ReadDiscInfo`/`IsExistCD`） | Mock disk_common_utils 自由函数 |

## gmock 陷阱（E_OK==0）

`E_OK` 的值是 **0**（storage_service_errno.h 枚举首项）。gmock 无期望调用默认返回 0——恰为「成功」。凡被测函数以 mock 函数返回值判断成败的用例，**必须显式 `EXPECT_CALL(...).WillOnce(Return(E_ERR))` 声明失败**，否则分支漂移（本仓已发生 9 例此类用例失败）。

## 主机侧运行（x86_64_virt musl）

```sh
# 库路径（示例；测试二进制位于 out/x86_64_virt/tests/unittest/storage_service/storage_service/storage_daemon/）
ld-musl-x86_64.so.1 --library-path "<packages/phone/system/lib64 及 innerkits 各依赖目录>" <test binary>
```

- 环境预建：`/data/local`、`/mnt/data/external`（多个 UT 的真实目录依赖）；HiLogAdapter 警告（无 hilog socket）无害。
- `storage_daemon_provider_test`：`SetCallingUid` 仅 mock `GetCallingAppUid`，`IPCSkeleton::GetCallingUid` 为真实现 → SMART 类用例 uid 门在宿主必失败（环境限制非用例问题）；磁盘分发路径由 lib_loader/disk_mgr_api 两级 UT 分层覆盖（provider UT 只覆盖门控与参数校验）。
- `disk_manager_lib_loader_test` 模式：`-Wl,--wrap=dlopen/dlsym/dlclose` + 测试文件内自建 fake `DiskMgrGetApi` 表（该目标不编译 disk_mgr_api.cpp，可自备符号）。

## 测试位置

| 模块 | 路径 |
|------|------|
| 密钥管理 | `services/storage_daemon/crypto/test/`（`key_manager_test`、`fscrypt_v1_test`、`fscrypt_v2_test`、`fscrypt_key_v1_ext_test`、`base_key_test`、`fbex_test`、`huks_master_test`、`iam_client_test`、`recover_manager_test` 等） |
| 用户管理 | `services/storage_daemon/user/test/` |
| 磁盘管理 | `services/storage_daemon/disk/test/` |
| 外置卷 | `services/storage_daemon/disk_manager/test/`（`ext_volume_operator_factory_test`、`ext_disk_mgr_api_test`、`disk_manager_lib_loader_test` 等） |
| IPC 层 | `services/storage_manager/ipc/test/`（`storage_manager_provider_test`、`storage_manager_provider_permission_test`、`storage_manager_provider_communication_test`） |
| Manager 其他子模块 | `services/storage_manager/storage/test/`、`scan/test/`、`storage_daemon_communication/test/`、`client/test/`、`utils/test/` |
| DFX 上报 | `services/storage_manager/dfx_report/test/`（`dfx_report_test`） |
| Fuzz | `test/fuzztest/`（`storagemanagerstub_fuzzer`、`storagedaemonprovider_fuzzer`、`keyctrl*` 等约 50+ 目标） |

## 构建与运行

构建命令在 OpenHarmony 源码根执行（不在本子目录）：

```sh
./build.sh --product-name rk3568 --build-target storage_daemon --ccache
./build.sh --product-name rk3568 --build-target storage_manager --ccache
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 storage_manager_provider_test FscryptKeyV1Test dfx_report_test
```

## 修改前检查

- 新增功能是否有对应单元测试？
- 非法状态转换是否被覆盖？
- 加密/多用户相关操作是否有板侧验证计划？
