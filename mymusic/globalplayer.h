// globalplayer.h
#ifndef GLOBALPLAYER_H
#define GLOBALPLAYER_H

#include<QObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class GlobalPlayer : public QObject
{
    Q_OBJECT
public:
    static GlobalPlayer* instance();

    QMediaPlayer* player();
    QMediaPlaylist* playlist();

private:
    GlobalPlayer();
    ~GlobalPlayer();

    QMediaPlayer* m_player;
    QMediaPlaylist* m_playlist;
};

#endif // GLOBALPLAYER_H
