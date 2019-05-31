# GOBANG FOR LINUX🌊

> 在Linux终端实现字符界面五子棋小游戏

---

大一上期在学习C语言的时候就萌生了在windows字符界面实现五子棋小游戏，所以便在大一上期实现了这个想法。不过因为初学C语言，所以用了很多奇奇怪怪的语法和思路。🙈

这次重构只改正了一点小BUG和添加了“A” “D” “W” “S” 实现的菜单选择。

---

这学期开始学习Linux，我就想起了我的五子棋小游戏。平时大家基本不会在windows下打开控制台，但使用Linux就必然要和支付界面打交道，那我何不将我的五子棋移植到Linux下来？

说干就干，首先把代码拷过去，意料之中的出现了一堆Error😂，观察发现Linux下并不支持_kbhit()和_getch()，于是经过漫长的查找终于将这两个功能在Linux下手动实现了

>_kbhit()

```c++
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
```

>_getch()

```c++
#include <termio.h>

int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;
  
    if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
        return -1;
    }
  
    tm_old = tm;
    cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
        return -1;
    }
  
    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
        return -1;
    }
    return ch;
}

```
---

还存在的问题是Linux下清屏函数不是windows下的

```c++
system("cls");
```

而是

```c++
system("clear");
```
