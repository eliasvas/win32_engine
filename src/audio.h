#ifndef AUDIO_H
#define AUDIO_H

/*
void
play_sound(void)
{
        IDirectSound8 *directSound = NULL;
        IDirectSoundBuffer *primaryBuffer = NULL;
        IDirectSoundBuffer8 *secondaryBuffer = NULL;
        IDirectSoundBuffer *temporaryBuffer = NULL;

        DSBUFFERDESC bufferDescription = {};
        WAVEFORMATEX waveFormat = {};

        DirectSoundCreate8(NULL, &directSound, NULL);
        directSound->SetCooperativeLevel(WND, DSSCL_PRIORITY);

        bufferDescription.dwSize = sizeof(DSBUFFERDESC);
        bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
        bufferDescription.dwBufferBytes = 0;
        bufferDescription.dwReserved = 0;
        bufferDescription.lpwfxFormat = NULL;
        bufferDescription.guid3DAlgorithm = GUID_NULL;

        directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, NULL);

        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nSamplesPerSec = 44100;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nChannels = 2;
        waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
        
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveFormat.cbSize = 0;

        primaryBuffer->SetFormat(&waveFormat);

        bufferDescription.dwFlags = DSBCAPS_CTRLVOLUME;
        bufferDescription.dwBufferBytes = 44100 * 2 * 2;
        bufferDescription.lpwfxFormat = &waveFormat;

        directSound->CreateSoundBuffer(&bufferDescription, &temporaryBuffer, NULL);
        temporaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void **) &secondaryBuffer);
        temporaryBuffer->Release();

        uint8_t *buffer;
        DWORD bufferSize;
        secondaryBuffer->Lock(0, bufferDescription.dwBufferBytes, (void **) &buffer, &bufferSize, NULL, 0, 0);

        for (int i = 0; i < 44100; i++) {
            float f = sin((float) i / 8) * sin((float) i / 16);
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

*/

#endif
