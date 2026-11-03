# 枚举与状态机约束

本文只记录状态转换的非法路径和常见枚举陷阱。卷操作详细约束见 `disk-volume-lifecycle.md`，密钥等级约束见 `crypto-key-management.md`。

## 非法状态转换（禁止）

| 跳转 | 为什么禁止 | 正确路径 |
|------|------------|----------|
| MOUNTED → REMOVED | 绕过 EJECTING，内核可能仍持有引用 | MOUNTED → EJECTING → REMOVED |
| UNMOUNTED → MOUNTED | 绕过文件系统检查 | UNMOUNTED → CHECKING → MOUNTED |
| CHECKING → REMOVED | 检查中不可直接移除 | CHECKING → MOUNTED 或 DAMAGED |
| DAMAGED → MOUNTED | 损坏卷不能直接挂载 | DAMAGED → TryToFix → DAMAGED_MOUNTED |
| BAD_REMOVAL → MOUNTED | 未正常弹出不可直接挂载 | BAD_REMOVAL → REMOVED → UNMOUNTED → CHECKING → MOUNTED |

## KeyType 常见陷阱

| 陷阱 | 说明 |
|------|------|
| EL2 当 EL1 用 | EL2 不随锁屏移除（锁屏移除的是 EL4/EL5），不可当作设备级密钥 |
| 跨等级激活 | EL2 未激活时不可激活 EL3/EL4，链式依赖 |
| flags 与 KeyType 混淆 | `CRYPTO_FLAG_EL2 = 2` 是位掩码，`EL2_KEY = 2` 是枚举值，语义不同 |

## DiskType 注意事项

- `DiskType` 是 `DiskInfo` 类的内嵌 `enum`（非 `enum class`），引用时用 `DiskInfo::SD_CARD`，不是独立 `SD_CARD`。当前取值：`SD_CARD=1`、`USB_FLASH=2`、`CD_DVD_BD=3`、`DATA_DISK_SSD=4`、`DATA_DISK_HDD=5`、`UNKNOWN_DISK_TYPE=255`（无 `MTP_PTP`）。
- 卷状态使用命名空间作用域的 `VolumeState::MOUNTED` 等，不是 `DiskInfo::MOUNTED`（`DiskInfo` 只内嵌 `DiskType`，无 `DiskState` 枚举）。当前 `VolumeState` 取值：`UNMOUNTED`、`CHECKING`、`MOUNTED`、`EJECTING`、`REMOVED`、`BAD_REMOVAL`、`DAMAGED`、`FUSE_REMOVED`、`DAMAGED_MOUNTED`（无 `ENCRYPTING`）。

## 加密标志位

`CRYPTO_FLAG_EL1=1, EL2=2, EL3=4, EL4=8, EL5=16` 是位掩码，组合使用 `|`（如 `CRYPTO_FLAG_EL1 | CRYPTO_FLAG_EL2`）。不要用加法代替位或运算。

## 修改前检查

- 状态转换是否经过合法路径？
- 枚举引用是否使用了正确的类作用域？
- 位掩码组合是否用了 `|` 而非 `+`？

## 测试指引

- 状态转换：`VolumeState` 跳转实现在 disk_manager SA（跨组件），本仓库通过 `storage_manager_provider_test`、`storagemanagerstub_fuzzer` 覆盖对外接口；非法跳转拒绝必须覆盖
- 枚举引用：对应模块单元测试
