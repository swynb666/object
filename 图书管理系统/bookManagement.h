#ifndef __BOOKMANAGEMENT_H__
#define __BOOKMANAGEMENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct Book // 书籍信息综合
{
    int num; // 书籍编号
    char name[20]; // 书名
    char author[20]; // 作者名
    char publish[40]; // 出版社
    int price; // 书单价
    int total; // 书总数
    int lend; // 书借出数量
    int rest; // 书剩余数量
}Book;

typedef struct BookList // 书籍链表
{
    Book book;
    struct BookList *next;
}BookList;

#define DAY 10 // 借书期限

//创建一个书籍链表
BookList *creatBookList();
//保存书籍信息到文件
int saveBookMessage(BookList *bookL);
//加载书籍信息到链表
int loadBookMessage(BookList *bookL);
//借书
int borrowBook(BookList *bookL);
//还书
int rebackBook(BookList *bookL);
//查询
int getBookMessage(BookList *bookL);
//增加书籍
int addBook(BookList *bookL);
//修改书籍信息
int changeBookMessage(BookList *bookL);
//打印借书记录
int printBorrowMessage();
//打印还书信息
int printBackMessage();
//打印当前用户借书记录
int printNowUserBorrow();
//输出当前用户逾期记录
int printLateMessage();
//输出当前用户还书记录
int printNowUserBack();
//打印所有书籍信息
int printAllBooksMessage(BookList *bookL);
//获取当前时间并转为数字格式
int getCurrentDateAsNumber(int days);
//根据书名返回书籍编号
int getBookNum(BookList *bookL, char bookname[20]);

#endif