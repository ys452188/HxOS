# HxOS - A Simple Hobby x86 Kernel

**HxOS** 是一个轻量级的 x86 操作系统内核，由 **cuso4IsVeryNice (Xianghan.Huang)** 开发。是一个用于学习操作系统原理的 Hobby OS，它实现了Multiboot引导兼容、基本的VGA文本显示、PS/2键盘驱动以及一个简陋的交互式Shell。

## ✨ 功能特性 (Features)

* **Multiboot 兼容**: 符合 Multiboot 1 规范，由 GRUB 直接引导。
* 
**VGA 驱动**: 支持 80x25 文本模式显存直接操作(MMIO 0xB8000) 。


* 
**键盘驱动**: 基于端口 I/O (0x60/0x64) 的 PS/2 键盘轮询 (Polling) 驱动 。


* **内置 Shell**:
* 支持命令解析与执行。
* 支持退格键 (Backspace) 删除字符 。


* 内置命令: `help`, `echo`, `clear` 。





## 🛠️ 开发环境 (Prerequisites)

* **编译器**: `clang` (目标架构: `i386-pc-none-elf`)
* **链接器**: `ld.lld`
* **构建工具**: `make` (可选) 或直接使用 Shell 脚本
* **ISO 制作**: `grub-mkrescue`, `xorriso`

**编译内核**:
运行提供的编译脚本生成 ELF 文件和 ISO 镜像。
```bash
sh 编译.sh

```


该脚本会执行以下步骤:
* 编译 `src/Boot.S` 为 `boot.o`
* 编译 `src/Kernel.c` 为 `kernel.o`
* 链接生成 `kernel.elf`
* 打包生成 `HxOS.iso`



## ⚙️ 系统启动流程 (Boot Process)

HxOS 的启动是一个从汇编到 C 语言的交接过程，具体流程如下：

### 1. BIOS/GRUB 阶段

* 计算机上电，BIOS 加载引导加载程序 (GRUB)。
* GRUB 读取 `product/boot/kernel.elf`，检测到 `Boot.S` 前端的 **Multiboot Header** (魔数 `0x1BADB002`) 。


* GRUB 将计算机置于 **32位保护模式**，并将控制权移交给 `linker.ld` 中定义的入口点 `_start` 。



### 2. 汇编引导阶段 (`Boot.S`)

* 
**入口**: CPU 跳转到 `_start` 标签 。


* **环境准备**:
* 设置内核栈指针 (`esp`)，为 C 语言函数调用提供栈空间。
* 保存 Multiboot 信息结构体的指针 (`ebx` 寄存器) 到 `multiboot_info_ptr` 变量 。




* **跳转**: 执行 `call kmain` 指令，正式进入 C 语言内核环境。

### 3. 内核初始化阶段 (`Kernel.c`)

* **进入 `kmain()**`:
* 调用 `vga_write` 打印欢迎标语 "HxOS - welcome!" 。


* 初始化 Shell 提示符 "HxOS=> "。



### 4. 主循环阶段 (The Loop)

* 内核进入无限 `while(1)` 循环。
* 
**轮询键盘**: 调用 `keyboard_getchar_poll()` 不断检查 `0x64` 端口状态，当有按键按下时从 `0x60` 端口读取扫描码并转换为 ASCII 字符 。


* 
**命令解析**: 读取用户输入到缓冲区，当检测到回车键时，解析命令并执行相应逻辑 (`help`, `clear`, `echo`) 。



## 📂 文件结构 (File Structure)

```text
.
├── src/
│   ├── Boot.S       # 内核汇编入口，包含 Multiboot 头
│   ├── Kernel.c     # 内核主函数，Shell 逻辑实现
│   ├── VGA.h        # 显卡文本模式驱动
│   ├── Keyboard.h   # 键盘轮询驱动及扫描码映射
│   └── Inout.h      # 端口 I/O 汇编内联函数
├── grub.cfg         # GRUB 引导配置文件
├── linker.ld        # 链接脚本，定义内存布局 (0x100000)
├── 编译.sh          # 自动构建脚本
└── README.md        # 项目说明文档

```

## 📝 待办事项 (To-Do)

* [ ] **中断处理 (IDT)**: 从键盘轮询 (Polling) 升级为中断驱动 (Interrupts)。
* [ ] **VGA 滚屏**: 实现屏幕写满后的自动滚屏逻辑。
* [ ] **内存管理**: 实现物理内存分配器 (PMM)。

---

**Happy Hacking!**