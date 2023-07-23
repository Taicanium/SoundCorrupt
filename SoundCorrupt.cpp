#include "AudioFile.h"
#include "GaussianNoise.hpp"

int main()
{
    printf("Enter a WAV file path below:\n");
    std::string sIn;
    std::cin >> sIn;

    AudioFile<double> audioFile;
    GaussianNoise gn;
    std::vector<double> longBuffer;

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
    }
    longBuffer.resize(szLongBuffer);

    int samplesCompleted = szLongBuffer;
    int prevAction1 = -1;
    int prevAction2 = -1;

    while (samplesCompleted < numSamples)
    {
        longBuffer.emplace(longBuffer.begin(), audioFile.samples[channel][samplesCompleted]);
        longBuffer.resize(szLongBuffer);
        samplesCompleted++;
        int nextAction = floor(fmod(gn.noise() * 100.0, 13.0));
        while (nextAction == prevAction1)
        {
            nextAction = floor(fmod(gn.noise() * 100.0, 13.0));
        }
        int i, repetition, beginningSample, silenceSample, bufferLength, silencePlaying;
        double sample, silenceNoise;
        int designatedAction = -1; // We can set this to a certain value from the switch-cases below to test a new addition.
        switch (designatedAction == -1 ? nextAction : designatedAction)
        {
        case 0: // Short stutter
            printf("Short stutter\n");
            repetition = floor(fmod(gn.noise() * 0.5, (rand() + 1) * 8.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szShortBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szShortBuffer - 1 - i % (int)(szShortBuffer)];
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 1: // Medium stutter
            printf("Medium stutter\n");
            repetition = floor(fmod(gn.noise() * 2.0, (rand() + 1) * 8.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szMediumBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szMediumBuffer - 1 - i % (int)(szMediumBuffer)];
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 2: // Long stutter
            printf("Long stutter\n");
            repetition = floor(fmod(gn.noise() * 8.0, (rand() + 1) * 8.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            for (i = beginningSample; i < beginningSample + szLongBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szLongBuffer - 1 - i % (int)(szLongBuffer)];
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 3: // Refresh buffer.
            printf("Refresh buffer\n");
            longBuffer.clear();
            for (i = samplesCompleted; i < samplesCompleted + szLongBuffer; i++)
            {
                sample = audioFile.samples[channel][i % numSamples];
                longBuffer.emplace(longBuffer.begin(), sample);
            }
            longBuffer.resize(szLongBuffer);
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 4: // Standard playthrough.
            printf("Standard playthrough\n");
            beginningSample = samplesCompleted;
            repetition = (((double)rand() / (double)RAND_MAX) * 0.5) + 0.25;
            for (i = beginningSample; i < beginningSample + szLongBuffer * repetition; i++)
            {
                audioFile.samples[channel][samplesCompleted % numSamples] = longBuffer[szLongBuffer - 1 - i % (int)(szLongBuffer)];
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 5: // Interspersed static.
            printf("Interspersed static\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 2.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            bufferLength = gn.noise() * szLongBuffer * 0.35;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
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
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 6: // Gaussian stutter.
            printf("Gaussian stutter\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 6.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            bufferLength = gn.noise() * szLongBuffer;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
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
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        /*case 7: // Interspersed silence.
            printf("Interspersed silence\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            bufferLength = gn.noise() * szLongBuffer * 0.75;
            silencePlaying = 0;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
            {
                silenceNoise = gn.noise() / 5.0;
                silencePlaying = fabs(silenceNoise) >= 1.0 ? ~silencePlaying : silencePlaying;
                if (silencePlaying == 0)
                {
                    audioFile.samples[channel][i % numSamples] = 0.0;
                }
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;*/
        case 8: // Amplitude clipping.
            printf("Amplitude clipping\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            bufferLength = gn.noise() * szLongBuffer * 0.75;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
            {
                if (audioFile.samples[channel][i % numSamples] < 0.0)
                {
                    audioFile.samples[channel][i % numSamples] = 0.0;
                }
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 9: // Sample doubling.
            printf("Sample doubling\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            silenceSample = beginningSample;
            bufferLength = gn.noise() * szLongBuffer * 0.75;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i += 2)
            {
                audioFile.samples[channel][(i + 1) % numSamples] = audioFile.samples[channel][i % numSamples];
                samplesCompleted += 2;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 10: // Sample destruction.
            printf("Sample destruction\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            bufferLength = gn.noise() * szLongBuffer * 0.75;
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
            {
                if (audioFile.samples[channel][i % numSamples] <= 0.0)
                {
                    silenceNoise = -fabs(gn.noise() / 16.0);
                    audioFile.samples[channel][i % numSamples] = (silenceNoise < -1.0 ? -1.0 : silenceNoise) / 2.0;
                }
                else
                {
                    silenceNoise = fabs(gn.noise() / 16.0);
                    audioFile.samples[channel][i % numSamples] = (silenceNoise > 1.0 ? 1.0 : silenceNoise) / 2.0;
                }
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
            break;
        case 11: // Overlaid static.
            printf("Overlaid static\n");
            repetition = floor(fmod(gn.noise() * 100.0, (rand() + 1) * 4.0 / RAND_MAX));
            beginningSample = samplesCompleted;
            bufferLength = gn.noise() * szLongBuffer * 0.35;
            silenceSample = 0;
            silencePlaying = floor(((rand() + 1) * 900.0 / RAND_MAX) + 100.0);
            for (i = beginningSample; i < beginningSample + bufferLength * repetition; i++)
            {
                silenceNoise = floor(((rand() + 1) * (double)(silencePlaying) / RAND_MAX) + 3.0);
                silenceSample++;
                if (silenceSample > silenceNoise)
                {
                    audioFile.samples[channel][i % numSamples] = gn.noise() / 16.0;
                    silenceSample = 0;
                }
                samplesCompleted++;
            }
            prevAction2 = prevAction1;
            prevAction1 = nextAction;
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