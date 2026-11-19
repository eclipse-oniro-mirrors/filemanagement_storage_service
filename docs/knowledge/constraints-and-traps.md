# 约束与陷阱

本文只记录项目硬性约束和最易犯的 9 个陷阱。各场景详细约束见对应知识文档：用户时序见 `user-directory-lifecycle.md`，卷状态见 `disk-volume-lifecycle.md`，密钥见 `crypto-key-management.md`，IPC 死锁见 `ipc-interface-guide.md`。

## 硬性约束

- 用户时序不可跳步：`PrepareAddUser`（生成 EL1-EL5 密钥+建目录）→ `CompleteAddUser`（清 need_restore）→ `ActiveUserKey`（解锁时激活 EL2-EL5）→ `PrepareStartUser`（触发 Daemon `StartUser`：建 EL2-EL5 服务目录+挂载，须在密钥激活后）。
- 卷状态不可跳转：`MOUNTED → EJECTING → REMOVED`。
- EL4/EL5 密钥锁屏移除（EL5 立即、EL4 延迟 10s）、解锁恢复，锁屏时不可访问 EL4/EL5 数据；EL2 不随锁屏移除。
- 用户隔离：密钥和目录的 userId 必须匹配。
- 错误码必须具体：禁止 E_ERR。
- 升级/恢复异常走版本回退标记（写 `need_restore` 版本），禁止删除密钥材料；密钥完整性校验须检查密钥文件是否完整，不得仅校验目录存在（详见 `crypto-key-management.md`）。
- EL5 解密窗口期禁令：`PrepareAddUser`/`CompleteAddUser` 完成后到 EL5 完全解密（`NotifyUeceActivation` 回调完成）前，禁止以非删除用户为目的调用 `InactiveKey(USER_DESTROY)`/`ClearKey` 销毁用户密钥，否则 EL5 无法解密（详见 `crypto-key-management.md`）。

## 6 个常见陷阱

### 陷阱 1：用户目录创建跳步

跳过 `ActiveUserKey` 直接 `PrepareStartUser`/`StartUser` → EL2-EL5 密钥未激活，加密服务目录与挂载不可访问（密钥在 `PrepareAddUser` 生成、在 `ActiveUserKey` 解锁时激活；`PrepareStartUser`/`StartUser` 建服务目录+挂载但不激活密钥，必须在 `ActiveUserKey` 之后）。

### 陷阱 2：卷状态非法跳转

从 MOUNTED 直接置 REMOVED → 内核可能仍持有引用，必须经过 EJECTING。

### 陷阱 3：Daemon 崩溃后 IPC 状态假设

Daemon 重启后内存状态为空，Manager 的缓存可能过期。必须重新同步状态，不要假设之前的 IPC 结果仍有效。

### 陷阱 4：锁屏时访问 EL4/EL5 数据

`LockUserScreen` 后 EL5 立即锁定、EL4 延迟 10s 锁定（已从内核移除）。此后访问 EL4/EL5 加密文件会失败，必须等 `UnlockUserScreen` 完成。EL2 不受锁屏影响，锁屏期间仍可访问。

### 陷阱 5：跨用户密钥操作

用 userId=100 的密钥操作 userId=105 的目录 → 密钥不匹配，数据解密失败。密钥和目录的 userId 必须一致。

### 陷阱 6：持有锁时执行 IPC

在持有 Manager 锁时调用 Daemon IPC → 可能双进程互锁。先释放锁再发起 IPC。

### 陷阱 7：磁盘源文件迁移时漏带编译宏（已实际发生）

磁盘 operator 从可执行文件迁入 `storage_disk_mgr_ext` SO 时，`EXTERNAL_STORAGE_QOS_TRANS` define 没有随迁 → QOS 产品上 exfat/ntfs 挂载静默退化为普通 ForkExec、丢失 `big_writes`。**任何把源文件移入新编译目标的改动，必须逐一核对原目标上的全部条件 defines/external_deps 是否需要随迁**（本产品 flag=false 不暴露，只在 QOS 产品回归）。

### 陷阱 8：DiskMgrApi 表三处不同步

新增磁盘函数只改了头文件表字段、漏改 `disk_mgr_api.cpp` 初始化列表或 Loader 封装 → 编译失败（字段数不匹配）或静默调用错函数（同类型字段错位）。表字段、初始化列表、Loader 封装、provider 调用点**四处必须一次改齐**（见 `architecture.md`）。

### 陷阱 9：加载失败用 std::call_once 锁死

dlopen 失败后 `call_once` 语义使 `Init()` 永不再执行 → 所有磁盘操作直至进程重启都返回 `E_NOT_SUPPORT`，重试逻辑形同虚设。加载失败必须走「冷却 1s 后下次调用自愈重试」（现实现 mutex + `lastAttemptTime_`）；同理，上层不要在收到一次 `E_NOT_SUPPORT` 后缓存「不可用」结论。

## 修改前检查

对照上述 9 个陷阱，当前改动是否触犯了任何一个？
