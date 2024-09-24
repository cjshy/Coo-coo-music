// globalplayer.cpp
#include "globalplayer.h"

GlobalPlayer* GlobalPlayer::instance()
{
    static GlobalPlayer instance;
    return &instance;


}

GlobalPlayer::GlobalPlayer()
{
    m_player = new QMediaPlayer;
    m_playlist = new QMediaPlaylist;
    m_player->setPlaylist(m_playlist);
}

GlobalPlayer::~GlobalPlayer()
{
    delete m_player;
    delete m_playlist;
}

QMediaPlayer* GlobalPlayer::player()
{
    return m_player;
}

QMediaPlaylist* GlobalPlayer::playlist()
{
    return m_playlist;
}
