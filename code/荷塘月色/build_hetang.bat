@echo off
rem 关闭命令回显。
rem 如果没有这一行，执行脚本时，每一条命令都会先显示在窗口中。
rem 使用 @ 可以让当前这一条命令本身也不显示。

setlocal
rem 创建一个局部环境变量作用域。
rem 当前批处理脚本中对 PATH、变量等所做的修改，
rem 会在执行 endlocal 后恢复，不影响系统或外部命令行环境。


rem 使用 MinGW-w64 提供的 GCC 编译器编译 C 源文件。
rem
rem gcc
rem     调用 GNU C 编译器。
rem
rem hetang_yuese.c
rem     要编译的 C 语言源文件。
rem     要求该文件和当前 build_hetang.bat 位于同一目录，
rem     或者这里填写源文件的完整路径。
rem
rem -std=c11
rem     按照 C11 标准编译代码。
rem     可以使用 C11 中规定的语言特性。
rem
rem -O2
rem     启用二级编译优化。
rem     GCC 会在编译期间优化循环、表达式、函数调用等，
rem     通常能明显提高程序运行效率，同时不会过度增加编译时间。
rem
rem -Wall
rem     开启一组常见编译警告。
rem     例如未使用变量、可疑类型转换、函数声明问题等。
rem
rem -Wextra
rem     在 -Wall 的基础上开启更多警告。
rem     例如未使用参数、部分符号类型比较等。
rem
rem -mwindows
rem     将程序编译为 Windows GUI 子系统程序。
rem
rem     使用这个参数后：
rem       1. 程序运行时不会自动弹出黑色控制台窗口；
rem       2. 程序入口一般写成 WinMain 或 wWinMain；
rem       3. 链接器会把程序标记为 Windows 图形界面程序。
rem
rem     如果源代码入口是普通的 main()，通常不要使用 -mwindows，
rem     或者改用 -mconsole。
rem
rem -lgdi32
rem     链接 Windows 的 GDI 图形库 gdi32。
rem
rem     GDI 用于：
rem       - 绘制直线、圆、椭圆和矩形
rem       - 显示文字
rem       - 创建画笔和画刷
rem       - 位图绘制
rem       - 双缓冲绘图
rem
rem     如果程序调用了 Ellipse、LineTo、TextOut、BitBlt、
rem     CreatePen、CreateSolidBrush 等函数，就需要链接 gdi32。
rem
rem -o hetang_yuese.exe
rem     指定最终生成的可执行文件名称。
rem     如果没有 -o，GCC 默认可能生成 a.exe。

gcc hetang_yuese.c -std=c11 -O2 -Wall -Wextra -mwindows -lgdi32 -o hetang_yuese.exe


rem errorlevel 保存上一条命令的退出状态。
rem
rem GCC 编译成功时通常返回 0；
rem 编译失败或链接失败时返回非 0。
rem
rem if errorlevel 1 表示：
rem 如果返回值大于或等于 1，就进入错误处理代码。

if errorlevel 1 (
    echo.
    rem 输出一个空行，方便阅读错误信息。

    echo Build failed. Make sure MinGW-w64 GCC is installed and gcc is in PATH.
    rem 提示用户检查：
    rem   1. 是否安装了 MinGW-w64 GCC；
    rem   2. gcc.exe 所在目录是否已经加入 PATH；
    rem   3. C 源代码本身是否存在编译错误。

    pause
    rem 暂停批处理窗口，等待用户按任意键。
    rem 防止用户双击运行脚本时，错误窗口一闪而过。

    exit /b 1
    rem 结束当前批处理脚本，并向外部返回错误码 1。
)


echo Build succeeded: hetang_yuese.exe
rem 只有编译成功后才会执行到这里。
rem 显示生成的 EXE 文件名称。


start "" hetang_yuese.exe
rem 启动刚刚生成的 Windows 程序。
rem
rem start 命令的第一个带引号参数会被当作窗口标题，
rem 所以这里使用空字符串 "" 作为窗口标题。
rem
rem 如果直接写：
rem     start "hetang_yuese.exe"
rem Windows 可能把它当成窗口标题，而不是程序路径。


endlocal
rem 结束 setlocal 创建的局部环境。
rem 恢复脚本执行前的环境变量状态。