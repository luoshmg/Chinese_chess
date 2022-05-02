// Title: Chinese chess
// Author: luoshmg
// Date: 2022-04

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
// 棋盘大小
#define ROW 10
#define COL 9
// 每一步命令长度都是4
#define CMD_LEN 4
// 没有棋子的地方用+当作棋盘交叉线
#define EMPTY '+'

static char *pieces[] = {"ABCDEFG", "abcdefg"};   // 棋子：红方0（帥仕相傌俥炮兵），黑方1（將士象馬車砲卒）
static char *mov_tys = "art";                     // 棋子移动方式：进退平
static char *positions = "fb";                    // 同类棋子在同一列的位置：前后
static char *surrend = "/fff";                    // 投降命令

void print_chessboard(char (*chessboard)[COL]);
int cmd_exist(int side, char *command);
int legal_mv(int side, char *command, int ty, char (*chessboard)[COL]);
int test_win(char (*chessboard)[COL]);
int inside_9_grid(int row_to, int col_to, int side);
int pass_river(int row_to, int side);

int main(void)
{
    int step = 0;   // 统计下了几步了
    int side;       // 红方0黑方1
    char (*commands)[CMD_LEN+1];          // 记录每一步棋的指令，0开始
    char temp[CMD_LEN+1];
    commands = &temp;
    char command_modify[CMD_LEN+1];
    int ty;         // 走法类：1表示没有多个棋在同一列的情况，2表示有
    
    // 初始化棋盘，假设身在红方(大写字母)
    int my_side = 0;
    char chessboard[ROW][COL] = 
    {
        {'e','d','c','b','a','b','c','d','e'},
        {'+','+','+','+','+','+','+','+','+'},
        {'+','f','+','+','+','+','+','f','+'},
        {'g','+','g','+','g','+','g','+','g'},
        {'+','+','+','+','+','+','+','+','+'},
        {'+','+','+','+','+','+','+','+','+'},
        {'G','+','G','+','G','+','G','+','G'},
        {'+','F','+','+','+','+','+','F','+'},
        {'+','+','+','+','+','+','+','+','+'},
        {'E','D','C','B','A','B','C','D','E'},
    };
    printf("GAME BEGINE\n");
    print_chessboard(chessboard);   // ok
    printf("\n");

    // 下棋
    // 1. 走法为:
        // 一类：F2t5（表示炮二平五）、D2a3（表示傌二进三）
        // 二类：bft2（表示后砲平二）、fdr5（表示前馬退五）
    // 2. 结束方式为：一方投降、將帥少了一个
    while (1)
    {
        side = step%2;

        printf("%s's turn: ", side==0?"RED":"BLACK");      // 红方先走
        scanf("%s", *commands);
        (*commands)[CMD_LEN] = '\0'; //只看命令的前4个字符
        // 投降
        if (strcmp(*commands, surrend) == 0)
        {
            printf("GAME OVER: %d step%s in total, %s win.\n", step, step==1?"":"s", side==0?"BLACK":"RED");
            break;
        }
        // 要求指令存在，ty表示走法类（一类1，二类2）
        ty = cmd_exist(side, *commands);
        if (ty)
        {
            // *commands需要把字符转为整数才能正常使用，'1'不等于1
            command_modify[0] = (*commands)[0];
            command_modify[1] = (ty==1? (*commands)[1]-'0' : (*commands)[1]);
            command_modify[2] = (*commands)[2];
            command_modify[3] = (*commands)[3] - '0';
            // 更新棋盘
            if (legal_mv(side, command_modify, ty, chessboard))
            {
                // 將帥都还在则继续
                if (test_win(chessboard) == -1)
                {
                    // 打印棋盘
                    print_chessboard(chessboard);
                    commands++;
                    step++;
                    printf("TOTAL STEP%s: %d\n", step==1?"":"S", step);
                    printf("\n");
                }
                else
                {
                    printf("GAME OVER: %d steps in total, %s win\n", step, side==0?"RED":"BLACK");
                    break;
                }
            }
        }
        else
        {
            printf("no such command\n");
        }
    }
    
    return 0;
}

// 打印棋盘
void print_chessboard(char (*chessboard)[COL])
{
    int row, col;

    // *p就是一行
    for (row=0; row<ROW; row++)
    {
        for (col=0; col<COL; col++)
        {
            printf("%c", chessboard[row][col]);
            if (col<COL-1)
            {
                printf("-");
            }
        }
        printf("\n");
    }

    return ;
}

// 判断一方输入的命令格式是否正确
// 不正确则返回0，正确则返回走棋类1，2（有相同棋子在同一列则为2）
int cmd_exist(int side, char *command)
{
    int exist = 0;

    // 第3、4个字符必须是移动方式和移动量，移动量要求>=1且<=9，如平五
    if (strchr(mov_tys, command[2]) && command[3]>='1' && command[3]<='9')
    {
        // 当同类棋子在不同列时，如炮二平五、砲二
        if (command[1]>='1' && command[1]<='9')
        {
            if (strchr(pieces[side], command[0]))
            {
                exist = 1;
            }
        }
        // 当同类棋子在同一列时，如后炮退二、后砲
        else if(strchr(positions, command[0]))
        {
            if (strchr(pieces[side], command[1]))
            {
                exist = 2;
            }   
        }
    }

    return exist;
}

// 找到赢方，0红方，1黑方，-1还没有
int test_win(char (*chessboard)[COL])
{
    int row, col;
    int A_amt = 0;
    int a_amt = 0;

    for (col=0; col<COL; col++)
    {
        for (row=0; row<ROW; row++)
        {
            if (chessboard[row][col] == 'A')
            {
                A_amt++;
            }
            else if (chessboard[row][col] == 'a')
            {
                a_amt++;
            } 
        }
    }
    if (a_amt==0)
    {
        return 0;
    }
    else if (A_amt==0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// 如果这一步可以走，则更新棋盘并返回1，否则提醒出错并返回0
int legal_mv(int side, char *command, int ty, char (*chessboard)[COL])
{
    // 1. 需要确定：
    // (1) 什么棋； (2) 怎么走； (3) 从哪里； (4) 到哪里
    char piece;
    char mv_ty = command[2];    //确定(2)怎么走
    int to_or_delta = command[3];
    int col_in = -1;
    int row_in = -1;
    int col_to;
    int row_to;
    int a_or_r; //进退，1进，-1退
    int f_or_b; //前后，1前，-1后
    int row, col;
    int amt;
    // 2. 什么需要考虑红黑方？
    // (2.1)直接给的col_in, (2.2)f_or_b, (2.3)a_or_r, (2.4)过河, (2.5)九宫格, 
    // (2.6)直接给的col_to

    // 确定(1)什么棋
    // 确定(3)从哪里col_in和row_in
    if (ty==1) //只要是一列没有重复，则col_in确定。如炮二..，傌二..
    {
        piece = command[0];
        col_in = (side==0? COL-command[1] : command[1]-1);   // (2.1)直接给的col_in受到红黑方影响（站在红方视角）
        // 确定row_in
        for (row=0; row<ROW; row++)
        {
            if (chessboard[row][col_in] == piece)
            {
                row_in = row;
            }
        }
    }
    else //一列有重复的，则找col_in。如后炮..，前傌..
    {
        if (side==0) // (2.2)f_or_b受到红黑方影响（站在红方视角）
        {
            f_or_b = (command[0]=='f'? 1 : -1);
        }
        else
        {
            f_or_b = (command[0]=='b'? 1 : -1);
        }
        piece = command[1];
        // 找piece的col_in
        amt=0;
        for (col=0; col<COL; col++)
        {
            for (row=0; row<ROW; row++)
            {
                if (chessboard[row][col] == piece)
                {
                    col_in = col;
                    amt++;
                }
            }
        }
        if (amt!=2)
        {
            printf("no 2 identical pieces %c in the col\n", piece);
            return 0;
        }
        // 确定row_in
        if (f_or_b == 1) //前
        {
            for (row=0; row<ROW; row++)
            {
                if (chessboard[row][col_in] == piece)
                {
                    row_in = row;
                }
            }
        }
        else //后
        {
            for (row=ROW-1; row>=0; row--)
            {
                if (chessboard[row][col_in] == piece)
                {
                    row_in = row;
                }
            }
        }
    }
    // 确定(4)到哪里，并且更新棋盘
    if (mv_ty=='t') //只要是平，则col_to确定
    {   
        // 先确定col_to和row_to
        col_to = (side==0? COL-to_or_delta : to_or_delta-1);    // (2.6)直接给的col_to受到红黑方的影响（站在红方视角）
        row_to = row_in;

        switch(piece)
        {
        case 'A': //帥
        case 'a':
            // 能走就更新棋盘，不能占领己方的棋，不能走出九宫阁，不能走太远
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
                inside_9_grid(row_to, col_to, side) &&
                abs(col_to-col_in)==1)
                {
                    chessboard[row_to][col_to] = piece;
                    chessboard[row_in][col_in] = EMPTY;
                    return 1;
                }
            else
                {
                    printf("can't move %c to that col\n", side==0? piece : tolower(piece));
                    return 0;
                }
            break;
        case 'E': //俥
        case 'e':
            amt = 0;
            // 不能跨越棋子
            if (col_to > col_in)
            {
                for (col=col_in+1; col<col_to; col++)
                {
                    if (chessboard[row_in][col] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else if (col_to < col_in)
            {
                for (col=col_in-1; col>col_to; col--)
                {
                    if (chessboard[row_in][col] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else
            {
                printf("can't state unmoved %c\n", piece);
                return 0;
            }
            if (amt > 0)
            {
                printf("cam't move %c to that col\n", piece);
            }
            // 能走就更新棋盘，不能占领己方的棋，不能走出棋盘
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            col_to>=0 && col_to<COL)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'F': //炮
        case 'f':
            amt = 0;
            // 不跨越棋子或者刚好跨越一个棋子占领敌方棋子
            if (col_to > col_in)
            {
                for (col=col_in+1; col<col_to; col++)
                {
                    if (chessboard[row_in][col] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else if (col_to < col_in)
            {
                for (col=col_in-1; col>col_to; col--)
                {
                    if (chessboard[row_in][col] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else
            {
                printf("can't state unmoved %c\n", piece);
                return 0;
            }
            if (amt > 1)
            {
                printf("cam't move %c to that col\n", piece);
            }
            // 能走就更新棋盘，不能占领己方的棋，不能走出棋盘
            if (amt==0 && chessboard[row_to][col_to]==EMPTY &&
            col_to>=0 && col_to<COL)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else if (amt==1 && strchr(pieces[1-side],chessboard[row_to][col_to]) && 
            col_to>=0 && col_to<COL)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'G': //兵
        case 'g':
            // 能走就更新棋盘，不能占领己方的棋，不能走出棋盘，不能走太远，没过河不能平
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            col_to>=0 && col_to<COL && abs(col_to-col_in)==1 && 
            pass_river(row_to, side))
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        default:
            printf("piece %c cann't move horizontally\n", piece);
            return 0;
            break;
        }
    }
    else if (mv_ty=='a' || mv_ty=='r') //不是平就确定进退
    {
        // (2.3)a_or_r受到红黑方影响（站在红方视角）
        if (side==0)
        {
            a_or_r = (mv_ty=='a'? 1 : -1);
        }
        else
        {
            a_or_r = (mv_ty=='b'? 1 : -1);
        }

        switch(piece)
        {
        case 'A': //帥
        case 'a':
            col_to = col_in;
            row_to = row_in - a_or_r * to_or_delta;
            // 能走就更新棋盘，不能占领己方的棋，不能走出九宫阁，不能走太远
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            inside_9_grid(row_to, col_to, side) && abs(row_to-row_in)==1)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'B': //仕
        case 'b':
            col_to = (side==0? COL-to_or_delta : to_or_delta-1);    // (2.6)直接给的col_to受到红黑方的影响（站在红方视角）
            row_to = row_in - a_or_r;
            // 能走就更新棋盘，不能占领己方的棋，不能走出九宫阁，只有特定的棋位能走
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            inside_9_grid(row_to, col_to, side))
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row and col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'C': //相
        case 'c':
            col_to = (side==0? COL-to_or_delta : to_or_delta-1);    // (2.6)直接给的col_to受到红黑方的影响（站在红方视角）
            row_to = row_in - a_or_r*2;
            // 能走就更新棋盘，不能占领己方的棋，不能过河，只有特定的棋位能走
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            !pass_river(row_to, side) && 
            (col_to==0 || col_to==2 || col_to==4 || col_to==6 || col_to==8) &&
            chessboard[(row_in+row_to)/2][(col_in+col_to)/2]==EMPTY) //没有堵象眼
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row and col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'D': //傌
        case 'd':
            col_to = (side==0? COL-to_or_delta : to_or_delta-1);    // (2.6)直接给的col_to受到红黑方的影响（站在红方视角）
            // 没有蹩马腿，不能走太远
            if (abs(col_to-col_in)<1 || abs(col_to-col_in)>2)
            {
                printf("can't move %c to that col\n", side==0? piece : tolower(piece));
                return 0;
            }
            else if (abs(col_to-col_in)==1 && chessboard[row_in-a_or_r][col_in] != EMPTY ||
            abs(col_to-col_in)==2 && chessboard[row_in][col_in+(col_to-col_in)/2] != EMPTY)
            {
                printf("can't move %c to that row and col\n", side==0? piece : tolower(piece));
                return 0;
            }
            else
            {
                row_to = row_in - a_or_r * (3-abs(col_to-col_in));
            }
            // 能走就更新棋盘，不能占领己方的棋，不能走出棋盘
            if (!strchr(pieces[side],chessboard[row_to][col_to]) &&
            row_to>=0 && row_to<ROW &&
            col_to>=0 && col_to<COL)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row and col\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'E': //俥
        case 'e':
            amt = 0;
            col_to = col_in;
            row_to = row_in - a_or_r * to_or_delta;
            // 不能跨越棋子
            if (row_to > row_in)
            {
                for (row=row_in+1; row<row_to; row++)
                {
                    if (chessboard[row][col_in] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else if (row_to < row_in)
            {
                for (row=row_in-1; row>row_to; row--)
                {
                    if (chessboard[row][col_in] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else
            {
                printf("can't state unmoved %c\n", piece);
                return 0;
            }
            if (amt > 0)
            {
                printf("cam't move %c to that row\n", piece);
            }
            // 能走就更新棋盘，不能占领己方棋子，不能走出棋盘
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            row_to>=0 && row_to<ROW)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'F': //炮
        case 'f':
            amt = 0;
            col_to = col_in;
            row_to = row_in - a_or_r * to_or_delta;
            // 不跨越棋子或者跨越一个一个棋子
            if (row_to > row_in)
            {
                for (row=row_in+1; row<row_to; row++)
                {
                    if (chessboard[row][col_in] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else if (row_to < row_in)
            {
                for (row=row_in-1; row>row_to; row--)
                {
                    if (chessboard[row][col_in] != EMPTY)
                    {
                        amt++;
                    }
                }
            }
            else
            {
                printf("can't state unmoved %c\n", piece);
                return 0;
            }
            if (amt > 1)
            {
                printf("cam't move %c to that row\n", piece);
            }
            // 能走就更新棋盘，不能占领己方棋子，不能走出棋盘
            if (amt==0 && chessboard[row_to][col_to]==EMPTY &&
            row_to>=0 && row_to<ROW)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else if (amt==1 && strchr(pieces[1-side],chessboard[row_to][col_to]) && 
            row_to>=0 && row_to<ROW)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        case 'G': //兵
        case 'g':
            col_to = col_in;
            row_to = row_in - a_or_r*to_or_delta;
            // 不能后退
            if (mv_ty == 'r')
            {
                printf("piece %c can't retreat\n", piece);
                return 0;
            }
            // 能走就更新棋盘，不能占领己方棋子，不能走出棋盘，不能走太远
            if (!strchr(pieces[side],chessboard[row_to][col_to]) && 
            row_to>=0 && row_to<ROW && abs(row_to-row_in)==1)
            {
                chessboard[row_to][col_to] = piece;
                chessboard[row_in][col_in] = EMPTY;
                return 1;
            }
            else
            {
                printf("can't move %c to that row\n", side==0? piece : tolower(piece));
                return 0;
            }
            break;
        default:
            printf("piece %c cann't move horizontally\n", side==0? piece : tolower(piece));
            return 0;
            break;
        }
    }
    else
    {
        printf("piece %c doesn't have such mv_ty", side==0? piece : tolower(piece));
        return 0;
    }
}

// 判断row_to, col_to是否在side方的九宫格内，是返回1，否返回0
int inside_9_grid(int row_to, int col_to, int side)
{
    if (col_to>=COL/2-1 && col_to<=COL/2+1 && 
    (side==0 && row_to>=ROW-3 && row_to<ROW ||
    side==1 && row_to>=0 && row_to<3))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// 判断side方走的row_to是否过河，是返回1，否返回0
int pass_river(int row_to, int side)
{
    if (side==0 && row_to>=0 && row_to<ROW/2 ||
    side==1 && row_to>ROW/2 && row_to<ROW)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}