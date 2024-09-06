#ifndef __HEAD_H__
#define __HEAD_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sqlite3.h>

#define MAX_EVENTS 1000
#define MAX_DOCTORS 5

/*医生信息结构体*/
typedef struct doctor
{
    char userName[64];     // 医生登录账号
    char userPass[64];     // 医生登录密码
    unsigned int doctorId; // 医生ID
    char doctorName[64];   // 姓名
    char sex[64];          // 性别
    unsigned int age;      // 年龄
    char unitName[64];     // 科室
    char titleName[64];    // 职称
    char phoneNumber[64];  // 电话
    int stat;              // 状态 fd在线 0离线
} DOCTOR;

/*科室信息结构体*/
typedef struct unit
{
    unsigned int unitId;            // 科室id
    char unitName[64];              // 科室名
    int doctorNum;                  // 科室医生数量
    DOCTOR unitDoctor[MAX_DOCTORS]; // 科室医生信息
} UNIT;

/*病人信息结构体*/
typedef struct patient
{
    char userName[64];      // 病人登录账号
    char userPass[64];      // 病人登录密码
    unsigned int patientId; // 病人ID
    char patientName[64];   // 姓名
    char sex[64];           // 性别
    unsigned int age;       // 年龄
    char phoneNumber[64];   // 电话
    int stat;               // 状态 fd在线 0离线
} PATIENT;

/*挂号信息结构体*/
typedef struct registered
{
    unsigned int regId;   // 挂号ID
    char patientName[64]; // 病人名字
    char doctorName[64];  // 医生名字
    char regUnitName[64]; // 科室名
    char regTime[64];     // 挂号时间
} REG;

/*接诊信息结构体*/
typedef struct treatment
{
    unsigned int treatId; // 接诊ID
    char patientName[64]; // 病人姓名
    char doctorName[64];  // 医生姓名
    char treatTime[64];   // 接诊时间
    char illDescrip[128]; // 病情描述
} TREAT;

typedef struct root
{
    char userName[64]; // 管理员账号
    char userPass[64]; // 管理员密码
} ROOT;

/*挂号消息队列结构体*/
typedef struct list
{
    REG data; // 消息队列数据
    struct list *next;
} LIST;

/*转发信息结构体*/
typedef struct message
{
    int type;        // 事件类型
    char buff[4092]; // 数据
} MSG;

#endif