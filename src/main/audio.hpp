#pragma once
#include <SDL.h>
#include <SDL_main.h>

class Sound{
public:
    Uint8 *wav_data = NULL;
    Uint32 wav_data_len = 0;
    char *wav_path = NULL;
    SDL_AudioSpec spec;
    SDL_AudioStream *stream;
    bool paused = false;
    bool loop = false;

    Sound(SDL_AudioStream* stream, const char* file, bool paused, bool looped){
        this->stream = stream;
        this->paused = paused;
        this->loop = looped;

        SDL_asprintf(&wav_path, file, SDL_GetBasePath());  /* allocate a string of the full file path */
        if (!SDL_LoadWAV(wav_path, &spec, &wav_data, &wav_data_len)) {
            SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        }
    }
   
    void Play(){
        if (paused){
            return;
        }

        if (SDL_GetAudioStreamQueued(stream) < (int)wav_data_len) {
            SDL_PutAudioStreamData(stream, wav_data, wav_data_len);
            if(!loop){
                paused = true;
            }
        }       
    }
};