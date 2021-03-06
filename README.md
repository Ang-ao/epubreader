# epub 阅读器

一个基于 `C++/Qt` 的简易 `epub` 阅读器，仅支持 `epub` 格式文件。

---
已实现：
- 支持 `windows` 和 `linux` 系统，`mac os` 未测试
- 打开 `epub` 文件，解压至本地
- 读取目录并可根据目录跳转至相应位置
- 可通过右下角滑动条调整字体大小
- 可通过拖动文件至窗口打开（`v0.3.0`）

---
已知问题：
- 无法使用 `runOnRam` 模式，目前仅能将书解压至本地后读取（`v0.1.0` 以来）
- ~~打开一本书后再打开一本新书会崩溃（`v0.2.0` 更新后）~~ （`v0.2.2` 修复）
- ~~目录重名会导致打开文件路径被覆盖问题（`v0.2.2` 发现）~~ （`v0.2.3` 修复）
- 部分格式的 `epub` 文件无法打开
- 部分图书无法获取封面

---
TODO:
- 美化界面
- 支持书库功能，通过记录 json 文件实现
- 尽力修复 bug
- 咕

# 构建方式

```shell
git clone https://github.com/Ang-ao/epubreader.git
cd epubreader
cmake -B build
cmake --build build -j4
```