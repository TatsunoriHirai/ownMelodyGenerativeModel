/*
  ==============================================================================

    FilterData.cpp
    Created: 21 Feb 2023 2:07:08pm
    Author:  hirait

  ==============================================================================
*/

#include "FilterData.h"

void FilterData::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    filter.reset();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;

    filter.prepare(spec);

    isPrepared = true;
}

void FilterData::process(juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);

    juce::dsp::AudioBlock<float> block{ buffer };
    filter.process(juce::dsp::ProcessContextReplacing<float> {block});
}

void FilterData::updateParameters(const int filterType, const float frequency, const float resonance, const float modulator)
{
    switch (filterType) {
    case 0:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case 1:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    case 2:
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    }

    float modFreq = frequency * modulator;
    modFreq = std::fmin(std::fmax(modFreq, 20.0f), 20000.0f);

    filter.setCutoffFrequency(modFreq);
    filter.setResonance(resonance);

}

void FilterData::reset()
{
    filter.reset();
}

