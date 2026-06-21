#include "core/AudioSystem.h"
#include <iostream>

AudioSystem::AudioSystem() {
    // Intentar cargar los sonidos por defecto
    loadSound("move", "resources/audio/move.wav");
    loadSound("capture", "resources/audio/capture.wav");
    loadSound("check", "resources/audio/check.wav");
    loadSound("game_over", "resources/audio/game_over.wav");
}

AudioSystem& AudioSystem::getInstance() {
    static AudioSystem instance;
    return instance;
}

bool AudioSystem::loadSound(const std::string& name, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filepath)) {
        buffers[name] = buffer;
        return true;
    }
    std::cerr << "ERROR::AUDIOSYSTEM::FAILED TO LOAD " << filepath << std::endl;
    return false;
}

void AudioSystem::playSound(const std::string& name) {
    cleanup(); // Limpiar sonidos terminados

    if (buffers.find(name) != buffers.end()) {
        auto sound = std::make_unique<sf::Sound>(buffers[name]);
        sound->play();
        sounds.push_back(std::move(sound));
    }
}

void AudioSystem::cleanup() {
    // Eliminar sonidos que ya terminaron de reproducirse
    sounds.erase(
        std::remove_if(sounds.begin(), sounds.end(),
                       [](const std::unique_ptr<sf::Sound>& s) { return s->getStatus() == sf::Sound::Stopped; }),
        sounds.end());
}
