#include "head.h"

DOCTOR online_doctor[1000];                // 记录当前在线的医生
PATIENT online_patient[1000];              // 记录当前在线的病人
int doctor_count = 0;                      // 记录在线医生人数
int patient_count = 0;                     // 记录在线病人人数
int sockfd;                                // 准备通信套接字
int epollfd;                               // 准备epoll套接字
sqlite3 *sq;                               // 数据库指针
struct epoll_event ev, events[MAX_EVENTS]; // epoll变量 事件数组
LIST *list;                                // 挂号信息链表
LIST *pTail;                               // 尾指针
int groupChat[1000];                       // 存放群聊人员fd
int groupChatCount = 0;                    // 记录群聊成员数
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

/*服务器关闭函数*/
void singleHandle(int sig)
{
    sqlite3_close(sq);
    close(sockfd);
    close(epollfd);
    printf("服务器已关闭\n");
    exit(0);
}

/*创建一个挂号消息链表*/
LIST *listCreate()
{
    LIST *list = (LIST *)malloc(sizeof(LIST));
    list->next = NULL;
    pTail = list; // 记录尾指针
    return list;
}

/*挂号队列尾插添加节点*/
void listInsert(REG data)
{
    LIST *newNode = (LIST *)malloc(sizeof(LIST));
    newNode->data = data;
    pTail->next = newNode;
    newNode->next = NULL;
    pTail = newNode;
}

/*删挂号节点通过医生姓名*/
void delNode(char doctorName[64])
{
    if (list->next == NULL)
    {
        printf("消息队列是空的\n");
        return;
    }
    LIST *p = list;
    while (p->next)
    {
        if (strcmp(p->next->data.doctorName, doctorName) == 0)
        {
            LIST *q;
            q = p->next;
            if (q->next == NULL)
            {
                pTail = list;
            }
            p->next = q->next;
            printf("删除挂号信息[医生%s] [病人%s]\n", q->data.doctorName, q->data.patientName);
            free(q);
            return;
        }
        p = p->next;
    }
}

/*删挂号节点通过病人姓名*/
void delNodeByPatient(char patientName[64])
{
    if (list->next == NULL)
    {
        printf("消息队列是空的\n");
        return;
    }
    LIST *p = list;
    while (p->next)
    {
        if (strcmp(p->next->data.patientName, patientName) == 0)
        {
            LIST *q;
            q = p->next;
            if (q->next == NULL)
            {
                pTail = list;
            }
            p->next = q->next;
            printf("删除挂号信息[医生%s] [病人%s]\n", q->data.doctorName, q->data.patientName);
            free(q);
            return;
        }
        p = p->next;
    }
}

/*遍历消息链表*/
void display()
{
    printf("开始遍历链表\n");
    LIST *p = list;
    while (p->next)
    {
        printf("病人：%s    医生：%s\n", p->next->data.patientName, p->next->data.doctorName);
        p = p->next;
    }
    printf("结束遍历链表\n");
}

/*查看第一个挂号结点*/
LIST getHeadNode(LIST *list, char doctorName[64])
{
    LIST headlNode;
    memset(&headlNode, 0, sizeof(LIST));
    if (list->next == NULL)
    {
        printf("消息队列是空的\n");
        strcpy(headlNode.data.doctorName, "NULL");
        return headlNode;
    }
    while (list->next)
    {
        if (strcmp(list->next->data.doctorName, doctorName) == 0)
        {
            headlNode = *list->next;
            return headlNode;
        }
        list = list->next;
    }
    return headlNode;
}

/*查找病人排队信息*/
MSG patientLine(REG reg)
{
    int flag = -1;
    int count = 0;
    MSG msg;
    LIST *p = list;
    while (p->next)
    {
        if (strcmp(p->next->data.patientName, reg.patientName) == 0)
        {
            flag = 0;
            break;
        }

        p = p->next;
    }
    if (flag == -1)
    {
        printf("没有%s的挂号信息\n", reg.patientName);
        msg.type = -1;
        return msg;
    }
    strcpy(reg.doctorName, p->next->data.doctorName);
    printf("找到%s对应的医生%s\n", reg.patientName, reg.doctorName);
    p = list;
    while (p->next)
    {
        if (strcmp(p->next->data.doctorName, reg.doctorName) == 0)
        {
            if (strcmp(p->next->data.patientName, reg.patientName) == 0)
            {
                msg.type = count;
                memcpy(msg.buff, &p->next->data, sizeof(REG));
                return msg;
            }
            else
            {
                count++;
            }
        }
        p = p->next;
    }
    msg.type = -1;
    return msg;
}

/*显示病人排队位置*/
void patientLineMessage(MSG msg, int fd)
{
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char patientName[64];
    sprintf(sql, "SELECT patientName FROM patient WHERE userName='%s'", msg.buff); // 查找病人的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(patientName, result[column]);

    REG reg;
    memset(&reg, 0, sizeof(reg));
    strcpy(reg.patientName, patientName);
    MSG regMsg = patientLine(reg);
    printf("%s前面排了%d人\n", reg.patientName, regMsg.type);
    memset(&msg, 0, sizeof(MSG));
    msg.type = regMsg.type;
    if (regMsg.type == 0)
    {
        write(fd, &regMsg, sizeof(MSG));
    }
    else
    {
        write(fd, &msg, sizeof(MSG));
    }
}

/*从数据库读取科室信息,发送科室信息*/
MSG unitSelect_(char unitName[64])
{
    UNIT unit;
    DOCTOR doctor;
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    // 筛选所选科室的在班医生信息
    sprintf(sql, "SELECT doctorId, userName, unitName, doctorName, titleName, phoneNumber FROM doctor WHERE unitName='%s'",
            unitName);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    // 填充医生结构体
    int count = 0;
    memset(unit.unitDoctor, 0, sizeof(unit.unitDoctor));
    for (int i = 1; i <= row; i++)
    {
        for (int j = 0; j < doctor_count; j++)
        {
            if ((strcmp(result[i * column + 1], online_doctor[j].userName) == 0) &&
                (strcmp(result[i * column + 2], unitName) == 0))
            {
                unit.unitDoctor[i - 1].doctorId = atoi(result[i * column]);
                strcpy(unit.unitDoctor[count].userName, result[i * column + 1]);
                strcpy(unit.unitDoctor[count].unitName, result[i * column + 2]);
                strcpy(unit.unitDoctor[count].doctorName, result[i * column + 3]);
                strcpy(unit.unitDoctor[count].titleName, result[i * column + 4]);
                strcpy(unit.unitDoctor[count].phoneNumber, result[i * column + 5]);
                count++;
                break;
            }
        }
    }

    unit.doctorNum = count;
    sqlite3_free_table(result);
    result = NULL;
    row = 0;
    column = 0;
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "SELECT unitId, unitName FROM unit WHERE unitName='%s'", unitName);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    // 填充科室结构体
    if (row > 0)
    {
        unit.unitId = atoi(result[column]);
        strcpy(unit.unitName, result[column + 1]);
    }
    MSG msg;
    msg.type = 100;
    memcpy(msg.buff, &unit, sizeof(unit));
    return msg;
}

/*科室选择*/
void unitSelect(MSG msg, int fd)
{
    if (strcmp("内科", msg.buff) == 0)
    {
        char unitName[64];
        strcpy(unitName, msg.buff);
        printf("病人要看内科\n");
        MSG msg;
        msg = unitSelect_(unitName);
        write(fd, &msg, sizeof(MSG));
    }
    else if (strcmp("外科", msg.buff) == 0)
    {
        char unitName[64];
        strcpy(unitName, msg.buff);
        printf("病人要看外科\n");
        MSG msg;
        msg = unitSelect_(unitName);
        write(fd, &msg, sizeof(MSG));
    }
    else if (strcmp("妇科", msg.buff) == 0)
    {
        char unitName[64];
        strcpy(unitName, msg.buff);
        printf("病人要看妇科\n");
        MSG msg;
        msg = unitSelect_(unitName);
        write(fd, &msg, sizeof(MSG));
    }
    else if (strcmp("儿科", msg.buff) == 0)
    {
        char unitName[64];
        strcpy(unitName, msg.buff);
        printf("病人要看儿科\n");
        MSG msg;
        msg = unitSelect_(unitName);
        write(fd, &msg, sizeof(MSG));
    }
    else
    {
        printf("没有该科室\n");
    }
}

/*病人登录函数*/
int patientLogin(MSG msg, int fd)
{
    PATIENT *patient;
    patient = (PATIENT *)msg.buff;
    char sql[1024], **result = NULL;
    char *errMsg;
    int row = 0, column = 0;
    sprintf(sql, "SELECT *FROM patient WHERE userName='%s' AND userPass='%s'", patient->userName, patient->userPass);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    int flag = 0;
    if (row > 0)
    {
        for (int i = 0; i < patient_count; i++)
        {
            if (strcmp(patient->userName, online_patient[i].userName) == 0)
            {
                flag = -1;
                break;
            }
        }
        if (flag == -1)
        {
            printf("%s账号重复登录\n", patient->userName);
            MSG msg;
            msg.type = 13; // 重复登录返回13标记
            write(fd, &msg, sizeof(msg));
        }
        else
        {
            printf("%s登录成功\n", patient->userName);
            MSG msg;
            msg.type = 11; // 登录成功返回11标记
            online_patient[patient_count].stat = fd;
            strcpy(online_patient[patient_count].userName, patient->userName);
            patient_count++;
            printf("病人在线人数:%d\n", patient_count);
            write(fd, &msg, sizeof(msg));
        }
    }
    else
    {
        printf("%s账号或密码错误\n", patient->userName);
        MSG msg;
        msg.type = 12; // 登录失败返回12标记
        write(fd, &msg, sizeof(msg));
    }
}

/*病人注册函数*/
int patientRegiste(MSG msg, int fd)
{
    PATIENT *patient;
    patient = (PATIENT *)msg.buff;
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "SELECT *FROM patient WHERE userName='%s'", patient->userName);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    if (row == 0)
    {
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "INSERT INTO patient (userName,userPass,patientName,sex,age,phoneNumber) VALUES ('%s','%s','%s','%s',%d,'%s');",
                patient->userName, patient->userPass, patient->patientName, patient->sex, patient->age, patient->phoneNumber);
        sqlite3_exec(sq, sql, 0, 0, &errMsg);
        printf("%s注册成功\n", patient->patientName);
        MSG msg;
        msg.type = 15; // 注册成功返回15标记
        write(fd, &msg, sizeof(msg));
    }
    else
    {
        printf("%s注册失败,账号重复\n", patient->userName);
        MSG msg;
        msg.type = 16; // 注册失败账号已存在返回16
        write(fd, &msg, sizeof(msg));
    }
}

/*病人修改密码函数*/
void patientChangePasswd(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024];
    PATIENT *patient;
    patient = (PATIENT *)msg.buff;
    sprintf(sql, "UPDATE patient SET userPass='%s' WHERE userName='%s';", patient->userPass, patient->userName);
    sqlite3_exec(sq, sql, 0, 0, &errMsg);
    MSG responseMsg;
    responseMsg.type = 0; // 假设0表示成功
    write(fd, &responseMsg, sizeof(responseMsg));
}

/*保存挂号记录函数*/
void saveReg(REG *reg)
{
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "INSERT INTO registered (patientName ,doctorName ,regTime ,regUnitName) VALUES ('%s','%s','%s','%s');",
            reg->patientName, reg->doctorName, reg->regTime, reg->regUnitName);
    sqlite3_exec(sq, sql, 0, 0, &errMsg);
}

/*挂号*/
void patientReg(MSG msg, int fd)
{

    REG *reg;
    reg = (REG *)msg.buff;
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    sprintf(sql, "SELECT patientName FROM patient WHERE userName='%s'", reg->patientName); // 查找病人的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(reg->patientName, result[column]);

    LIST *p = list;
    while (p->next)
    {
        if (strcmp(reg->patientName, p->next->data.patientName) == 0)
        {
            printf("%s请勿重复挂号\n", reg->patientName);
            msg.type = -1;
            write(fd, &msg, sizeof(msg));
            return;
        }
        p = p->next;
    }
    msg.type = 0;
    write(fd, &msg, sizeof(msg));
    saveReg(reg);
    printf("挂号信息 病人：%s 医生：%s 科室：%s \n", reg->patientName, reg->doctorName, reg->regUnitName);
    listInsert(*reg);
}

/*病人退出客户端*/
void patientExit(MSG msg, int fd)
{

    for (int i = 0; i < patient_count; i++)
    {
        if (online_patient[i].stat == fd)
        {
            printf("%s已退出登录\n", online_patient[i].userName);
            for (int j = i; j < patient_count - 1; j++)
            {
                online_patient[j] = online_patient[j + 1];
            }
        }
    }
    patient_count--;
}

/*医生退出客户端*/
void doctorExit(MSG msg, int fd)
{
    for (int i = 0; i < doctor_count; i++)
    {
        if (online_doctor[i].stat == fd)
        {
            printf("%s已退出登录\n", online_doctor[i].userName);
            for (int j = i; j < doctor_count - 1; j++)
            {
                online_doctor[j] = online_doctor[j + 1];
            }
        }
    }
    doctor_count--;
}

/*保存治疗记录函数*/
void saveTreatRecord(TREAT *treat)
{
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "INSERT INTO treat (patientName ,doctorName ,treatTime ,illDescrip) VALUES ('%s','%s','%s','%s');", treat->patientName, treat->doctorName, treat->treatTime, treat->illDescrip);
    sqlite3_exec(sq, sql, 0, 0, &errMsg);
}

/*病人取消挂号函数*/
void cancelReg(MSG msg, int fd)
{
    if (list->next == NULL)
    {
        msg.type = -1;
        printf("没有人挂号\n");
        write(fd, &msg, sizeof(MSG));
        return;
    }
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char patientName[64];
    sprintf(sql, "SELECT patientName FROM patient WHERE userName='%s'", msg.buff); // 查找病人的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(patientName, result[column]);
    sqlite3_free_table(result);
    LIST *p = list;
    char doctorName[64];
    while (p->next)
    {
        if (strcmp(p->next->data.patientName, patientName) == 0)
        {
            delNodeByPatient(patientName);
            msg.type = 0;
            write(fd, &msg, sizeof(MSG));
            return;
        }
        p = p->next;
    }
    msg.type = 1;
    write(fd, &msg, sizeof(MSG));
}

/*医生登录函数*/
int doctorLogin(MSG msg, int fd)
{
    DOCTOR *doctor;
    doctor = (DOCTOR *)msg.buff;
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "SELECT *FROM doctor WHERE userName='%s' AND userPass='%s'", doctor->userName, doctor->userPass);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    int flag = 0;
    if (row > 0)
    {
        for (int i = 0; i < doctor_count; i++)
        {
            if (strcmp(doctor->userName, online_doctor[i].userName) == 0)
            {
                flag = -1;
                break;
            }
        }
        if (flag == -1)
        {
            printf("%s重复登录\n", doctor->userName);
            MSG msg;
            msg.type = 13; // 重复登录返回13标记
            write(fd, &msg, sizeof(msg));
        }
        else
        {
            printf("%s登录成功\n", doctor->userName);
            MSG msg;
            msg.type = 11; // 登录成功返回11标记
            online_doctor[doctor_count].stat = fd;
            strcpy(online_doctor[doctor_count].userName, doctor->userName);
            doctor_count++;
            printf("医生在线人数:%d\n", doctor_count);
            memcpy(msg.buff, doctor->userName, sizeof(doctor->userName));
            write(fd, &msg, sizeof(msg));
        }
    }
    else
    {
        printf("%s账号或密码错误\n", doctor->userName);
        MSG msg;
        msg.type = 12; // 登录失败返回12标记
        write(fd, &msg, sizeof(msg));
    }
}

/*管理员登录函数*/
int rootLogin(MSG msg, int fd)
{
    ROOT *root;
    root = (ROOT *)msg.buff;
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "SELECT *FROM root WHERE userName='%s' AND userPass='%s'", root->userName, root->userPass);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    int flag = 0;
    if (row > 0)
    {
        printf("%s登录成功\n", root->userName);
        MSG msg;
        msg.type = 11; // 登录成功返回11标记
        memcpy(msg.buff, root->userName, sizeof(root->userName));
        write(fd, &msg, sizeof(msg));
    }
    else
    {
        printf("%s账号或密码错误\n", root->userName);
        MSG msg;
        msg.type = 12; // 登录失败返回12标记
        write(fd, &msg, sizeof(msg));
    }
}

/*医生接诊函数*/
void doctorTreat(MSG msg, int fd)
{
    DOCTOR *doctor;
    doctor = (DOCTOR *)msg.buff;
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    // 通过医生账号查找到医生信息以便转发相应的挂号信息
    sprintf(sql, "SELECT doctorName, unitName FROM doctor WHERE userName='%s'", doctor->userName);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg); // 得到医生姓名和科室

    strcpy(doctor->doctorName, result[column]); // 填充医生结构体
    strcpy(doctor->unitName, result[column + 1]);
    LIST node = getHeadNode(list, doctor->doctorName); // 通过医生姓名得到队列第一个病人挂号信息
    memcpy(msg.buff, &node.data, sizeof(REG));
    write(fd, &msg, sizeof(msg)); // 将挂号信息发送给医生客户端
    printf("%s开始接诊%s病人\n", node.data.doctorName, node.data.patientName);
}

/*转发医生给病人消息*/
void sendDoctorToPatient(MSG msg, int fd)
{
    TREAT *treat;
    treat = (TREAT *)msg.buff;
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char userName[64];
    sprintf(sql, "SELECT userName FROM patient WHERE patientName='%s'", treat->patientName); // 根据病人名找到病人用户名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(userName, result[column]);
    int i;
    for (i = 0; i < patient_count; i++)
    {
        if (strcmp(online_patient[i].userName, userName) == 0) // 通过用户名找到病人fd
            break;
    }
    printf("转发[医生%s]->[病人%s]:%s\n", treat->doctorName, treat->patientName, treat->illDescrip);
    msg.type = 0;
    write(online_patient[i].stat, &msg, sizeof(msg));
}

/*转发病人给医生的消息*/
void sendPatientToDoctor(MSG msg, int fd)
{
    TREAT *treat;
    treat = (TREAT *)msg.buff;
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char userName[64];
    sprintf(sql, "SELECT userName FROM doctor WHERE doctorName='%s'", treat->doctorName); // 查找医生的userName
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(userName, result[column]);
    int i;
    for (i = 0; i < doctor_count; i++)
    {
        if (strcmp(online_doctor[i].userName, userName) == 0) // 通过userName找到对应的医生客户端fd
            break;
    }
    printf("转发[病人%s]->[医生%s]:%s\n", treat->patientName, treat->doctorName, treat->illDescrip);
    write(online_doctor[i].stat, &msg, sizeof(msg)); // 将消息转发给医生
}

/*医患交流群*/
void groupChat_(MSG msg, int fd)
{
    TREAT *treat;
    treat = (TREAT *)msg.buff;
    printf("%s\n", treat->illDescrip);
    if (strcmp(treat->illDescrip, "join") == 0)
    {
        groupChat[groupChatCount] = fd;
        groupChatCount++;
        msg.type = 0; // 加入群聊成功返回0
        write(fd, &msg, sizeof(MSG));
        strcpy(treat->illDescrip, "加入了群聊");
        printf("有人加入,群聊人数%d\n", groupChatCount);
    }
    if (strcmp(treat->illDescrip, "quit") == 0)
    {
        for (int i = 0; i < groupChatCount; i++)
        {
            if (fd == groupChat[i])
            {
                for (int j = i; j < groupChatCount - 1; j++)
                {
                    groupChat[j] = groupChat[j + 1];
                }
                groupChatCount--;
                printf("有人退出,群聊人数%d\n", groupChatCount);
            }
        }
    }
    for (int i = 0; i < groupChatCount; i++)
    {
        if (fd != groupChat[i])
        {
            write(groupChat[i], &msg, sizeof(msg));
            printf("转发[病人%s]:%s\n", treat->patientName, treat->illDescrip);
        }
    }
}

/*转发给医生的接诊记录*/
void printTreatDoctor(MSG msg, int fd)
{
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char doctorName[64];
    sprintf(sql, "SELECT doctorName FROM doctor WHERE userName='%s'", msg.buff); // 查找医生的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(doctorName, result[column]);
    sqlite3_free_table(result);

    sprintf(sql, "SELECT patientName, doctorName, treatTime, illDescrip FROM treat WHERE doctorName='%s'", doctorName); // 查找医生的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    for (int i = 1; i <= row; i++)
    {
        TREAT treat;
        strcpy(treat.patientName, result[i * column]);
        strcpy(treat.doctorName, result[i * column + 1]);
        strcpy(treat.treatTime, result[i * column + 2]);
        strcpy(treat.illDescrip, result[i * column + 3]);
        MSG msg;
        memcpy(msg.buff, &treat, sizeof(TREAT));
        write(fd, &msg, sizeof(msg));
    }
    sqlite3_free_table(result);
    TREAT treat;
    strcpy(treat.illDescrip, "over");
    memset(&msg, 0, sizeof(MSG));
    memcpy(msg.buff, &treat, sizeof(TREAT));
    write(fd, &msg, sizeof(msg));
}

/*转发给病人的就诊记录*/
void printTreatPatient(MSG msg, int fd)
{
    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char patientName[64];
    sprintf(sql, "SELECT patientName FROM patient WHERE userName='%s'", msg.buff); // 查找病人的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(patientName, result[column]);
    sqlite3_free_table(result);
    sprintf(sql, "SELECT patientName, doctorName, treatTime, illDescrip FROM treat WHERE patientName='%s'", patientName); // 查找病人的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    for (int i = 1; i <= row; i++)
    {
        TREAT treat;
        strcpy(treat.patientName, result[i * column]);
        strcpy(treat.doctorName, result[i * column + 1]);
        strcpy(treat.treatTime, result[i * column + 2]);
        strcpy(treat.illDescrip, result[i * column + 3]);
        MSG msg;
        memcpy(msg.buff, &treat, sizeof(TREAT));
        write(fd, &msg, sizeof(msg));
    }
    sqlite3_free_table(result);

    TREAT treat;
    strcpy(treat.illDescrip, "over");
    memset(&msg, 0, sizeof(MSG));
    memcpy(msg.buff, &treat, sizeof(TREAT));
    write(fd, &msg, sizeof(msg));
}

/*医生查看排队病人*/
void printRegPatient(MSG msg, int fd)
{
    REG reg;
    if (list->next == NULL)
    {
        msg.type = -1;
        strcpy(reg.patientName, "NULL");
        memcpy(msg.buff, &reg, sizeof(REG));
        write(fd, &msg, sizeof(MSG));
        return;
    }

    char sql[1024], **result = NULL, *errMsg;
    int row = 0, column = 0;
    char doctorName[64];
    sprintf(sql, "SELECT doctorName FROM doctor WHERE userName='%s'", msg.buff); // 查找医生的姓名
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    strcpy(doctorName, result[column]);

    LIST *p = list;
    int flag = -1;
    while (p->next)
    {
        if (strcmp(doctorName, p->next->data.doctorName) == 0)
        {
            memset(&msg, 0, sizeof(msg));
            memcpy(msg.buff, &p->next->data, sizeof(REG));
            flag = 0;
            write(fd, &msg, sizeof(MSG));
        }
        p = p->next;
    }
    if (flag == 0)
    {
        strcpy(reg.patientName, "over");
    }
    else
    {
        strcpy(reg.patientName, "NULL");
    }
    memcpy(msg.buff, &reg, sizeof(REG));
    write(fd, &msg, sizeof(MSG));
}

/*管理员查看医生信息*/
void printDoctorMessage(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024] = "SELECT userName, userPass, doctorName, unitName, titleName FROM doctor";
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    for (int i = 1; i <= row; i++)
    {
        DOCTOR doctor;
        strcpy(doctor.userName, result[i * column]);
        strcpy(doctor.userPass, result[i * column + 1]);
        strcpy(doctor.doctorName, result[i * column + 2]);
        strcpy(doctor.unitName, result[i * column + 3]);
        strcpy(doctor.titleName, result[i * column + 4]);
        MSG msg;
        memcpy(msg.buff, &doctor, sizeof(DOCTOR));
        write(fd, &msg, sizeof(msg));
    }

    DOCTOR doctor;
    strcpy(doctor.doctorName, "over");
    memset(&msg, 0, sizeof(MSG));
    memcpy(msg.buff, &doctor, sizeof(DOCTOR));
    write(fd, &msg, sizeof(msg));
}

/*管理员查看病人信息*/
void printPatientMessage(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024] = "SELECT userName, userPass, patientName, sex, age FROM patient";
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    for (int i = 1; i <= row; i++)
    {
        PATIENT patient;
        strcpy(patient.userName, result[i * column]);
        strcpy(patient.userPass, result[i * column + 1]);
        strcpy(patient.patientName, result[i * column + 2]);
        strcpy(patient.sex, result[i * column + 3]);
        patient.age = atoi(result[i * column + 4]);
        MSG msg;
        memcpy(msg.buff, &patient, sizeof(PATIENT));
        write(fd, &msg, sizeof(msg));
    }

    PATIENT patient;
    strcpy(patient.patientName, "over");
    memset(&msg, 0, sizeof(MSG));
    memcpy(msg.buff, &patient, sizeof(PATIENT));
    write(fd, &msg, sizeof(msg));
}

/*管理员查看诊断信息*/
void printTreatMessage(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024] = "SELECT patientName, doctorName, treatTime, illDescrip FROM treat";
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    for (int i = 1; i <= row; i++)
    {
        TREAT treat;
        strcpy(treat.patientName, result[i * column]);
        strcpy(treat.doctorName, result[i * column + 1]);
        strcpy(treat.treatTime, result[i * column + 2]);
        strcpy(treat.illDescrip, result[i * column + 3]);
        MSG msg;
        memcpy(msg.buff, &treat, sizeof(TREAT));
        write(fd, &msg, sizeof(msg));
    }

    TREAT treat;
    strcpy(treat.illDescrip, "over");
    memset(&msg, 0, sizeof(MSG));
    memcpy(msg.buff, &treat, sizeof(TREAT));
    write(fd, &msg, sizeof(msg));
}

/*管理员修改医生信息*/
void changeDoctorMessage(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024];
    DOCTOR *doctor;
    doctor = (DOCTOR *)msg.buff;
    sprintf(sql, "UPDATE doctor SET userPass='%s', doctorName='%s', sex='%s', age=%d, unitName='%s', titleName='%s', phoneNumber='%s' WHERE userName='%s';", doctor->userPass, doctor->doctorName, doctor->sex, doctor->age, doctor->unitName, doctor->titleName, doctor->phoneNumber, doctor->userName);
    sqlite3_exec(sq, sql, 0, 0, &errMsg);
    MSG responseMsg;
    responseMsg.type = 0; // 假设0表示成功
    write(fd, &responseMsg, sizeof(responseMsg));
}

/*管理员修改病人信息*/
void changePatientMessage(MSG msg, int fd)
{
    char **result = NULL, *errMsg;
    int row = 0, column = 0;
    char sql[1024];
    PATIENT *patient;
    patient = (PATIENT *)msg.buff;
    sprintf(sql, "UPDATE patient SET userPass='%s', patientName='%s', sex='%s', age=%d, phoneNumber='%s' WHERE userName='%s';", patient->userPass, patient->patientName, patient->sex, patient->age, patient->phoneNumber, patient->userName);
    sqlite3_exec(sq, sql, 0, 0, &errMsg);
    MSG responseMsg;
    responseMsg.type = 0; // 假设0表示成功
    write(fd, &responseMsg, sizeof(responseMsg));
}

/*添加医生*/
void addDoctor(MSG msg, int fd)
{
    DOCTOR *doctor;
    doctor = (DOCTOR *)msg.buff;
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "SELECT *FROM doctor WHERE userName='%s' OR doctorName='%s'", doctor->userName, doctor->doctorName);
    sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    if (row == 0)
    {
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "INSERT INTO doctor (userName,userPass,doctorName,sex,age,unitName,titleName,phoneNumber) VALUES ('%s','%s','%s','%s',%d,'%s','%s','%s');", doctor->userName, doctor->userPass, doctor->doctorName, doctor->sex, doctor->age, doctor->unitName, doctor->titleName, doctor->phoneNumber);
        sqlite3_exec(sq, sql, 0, 0, &errMsg);
        printf("%s添加成功\n", doctor->doctorName);
        MSG msg;
        msg.type = 15; // 注册成功返回15标记
        write(fd, &msg, sizeof(msg));
    }
    else
    {
        printf("%s添加失败,医生已存在\n", doctor->doctorName);
        MSG msg;
        msg.type = 16; // 注册失败账号已存在返回16
        write(fd, &msg, sizeof(msg));
    }
}

/*解雇医生*/
void deleteDoctor(MSG msg, int fd)
{
    char sql[1024], **result = NULL;
    int row = 0, column = 0;
    char *errMsg;
    sprintf(sql, "SELECT * FROM doctor WHERE doctorName='%s'", msg.buff);
    int rc = sqlite3_get_table(sq, sql, &result, &row, &column, &errMsg);
    if (rc != SQLITE_OK)
    {
        // 查询失败，返回错误信息给客户端
        MSG response;
        response.type = 1;
        write(fd, &response, sizeof(response));
        sqlite3_free(errMsg);
        sqlite3_free_table(result);
        return;
    }
    if (row == 0)
    {
        // 医生不存在，返回信息给客户端
        MSG response;
        response.type = 2;
        write(fd, &response, sizeof(response));
        sqlite3_free_table(result);
        return;
    }
    // 医生存在，执行删除操作
    sprintf(sql, "DELETE FROM doctor WHERE doctorName='%s'", msg.buff);
    rc = sqlite3_exec(sq, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        // 删除失败，返回错误信息给客户端
        MSG response;
        response.type = -1;
        write(fd, &response, sizeof(response));
        sqlite3_free(errMsg);
    }
    else
    {
        // 删除成功，返回成功信息给客户端
        MSG response;
        response.type = 0;
        write(fd, &response, sizeof(response));
    }
    sqlite3_free_table(result);
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

    // 设置套接字选项，允许在短时间内重复绑定同一个地址
    int val = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // 设置服务器地址
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = strToInt("192.168.150.131");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);

    // 绑定服务器地址
    int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        printf("bind失败\n");
        return 0;
    }

    // 注册信号处理函数，处理Ctrl+C关闭服务器
    signal(SIGINT, singleHandle);

    // 打开或创建数据库文件
    sq = NULL;
    char *errMsg = 0;
    ret = sqlite3_open("./hospital.db", &sq);
    if (ret < 0)
    {
        printf("打开数据库失败\n");
        return 0;
    }

    // 创建链表
    list = listCreate();

    // 监听套接字
    listen(sockfd, 5);

    // 创建epoll实例
    epollfd = epoll_create1(0);

    // 将监听套接字加入epoll监听
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
    printf("服务器已启动\n");
    while (1)
    {
        // 等待事件发生
        int nEvens = epoll_wait(epollfd, events, MAX_EVENTS, 0);
        for (int i = 0; i < nEvens; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                // 处理新连接
                struct sockaddr_in client;
                socklen_t len = sizeof(struct sockaddr_in);
                int newFd = accept(sockfd, (struct sockaddr *)&client, &len);
                if (newFd == -1)
                {
                    perror("接受连接失败");
                    continue;
                }

                // 将新连接加入epoll监听
                ev.data.fd = newFd;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, newFd, &ev);
                printf("fd:%d已连接\n", newFd);
            }
            else
            {
                // 处理客户端消息
                MSG msg;
                int ret = read(events[i].data.fd, &msg, sizeof(msg));
                if (ret == 0) // 客户端非正常退出
                {
                    printf("fd:%d已强制退出\n", events[i].data.fd);
                    ev.data.fd = events[i].data.fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    continue;
                }

                switch (msg.type)
                {
                case 1: // 登录
                    printf("病人登录\n");
                    patientLogin(msg, events[i].data.fd);
                    break;
                case 2: // 注册
                    printf("病人注册\n");
                    patientRegiste(msg, events[i].data.fd);
                    break;
                case 3: // 客户端退出
                    printf("fd:%d已正常退出\n", events[i].data.fd);
                    ev.data.fd = events[i].data.fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    break;
                case 4: // 医生登录
                    printf("医生登录\n");
                    doctorLogin(msg, events[i].data.fd);
                    break;
                case 5: // 客户端退出登录
                    printf("病人退出登录\n");
                    patientExit(msg, events[i].data.fd);
                    break;
                case 25: // 管理员登录
                    rootLogin(msg, events[i].data.fd);
                    break;
                case 21: // 医生端退出登录
                    printf("医生退出登录\n");
                    doctorExit(msg, events[i].data.fd);
                    break;
                case 22: // 医生查看接诊记录
                    printf("医生查看接诊记录\n");
                    printTreatDoctor(msg, events[i].data.fd);
                    break;
                case 23: // 病人查看就诊记录
                    printf("病人查看就诊记录\n");
                    printTreatPatient(msg, events[i].data.fd);
                    break;
                case 24: // 医生查看待诊病人
                    printf("医生查看待诊病人\n");
                    printRegPatient(msg, events[i].data.fd);
                    break;
                case 6: // 选择科室
                    printf("病人选择科室\n");
                    unitSelect(msg, events[i].data.fd);
                    break;
                case 7: // 查看排队人数
                    printf("病人查看排队人数\n");
                    patientLineMessage(msg, events[i].data.fd);
                    break;
                case 8: // 挂号
                    printf("病人挂号\n");
                    patientReg(msg, events[i].data.fd);
                    break;
                case 18:
                    printf("医患群聊\n");
                    groupChat_(msg, events[i].data.fd);
                    break;
                case 19:
                    printf("取消挂号\n");
                    cancelReg(msg, events[i].data.fd);
                    break;
                case 30:
                    printf("病人修改密码\n");
                    patientChangePasswd(msg, events[i].data.fd);
                    break;
                case 9: // 接诊
                    printf("医生接诊\n");
                    doctorTreat(msg, events[i].data.fd);
                    break;
                case 10: // 转发医生给病人的消息
                    printf("转发医生消息\n");
                    sendDoctorToPatient(msg, events[i].data.fd);
                    break;
                case 100: // 转发病人给医生的消息
                    printf("转发病人消息\n");
                    sendPatientToDoctor(msg, events[i].data.fd);
                    break;
                case 20: // 结束一次会诊删除一个节点
                    printf("删除就诊完的挂号节点\n");
                    TREAT *treat = (TREAT *)msg.buff;
                    saveTreatRecord(treat);
                    delNode(treat->doctorName);
                    break;
                case 50: // 管理员查看医生信息
                    printDoctorMessage(msg, events[i].data.fd);
                    break;
                case 51: // 管理员查看病人信息
                    printPatientMessage(msg, events[i].data.fd);
                    break;
                case 52: // 管理员查看诊断信息
                    printTreatMessage(msg, events[i].data.fd);
                    break;
                case 53: // 管理员修改医生信息
                    changeDoctorMessage(msg, events[i].data.fd);
                    break;
                case 54: // 管理员修改病人信息
                    changePatientMessage(msg, events[i].data.fd);
                    break;
                case 55: // 管理员退出登录
                    printf("管理员退出登录\n");
                    break;
                case 56: // 管理员添加医生
                    addDoctor(msg, events[i].data.fd);
                    break;
                case 57: // 管理员解雇医生
                    deleteDoctor(msg, events[i].data.fd);
                    break;
                default:
                    printf("输入有误请重新输入\n");
                    break;
                }
            }
        }
    }

    return 0;
}
