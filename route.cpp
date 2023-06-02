
#define _CRT_SECURE_NO_WARNINGS
#include<ctime>
#include<cmath>
#include<cstdio>
#include <fstream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>
#include<process.h>
#include<iostream>
#include<vector>
using namespace std;
using std::vector;
//std::vector;

//函数名，变量名、结构名、
#define INF 10000000
#define NameLength 60
#define CityNum 12//城市：北京、上海、广州、深圳、天津、成都、重庆、武汉、沈阳、杭州、西安、南京
//高开销:北京、武汉、天津		中开销：上海、广州、深圳、沈阳		低开销：南京、成都、重庆、杭州、西安
#define LowCost 0.2
#define MidCost 0.5
#define HighCost 0.9
#define CarCost 2
#define TrainCost 5
#define PlaneCost 9



//typedef enum city_list{北京, 天津, 武汉, 上海, 广州, 深圳, 沈阳, 南京, 成都, 重庆, 杭州, 西安};
enum city_name { Beijing, Tianjin, Wuhan, Shanghai, Guangzhou, Shenzhen, Shenyang, Nanjing, Chengdu, Chongqing, Hangzhou, Xian, None };//用来将中文转换为对应拼音字符，防止中文出现奇怪的bug
enum city_cost { high, mid, low };
enum user_status { waiting, travelling, arrived };
//enum city;


typedef struct user_requirement {
	char leaveLoc[NameLength];
	char arriveLoc[NameLength];
	char leaveDate[11];
	char leaveTime[5];
	char arriveDate[11];
	char arriveTime[5];
	int mode;
	int userID;

};//表示用户的起点、终点、出发日期、出发时间、到达日期、到达时间的需求

typedef struct time_table {
	char startLoc[NameLength];
	char endLoc[NameLength];
	char transportType[NameLength];
	char startDate[11];
	char startTime[6];
	char endDate[11];
	char endTime[6];
	float travelTime;//交通工具所花费的时间,单位是小时
	float transportCost;//根据交通工具算出的开销值

	city_name sourceName;
	city_name destName;//做一下类型转化，防止汉字可能出bug
	city_cost sourceCost;
	city_cost destCost;

	int transportID;

	struct time_table* nextptr;//时刻表用链表存储
};//交通工具的时刻表
time_table* mainList = NULL;

typedef struct condition {//旅客状态结构
	user_status nowStatus;//当前状态
	city_name nowCity;//当前处在城市(若不在交通工具上)
	char nowTransport[NameLength];//当前乘坐的交通工具
	char nowDate[11];
	char nowTime[5];
	int transportID;
	int userID;
};


char MaxDate[11] = { "2099-12-31" };
char MaxTime[6] = { "23:59" };

char nowDate[11];
char nowTime[6];

int timeTableNum = 0;
int visit[CityNum] = { 0 };
time_table findPath[CityNum + 1] = { 0 };//因为拒绝环路，所以最长路径长度不会超过CityNum
int routeNum = 0;
int stackTop = 0;
time_table route[CityNum + 1];
float minCost = 10000000;//最小开销值，初始为极大值



user_requirement user_input();
void get_time_table();//从文件中读取时刻表信息
float get_time_diff(char startDate[], char endDate[], char startTime[], char endTime[]);//计算时间差
int get_day_diff(int year, int month, int day);
int one_year_day(int n);
int every_year_day(int year1, int year2);
void get_city_type(char name[], city_name& CN, city_cost& CR);//中文转为city_name
float get_transport_cost(char type[], float diff, city_cost CR);//计算时刻表的对应的开销值
void delete_space();


void get_route(int mode, char leaveLoc[], char arriveLoc[], char leaveDate[], char leaveTime[], char arriveDate[], char arriveTime[]);
float get_wait_cost(city_cost stopCity, float waitTime);
void search(time_table* p, time_table* prePtr, city_name sourceCN, city_name destCN, int mode, char limitDate[], char limitTime[], float currCost, float currTime, vector<time_table>& usingPath);


int main() {
	get_time_table();

	int userNum = 0;
	user_requirement* inputArray;
	cout << "请输入需要查询的旅客数目：";
	cin >> userNum;
	cout << endl;
	inputArray = (user_requirement*)malloc(sizeof(user_requirement) * userNum);

	for (int i = 0; i < userNum; i++) {
		cout << "现在输入第" << i + 1 << "位旅客的旅行要求:" << endl;
		cout << "请输入出行模式、出发地点、目的地点、出发日期、出发时间:";
		inputArray[i] = user_input();

        // printf("where are you??\n");
        // cout<<"hey!!!"<<endl;


        // cout<<inputArray[i].leaveLoc<<' '<<inputArray[i].leaveDate<<' '
        // <<inputArray[i].leaveTime<<' '<<inputArray[i].arriveLoc<<'\n';

		get_route(inputArray[i].mode, inputArray[i].leaveLoc, inputArray[i].arriveLoc, inputArray[i].leaveDate, inputArray[i].leaveTime, inputArray[i].arriveDate, inputArray[i].arriveTime);
	}

	delete_space();

	system("pause");

	return 0;
}



user_requirement user_input() {
    int mode;
    user_requirement userPlan;
    //scanf("%d", &userPlan.mode);
    cin >> mode;
    //userPlan.mode = userPlan.mode;
    //scanf("%s %s", userPlan.leaveLoc, userPlan.arriveLoc);
    scanf("%s %s %s %s", userPlan.leaveLoc, userPlan.arriveLoc, userPlan.leaveDate, userPlan.leaveTime);//之后这里要更换为当前的日期和时间
    if (mode == 0) {//如果是无限时最低开销策略，就不用管日期和时间了，只要有出发地和目的地就行了
        userPlan.mode = 0;
        return userPlan;
    }
    cout << "请出入到达日期、到达时间：";
    //cout << "请输入出发日期、出发时间、到达日期、到达时间：";
    //scanf("%s %s %s %s", userPlan.leaveDate, userPlan.leaveTime, userPlan.arriveDate, userPlan.arriveTime);
    scanf("%s %s", userPlan.arriveDate, userPlan.arriveTime);
    userPlan.mode = 1;
    return userPlan;
}



void get_time_table() {
    time_table* p = NULL, * currPtr = NULL;
    FILE* fptr = NULL;
    char ch[NameLength];
    //char ch;
    //errno_t err;
    char filename[NameLength] = "TimeTable.txt";


    //err = fopen_s(&fptr, filename, "r");
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Error! You can't open the TimeTable.txt.\n");
        return;
    }

    while (fscanf(fptr, "%s", ch) != EOF) {//用!feof(fptr)有问题，会多读一行

        p = (time_table*)malloc(sizeof(time_table));

        strcpy(p->startLoc, ch);
        //fscanf(fptr, "%s", p->startLoc);
        fscanf(fptr, "%s", p->endLoc);
        fscanf(fptr, "%s", p->transportType);
        fscanf(fptr, "%s", p->startDate);
        fscanf(fptr, "%s", p->startTime);
        fscanf(fptr, "%s", p->endDate);
        fscanf(fptr, "%s", p->endTime);
        p->transportID = ++timeTableNum;
        p->nextptr = NULL;

        get_city_type(p->startLoc, p->sourceName, p->sourceCost);
        get_city_type(p->endLoc, p->destName, p->destCost);

        p->travelTime = get_time_diff(p->startDate, p->endDate, p->startTime, p->endTime);

        p->transportCost = get_transport_cost(p->transportType, p->travelTime, p->sourceCost);

        // printf("%s %s %s %s %s %s %s %.2f %.2f %d\n", p->startLoc, p->endLoc, p->transportType, p->startDate, p->startTime, p->endDate, p->endTime, p->travelTime, p->transportCost, p->transportID);
        //get_transport_cost(p);

        if (mainList == NULL) {
            mainList = p;
            currPtr = p;
            p = NULL;
        }
        else {
            currPtr->nextptr = p;
            currPtr = p;
            p = NULL;
        }
    }
    fclose(fptr);
}

float get_time_diff(char startDate[], char endDate[], char startTime[], char endTime[]) {//Date格式：yyyy-mm-dd， Time格式：hh:mm
    //char loc1[NameLength], loc2[NameLength];
    char year1[5], year2[5];
    char month1[3], month2[3];
    char day1[3], day2[3];
    char hour1[3], hour2[3];
    char minute1[3], minute2[3];//字符串转数字atoi函数要有'\0'才能正常

    //int m1 = 0, m2 = 0, d1 = 0, d2 = 0, h1 = 0, h2 = 0, min1 = 0, min2 = 0;
    //int monthFlag = 0, dayFlag = 0, hourFlag = 0, minuteFlag = 0;//记录是否有跨的
    int yearDiff = 0, monthDiff = 0, dayDiff = 0, minuteDiff = 0, hourDiff = 0;
    float totalDiff = 0;

    strncpy_s(year1, startDate, 4);
    strncpy_s(month1, startDate + 5, 2);
    strncpy_s(day1, startDate + 8, 2);

    strncpy_s(year2, startDate, 4);
    strncpy_s(month2, endDate + 5, 2);
    strncpy_s(day2, endDate + 8, 2);

    strncpy_s(hour1, startTime, 2);
    strncpy_s(minute1, startTime + 3, 2);
    strncpy_s(hour2, endTime, 2);
    strncpy_s(minute2, endTime + 3, 2);

    minuteDiff = atoi(minute2) - atoi(minute1);
    if (minuteDiff < 0) {
        minuteDiff += 60;
        hourDiff--;
    }

    hourDiff += atoi(hour2) - atoi(hour1);
    if (hourDiff < 0) {
        hourDiff += 24;
        dayDiff--;
    }

    if (atoi(year1) == atoi(year2))//未跨年时
        dayDiff += get_day_diff(atoi(year2), atoi(month2), atoi(day2)) - get_day_diff(atoi(year1), atoi(month1), atoi(day1));
    else//有跨年
        dayDiff += one_year_day(atoi(year1)) - get_day_diff(atoi(year1), atoi(month1), atoi(day1)) + get_day_diff(atoi(year2), atoi(month2), atoi(day2)) + every_year_day(atoi(year1), atoi(year2));

    //printf("\n %d年%d月%d日 - %d年%d月%d日 之间共有%d天 \n", year1, month1, day1, year2, month2, day2, dayDiff);
    totalDiff = dayDiff * 24 + hourDiff + (float)minuteDiff / 60;

    return totalDiff;
}

int get_day_diff(int year, int month, int day) {
    int sum = 0, leap;
    switch (month) {
    case 1: sum = 0;
        break;
    case 2: sum = 31;
        break;
    case 3: sum = 59;
        break;
    case 4: sum = 90;
        break;
    case 5: sum = 120;
        break;
    case 6: sum = 151;
        break;
    case 7: sum = 181;
        break;
    case 8: sum = 212;
        break;
    case 9: sum = 243;
        break;
    case 10: sum = 273;
        break;
    case 11: sum = 304;
        break;
    case 12: sum = 334;
        break;
    default:printf("月份的输入有误！\n");
    }
    sum += day;
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))//处理闰年问题
        leap = 1;
    else
        leap = 0;

    if (leap == 1 && month > 2)
        sum++;
    return sum;
}

int one_year_day(int n) {   //计算一年中的总天数
    int totle_days;
    if (n % 400 == 0 || (n % 4 == 0 && n % 100 != 0))
        totle_days = 366;
    else
        totle_days = 365;
    return totle_days;
}
int every_year_day(int year1, int year2) {  //计算 两个年份中间相隔所有年份的天数
    int  i, sum = 0, totle_sum = 0;
    for (i = year1 + 1; i < year2; i++) {
        if (i % 400 == 0 || (i % 4 == 0 && i % 100 != 0))
            sum = 366;
        else
            sum = 365;

        totle_sum += sum;
    }
    return totle_sum;
}

float get_transport_cost(char type[], float diff, city_cost CR) {
    float transportCost = 0;
    if (!strcmp(type, "汽车"))
        transportCost = diff * CarCost;
    if (!strcmp(type, "火车"))
        transportCost = diff * TrainCost;
    if (!strcmp(type, "飞机"))
        transportCost = diff * PlaneCost;

    switch (CR)
    {
    case high: transportCost *= HighCost;
        break;
    case mid: transportCost *= MidCost;
        break;
    case low: transportCost *= LowCost;
        break;
    default:
        break;
    }

    return transportCost;
}


void get_city_type(char name[], city_name& CN, city_cost& CR) {
    //高开销:北京、武汉、天津		中开销：上海、广州、深圳、沈阳		低开销：南京、成都、重庆、杭州、西安
    if (!strcmp(name, "北京")) CN = Beijing;
    if (!strcmp(name, "天津")) CN = Tianjin;
    if (!strcmp(name, "武汉")) CN = Wuhan;
    if (!strcmp(name, "上海")) CN = Shanghai;
    if (!strcmp(name, "广州")) CN = Guangzhou;
    if (!strcmp(name, "深圳")) CN = Shenzhen;
    if (!strcmp(name, "沈阳")) CN = Shenyang;
    if (!strcmp(name, "南京")) CN = Nanjing;
    if (!strcmp(name, "成都")) CN = Chengdu;
    if (!strcmp(name, "重庆")) CN = Chongqing;
    if (!strcmp(name, "杭州")) CN = Hangzhou;
    if (!strcmp(name, "西安")) CN = Xian;

    if (!strcmp(name, "Beijing")) CN = Beijing;
    if (!strcmp(name, "Tianjin")) CN = Tianjin;
    if (!strcmp(name, "Wuhan")) CN = Wuhan;
    if (!strcmp(name, "Shanghai")) CN = Shanghai;
    if (!strcmp(name, "Guangzhou")) CN = Guangzhou;
    if (!strcmp(name, "Shenzhen")) CN = Shenzhen;
    if (!strcmp(name, "Shenyang")) CN = Shenyang;
    if (!strcmp(name, "Nanjing")) CN = Nanjing;
    if (!strcmp(name, "Chengdu")) CN = Chengdu;
    if (!strcmp(name, "Chongqing")) CN = Chongqing;
    if (!strcmp(name, "Hangzhou")) CN = Hangzhou;
    if (!strcmp(name, "Xian")) CN = Xian;

    if (CN >= Beijing && CN <= Wuhan)
        CR = high;

    if (CN >= Shanghai && CN <= Shenyang)
        CR = mid;

    if (CN >= Nanjing && CN <= Xian)
        CR = low;

    return;
}


void delete_space() {
    time_table* temp;
    while (mainList != NULL) {
        temp = mainList;
        mainList = mainList->nextptr;
        //delete[] temp;
        free(temp);
    }
    temp = NULL;
    mainList = NULL;
}





void get_route(int mode, char leaveLoc[], char arriveLoc[], char leaveDate[], char leaveTime[], char arriveDate[], char arriveTime[]) {
    /*if (get_time_diff(nowDate, leaveDate, nowTime, leaveTime) < 0) {//用出发时间-当前时间
        cout << "抱歉，当前时间已经超过您选择的出发时间，请重新选择" << endl;
        return;
    }*/

    minCost = 1000000;
    routeNum = 0;
    stackTop = 0;
    //初始化


    time_table* p = mainList, * prePtr = mainList;
    vector<time_table> usingPath;

    city_name sourceCN, destCN;
    city_cost sourceCR, destCR;

    get_city_type(leaveLoc, sourceCN, sourceCR);
    get_city_type(arriveLoc, destCN, destCR);

    float initWaitTime = 0;
    float initCost = 0;

    while (prePtr != NULL) {
        if (prePtr->sourceName == sourceCN) {
            initWaitTime = get_time_diff(leaveDate, prePtr->startDate, leaveTime, prePtr->endTime);//时刻表时间-出发时间
            initCost = get_wait_cost(prePtr->sourceCost, initWaitTime);
            if(initWaitTime < 0){
                prePtr = prePtr->nextptr;
                continue;
            }    
            if (mode == 0) 
                search(prePtr, prePtr, sourceCN, destCN, mode, MaxDate, MaxTime, initCost, initWaitTime, usingPath);
            else if (mode == 1)
                search(prePtr, prePtr, sourceCN, destCN, mode, arriveDate, arriveTime, initCost, initWaitTime, usingPath);
        }
        prePtr = prePtr->nextptr;

    }

    //if (prePtr == NULL) {
    //	printf("Bad timetable!");
    //	return;
    //}//几乎不可能出现,因为每个城市都肯定有出来的时刻表

    cout << endl;
    if (mode == 0)
        cout << "不限时开销最低路线为：" << endl;
    else
        cout << "在您规定时间内的开销最小路线为：" << endl;
    for (int i = 0; i < routeNum; i++) {
        //cout << route[i].startLoc;
        printf("%s %s %s %s %s %s %s %d\n", route[i].startLoc, route[i].endLoc, route[i].transportType, route[i].startDate, route[i].startTime, route[i].endDate, route[i].endTime, route[i].transportID);
    }
    cout << endl;


    if (routeNum == 0)
        cout << "没有到达该城市的路线！请重新规划路线";//基本不可能出现
    else if (route[routeNum - 1].destName != destCN)
        cout << "不能到达！请重新规划路线";

}


void search(time_table* p, time_table* prePtr, city_name sourceCN, city_name destCN, int mode, char limitDate[], char limitTime[], float currCost, float currTime, vector<time_table>& usingPath) {

    if (get_time_diff(limitDate, p->endDate, limitTime, p->endTime) > 0)//该时刻表到达时间-限定到达时间>0,说明这个时刻表不可选
        return;//直接返回
    time_table* currPtr = mainList;
    float day_diff = 0;

    //*********************************************
    float waitTime = 0;
    float waitCost = 0;
    if (p == prePtr)//这种情况只会出现在从外部第一次调用时出现，此时要将源节点设为已访问
        visit[p->sourceName] = 1;
    visit[p->destName] = 1;

    waitTime = get_time_diff(prePtr->endDate, p->startDate, prePtr->endTime, p->startTime);

    waitCost = get_wait_cost(prePtr->destCost, waitTime);//根据前一个时刻表的目的地或当前节点的出发地(同一个地点)，确定等待开销值

    currCost += p->transportCost + waitCost;
    currTime += p->travelTime + waitTime;

    findPath[stackTop++] = *p;
    usingPath.push_back(*p);//保存当前节点

    if (currCost > minCost) {//剪枝，若开销已超，则回退
        if (p == prePtr)
            visit[p->sourceName] = 0;
        visit[p->destName] = 0;
        currCost -= p->transportCost + waitCost;
        currTime -= p->travelTime + waitTime;
        stackTop--;
        usingPath.pop_back();
        return;
    }
    //*********************************************


    if (p->destName == destCN) {//说明此时已经到达终点
        if (currCost <= minCost) {//如果这条路径的开销比之前路径的小，则更新路径
            minCost = currCost;
            for (int i = 0; i < stackTop; i++) {
                route[i] = usingPath[i];
                route[i] = findPath[i];
            }
            routeNum = stackTop;
        }
    }
    else {//若还没有抵达终点
        while (currPtr != NULL) {
            day_diff = 0;
            day_diff = get_time_diff(p->endDate, currPtr->startDate, p->endTime, currPtr->startTime);
            if ((!visit[currPtr->destName]) && currPtr->sourceName == p->destName && day_diff > 0) {
                visit[currPtr->sourceName] = 1;//找到匹配的当前节点后，将这个当前时刻表的出发地设为已访问
                if (mode == 0)
                    search(currPtr, p, sourceCN, destCN, mode, MaxDate, MaxTime, currCost, currTime, usingPath);
                else if (mode == 1)
                    search(currPtr, p, sourceCN, destCN, mode, limitDate, limitTime, currCost, currTime, usingPath);
            }
            //同时满足条件：1、当前时刻表目的地未在路径中被访问过(避免走环路) 2、前后两个时刻表匹配 3、时间不能乱序 4、没有超过最小值(值得更新,否则剪枝)
            currPtr = currPtr->nextptr;
        }

    }

    stackTop--;//弹栈
    usingPath.pop_back();//将p回退
    visit[p->sourceName] = 0;
    visit[p->destName] = 0;
}


float get_wait_cost(city_cost stopCity, float waitTime) {
    float waitCost = 0;
    switch (stopCity) {
    case high: waitCost = HighCost * waitTime;
        break;
    case mid: waitCost = MidCost * waitTime;
        break;
    case low: waitCost = LowCost * waitTime;
        break;
    default:
        break;
    }

    return waitCost;
}


