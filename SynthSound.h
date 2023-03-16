/*
  ==============================================================================

    SynthSound.h
    Created: 21 Feb 2023 2:10:44pm
    Author:  hirait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class SynthSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};