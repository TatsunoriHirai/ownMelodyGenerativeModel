/*
  ==============================================================================

    pianoRollComponent.h
    Created: 21 Feb 2023 6:59:29pm
    Author:  hirait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class pianoRollComponent  : public juce::Component,
    private juce::MidiKeyboardState::Listener
{
public:
    pianoRollComponent(juce::MidiKeyboardState& stateToUse);
    ~pianoRollComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent& m) override;
    void mouseEnter(const juce::MouseEvent& m) override;
    void mouseExit(const juce::MouseEvent& m) override;
    void mouseDown(const juce::MouseEvent& m) override;
    void mouseUp(const juce::MouseEvent& m) override;
    void mouseDrag(const juce::MouseEvent& m) override;


    //int key_h; // width of each key in y-axis
    int wkey_h = -1; // width of each white key in y-axis
    float blackKeyWidthProportion = 0.0;
    int bkey_h = -1; // width of each black key in y-axis
    int key_w = -1; // width of each key in x-axis corresponding to 16th note length.
    int numKeys = -1; // number of keys in pianoroll
    int startKeyNum = -1; // midi note number of the lowest key
    int pointedNote = -1; // note number of pointing note on pianoroll
    std::vector<std::vector<int>> inputNotes;
    float curPosition = 0.0; // position of cursor on piano-roll (0.0-1.0)
    std::vector<bool> noteOnFlags, noteOffFlags; // flags for controling note on while playing melody
    int mode = 1; // (1: pen mode, 2: select mode)
    juce::MidiKeyboardState& state;
    int midiChannel = 1;
    float velocity = 1.0f;
    int maxMidiChannels = 5;
    std::vector<std::vector<int>> saveNotes;
    std::vector<int> highlight;

private:
    std::vector<float> keyInterval;
    int noteOnTiming = -1, noteOffTiming = -1;
    int curDragX = -1;
    bool isDragged = false;
    int mouseX;
    int pointingInputeNote = -1; // pointing input note
    int prevX; // previous x position when mouse is pressed
    bool dragInNote4LenMod = false; // state of input note length modification drag

    // pure virtual functions for MidiKeyboardState::Listener
    void handleNoteOn(juce::MidiKeyboardState*, int, int, float) override;
    void handleNoteOff(juce::MidiKeyboardState*, int, int, float) override;

    std::vector<std::vector<int>> duplicateCheck( std::vector<std::vector<int>> inputNotes );

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (pianoRollComponent)
};

