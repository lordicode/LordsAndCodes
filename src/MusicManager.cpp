#include "MusicManager.h"
#include <iostream>
#include <algorithm>

// Define maximum number of concurrent sounds
const size_t MAX_SOUNDS = 16;

MusicManager::MusicManager() = default;
MusicManager::~MusicManager() = default;

bool MusicManager::loadMusic(const std::string& id, const std::string& filename) {
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filename)) {
        std::cerr << "Failed to load music: " << filename << std::endl;
        return false;
    }
    m_musicMap[id] = std::move(music);
    return true;
}

void MusicManager::playMusic(const std::string& id, bool loop) {
    // Stop current music if any is playing
    if (!m_currentMusicID.empty()) {
        auto currentIt = m_musicMap.find(m_currentMusicID);
        if (currentIt != m_musicMap.end()) {
            currentIt->second->stop();
        }
    }
    
    // Find and play new music
    auto it = m_musicMap.find(id);
    if (it == m_musicMap.end()) {
        std::cerr << "Music not found: " << id << std::endl;
        return;
    }
    
    it->second->setLooping(loop);
    it->second->setVolume(m_musicVolume);
    it->second->play();
    m_currentMusicID = id;
}

void MusicManager::pauseMusic() {
    if (!m_currentMusicID.empty()) {
        auto it = m_musicMap.find(m_currentMusicID);
        if (it != m_musicMap.end()) {
            it->second->pause();
        }
    }
}

void MusicManager::stopMusic() {
    if (!m_currentMusicID.empty()) {
        auto it = m_musicMap.find(m_currentMusicID);
        if (it != m_musicMap.end()) {
            it->second->stop();
        }
        m_currentMusicID.clear();
    }
}

void MusicManager::setMusicVolume(const std::string& id, float volume) {
    m_musicVolume = std::max(0.f, std::min(100.f, volume));  
    if (m_currentMusicID == id) {
        auto it = m_musicMap.find(id);
        if (it != m_musicMap.end()) {
            it->second->setVolume(m_musicVolume);
        }
    }
}

bool MusicManager::isMusicPlaying(const std::string& id) const {
    if (m_currentMusicID != id) {
        return false;
    }
    
    auto it = m_musicMap.find(id);
    if (it == m_musicMap.end()) {
        return false;
    }
    
    return it->second->getStatus() == sf::Music::Status::Playing;
}

bool MusicManager::loadSound(const std::string& id, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return false;
    }
    m_soundBuffers[id] = buffer;
    return true;
}

void MusicManager::playSound(const std::string& id, float volumeOverride) {
    auto it = m_soundBuffers.find(id);
    if (it == m_soundBuffers.end()) {
        std::cerr << "Sound not found: " << id << std::endl;
        return;
    }
    
    for (auto& sound : m_sounds) {
        if (sound->getStatus() != sf::Sound::Status::Playing) {
            sound->setBuffer(it->second);
            float volume = (volumeOverride >= 0.f) ? 
                          std::max(0.f, std::min(100.f, volumeOverride)) : m_soundVolume;
            sound->setVolume(volume);
            sound->play();
            return;
        }
    }
    
    if (m_sounds.size() < MAX_SOUNDS) {
        auto newSound = std::make_unique<sf::Sound>(it->second);
        float volume = (volumeOverride >= 0.f) ? 
                      std::max(0.f, std::min(100.f, volumeOverride)) : m_soundVolume;
        newSound->setVolume(volume);
        newSound->play();
        m_sounds.push_back(std::move(newSound));
    }
}

void MusicManager::setSoundVolume(float volume) {
    m_soundVolume = std::max(0.f, std::min(100.f, volume));
    for (auto& sound : m_sounds) {
        if (sound->getStatus() == sf::Sound::Status::Playing) {
            sound->setVolume(m_soundVolume);
        }
    }
}

void MusicManager::stopAllSounds() {
    for (auto& sound : m_sounds) {
        sound->stop();
    }
}

sf::Sound& MusicManager::obtainAvailableSound(const sf::SoundBuffer& buffer) {
    for (auto& sound : m_sounds) {
        if (sound->getStatus() != sf::Sound::Status::Playing) {
            sound->setBuffer(buffer);
            return *sound;
        }
    }
    
    if (m_sounds.size() < MAX_SOUNDS) {
        m_sounds.push_back(std::make_unique<sf::Sound>(buffer));
        return *m_sounds.back();
    }
    
    m_sounds[0]->setBuffer(buffer);
    return *m_sounds[0];
}