/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"

//==============================================================================
/**
*/
class NAP2023_hiraiAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    NAP2023_hiraiAudioProcessor();
    ~NAP2023_hiraiAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }

    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    juce::MidiKeyboardState keyboardState;
    juce::dsp::Oscillator<float> osc{ [](float x) {return x / juce::MathConstants<float>::pi; } }; // saw wave
    juce::dsp::Gain<float> gain;
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    int numMaxVoices = 16;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NAP2023_hiraiAudioProcessor)
};
