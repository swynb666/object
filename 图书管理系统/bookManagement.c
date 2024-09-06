#include "bookManagement.h"
#include "userLoginAndRegister.h"

/*
*功能：创建一个书籍信息链表
*参数: 无
*返回值: 
    头结点
*/
BookList *creatBookList()
{
    BookList *bookL = malloc(sizeof(BookList));
    memset(bookL, 0, sizeof(bookL));
    bookL->next = NULL;
    return bookL;
}

/*
*功能：将书籍信息保存到文件中
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/
int saveBookMessage(BookList *bookL)
{
    //1.判断链表是否为空
    if(bookL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.打开文件
    FILE *fp = fopen("./bookMessage.txt", "w+");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //3.循环将链表内容写入文件
    bookL = bookL->next;
    while(bookL)
    {
        fprintf(fp, "%d:%s:%s:%s:%d:%d:%d:%d:\n", bookL->book.num, bookL->book.name, bookL->book.author, bookL->book.publish, bookL->book.price, bookL->book.total, bookL->book.lend, bookL->book.rest);
        bookL = bookL->next;
    }
    //4.关闭文件
    fclose(fp);
    return 0;
}

/*
*功能：将书籍信息存到链表中
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/

int loadBookMessage(BookList *bookL)
{
    //1.打开用户信息文件、
    FILE *fp = fopen("./bookMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.循环读取并且格式化插入
    char buff[100] = {0};
    while(fgets(buff, sizeof(buff), fp))
    {
        BookList *newBook = (BookList *)malloc(sizeof(BookList));

        newBook->book.num = atoi(strtok(buff, ":"));
        strcpy(newBook->book.name, strtok(NULL, ":"));
        strcpy(newBook->book.author, strtok(NULL, ":"));
        strcpy(newBook->book.publish, strtok(NULL, ":"));
        newBook->book.price = atoi(strtok(NULL, ":"));
        newBook->book.total = atoi(strtok(NULL, ":"));
        newBook->book.lend = atoi(strtok(NULL, ":"));
        newBook->book.rest = atoi(strtok(NULL, ":"));

        newBook->next = bookL->next;
        bookL->next = newBook; 
    }
    //关闭文件
    fclose(fp);
    return 0;
}

/*
*功能：借书
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/
int borrowBook(BookList *bookL)
{
    //1.链表判空
    if(bookL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    if(nowUser->user.count == 10)
    {
        printf("\t\t\t\t\t你已经借10本书了先还书再借!!!\n");
        return -1;
    }

    //2.输入要借的书
    char bookname[20] = {0};
    printf("\t\t\t\t\t请输入你要借的书名:\n");
    printf("\t\t\t\t\t");
    scanf("%s", bookname);

    //3.遍历链表查询书籍
    bookL = bookL->next;
    while(bookL)
    {
        if(strcmp(bookL->book.name, bookname) == 0)
        {
            if(bookL->book.rest > 0)
            {
                printf("\t\t\t\t\t借书成功\n");
                bookL->book.rest -= 1;
                bookL->book.lend += 1;
                int time1 = getCurrentDateAsNumber(0);
                int time2 = getCurrentDateAsNumber(DAY);
                FILE *fp = fopen("./borrowMessage.txt", "a");
                fprintf(fp, "%s:%s:%d:%d:\n", nowUser->user.name, bookL->book.name, time1, time2);
                fclose(fp);
                nowUser->user.count += 1;
                nowUser->user.borrow[nowUser->user.count-1] = bookL->book.num;
            
                return 0;
            }
            else
            {
                printf("\t\t\t\t\t该书被借完了\n");
                return -1;
            }
        }
        bookL = bookL->next;
    }
    printf("\t\t\t\t\t没有这本书\n");
    return -1;
}

/*
*功能：获得借书日期
*参数: 无
*返回值: 
    成功返回借书日期，失败返回-1
*/
int getCurrentDateAsNumber(int days) 
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_mday += days;

    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;

    // 将年月日格式化成数字，例如：20231001
    int dateAsNumber = year * 10000 + month * 100 + day;
    return dateAsNumber;
}

/*
*功能：还书
*参数: 头结点
*返回值: 
    成功返回0，失败返回-1
*/
int rebackBook(BookList *bookL)
{
    printf("\t\t\t\t\t请输入你要还的书:\n");
    char bookname[20];
    printf("\t\t\t\t\t");
    scanf("%s", bookname);
    int num = getBookNum(bookL, bookname);
    //1.确认当前用户是否借了该书
    int flag = -1;
    for(int i=0; i<nowUser->user.count; i++)
    {
        if(nowUser->user.borrow[i] == num)
        {
            //将还书记录存储到文件
            FILE *fp = fopen("./backMessage.txt", "a");
            fprintf(fp, "%s:%s:%d:\n", nowUser->user.name, bookname, getCurrentDateAsNumber(0));
            fclose(fp);
            //确实借了，还书 
            nowUser->user.count -= 1;
            flag = 0;
        }
    }
    if(flag == -1)
    {
        printf("\t\t\t\t\t你没有借《%s》\n", bookname);
        return -1;
    }
    //2.找到书籍在链表中的位置
    bookL = bookL->next;
    while(bookL)
    {
        if(bookL->book.num == num)
        break;
        bookL = bookL->next;
    }
    //3.修改书籍信息
    bookL->book.lend -= 1;
    bookL->book.rest += 1;
    printf("\t\t\t\t\t还书成功\n");
    return 0;
}

/*
*功能：根据书名返回书籍号
*参数: 头结点
*返回值: 
    成功返回书籍号，失败返回-1
*/
int getBookNum(BookList *bookL, char bookname[20])
{
    bookL = bookL->next;
    while(bookL)
    {
        if(strcmp(bookL->book.name, bookname) == 0)
        {
            return bookL->book.num;
        }
        bookL = bookL->next;
    }
    return -1;
}

/*
*功能：查找作者和书名
*参数: 头结点， 查找关键字
*返回值: 
    成功返回0，失败返回-1
*/
int getBookMessage(BookList *bookL)
{
    //1.输出查找关键字
    char key[20];
    printf("\t\t\t\t\t请输入关键字查找:\n");
    printf("\t\t\t\t\t");
    scanf("%s", key);
    bookL = bookL->next;
    int flag = -1;
    while(bookL)
    {
        if(strcmp(bookL->book.author, key) == 0 || strcmp(bookL->book.name, key) == 0)
        {
            flag = 0;
            printf("\t\t\t《%-*s》———%s   剩余%d\n", 10, bookL->book.name, bookL->book.author, bookL->book.rest);
        }
        bookL = bookL->next;
    }
    if(flag != 0)
    printf("\t\t\t\t\t未找到该书或者作者相关信息\n");
}

/*
*功能：打印借书记录
*参数: 无
*返回值: 
成功返回0，失败返回-1
*/
int printBorrowMessage(BookList *bookL)
{
    //1.打开借书信息文件
    FILE *fp = fopen("./borrowMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.循环输出
    char buff[100];
    while(fgets(buff, sizeof(buff), fp))
    {
        printf("\t\t\t%-*s-", 4, strtok(buff, ":"));
        printf("《%-*s》",10, strtok(NULL, ":"));
        int data = atoi(strtok(NULL, ":"));
        int day = data % 100;
        int month = (data % 10000) / 100;
        int year = data / 10000;
        printf("借书日:%5d年%3d月%3d日-", year, month, day);

        data = atoi(strtok(NULL, ":"));
        day = data % 100;
        month = (data % 10000) / 100;
        year = data / 10000;
        printf("应还日:%5d年%3d月%3d日\n", year, month, day);
    }
    //3.关闭文件
    fclose(fp);
    printf("\t\t\t\t\t以上全部借书记录\n");
    return 0;
}
/*
*功能：打印所有书籍信息
*参数: 头结点
*返回值: 
成功返回0，失败返回-1
*/
int printAllBooksMessage(BookList *bookL)
{
    if(bookL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }

    bookL = bookL->next;
    while (bookL)
    {
        printf("\t\t\t《%-*s》%s-剩余%2d\n", 10, bookL->book.name, bookL->book.author, bookL->book.rest);
        bookL = bookL->next;    
    }
    
    return 0;
}

/*
*功能：打印还书信息
*参数: 无
*返回值: 
成功返回0，失败返回-1
*/
int printBackMessage()
{
    //1.打开还书信息文件
    FILE *fp = fopen("./backMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.循环输出到终端
    char buff[100];
    while(fgets(buff, sizeof(buff), fp))
    {
        printf("\t\t\t%-*s\t", 4, strtok(buff, ":"));
        printf("《%-*s》\t", 10, strtok(NULL, ":"));
        int data = atoi(strtok(NULL, ":"));
        int day = data % 100;
        int month = (data % 10000) / 100;
        int year = data / 10000;
        printf("还书日:%5d年%3d月%3d日\n", year, month, day);
    }
    fclose(fp);
    return 0;
}

/*
*功能：增加书籍
*参数: 头结点
*返回值: 
成功返回0，失败返回-1
*/
int addBook(BookList *bookL)
{
    //1.创建一个节点
    BookList *newBook = malloc(sizeof(BookList));
    printf("\t\t\t\t\t请输入书籍编号:\n");
    scanf("%d", &(newBook->book.num));
    printf("\t\t\t\t\t请输入书名:\n");
    scanf("%s", newBook->book.name);
    printf("\t\t\t\t\t请输入作者:\n");
    scanf("%s", newBook->book.author);
    printf("\t\t\t\t\t请输入出版社:\n");
    scanf("%s", newBook->book.publish);
    printf("\t\t\t\t\t请输入价格:\n");
    scanf("%d", &(newBook->book.price));
    printf("\t\t\t\t\t请输入总数:\n");
    scanf("%d", &(newBook->book.total));
    newBook->book.lend = 0;
    newBook->book.rest = newBook->book.total;

    //2.插入链表
    newBook->next = bookL->next;
    bookL->next = newBook;
    saveBookMessage(bookL);
    printf("\t\t\t已成功添加书籍《%-*s》\n", 10, newBook->book.name);
    return 0;
}

/*
*功能：修改书籍信息
*参数: 头结点
*返回值: 
成功返回0，失败返回-1
*/
int changeBookMessage(BookList *bookL)
{
    //1.链表判空
    if(bookL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.根据书名修改
    char bookname[20];
    printf("\t\t\t\t\t请输入要改的书名:\n");
    scanf("%s", bookname);
    
    //3.查找该书名节点
    BookList *saveBookL = bookL;
    bookL = bookL->next;
    while (bookL)
    {
        if(strcmp(bookL->book.name, bookname) == 0)
        {
            printf("\t\t\t\t\t已找到书籍\n");
            break;
        }
        bookL = bookL->next;
    }
    //4.修改数据
    printf("\t\t\t\t\t请输入编号:\n");
    scanf("%d", &(bookL->book.num));
    printf("\t\t\t\t\t请输入书名:\n");
    scanf("%s", bookL->book.name);
    printf("\t\t\t\t\t请输入作者:\n");
    scanf("%s", bookL->book.author);
    printf("\t\t\t\t\t请输入出版社:\n");
    scanf("%s", bookL->book.publish);
    printf("\t\t\t\t\t请输入价格:\n");
    scanf("%d", &(bookL->book.price));
    printf("\t\t\t\t\t请输入总数:\n");
    scanf("%d", &(bookL->book.total));
    printf("\t\t\t\t\t请输入借出:\n");
    scanf("%d", &(bookL->book.lend));
    printf("\t\t\t\t\t请输入剩余:\n");
    scanf("%d", &(bookL->book.rest));

    saveBookMessage(saveBookL);
    printf("\t\t\t已成功修改书籍《%-*s》\n", 10,  bookL->book.name);
    return 0;
}

/*
*功能：输出当前用户借书记录
*参数: 无
*返回值: 
成功返回0，失败返回-1
*/
int printNowUserBorrow()
{
    //1.打开借书记录文件
    FILE *fp = fopen("./borrowMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }

    //2.筛选当前用户的借书记录
    char buff[100];
    int flag = -1;
    while (fgets(buff, sizeof(buff), fp))   
    {
        if(strcmp(nowUser->user.name, strtok(buff, ":")) == 0)
        {
        printf("\t\t\t%-*s-", 10,  strtok(NULL, ":"));
        int data = atoi(strtok(NULL, ":"));
        int day = data % 100;
        int month = (data % 10000) / 100;
        int year = data / 10000;
        printf("借书日:%5d年%3d月%3d日-", year, month, day);

        data = atoi(strtok(NULL, ":"));
        day = data % 100;
        month = (data % 10000) / 100;
        year = data / 10000;
        printf("应还日:%5d年%3d月%3d日\n", year, month, day);
        flag = 0;
        }
    }
    fclose(fp);
    if(flag == -1)
    {
        printf("\t\t\t\t\t暂无借书记录\n");
        return 0;
    }
    return 0;
}

/*
*功能：输出逾期记录
*参数: 无
*返回值: 
成功返回0，失败返回-1
*/
int printLateMessage()

{
    //1.打开借书记录文件
    FILE *fp = fopen("./borrowMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.筛选当前用户的借书记录
    char buff[100];
    char bookname[20];
    int data1 = getCurrentDateAsNumber(0); // 获取当前时间
    int flag = -1;
    while (fgets(buff, sizeof(buff), fp))   
    {
        if(strcmp(nowUser->user.name, strtok(buff, ":")) == 0)
        {
            strcpy(bookname, strtok(NULL, ":"));
            (strtok(NULL, ":"));
            int data2 = atoi(strtok(NULL, ":")); // 应还书时间
            if(data1 > data2)
            {
                printf("\t\t\t《%-*s》  已逾期\n", 10, bookname);
            }
        flag = 0;
        }
    }
    fclose(fp);
    if(flag == -1)
    {
        printf("\t\t\t\t\t暂无逾期记录\n");
        return 0;
    }
    return 0;
}

/*
*功能：输出当前用户还书信息
*参数: 无
*返回值: 
成功返回0，失败返回-1
*/
int printNowUserBack()
{
     //1.打开还书信息文件
    FILE *fp = fopen("./backMessage.txt", "r");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.循环输出到终端
    char buff[100];
    while(fgets(buff, sizeof(buff), fp))
    {
        if(strcmp(strtok(buff, ":"), nowUser->user.name) == 0)
        {
            printf("\t\t《%-*s》\t", 10, strtok(NULL, ":"));
            int data = atoi(strtok(NULL, ":"));
            int day = data % 100;
            int month = (data % 10000) / 100;
            int year = data / 10000;
            printf("还书日:%5d年%3d月%3d日\n", year, month, day);
        }
    }
    fclose(fp);
    return 0;
}