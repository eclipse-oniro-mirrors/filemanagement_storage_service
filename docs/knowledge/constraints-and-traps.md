# 约束与陷阱

本文只记录项目硬性约束和最易犯的 6 个陷阱。各场景详细约束见对应知识文档：用户时序见 `user-directory-lifecycle.md`，卷状态见 `disk-volume-lifecycle.md`，密钥见 `crypto-key-management.md`，IPC 死锁见 `ipc-interface-guide.md`。

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

## 修改前检查

对照上述 6 个陷阱，当前改动是否触犯了任何一个？
