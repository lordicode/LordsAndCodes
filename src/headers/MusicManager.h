#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class MusicManager {
public:
    MusicManager();
    ~MusicManager();

    bool  loadMusic(const std::string& id, const std::string& filename);
    void  playMusic(const std::string& id, bool loop = true);
    void  pauseMusic();
    void  stopMusic();
    void  setMusicVolume(const std::string& id, float volume);
    bool  isMusicPlaying(const std::string& id) const;

    bool  loadSound(const std::string& id, const std::string& filename);
    void  playSound(const std::string& id, float volumeOverride = -1.f);
    void  setSoundVolume(float volume);
    void  stopAllSounds();

private:
    std::map<std::string, std::unique_ptr<sf::Music>> m_musicMap;
    std::string                                      m_currentMusicID;
    float                                            m_musicVolume = 100.f;

    std::map<std::string, sf::SoundBuffer>           m_soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>>          m_sounds;       
    float                                            m_soundVolume = 100.f;

    sf::Sound& obtainAvailableSound(const sf::SoundBuffer& buffer);
};
