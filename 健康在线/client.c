#include "head.h"

int sockfd;

/*字符串ip转网络ip*/
int strToInt(char *strIp)
{
    int ip;
    inet_pton(AF_INET, strIp, &ip);
    return ip;
}

/*网络ip转字符串ip*/
char *intToStr(int intIp)
{
    char *ip = malloc(16);
    inet_ntop(AF_INET, &intIp, ip, sizeof(ip));
    return ip;
}

/*获取本地时间函数*/
char *get_local_time_str()
{
    time_t rawtime;
    struct tm *timeinfo;
    static char time_str[20]; // 用于存储时间字符串的静态数组
    // 获取当前时间
    time(&rawtime);
    // 获取本地时间信息
    timeinfo = localtime(&rawtime);
    // 将时间格式化为字符串
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    return time_str;
}

/*清空缓存区*/
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/*等待函数*/
void wait_()
{
    printf("输入回车继续>>");
    clearInputBuffer();
    char c;
    while (c != '\n')
    {
        c = getchar();
    }
    return;
}

/*病人接收医生消息线程*/
void *patientRecv(void *p)
{
    MSG msg;
    TREAT *treat;
    while (1)
    {
        if (read(sockfd, &msg, sizeof(msg)) <= 0)
        {
            printf("已结束会诊\n");
            break;
        }
        else
        {
            treat = (TREAT *)msg.buff;
            printf("[医生%s]%s\n", treat->doctorName, treat->illDescrip);
        }
    }
}

/*医患群聊接收群消息线程*/
void *groupRecv(void *p)
{
    MSG msg;
    TREAT *treat;
    while (1)
    {
        if (read(sockfd, &msg, sizeof(msg)) <= 0)
        {
            printf("已结束群聊\n");
            break;
        }
        else
        {
            treat = (TREAT *)msg.buff;
            printf("[%s]:%s\n", treat->patientName, treat->illDescrip);
        }
    }
}

/*病人登录函数*/
int patientLogin(char *userName)
{
    PATIENT patient;
    memset(&patient, 0, sizeof(PATIENT));
    printf("请输入账号:\n");
    scanf("%s", patient.userName);
    printf("请输入密码:\n");
    scanf("%s", patient.userPass);

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    msg.type = 1;
    memcpy(msg.buff, &patient, sizeof(PATIENT));
    write(sockfd, &msg, sizeof(msg));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(msg));
    if (msg.type == 11)
    {
        strcpy(userName, patient.userName);
        printf("登录成功\n");
        return 0;
    }
    if (msg.type == 12)
    {
        printf("账号或密码错误\n");
        return -1;
    }
    if (msg.type == 13)
    {
        printf("账号已在线请勿重复登录\n");
        return -1;
    }
}

/*病人注册函数*/
int patientRegiste()
{
    PATIENT patient;
    printf("请输入账号:\n");
    scanf("%s", patient.userName);
    printf("请输入密码:\n");
    scanf("%s", patient.userPass);
    printf("请输入名字:\n");
    scanf("%s", patient.patientName);
    printf("请输入性别:\n");
    scanf("%s", patient.sex);
    printf("请输入年龄:\n");
    scanf("%d", &(patient.age));
    printf("请输入电话:\n");
    scanf("%s", patient.phoneNumber);

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    msg.type = 2;
    memcpy(msg.buff, &patient, sizeof(PATIENT));
    write(sockfd, &msg, sizeof(msg));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(msg));
    if (msg.type == 15)
    {
        printf("注册成功\n");
        return 0;
    }
    if (msg.type == 16)
    {
        printf("账号已存在\n");
        return 1;
    }
}

/*医生登录函数*/
int doctorLogin(char *userName)
{
    DOCTOR doctor;
    memset(&doctor, 0, sizeof(PATIENT));
    printf("请输入账号:\n");
    scanf("%s", doctor.userName);
    printf("请输入密码:\n");
    scanf("%s", doctor.userPass);

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    msg.type = 4;
    memcpy(msg.buff, &doctor, sizeof(PATIENT));
    write(sockfd, &msg, sizeof(msg));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(msg));
    if (msg.type == 11)
    {
        printf("登录成功\n");
        strcpy(userName, msg.buff);
        return 0;
    }
    if (msg.type == 12)
    {
        printf("账号或密码错误\n");
        return -1;
    }
    if (msg.type == 13)
    {
        printf("该账号已在线请勿重复登录\n");
        return -1;
    }
}

/*管理员登录函数*/
int rootLogin(char *userName)
{
    ROOT root;
    memset(&root, 0, sizeof(PATIENT));
    printf("请输入账号:\n");
    scanf("%s", root.userName);
    printf("请输入密码:\n");
    scanf("%s", root.userPass);

    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    msg.type = 25;
    memcpy(msg.buff, &root, sizeof(ROOT));
    write(sockfd, &msg, sizeof(msg));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(msg));
    if (msg.type == 11)
    {
        printf("管理员登录成功\n");
        strcpy(userName, msg.buff);
        return 0;
    }
    if (msg.type == 12)
    {
        printf("账号或密码错误\n");
        return -1;
    }
}

/*病人选择科室函数*/
MSG selectUnit(int n)
{
    MSG msg;
    msg.type = 6;
    char unitName[64];
    if (n == 1)
    {
        strcpy(unitName, "内科");
    }
    else if (n == 2)
    {
        strcpy(unitName, "外科");
    }
    else if (n == 3)
    {
        strcpy(unitName, "妇科");
    }
    else if (n == 4)
    {
        strcpy(unitName, "儿科");
    }
    strcpy(msg.buff, unitName);
    write(sockfd, &msg, sizeof(msg)); // 告诉服务器我要看什么科
    memset(&msg, 0, sizeof(msg));
    read(sockfd, &msg, sizeof(msg));
    return msg;
}

/*病人修改密码*/
void patientChangePasswd(char *userName)
{
    MSG msg;
    PATIENT patient;
    msg.type = 30;
    printf("请输入新密码:\n");
    strcpy(patient.userName, userName);
    scanf("%s", patient.userPass);
    memcpy(msg.buff, &patient, sizeof(PATIENT));
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("密码修改成功\n");
        wait_();
    }
    else
    {
        printf("修改失败\n");
        wait_();
    }
}

/*病人挂号函数*/
int patientReg(MSG msg, char *userName)
{
    UNIT *unit = (UNIT *)msg.buff;
    if (unit->doctorNum == 0)
    {
        printf("%s暂无在班医生......\n", unit->unitName);
        sleep(1);
        return 0;
    }
    printf("在班医生有\n");
    for (int i = 0; i < unit->doctorNum; i++)
    {
        printf("姓名:%-8s 职位:%-20s 电话:%-18s \n", unit->unitDoctor[i].doctorName, unit->unitDoctor[i].titleName, unit->unitDoctor[i].phoneNumber);
    }

    int loop = 1;
    while (loop)
    {
        printf("请输入您要挂号的医生姓名:\n");
        char doctorName[64];
        scanf("%s", doctorName);
        int flag = -1;
        for (int i = 0; i < unit->doctorNum; i++)
        {
            if (strcmp(doctorName, unit->unitDoctor[i].doctorName) == 0)
            {
                flag = 0;
                REG reg;
                strcpy(reg.patientName, userName); // 病人选择好医生,填充挂号结构体
                strcpy(reg.doctorName, doctorName);
                strcpy(reg.regUnitName, unit->unitName);
                char *time_str = get_local_time_str();
                strcpy(reg.regTime, time_str);

                MSG msg;
                msg.type = 8;
                memcpy(msg.buff, &reg, sizeof(REG)); // 将挂号信息发送给服务器
                write(sockfd, &msg, sizeof(msg));
                memset(&msg, 0, sizeof(MSG));
                read(sockfd, &msg, sizeof(MSG));
                if (msg.type == -1)
                {
                    printf("挂号失败,请勿重复挂号\n");
                    wait_();
                }
                else
                {
                    printf("挂号成功,时间:%s\n", time_str);
                    wait_();
                }
                loop = 0;
                break;
            }
        }
        if (flag == -1)
        {
            printf("没有%s医生在班\n", doctorName);
            loop = 0;
            wait_();
        }
    }
}

/*医生消息接收线程*/
void *doctorRecv(void *p)
{
    MSG msg;
    TREAT *treat;
    while (1)
    {
        if (read(sockfd, &msg, sizeof(msg)) <= 0)
        {
            printf("已结束会诊\n");
            break;
        }
        else
        {
            treat = (TREAT *)msg.buff;
            printf("[病人%s]%s\n", treat->patientName, treat->illDescrip);
        }
    }
}

/*判断病人是否排到*/
MSG patientNext(char *userName)
{
    MSG msg;
    msg.type = 7;
    strcpy(msg.buff, userName);
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(msg));
    read(sockfd, &msg, sizeof(msg));
    if (msg.type != -1 && msg.type != 0)
    {
        printf("您前面排了%d人\n", msg.type);
        wait_();
    }
    if (msg.type == -1)
    {
        printf("没有您的挂号信息......\n");
        wait_();
    }

    return msg;
}

/*病人开始会诊函数*/
void patientTreat(MSG regMsg)
{
    printf("已排到您开始会诊\n");
    TREAT treat;
    REG *reg;
    reg = (REG *)regMsg.buff;
    MSG msg;
    msg.type = 100;
    strcpy(treat.doctorName, reg->doctorName);
    strcpy(treat.patientName, reg->patientName);
    memcpy(msg.buff, &treat, sizeof(TREAT));
    printf("医生姓名:%s 病人姓名:%s\n", reg->doctorName, reg->patientName);
    write(sockfd, &msg, sizeof(MSG));
    pthread_t pt;
    pthread_create(&pt, NULL, patientRecv, NULL);
    printf("描述您的病情:\n");
    while (1)
    {
        MSG msg;
        memset(treat.illDescrip, 0, sizeof(treat.illDescrip));
        msg.type = 100;
        scanf("%s", treat.illDescrip);
        memcpy(msg.buff, &treat, sizeof(TREAT));
        if (strcmp(treat.illDescrip, "quit") == 0)
        {
            pthread_cancel(pt);
            break;
        }
        write(sockfd, &msg, sizeof(MSG));
    }
}

/*医生开始接诊函数*/
void doctorTreat(char userName[64])
{
    MSG msg;
    DOCTOR doctor;
    strcpy(doctor.userName, userName);
    msg.type = 9;
    memcpy(msg.buff, &doctor, sizeof(doctor));
    write(sockfd, &msg, sizeof(msg)); // 告诉服务器我要开始接诊了

    REG *reg;
    read(sockfd, &msg, sizeof(msg)); // 服务器返回一个挂号信息结构体
    reg = (REG *)msg.buff;
    if (strcmp(reg->doctorName, "NULL") == 0)
    {
        printf("没有你的病人\n");
        wait_();
        return;
    }
    TREAT treat;
    strcpy(treat.doctorName, reg->doctorName);
    strcpy(treat.patientName, reg->patientName);
    printf("医生姓名:%s 病人姓名:%s\n", treat.doctorName, treat.patientName);
    msg.type = 10;
    memcpy(msg.buff, &treat, sizeof(TREAT));
    write(sockfd, &msg, sizeof(MSG)); // 让服务器记下病人的通信fd我好发消息
    pthread_t pt;
    pthread_create(&pt, NULL, doctorRecv, NULL);
    while (1)
    {
        TREAT treat;
        scanf("%s", treat.illDescrip);
        strcpy(treat.doctorName, reg->doctorName);
        strcpy(treat.patientName, reg->patientName);
        MSG msg;
        msg.type = 10;
        memcpy(msg.buff, &treat, sizeof(TREAT));
        if (strcmp(treat.illDescrip, "quit") == 0)
        {
            pthread_cancel(pt);
            memset(&msg, 0, sizeof(MSG));
            msg.type = 20;
            printf("请输入诊断结果:\n");
            memset(treat.illDescrip, 0, sizeof(treat.illDescrip));
            scanf("%s", treat.illDescrip);
            strcpy(treat.treatTime, get_local_time_str());
            memcpy(msg.buff, &treat, sizeof(treat));
            write(sockfd, &msg, sizeof(MSG));
            break;
        }
        write(sockfd, &msg, sizeof(MSG));
    }
}

/*显示医生问诊记录*/
void printTreatDoctor(char *userName)
{
    MSG msg;
    msg.type = 22;
    strcpy(msg.buff, userName);
    write(sockfd, &msg, sizeof(MSG));
    while (1)
    {
        TREAT *treat;
        memset(&msg, 0, sizeof(msg));
        read(sockfd, &msg, sizeof(MSG));

        treat = (TREAT *)msg.buff;
        if (strcmp(treat->illDescrip, "over") == 0)
        {
            printf("全部接诊信息显示完毕\n");
            break;
        }
        printf("病人:%-6s   医生:%-6s   时间:%-12s 诊断结果:%-20s\n", treat->patientName, treat->doctorName, treat->treatTime, treat->illDescrip);
    }
    wait_();
}

/*显示挂号排队病人*/
void printRegPatient(char *userName)
{
    MSG msg;
    msg.type = 24;
    strcpy(msg.buff, userName);
    write(sockfd, &msg, sizeof(MSG));
    while (1)
    {
        REG *reg;
        memset(&msg, 0, sizeof(msg));
        read(sockfd, &msg, sizeof(MSG));

        reg = (REG *)msg.buff;
        if (strcmp(reg->patientName, "NULL") == 0)
        {
            printf("没有挂号病人\n");
            break;
        }
        if (strcmp(reg->patientName, "over") == 0)
        {
            printf("全部排队信息显示完毕\n");
            break;
        }
        printf("病人:%-6s   医生:%-6s   时间:%-12s\n", reg->patientName, reg->doctorName, reg->regTime);
    }
    wait_();
}

/*显示病人就诊记录*/
void printTreatPatient(char *userName)
{
    MSG msg;
    msg.type = 23;
    strcpy(msg.buff, userName);
    write(sockfd, &msg, sizeof(msg));
    while (1)
    {
        TREAT *treat;
        memset(&msg, 0, sizeof(msg));
        if (read(sockfd, &msg, sizeof(MSG)) <= 0)
        {
            printf("全部接诊信息显示完毕\n");
            break;
        }
        treat = (TREAT *)msg.buff;
        if (strcmp(treat->illDescrip, "over") == 0)
        {
            printf("全部接诊信息显示完毕\n");
            break;
        }
        printf("病人:%-6s   医生:%-6s   时间:%-12s 诊断结果:%-20s\n", treat->patientName, treat->doctorName, treat->treatTime, treat->illDescrip);
    }
    wait_();
}

/*病人取消挂号*/
void cancelReg(char *userName)
{
    MSG msg;
    msg.type = 19;
    strcpy(msg.buff, userName);
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("取消成功\n");
        wait_();
    }
    else
    {
        printf("没有您的挂号信息\n");
        wait_();
    }
}

/*医患群聊函数*/
void groupChat(char *userName)
{
    MSG msg;
    msg.type = 18;
    TREAT treat;
    strcpy(treat.patientName, userName);
    strcpy(treat.illDescrip, "join");
    memcpy(msg.buff, &treat, sizeof(TREAT));
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(msg));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("已加入群聊\n");
        pthread_t pt;
        pthread_create(&pt, NULL, groupRecv, NULL);
        while (1)
        {
            MSG msg;
            memset(&msg, 0, sizeof(msg));
            scanf("%s", treat.illDescrip);
            memcpy(msg.buff, &treat, sizeof(TREAT));
            msg.type = 18;
            write(sockfd, &msg, sizeof(MSG));
            if (strcmp(treat.illDescrip, "quit") == 0)
            {

                printf("已退出群聊\n");
                pthread_cancel(pt);
                wait_();
                break;
            }
        }
    }
    else
    {
        printf("加入群聊失败\n");
    }
}

/*管理员查看医生信息*/
void printDoctorMessage()
{
    MSG msg;
    msg.type = 50;
    write(sockfd, &msg, sizeof(MSG));

    while (1)
    {
        DOCTOR *doctor;
        memset(&msg, 0, sizeof(msg));
        read(sockfd, &msg, sizeof(MSG));
        doctor = (DOCTOR *)msg.buff;
        if (strcmp(doctor->doctorName, "over") == 0)
        {
            printf("全部医生信息显示完毕\n");
            break;
        }
        printf("账号:%s 密码%s  姓名:%s 科室:%s 任职:%s\n", doctor->userName, doctor->userPass, doctor->doctorName, doctor->unitName, doctor->titleName);
    }
    wait_();
}

/*管理员查看病人信息*/
void printPatientMessage()
{
    MSG msg;
    msg.type = 51;
    write(sockfd, &msg, sizeof(MSG));

    while (1)
    {
        PATIENT *patient;
        memset(&msg, 0, sizeof(msg));
        read(sockfd, &msg, sizeof(MSG));
        patient = (PATIENT *)msg.buff;
        if (strcmp(patient->patientName, "over") == 0)
        {
            printf("全部医生信息显示完毕\n");
            break;
        }
        printf("账号:%s 密码%s  姓名:%s 性别:%s 年龄:%d\n", patient->userName, patient->userPass, patient->userPass, patient->sex, patient->age);
    }
    wait_();
}

/*管理员查看诊断信息*/
void printTreatMessage()
{
    MSG msg;
    msg.type = 52;
    write(sockfd, &msg, sizeof(MSG));

    while (1)
    {
        TREAT *treat;
        memset(&msg, 0, sizeof(msg));
        read(sockfd, &msg, sizeof(MSG));
        treat = (TREAT *)msg.buff;
        if (strcmp(treat->illDescrip, "over") == 0)
        {
            printf("全部医生信息显示完毕\n");
            break;
        }
        printf("姓名:%s 主治医生:%s 时间:%s 病症:%s\n", treat->patientName, treat->doctorName, treat->treatTime, treat->illDescrip);
    }
    wait_();
}

/*管理员修改医生信息*/
void changeDoctorMessage()
{
    MSG msg;
    msg.type = 53;
    DOCTOR doctor;
    printf("请输入要修改的医生账号:\n");
    scanf("%s", doctor.userName);
    printf("请输入修改后的密码:\n");
    scanf("%s", doctor.userPass);
    printf("请输入修改后的姓名:\n");
    scanf("%s", doctor.doctorName);
    printf("请输入修改后的性别:\n");
    scanf("%s", doctor.sex);
    printf("请输入修改后的年龄:\n");
    scanf("%d", &doctor.age);
    printf("请输入修改后的科室:\n");
    scanf("%s", doctor.unitName);
    printf("请输入修改后的任职:\n");
    scanf("%s", doctor.titleName);
    printf("请输入修改后的电话:\n");
    scanf("%s", doctor.phoneNumber);

    memcpy(msg.buff, &doctor, sizeof(DOCTOR));
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("修改成功\n");
    }
    else
    {
        printf("修改失败\n");
    }
}

/*管理员修改病人信息*/
void changePatientMessage()
{
    MSG msg;
    msg.type = 54;
    PATIENT patient;
    printf("请输入要修改的病人账号:\n");
    scanf("%s", patient.userName);
    printf("请输入修改后的密码:\n");
    scanf("%s", patient.userPass);
    printf("请输入修改后的姓名:\n");
    scanf("%s", patient.patientName);
    printf("请输入修改后的性别:\n");
    scanf("%s", patient.sex);
    printf("请输入修改后的年龄:\n");
    scanf("%d", &patient.age);
    printf("请输入修改后的电话:\n");
    scanf("%s", patient.phoneNumber);
    memcpy(msg.buff, &patient, sizeof(PATIENT));
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("修改成功\n");
    }
    else
    {
        printf("修改失败\n");
    }
}

/*管理员添加医生*/
void addDoctor()
{
    MSG msg;
    msg.type = 56;
    DOCTOR doctor;

    printf("请输入医生账号:\n");
    scanf("%s", doctor.userName);
    printf("请输入医生密码:\n");
    scanf("%s", doctor.userPass);
    printf("请输入医生姓名:\n");
    scanf("%s", doctor.doctorName);
    printf("请输入医生性别:\n");
    scanf("%s", doctor.sex);
    printf("请输入医生年龄:\n");
    scanf("%d", &doctor.age);
    printf("请输入医生科室:\n");
    scanf("%s", doctor.unitName);
    printf("请输入医生任职:\n");
    scanf("%s", doctor.titleName);
    printf("请输入医生电话:\n");
    scanf("%s", doctor.phoneNumber);

    memcpy(msg.buff, &doctor, sizeof(DOCTOR));
    write(sockfd, &msg, sizeof(MSG));

    printf("医生信息已发送\n");
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 15)
        printf("添加成功\n");
    else
        printf("添加失败\n");
}

/*管理员解雇医生*/
void deleteDoctor()
{
    MSG msg;
    msg.type = 57;
    printf("请输入要解雇的医生姓名:\n");
    scanf("%s", msg.buff);
    write(sockfd, &msg, sizeof(MSG));

    memset(&msg, 0, sizeof(MSG));
    read(sockfd, &msg, sizeof(MSG));
    if (msg.type == 0)
    {
        printf("已解雇医生\n");
        wait_();
    }
    else if (msg.type = -1)
    {
        printf("医生不存在\n");
        wait_();
    }
    else if (msg.type = 1)
    {
        printf("查询医生失败\n");
        wait_();
    }
}

void displayMenu(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("无法打开菜单文件");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }

    fclose(file);
}

int main(int argc, char const *argv[])
{
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("创建套接字失败");
        return -1;
    }

    // 设置服务器地址
    struct sockaddr_in server;
    server.sin_addr.s_addr = strToInt("192.168.150.131");
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (ret == -1)
    {
        printf("服务器是不是没开\n");
        return 0;
    }

    while (1)
    {
        displayMenu("mainMenu.txt");
        int choice;
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 10)
        {
            printf("输入有误，请重新输入\n");
            // 清除输入缓冲区
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1: // 登录
        {
            char userName[64];
            if (patientLogin(userName) == 0) // 登录成功
            {
                while (1)
                {
                    displayMenu("patientMenu.txt");
                    int subChoice;
                    if (scanf("%d", &subChoice) != 1 || subChoice < 1 || subChoice > 10)
                    {
                        printf("输入有误，请重新输入\n");
                        // 清除输入缓冲区
                        while (getchar() != '\n')
                            ;
                        continue;
                    }
                    MSG msg;
                    msg.type = 6;

                    switch (subChoice)
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        msg = selectUnit(subChoice);
                        patientReg(msg, userName);
                        break;
                    case 5:
                        msg = patientNext(userName);
                        if (msg.type == 0)
                        {
                            REG *reg = (REG *)msg.buff;
                            patientTreat(msg);
                        }
                        break;
                    case 6:
                        printTreatPatient(userName);
                        break;
                    case 7:
                        groupChat(userName);
                        break;
                    case 8:
                        cancelReg(userName);
                        break;
                    case 9:
                        patientChangePasswd(userName);
                        break;
                    case 10:
                        msg.type = 5;
                        write(sockfd, &msg, sizeof(msg));
                        break;
                    default:
                        printf("输入有误请重新输入\n");
                        break;
                    }

                    if (subChoice == 10)
                    {
                        break;
                    }
                }
            }
            break;
        }
        case 2: // 注册
            patientRegiste();
            break;
        case 3: // 退出
        {
            MSG msg;
            memset(&msg, 0, sizeof(MSG));
            msg.type = 3;
            write(sockfd, &msg, sizeof(msg));
            printf("您已退出医院系统\n");
            return 0;
        }
        case 4: // 医生登录
        {
            char userName[64];
            if (doctorLogin(userName) == 0)
            {
                while (1)
                {
                    displayMenu("doctorMenu.txt");

                    int subChoice;
                    if (scanf("%d", &subChoice) != 1 || subChoice < 1 || subChoice > 10)
                    {
                        printf("输入有误，请重新输入\n");
                        // 清除输入缓冲区
                        while (getchar() != '\n')
                            ;
                        continue;
                    }

                    MSG msg;
                    switch (subChoice)
                    {
                    case 1:
                        doctorTreat(userName);
                        break;
                    case 2:
                        printRegPatient(userName);
                        break;
                    case 3:
                        printTreatDoctor(userName);
                        break;
                    case 4:
                        groupChat(userName);
                        break;
                    case 5:
                        msg.type = 21;
                        write(sockfd, &msg, sizeof(msg));
                        break;
                    default:
                        printf("输入有误请重新输入\n");
                        break;
                    }

                    if (subChoice == 5)
                    {
                        break;
                    }
                }
            }
            break;
        }
        case 5: // 管理员登录
        {
            char userName[64];
            if (rootLogin(userName) == 0)
            {
                while (1)
                {
                    displayMenu("rootMenu.txt");
                    int subChoice;
                    if (scanf("%d", &subChoice) != 1 || subChoice < 1 || subChoice > 10)
                    {
                        printf("输入有误，请重新输入\n");
                        // 清除输入缓冲区
                        while (getchar() != '\n')
                            ;
                        continue;
                    }

                    switch (subChoice)
                    {
                    case 1:
                        printDoctorMessage();
                        break;
                    case 2:
                        printPatientMessage();
                        break;
                    case 3:
                        printTreatMessage();
                        break;
                    case 4:
                        changeDoctorMessage();
                        break;
                    case 5:
                        changePatientMessage();
                        break;
                    case 6:
                    {
                        addDoctor();
                        break;
                    }
                    case 7:
                    {
                        deleteDoctor();
                        break;
                    }
                    case 8:
                    {
                        MSG msg;
                        msg.type = 55;
                        write(sockfd, &msg, sizeof(MSG));
                        printf("管理员退出登录\n");
                        break;
                    }
                    default:
                        printf("输入有误请重新输入\n");
                        break;
                    }

                    if (subChoice == 8)
                    {
                        break;
                    }
                }
            }
            break;
        }
        default:
            printf("输入有误请重新输入\n");
            break;
        }
    }

    return 0;
}
