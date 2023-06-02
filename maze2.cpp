#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<stdbool.h>
#include<string.h>
//坐标结构体
typedef struct pation
{
    int row;//行
    int col;//列
}PT;


typedef PT STDataType;

typedef struct Stack
{
    STDataType* arr;//顺序表
    int capacity;//容量
    int top;//栈顶
}ST;


void StackInit(ST* ps);
void StackDestroy(ST* ps);

void StackPush(ST* ps, STDataType x);
void StackPop(ST* ps);

STDataType StackTop(ST* ps);
int StackSize(ST* ps);
bool StackEmpty(ST* ps);

const void checkcapacity(ST* ps)
{
    assert(ps);

    if (ps->top == ps->capacity)
    {
        ps->capacity *= 2;
        STDataType* tmp = (STDataType*)realloc(ps->arr, ps->capacity * sizeof(STDataType));
        if (tmp == NULL)
        {
            exit(-1);
        }
        ps->arr = tmp;
    }
}
void StackInit(ST* ps)
{
    assert(ps);

    ps->arr = (STDataType*)malloc(3 * sizeof(STDataType));
    if (ps->arr == NULL)
    {
        exit(-1);
    }
    ps->capacity = 3;
    ps->top = 0;//初始化时没有有效数据，这里记录下标为0处为栈顶
}
void StackDestroy(ST* ps)
{
    assert(ps);
    free(ps->arr);
    ps->arr = NULL;
    ps->top = 0;
    ps->capacity = 0;
}

void StackPush(ST* ps, STDataType x)
{
    assert(ps);
    checkcapacity(ps);
    ps->arr[ps->top] = x;
    ps->top++;
}
void StackPop(ST* ps)
{
    assert(ps);
    assert(ps->top > 0);//确保不是空栈 栈空了删除没意义
    ps->top--;
}

STDataType StackTop(ST* ps)
{
    assert(ps);
    assert(ps->top > 0);//确保不是空栈 栈空了找栈顶没意义

    return ps->arr[ps->top - 1];
}
int StackSize(ST* ps)
{
    assert(ps);
    return ps->top;
}
bool StackEmpty(ST* ps)
{
    assert(ps);
    return ps->top == 0;
}


ST path;//定义一个全局变量 路径

void printMaze(int** maze, int N, int M)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }
    printf("\n");

}
void PrintPath(ST* ps)
{
    //如果直接把栈的数据取出来，会发现是反的
    //所以我们需要额外的一个栈帮助我们倒数据再打印
    ST rpath;
    StackInit(&rpath);
    //倒数据
    while (!StackEmpty(&path))
    {
        StackPush(&rpath, StackTop(&path));
        StackPop(&path);
    }

    //打印倒完后的数据
    while (!StackEmpty(&rpath))
    {//按格式打印
        printf("(%d,%d)\n", StackTop(&rpath).row, StackTop(&rpath).col);
        StackPop(&rpath);
    }
    StackDestroy(&rpath);
}
bool IsPass(int** maze, int N, int M, PT pos)//封装一个函数来判断是此坐标是否为通路
{
    if (pos.row >= 0 && pos.row < N
        && pos.col >= 0 && pos.col < M
        && maze[pos.row][pos.col] == 0)//判断合法性以及是否为通路
    {
        return true;
    }
    else
        return false;

}
bool GetMazePath(int** maze, int N, int M, PT cur)
{
    StackPush(&path, cur);//存当前的坐标到栈里
    if (cur.row == N - 1 && cur.col == M - 1)
    {
        return true;
    }
    //探测上下左右方向的通路
    PT next;
    maze[cur.row][cur.col] = 2;//每走一个坐标 标记为2 表示已走过


    //上
    next = cur;
    next.row -= 1;
    if (IsPass(maze, N, M, next))//判断是否是通路
    {    //是通路就递归自己进入通路
        if (GetMazePath(maze, N, M, next))
            return true;//找到出口就不用继续探测了
    }

    //下
    next = cur;
    next.row += 1;
    if (IsPass(maze, N, M, next))//判断是否是通路
    {    //是通路就递归自己进入通路
        if (GetMazePath(maze, N, M, next))
            return true;//找到出口就不用继续探测了
    }

    //左
    next = cur;
    next.col -= 1;
    if (IsPass(maze, N, M, next))//判断是否是通路
    {    //是通路就递归自己进入通路
        if (GetMazePath(maze, N, M, next))
            return true;//找到出口就不用继续探测了
    }


    //右
    next = cur;
    next.col += 1;
    if (IsPass(maze, N, M, next))//判断是否是通路
    {    //是通路就递归自己进入通路
        if (GetMazePath(maze, N, M, next))
            return true;//找到出口就不用继续探测了
    }

    StackPop(&path);//如果是死路，回溯前要先把这个坐标删除
    return false;
}
int main()
{
    int N = 0, M = 0;
    while (scanf("%d%d", &N, &M) != EOF)//应对多个迷宫
    {
        //动态开辟一个二维数组储存迷宫
        //int maze[N][M];变长数组
        int** maze = (int**)malloc(sizeof(int*) * N);//创建指针数组-N行
        for (int i = 0; i < N; i++)
        {
            maze[i] = (int*)malloc(sizeof(int) * M);//创建指针数组-M列
        }
        //读取迷宫到 maze 里
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                scanf("%d", &maze[i][j]);
            }
        }

        //打印迷宫测试
        //printMaze(maze, N, M);
        StackInit(&path);//初始化栈

        PT entry = { 0,0 };//起点
        if (GetMazePath(maze, N, M, entry))
        {
            //printf("true\n");
            PrintPath(&path);//打印栈
        }
        else
            printf("false\n");

        StackDestroy(&path);//销毁栈

        //销毁迷宫
        for (int i = 0; i < N; i++)
        {//从内向外销毁，先销毁每一行，再销毁maze
            free(maze[i]);
        }
        free(maze);
    }

    return 0;
}
