#ifndef __USERLOGINANDREGISTER_H__
#define __USERLOGINANDREGISTER_H__

#include "bookManagement.h"

#define COUNT 10 // 用户一共可借数

typedef struct User
{
    char name[20]; // 用户账号
    char password[20]; // 用户密码
    int count; // 已借数量
    int borrow[COUNT]; // 保存借的书的编号
}User;

typedef struct UserList
{
    User user;
    struct UserList *next;
}UserList;

UserList *nowUser; // 当前用户

//创建一个用户链表
UserList *create();
//加载文件数据到链表
int loadUserMessage(UserList *userL, int flag);
//保存链表数据到文件
int saveUserMessage(UserList *userL);
//登录
int login(UserList *userL);
//注册
int registe(UserList *userL);
//输出所有用户信息
int printUserMessage(UserList *userL);
//修改当前用户密码
int changePassword(UserList *userL);
//清空缓存
void clearInputBuffer();
//显示登录界面
void mainMenu();
//用户登录成功界面
void userMenu(BookList *bookL, UserList *userL);
//管理员登录成功界面
void adminMenu(BookList *bookL, UserList *userL);
//等待函数
void wait();
#endif