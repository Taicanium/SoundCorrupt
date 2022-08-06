#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>

#include "AudioFile.h"
#include "GaussianNoise.hpp"

int main()
{
    AudioFile<double> audioFile;
    GaussianNoise gn;
    std::vector<double> longBuffer;

    printf("Enter a file path below:\n");
    std::string sIn;
    std::cin >> sIn;

    audioFile.load(sIn);
    gn.init();

    int channel = 0;
    int numSamples = audioFile.getNumSamplesPerChannel();
    int samplesPerSecond = audioFile.getNumSamplesPerChannel() / audioFile.getLengthInSeconds();
    printf("\nSamples: %d\nSamples per second: %d\nFile length: %f\n\n", numSamples, samplesPerSecond, audioFile.getLengthInSeconds());

    double szShortBuffer = samplesPerSecond / (24 + fmod(gn.noise(), 0.5));
    double szMediumBuffer = samplesPerSecond / (6 + fmod(gn.noise(), 0.5));
    double szLongBuffer = samplesPerSecond / (0.75 + fmod(gn.noise(), 0.5));
    longBuffer = std::vector<double>(szLongBuffer);

    printf("Populating buffer...\n\n");

    for (int i = 0; i < szLongBuffer; i++)
    {
        double sample = audioFile.samples[channel][i];
        longBuffer.emplace(longBuffer.begin(), sample);
        longBuffer.resize(szLongBuffer);
    }

    int samplesCompleted = szLongBuffer;
    int prevAction1 = -1;
    int prevAction2 = -1;

    while (samplesCompleted < numSamples)
    {
        longBuffer.emplace(longBuffer.begin(), audioFile.samples[channel][samplesCompleted]);
        longBuffer.resize(szLongBuffer);
        samplesCompleted++;
        int nextAction = floor(fmod(gn.noise() * 100.0, 8.0));
        while (nextAction == prevAction1 && nextAction == prevAction2)
        {
            nextAction = floor(fmod(gn.noise() * 100.0, 8.0));
        }
        prevAction2 = prevAction1;
        prevAction1 = nextAction;
        int i, repetition, beginningSample, silenceSample, silenceLength, silencePlaying;
        double sample, silenceNoise;
        switch (nextAction)
        {
        case 0: // Short stutter
            printf("Short stutter\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 24.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szShortBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szShortBuffer - 1 - i % (int)(szShortBuffer)];
                samplesCompleted++;
            }
            break;
        case 1: // Medium stutter
            printf("Medium stutter\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 12.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szMediumBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szMediumBuffer - 1 - i % (int)(szMediumBuffer)];
                samplesCompleted++;
            }
            break;
        case 2: // Long stutter
            printf("Long stutter\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 6.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szLongBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szLongBuffer - 1 - i % (int)(szLongBuffer)];
                samplesCompleted++;
            }
            break;
        case 3: // Refresh buffer.
            printf("Refresh buffer\n");
            longBuffer.clear();
            for (i = samplesCompleted; i < samplesCompleted + szLongBuffer; i++)
            {
                sample = audioFile.samples[channel][i % numSamples];
                longBuffer.emplace(longBuffer.begin(), sample);
                longBuffer.resize(szLongBuffer);
            }
        case 4: // Standard playthrough.
            printf("Standard playthrough\n");
            beginningSample = samplesCompleted;
            repetition = (((double)rand() / (double)RAND_MAX) * 0.5) + 0.25;
            for (i = beginningSample; i < beginningSample + szLongBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szLongBuffer - 1 - i % (int)(szLongBuffer)];
                samplesCompleted++;
            }
            break;
        case 5: // Interspersed static.
            printf("Interspersed static\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            silenceLength = gn.noise() * szLongBuffer * 0.75;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + silenceLength * repetition; i++)
            {
                silenceNoise = gn.noise();
                silencePlaying = fabs(silenceNoise) >= 1.0 ? ~silencePlaying : silencePlaying;
                silenceSample++;
                if (silencePlaying == 0)
                {
                    silenceSample = 0;
                }
                audioFile.samples[channel][samplesCompleted % numSamples] = silencePlaying ? longBuffer[szLongBuffer - 1 - silenceSample % (int)(szLongBuffer)] : 0.0;
                samplesCompleted++;
            }
            break;
        case 6: // Gaussian stutter.
            printf("Gaussian stutter\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 6.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            silenceLength = gn.noise() * szLongBuffer;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + silenceLength * repetition; i++)
            {
                silenceNoise = gn.noise() / 5.3;
                silencePlaying = fabs(silenceNoise) >= 1.0 ? ~silencePlaying : silencePlaying;
                silenceSample++;
                if (silencePlaying == 0)
                {
                    silenceSample = 0;
                }
                audioFile.samples[channel][samplesCompleted % numSamples] = silencePlaying ? longBuffer[szLongBuffer - 1 - silenceSample % (int)(szLongBuffer)] : 0.0;
                samplesCompleted++;
            }
            break;
        case 7: // Interspersed silence.
            printf("Interspersed silence\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            silenceLength = gn.noise() * szLongBuffer * 0.75;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + silenceLength * repetition; i++)
            {
                silenceNoise = gn.noise() / 5.0;
                silencePlaying = fabs(silenceNoise) >= 1.0 ? ~silencePlaying : silencePlaying;
                silenceSample++;
                if (silencePlaying == 0)
                {
                    audioFile.samples[channel][(beginningSample + silenceSample) % numSamples] = 0.0;
                }
                samplesCompleted++;
            }
            break;
        default: // Do nothing.
            break;
        }
    }

    printf("\nMixing down to mono...\n");

    std::vector<double> newSamples = std::vector<double>(numSamples);
    for (int i = 0; i < numSamples; i++)
    {
        newSamples[i] = audioFile.samples[channel][i];
    }
    audioFile.setNumChannels(1);
    for (int i = 0; i < numSamples; i++)
    {
        audioFile.samples[channel][i] = newSamples[i];
    }

    printf("\nSaving to fileOut.wav...\n");

    audioFile.save("fileOut.wav");

    return 0;
}
