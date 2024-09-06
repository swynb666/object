#ifndef MESSAGE_H
#define MESSAGE_H
#include <string.h>
#include <QString>
struct QMSG
{
    char buf[1022]; // 存放各种类型消息
    short type; // 消息类型
};

struct REG // 注册信息结构体
{
    char name[128]; // 账号
    char passwd[128]; // 密码
    bool sex; // 性别

    REG(){}
    REG(const char *name, const char *passwd, bool sex)
    {
        strcpy(this->name, name);
        strcpy(this->passwd, passwd);
        this->sex = sex;
    }
};

struct song
{
    char songName[128];   //歌曲名
    char singerName[128];     //歌手
    char albumName[128]; //专辑
    int duration2;      //时间
    int musicId;       //音乐id
};

struct favoriteSong
{
    song Song;
    char userName[128]; // 所属用户
};


#endif // MESSAGE_H
