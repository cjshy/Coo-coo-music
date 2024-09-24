#ifndef COMMENT_H
#define COMMENT_H
#include<stdio.h>
#include<string.h>
#include<QList>
struct Qmesg
{
    int flag;
    char data[2044];
};

struct user_text
{
    user_text(const char *name,const char *passwd,const char *phone =NULL)
    {
        strcpy(this->name,name);
        strcpy(this->passwd,passwd);
        if(phone != NULL)
        {
            strcpy(this->Phone,phone);
        }
    }
    char Account[128];
    char name[128];
    char passwd[128];
    char Phone[128];
};

struct song_text
{
    song_text(const char *user_id = nullptr, const char *music_id = nullptr)
    {
        if (user_id != nullptr)
        {
            strncpy(this->user_id, user_id, sizeof(this->user_id) - 1);
            this->user_id[sizeof(this->user_id) - 1] = '\0'; // 确保字符串以 null 结尾
        }
        if (music_id != nullptr)
        {
            strncpy(this->music_id, music_id, sizeof(this->music_id) - 1);
            this->music_id[sizeof(this->music_id) - 1] = '\0'; // 确保字符串以 null 结尾
        }
    }
    char user_name[128];
    char user_id[128];
    char music_id[128];
    char music_name[128];
    char singer[128];
    char time[128];
    char zhuanji[128];
};

// QList<song_text> Mylove_list;
#endif // COMMENT_H
