#include "userLoginAndRegister.h"
/*
*功能：创建一个带头结点的用户链表
*参数: 无
*返回值: 
    成功返回头结点，失败返回空
*/
UserList *create()
{
    UserList *userL = malloc(sizeof(UserList));
    if(userL == NULL)
    {
        printf("%s\n", __func__);
        return NULL;
    }
    memset(userL, 0, sizeof(UserList));
    userL->next = NULL;
    return userL;
}

/*
*功能：将用户信息保存到文件中
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/

int saveUserMessage(UserList *userL)
{
    //1.判断链表是否为空
    if(userL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.打开文件
    FILE *fp = fopen("./userMessage.txt", "w+");
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //3.循环将链表内容写入文件
    userL = userL->next;
    while(userL)
    {
        fprintf(fp, "%s:%s:%d:", userL->user.name, userL->user.password, userL->user.count);
        for(int i=0; i<userL->user.count; i++)
        {
            fprintf(fp, "%d:", userL->user.borrow[i]);
        }
        fputc('\n', fp);
        userL = userL->next;
    }
    //4.关闭文件
    fclose(fp);
    return 0;
}


/*
*功能：从文件中读取用户数据到并插入到链表
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/
int loadUserMessage(UserList *userL, int flag)
{
    //1.打开用户信息文件、
    FILE *fp;
    if(flag == 3)
    {
        fp = fopen("./adminMessage.txt", "r");
    }
    else
    {
        fp = fopen("./userMessage.txt", "r");
    }
    if(fp == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    //2.循环读取并且格式化插入
    char buff[100] = {0};
    free(userL);
    userL = create();
    while(fgets(buff, sizeof(buff), fp))
    {
        UserList *userNode = (UserList *)malloc(sizeof(UserList));
        memset(userNode, 0, sizeof(UserList));
        strcpy(userNode->user.name, strtok(buff, ":"));
        strcpy(userNode->user.password, strtok(NULL, ":"));
        userNode->user.count = atoi(strtok(NULL, ":"));
        for(int i=0; i<userNode->user.count; i++)
        {
            userNode->user.borrow[i] = atoi(strtok(NULL, ":"));
        }

        userNode->next = userL->next;
        userL->next = userNode;
    }
    //关闭文件
    fclose(fp);
    return 0;
}

/*
*功能：登录
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/
int login(UserList *userL)
{
    //1.判断链表为空
    if(userL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }

    //2.先让用户输入账号密码
    char name[20] = {0};
    char password[20] = {0};
    printf("\t\t\t\t\t请输入账号:\n");
    printf("\t\t\t\t\t");
    scanf("%s", name);
    printf("\t\t\t\t\t请输入密码:\n");
    printf("\t\t\t\t\t");
    scanf("%s", password);

    //3.遍历用户链表节点核对信息
    userL = userL->next;
    while(userL)
    {
        if((strcmp(userL->user.name, name) == 0) && (strcmp(userL->user.password, password) == 0))
        {
            printf("\t\t\t\t\t登录成功!!!\n");
            nowUser = userL;
            return 0;
        }
        userL = userL->next;
    }
    printf("\t\t\t\t\t账号或密码错误\n");
    return -1;
}

/*
*功能：注册
*参数: 头指针
*返回值: 
    成功返回0，失败返回-1
*/
int registe(UserList *userL)
{
    //1.让用户输入账号判断账号是否存在
    char name[20] = {0};
    printf("\t\t\t\t\t请输入账号:\n");
    printf("\t\t\t\t\t");
    scanf("%s", name);
    UserList *saveUserL = userL;
    userL = userL->next;
    while(userL)
    {
        if(strcmp(userL->user.name, name) == 0)
        {
            printf("\t\t\t\t\t账号已存在!!!\n");
            return -1;
        }
        userL = userL->next;
    }
    //2.账号存在输入密码
    char passwd[20] = {0};
    printf("\t\t\t\t\t请输入密码:\n");
    printf("\t\t\t\t\t");
    scanf("%s", passwd);

    //3.将信息点插入用户信息链表
    UserList *newNode = malloc(sizeof(UserList));
    memset(newNode, 0, sizeof(UserList));
    strcpy(newNode->user.name, name);
    strcpy(newNode->user.password, passwd);
    newNode->next = saveUserL->next;
    saveUserL->next = newNode;
    return 0;
}

/*
*功能：输出用户信息
*参数: 链表头
*返回值: 
    成功0，失败-1
*/
int printUserMessage(UserList *userL)
{
    if(userL->next == NULL)
    {
        printf("%s\n", __func__);
        return -1;
    }
    userL = userL->next;
    while(userL)
    {
        printf("\t\t\t账号:%6s ", userL->user.name);
        printf("密码:%6s ", userL->user.password);
        printf("已借:%2d ", userL->user.count);
        for(int i=0; i<userL->user.count; i++)
        printf("编号%4d ", userL->user.borrow[i]);
        putchar(10);
        userL = userL->next;
    }
    return 0;
}

/*
*功能：当前用户修改密码
*参数: 无
*返回值: 
    成0， 失败-1
*/
int changePassword(UserList *userL)
{
    char buff[20];
    char newPassword1[20];
    char newPassword2[20];
    //1.判断原密码是否正确
    printf("\t\t\t\t\t请输入原密码:\n");
    scanf("%s", buff);

    //2.输入两次新密码
    if(strcmp(buff, nowUser->user.password) == 0)
    {
        while (1)
        {
        printf("\t\t\t\t\t请输入新密码:\n");
        scanf("%s", newPassword1);
        printf("\t\t\t\t\t请确认新密码:\n");
        scanf("%s", newPassword2);
        if(strcmp(newPassword1, newPassword2) == 0)
        {
            strcpy(nowUser->user.password, newPassword1);
            printf("\t\t\t\t\t新密码是:%s\n", nowUser->user.password);
            printf("\t\t\t\t\t修改密码成功!!!\n");
            saveUserMessage(userL);
            return 0;
        }
        else
        {
            printf("\t\t\t\t\t两次密码不统一请重新输入:\n");
        }
        }
        
    }
    else
    {
        printf("\t\t\t\t\t密码错误\n");
        return -1;
    }
    
}

//等待函数
void wait()
{
    printf("\t\t\t输入回车继续>>");
    clearInputBuffer();
    char c;
    while(c != '\n')
    {
        c = getchar();
    }
    return;
}

//清空缓存区
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
*功能：显示登录界面
*参数: 无
*返回值: 
    无
*/
void mainMenu()
{
    
    UserList *userL = create();
    BookList *bookL = creatBookList();
    loadBookMessage(bookL);
    char input[10];
    int n;
    while(1)
    {
        printf("\t\t\t*************豪大大图书馆*************\n");
        printf("\t\t\t********1.登录     \t2.注册********\n");
        printf("\t\t\t********3.管理员登录\t0.退出********\n");
        printf("\t\t\t请输入选项:\n");
        if(fgets(input, sizeof(input), stdin)!= NULL) 
        {
            if(sscanf(input, "%d", &n) != 1) 
            {
                printf("\t\t\t\t\t输入有误请重新输入!!!\n");
                clearInputBuffer();
                continue;
            }
        } 
        else
        {
            printf("\t\t\t\t\t输入有误请重新输入!!!\n");
            clearInputBuffer();
            continue;
        }
    loadUserMessage(userL, n);
    switch (n)
    {
    case 1:
        if(login(userL) == 0)
        userMenu(bookL, userL);
        break;
    case 2:
        if(registe(userL) == 0);
        saveUserMessage(userL);
        break;
    case 3:
        if(login(userL) == 0)
        adminMenu(bookL, userL);
        break;
    case 0:
        printf("\t\t\t\t\t已退出\n");
        saveBookMessage(bookL);
        saveUserMessage(userL);
        free(userL);
        free(bookL);
        return;
    default:
        printf("\t\t\t\t\t输入有误请重新输入!!!\n");
    break;
    }
    clearInputBuffer();
    }
}

/*
*功能：普通用户登录成功后界面
*参数: 无
*返回值: 
    无
*/
void userMenu(BookList *bookL, UserList *userL)
{
    int n;
    while(1)
    {
        
        printf("\t\t\t********1.借书    \t2.还书***********\n");
        printf("\t\t\t********3.查书    \t4.书库***********\n");
        printf("\t\t\t********5.借阅记录\t6.逾期记录********\n");
        printf("\t\t\t********7.还书记录\t8.修改密码********\n");
        printf("\t\t\t********0.返回上级\n");
        printf("\t\t\t请输入选项:\n");
        scanf("%d", &n);
        switch(n)
        {
            case 1:
                borrowBook(bookL);
                saveUserMessage(userL);
                break;
            case 2:
                rebackBook(bookL);
                saveUserMessage(userL);
                break;
            case 3:
                getBookMessage(bookL);
                wait();
                break;
            case 4:
                printAllBooksMessage(bookL);
                wait();
                break;
            case 5:
                printNowUserBorrow();
                wait();
                break;
            case 6:
                //逾期记录
                printLateMessage();
                wait();
                break;
            case 7:
                printNowUserBack();
                wait();
                break;
            case 8:
                changePassword(userL);
                break;
            case 0:
                return;
            default:
                printf("\t\t\t\t\t输入有误请重新输入\n");
        }
    }
    
    
    
}

/*
*功能：管理员登录成功后界面
*参数: 无
*返回值: 
    无
*/
void adminMenu(BookList *bookL, UserList *userL)
{   
    int n;
    
    while(1)
    {
        printf("\t\t\t********1.显示所有书籍信息\t2.修改书籍信息\n");
        printf("\t\t\t********3.添加书籍        \t4.显示借书信息\n");
        printf("\t\t\t********5.显示还书信息     \t6.用户信息\n");
        printf("\t\t\t********0.返回\n");
        printf("\t\t\t请输入选项:\n");
        scanf("%d", &n);
        switch (n)
        {
        case 1:
            printAllBooksMessage(bookL);
            wait();
            break;
        case 2:
            changeBookMessage(bookL);
            break;
        case 3:
            addBook(bookL);
            break;
        case 4:
            printBorrowMessage();
            wait();
            break;
        case 5:
            printBackMessage();
            wait();
            break;
        case 6:
            loadUserMessage(userL, 0);
            printUserMessage(userL);
            wait();
            break;
        case 0:
            return;
        default:
            printf("\t\t\t\t\t输入有误请重新输入\n");
            break;
        }
    }
}

