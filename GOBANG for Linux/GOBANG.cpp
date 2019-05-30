#include<iostream>
#include<cstdlib>
#include <termio.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include<string>
using namespace std;

#define black 0;
#define white 1;

typedef int	LENGTH;//定义长度类型
typedef int LENIENT;//定义宽度类型
typedef int COORDINATE;
typedef int COLOR;

const LENGTH length = 20;
const LENIENT lenient = 15;//定义棋盘大小

static int Mtime = 0;//用于记录下棋次数从而确定执方以及存放棋子坐标数据

static COLOR win_color = 4;

COORDINATE x_coordinate;
COORDINATE y_coordinate;

struct mess//记录棋子位置数据
{
	COORDINATE x;
	COORDINATE y;
}chessmess[900] = { 0 };


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


void draw_checkerboard()//调入棋盘长宽数据绘制棋盘
{
	int k = 0;//用于输出棋盘前坐标数字
	cout << "--------------------------------------------------------" << endl;
	cout << endl;
	cout << endl;
	cout << " ┌ X ";
	for (int i = 1; i <= length; i++)
	{
		if (i <= 9)
		{
			k = i;
		}
		else
		{
			k = i % 10;
		}
		cout << k << ' ';
	}
	cout << endl;
	//
	cout << "  Y";
	cout << "┌ ";
	cout << "┬";
	for (int i = 1; i <= length - 1; i++)
	{
		cout << "─┬";
	}
	cout << "─┐";
	cout << endl;
	//第一排

	//
	for (int i = 1; i <= lenient; i++)
	{
		cout << "  ";
		if (i <= 9)
		{
			k = i;
		}
		else
		{
			k = i % 10;
		}
		cout << k;
		cout << "├";
		for (int k = 1; k <= length; k++)
		{
			cout << "─┼";
		}
		cout << "─┤";
		cout << endl;
	}
	//中间排

	//
	cout << "   ";
	cout << "└ ";
	cout << "┴";
	for (int i = 1; i <= length - 1; i++)
	{
		cout << "─┴";
	}
	cout << "─┘";
	cout << endl;
	//最后排
}

void draw_checkerboard_x()//重新绘制棋盘
{
	//因为每次都用DCX函数重绘历史棋子记录，故在函数内部归零
	int k = 0;//用于输出棋盘前的数字
	cout << "--------------------------------------------------------" << endl;
	cout << endl;
	cout << endl;
	cout << " ┌ X ";
	for (int i = 1; i <= length; i++)
	{
		if (i <= 9)
		{
			k = i;
		}
		else
		{
			k = i % 10;
		}
		cout << k << ' ';
	}
	cout << endl;
	//
	cout << "  Y";
	cout << "┌ ";
	cout << "┬";
	for (int i = 1; i <= length - 1; i++)
	{
		cout << "─┬";
	}
	cout << "─┐";
	cout << endl;
	//第一排

	//
	for (int i = 1; i <= lenient; i++)
	{
		int count3 = 0;//用于判断下方的循环（遍历棋子数据确认棋盘左侧是否有棋子从而影响棋盘输出）是否中途退出；
		cout << "  ";
		if (i <= 9)
		{
			k = i;
		}
		else
		{
			k = i % 10;
		}
		cout << k;
		for (int m = 0; m <= Mtime; m++)//遍历棋子数据确认棋盘左侧是否有棋子从而影响棋盘输出
		{
			if (chessmess[m].x == 1 && chessmess[m].y == i)
			{
				cout << "├ ";
				count3 = 1;
				break;
			}
		}
		if (count3 == 0)
		{
			cout << "├─";
		}
		for (int j = 1; j <= length; j++)
		{
			int count = 0;//用于判断下方的循环（遍历棋子数据确认是否输出棋子）是否中途退出
			int count2 = 0;//用于判断下方的循环（遍历棋子数据确认是否在下一步输出占位空格）是否中途退出
			int p;
			for (p = 0; p <= Mtime; p++)
			{
				if (chessmess[p].x == j && chessmess[p].y == i)
				{
					if (p % 2 == 0)
					{
						cout << "● ";
					}
					else
					{
						cout << "○ ";
					}
					count = 1;
					break;
				}
			}
			if (count == 0)
			{
				for (p = 0; p <= Mtime; p++)
				{
					if (chessmess[p].x == j + 1 && chessmess[p].y == i)
					{
						cout << "┼ ";
						count2 = 1;
						break;
					}
				}
				if (count2 == 0)
				{
					cout << "┼─";
				}
			}

		}
		cout << "┤";
		cout << endl;
	}
	//中间排

	//
	cout << "   ";
	cout << "└ ";
	cout << "┴";
	for (int i = 1; i <= length - 1; i++)
	{
		cout << "─┴";
	}
	cout << "─┘";
	cout << endl;
	//最后排
}

void chess()//绘制棋子
{
	int count = 0; //用于判断下方的循环（遍历棋子数据确认是否存在棋子）是否中途退出
	cout << endl;
	cout << "Please enter the coordinates：" << endl;
	cout << endl;


	/*cout << "Coordinate-X："; cin >> x_coordinate;
	cout << endl;
	cout << "Coordinate-Y："; cin >> y_coordinate;*/


	cout << "Coordinate-X：";
	char buffer_x[20];
	cin >> buffer_x;
	if((buffer_x[0]>=48&&buffer_x[0]<=50)&&(buffer_x[1]>=48&&buffer_x[1]<=57)&&buffer_x[2]=='\0')
	{
		x_coordinate = (buffer_x[0]-48) * 10 + buffer_x[1]-48;
	}
	else
	{
		if((buffer_x[0]>=48&&buffer_x[0]<=57)&&buffer_x[1]=='\0')
		{
			x_coordinate = buffer_x[0] - 48;
		}
		else
		{
			cout << endl;
			cout << endl;
			cout << "You entered the illegal data, please enter again" << endl;
			cout << endl;
			chess();
			return;
		}
	}
	cout << "Coordinate-Y：";
	char buffer_y[20];
	cin >> buffer_y;
	if((buffer_y[0]>=48&&buffer_y[0]<=50)&&(buffer_y[1]>=48&&buffer_y[1]<=57)&&buffer_y[2]=='\0')
	{
		y_coordinate = (buffer_y[0]-48) * 10 + (buffer_y[1]-48);
	}
	else
	{
		if((buffer_y[0]>=48&&buffer_y[0]<=57)&&buffer_y[1]=='\0')
		{
			y_coordinate = buffer_y[0] - 48;
		}
		else
		{
			cout << endl;
			cout << endl;
			cout << "You entered the illegal data, please enter again" << endl;
			cout << endl;
			chess();
			return;
		}
	}

	/*if (y_coordinate > lenient || x_coordinate > length)
	{
		cout << "You entered the illegal data, please enter again" << endl;
		cout << endl;
		chess();
		return;
	}*/
	for (int i = 0; i <= Mtime; i++)
	{
		if (x_coordinate == chessmess[i].x&&y_coordinate == chessmess[i].y)
		{
			break;
		}
		count++;
	}
	if (count != Mtime + 1)
	{
		cout << "您输入的棋子坐标处已存在棋子，不能重复录入，请重新输入" << endl;
		cout << endl;
		chess();
		return;
	}
	chessmess[Mtime].x = x_coordinate;
	chessmess[Mtime].y = y_coordinate;
	Mtime++;
	cout << endl;
	//system("cls");
	system("clear");
	draw_checkerboard_x();
}

int judge_x(COORDINATE x, COORDINATE y, COLOR color)
{
	int count = color;//用于判断下方的循环（遍历棋子数据确认棋盘指定位置上是否存在棋子）是否中途退出；
	bool isHaveQZ = false;
	for (int i = color; i < Mtime; i = i + 2)//遍历棋子数据确认棋盘指定位置上是否存在棋子
	{
		if (chessmess[i].x == x && chessmess[i].y == y)
		{
			isHaveQZ = true;
			break;
		}
		//count = count + 2;
	}
	if (!isHaveQZ)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int judge()
{
	int color = 4;//初始化颜色变量为非0 1；
	int count = 0;//用于判断下方的循环（遍历棋子数据确认是否五子连珠）是否中途退出；
	for (int i = 0; i < Mtime; i = i + 1)
	{
		if (i % 2 == 0)
		{
			color = black;
		}
		else
		{
			color = white;
		}
		if (
			judge_x(chessmess[i].x + 1, chessmess[i].y + 1, color)
			&& judge_x(chessmess[i].x + 2, chessmess[i].y + 2, color)
			&& judge_x(chessmess[i].x + 3, chessmess[i].y + 3, color)
			&& judge_x(chessmess[i].x + 4, chessmess[i].y + 4, color)
			)
		{
			win_color = color;
			return 0;

		}

		if (
			judge_x(chessmess[i].x - 1, chessmess[i].y + 1, color)
			&& judge_x(chessmess[i].x - 2, chessmess[i].y + 2, color)
			&& judge_x(chessmess[i].x - 3, chessmess[i].y + 3, color)
			&& judge_x(chessmess[i].x - 4, chessmess[i].y + 4, color)
			)
		{
			win_color = color;
			return 0;

		}

		if (
			judge_x(chessmess[i].x + 1, chessmess[i].y, color)
			&& judge_x(chessmess[i].x + 2, chessmess[i].y, color)
			&& judge_x(chessmess[i].x + 3, chessmess[i].y, color)
			&& judge_x(chessmess[i].x + 4, chessmess[i].y, color)
			)
		{
			win_color = color;
			return 0;

		}

		if (
			judge_x(chessmess[i].x, chessmess[i].y + 1, color)
			&& judge_x(chessmess[i].x, chessmess[i].y + 2, color)
			&& judge_x(chessmess[i].x, chessmess[i].y + 3, color)
			&& judge_x(chessmess[i].x, chessmess[i].y + 4, color)
			)
		{
			win_color = color;
			return 0;

		}
		count++;
	}
	if (count == Mtime + 1)
	{
		return 4;
	}
}

void tip()
{
    cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║温馨提示：                                                                    ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║    1、仅测试了Ubuntu和Deepin，其中Ubuntu和windows下Xshell均显示正常，        ║" << endl;
    cout << "║      Deepin默认终端会有一点点错位，还没想到兼容的解决方案。                  ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║    2、下棋时请先输入横坐标再输入纵坐标。并且棋盘只显示坐标的个位数字         ║" << endl;
    cout << "║      从左到右，从上到下坐标依次增加，请输入时自行添加十位数字。              ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║    3、输入错误时请不要按删除键。。没用，你就多按几个数字，因为我写了         ║" << endl;
    cout << "║      坐标数据保护的，所以如果输错了就输入一个大数就可以啦。                  ║" << endl;
    cout << "║                                                                              ║" << endl;                    cout << "║    4、游戏中请用‘W' 'D' 'A' 'D'代替上下左右控制菜单                          ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║    5、中途退出请按ALT+，重新进入游戏就再次输入./GOBANG                       ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║               为了看起来帅气，接下来的界面我就用English了哦✨                 ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "║                  Please press any key to continue...                         ║" << endl;
    cout << "║                                                                              ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
	char c = getch();
	system("clear");
	for (int i = 0; i <= 6;i++)
	{
		cout << endl;
	}
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "║      如果大家喜欢的话，欢迎到GitHub关注我，上给这个小项目点个star 🌟         ║" << endl;
	cout << "║               https://github.com/Mrliuxchn/Some-Terminal-Game                ║" << endl;
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
	char b = getch();
	system("clear");
}


void hello_A()
{
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                           THE MOST HANDSOME GOBANG                           ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                             ⚡ A、START PVP                                   ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                B、START PVE                                  ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                C、EXIT BRUTALLY                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                   COMPILER: LX and HJH       ║" << endl;
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
}

void hello_B()
{
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                           THE MOST HANDSOME GOBANG                           ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                A、START PVP                                  ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                             ⚡ B、START PVE                                   ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                C、EXIT BRUTALLY                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                   COMPILER: LX and HJH       ║" << endl;
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
}

void hello_C()
{
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                           THE MOST HANDSOME GOBANG                           ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                A、START PVP                                  ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                B、START PVE                                  ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                             ⚡ C、EXIT BRUTALLY                               ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                                              ║" << endl;
	cout << "║                                                   COMPILER: LX and HJH       ║" << endl;
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
}


void win(int n)
{
	for (int i = 1; i <= 3; i++)
	{
		cout << endl;
	}
	cout << "--------------------------------------------------------------------------------" << endl;
	cout << endl;
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << endl;
	cout << "║                                                                              ║" << endl;
	switch (n)
	{
	case 0: {cout << "║                          🔥  OH,PLAYER BLACK YOU ARE WIN!     😁               ║" << endl; break; }
	case 1: {cout << "║                          🔥  OH,PLAYER WHITE YOU ARE WIN!     😁               ║" << endl; break; }
	default:
		break;
	}
	cout << "║                                                                              ║" << endl;
	cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
	cout << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
}

int main()
{
    system("clear");
    tip();
    hello_A();
	COORDINATE choice = 1;//确立菜单选项位置;
	char choice_char;//存放用户键盘输入消息;
	while (1)//重绘循环;
	{
		switch (choice)//防止choice数据溢出范围;
		{
		case 4: {choice = 1; break; }
		case 0: {choice = 3; break; }
		default:
			break;
		}
		switch (choice)//根据菜单选项位置重绘菜单;
		{
		case 1: { system("clear");hello_A(); break; }
		case 2: { system("clear");hello_B(); break; }
		case 3: { system("clear");hello_C(); break; }
		default:
			break;
		}
		if (kbhit)//当按下键盘按键;
		{
			choice_char = getch();//无需确认的收集键盘消息;
			switch (choice_char)
			{
			case 'd':
			{
				switch (choice)
				{
				case 1: {
                    system("clear");
		            draw_checkerboard();
		            while (judge())
		            {
			            chess();
		            }
		            win(win_color);
                    char c = getch();
	                return 0;
                        }



				case 2: {
		            system("clear");
		            cout << "Sorry,this model haven't open now,You can choose again" << endl;
		            cout << "Please press any key to continue" << endl;
                    char c = getch();
		            system("clear");
		            hello_A();
                    break;
                        }



				case 3: {system("clear"); char c = getch();return 0;}
				default:
					break;
				}
				break;
			}
			case 'a':
			{


				switch (choice)
				{
				case 1: {
                    system("clear");
		            draw_checkerboard();
		            while (judge())
		            {
			            chess();
		            }
		            win(win_color);
                    char c = getch();
	                return 0;
                        }



				case 2: {
		            system("clear");
		            cout << "Sorry,this model haven't open now,You can choose again" << endl;
		            cout << "Please press any key to continue" << endl;
		            char c = getch();
		            system("clear");
		            hello_A();
                    break;
                        }



				case 3: {system("clear"); char c = getch();return 0;}
				default:
					break;
				}
				break;
			}
			case 'w': { choice--; break; }
			case 's': {choice++; break; }
			default:
				break;
			}
		}
	}
    return 0;
}
