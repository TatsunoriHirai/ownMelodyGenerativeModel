/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NAP2023_hiraiAudioProcessor::NAP2023_hiraiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams())
#endif
{
    for (int i = 0; i < numMaxVoices; i++) {
        synth.addSound(new SynthSound());
        synth.addVoice(new SynthVoice());
    }
}

NAP2023_hiraiAudioProcessor::~NAP2023_hiraiAudioProcessor()
{
}

//==============================================================================
const juce::String NAP2023_hiraiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NAP2023_hiraiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NAP2023_hiraiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NAP2023_hiraiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NAP2023_hiraiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NAP2023_hiraiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NAP2023_hiraiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NAP2023_hiraiAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NAP2023_hiraiAudioProcessor::getProgramName (int index)
{
    return {};
}

void NAP2023_hiraiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NAP2023_hiraiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

void NAP2023_hiraiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NAP2023_hiraiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NAP2023_hiraiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int i = 0; i < synth.getNumVoices(); ++i) {
        //if (auto voice = dynamic_cast<juce::SynthesiserVoice*>(synth.getVoice(i))) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {

            // OSC
            auto& oscWaveChoice = *apvts.getRawParameterValue("OSC1WAVETYPE");
            auto& fmDepth = *apvts.getRawParameterValue("OSC1FMDEPTH");
            auto& fmFreq = *apvts.getRawParameterValue("OSC1FMFREQ");

            // ADSR
            auto& attack = *apvts.getRawParameterValue("ATTACK");
            auto& decay = *apvts.getRawParameterValue("DECAY");
            auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
            auto& release = *apvts.getRawParameterValue("RELEASE");

            // ADSR
            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERFREQ");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");

            // MOD ADSR
            auto& modattack = *apvts.getRawParameterValue("MODATTACK");
            auto& moddecay = *apvts.getRawParameterValue("MODDECAY");
            auto& modsustain = *apvts.getRawParameterValue("MODSUSTAIN");
            auto& modrelease = *apvts.getRawParameterValue("MODRELEASE");

            voice->getOscillator().setWaveType(oscWaveChoice);
            voice->getOscillator().setFmParams(fmDepth, fmFreq);
            voice->updateAdsr(attack, decay, sustain, release);
            voice->updateFilter(filterType.load(), cutoff.load(), resonance.load());
            voice->updateModAdsr(modattack, moddecay, modsustain, modrelease);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool NAP2023_hiraiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NAP2023_hiraiAudioProcessor::createEditor()
{
    return new NAP2023_hiraiAudioProcessorEditor (*this);
}

//==============================================================================
void NAP2023_hiraiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NAP2023_hiraiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NAP2023_hiraiAudioProcessor();
}

// Value Tree
juce::AudioProcessorValueTreeState::ParameterLayout NAP2023_hiraiAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Combobox: switch oscillator
    // OSC select
    params.push_back(std::make_unique<juce::AudioParameterChoice>("OSC1WAVETYPE", "Osc 1 Wave Type", juce::StringArray{ "Sine", "Saw", "Square" }, 2));

    // FM Osc Freq
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OSC1FMFREQ", "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.1f, 0.3f }, 0.0f, "Hz"));

    // FM Osc Depth
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OSC1FMDEPTH", "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f, 0.3f }, 0.0f));

    // ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.1f));

    //Filter
    params.push_back(std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray{ "Low-Pass", "Band-Pass", "High-Pass" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERFREQ", "Filter Freq", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 1600.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERRES", "Filter Resonance", juce::NormalisableRange<float> { 0.1f, 2.0f, 0.1f }, 1.0f));

    // Filter ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MODATTACK", "MOD Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MODDECAY", "MOD Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MODSUSTAIN", "MOD Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MODRELEASE", "MOD Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.2f));

    return { params.begin(), params.end() };
}
