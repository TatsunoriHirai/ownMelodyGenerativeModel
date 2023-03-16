/*
  ==============================================================================

    AdsrData.h
    Created: 21 Feb 2023 2:06:01pm
    Author:  hirait

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AdsrData : public juce::ADSR {
public:
    void updateADSR(const float attack, const float decay, const float sustain, const float release);

private:
    juce::ADSR::Parameters adsrParams;
};
