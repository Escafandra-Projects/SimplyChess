#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <vector>

class AudioSystem {
private:
    std::map<std::string, sf::SoundBuffer> buffers;
    std::vector<sf::Sound> sounds;
    
    // Constructor privado para Singleton
    AudioSystem();
    ~AudioSystem() = default;

public:
    // Eliminar copias
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    // Obtener la instancia
    static AudioSystem& getInstance();

    // Cargar un sonido
    bool loadSound(const std::string& name, const std::string& filepath);

    // Reproducir un sonido
    void playSound(const std::string& name);
    
    // Limpiar sonidos que ya no se están reproduciendo
    void cleanup();
};
