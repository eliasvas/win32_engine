#ifndef AUDIO_H
#define AUDIO_H

#include "tools.h"
#include <windows.h>
#include <stdint.h>
#include <mmsystem.h>
#include <math.h>
#include <dsound.h>

//this is just to test out DirectSound, press [L] to hear something...
void
play_sound(void)
{
        IDirectSound8 *directSound = NULL;
        IDirectSoundBuffer *primaryBuffer = NULL;
        IDirectSoundBuffer8 *secondaryBuffer = NULL;
        IDirectSoundBuffer *temporaryBuffer = NULL;

        DSBUFFERDESC buffer_desc = {};
        WAVEFORMATEX wave_format = {};

        DirectSoundCreate8(NULL, &directSound, NULL);
        directSound->SetCooperativeLevel(WND, DSSCL_PRIORITY);

        buffer_desc.dwSize = sizeof(DSBUFFERDESC);
        buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
        buffer_desc.dwBufferBytes = 0;
        buffer_desc.dwReserved = 0;
        buffer_desc.lpwfxFormat = NULL;
        buffer_desc.guid3DAlgorithm = GUID_NULL;

        directSound->CreateSoundBuffer(&buffer_desc, &primaryBuffer, NULL);

        wave_format.wFormatTag = WAVE_FORMAT_PCM;
        wave_format.nSamplesPerSec = 44100;
        wave_format.wBitsPerSample = 16;
        wave_format.nChannels = 2;
        wave_format.nBlockAlign = (wave_format.wBitsPerSample / 8) * wave_format.nChannels;
        
        wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
        wave_format.cbSize = 0;

        primaryBuffer->SetFormat(&wave_format);

        buffer_desc.dwFlags = DSBCAPS_CTRLVOLUME;
        buffer_desc.dwBufferBytes = 44100 * 2 * 2;
        buffer_desc.lpwfxFormat = &wave_format;

        directSound->CreateSoundBuffer(&buffer_desc, &temporaryBuffer, NULL);
        temporaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void **) &secondaryBuffer);
        temporaryBuffer->Release();

        uint8_t *buffer;
        DWORD bufferSize;
        secondaryBuffer->Lock(0, buffer_desc.dwBufferBytes, (void **) &buffer, &bufferSize, NULL, 0, 0);

        for (int i = 0; i < 44100; i++) {
            float f = sin((float) i / 8) * cos((float) i / 16);
            int16_t a = (int16_t) (f * 10000);

            buffer[i * 4 + 0] = i >  22050 ? (a >> 0) : 0;
            buffer[i * 4 + 1] = i >  22050 ? (a >> 8) : 0;
            buffer[i * 4 + 2] = i <= 22050 ? (a >> 0) : 0;
            buffer[i * 4 + 3] = i <= 22050 ? (a >> 8) : 0;
        }

        secondaryBuffer->Unlock((void *) buffer, bufferSize, NULL, 0);

        secondaryBuffer->SetCurrentPosition(0);
        secondaryBuffer->SetVolume(DSBVOLUME_MAX);
        secondaryBuffer->Play(0, 0, 0);
}


#endif
