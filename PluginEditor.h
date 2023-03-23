/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#include<fstream>
#include <filesystem>

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/OscComponent.h"
#include "UI/ButtonComponent.h"
#include "UI/pianoRollComponent.h"
#include "UI/GenComponent.h"
#include "UI/TrainComponent.h"


//==============================================================================
/**
*/
class NAP2023_hiraiAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce:: Timer
{
public:
    NAP2023_hiraiAudioProcessorEditor (NAP2023_hiraiAudioProcessor&);
    ~NAP2023_hiraiAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent& m) override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NAP2023_hiraiAudioProcessor& audioProcessor;

    bool keyPressed(const juce::KeyPress& key) override;
    void timerCallback() override;
    void saveMidiFile(std::vector<std::vector<int>> melody, std::string outputFileName);

    // components
    OscComponent osc;
    TrainComponent trainComponent;
    GenComponent genComponent;
    pianoRollComponent pianoRoll;
    juce::MidiKeyboardComponent keyboardComponent;
    ButtonComponent playbackBtn;
    ButtonComponent stopBtn;
    ButtonComponent saveBtn;
    ButtonComponent penBtn;
    ButtonComponent selectBtn;
    ButtonComponent clearBtn;
    ButtonComponent progressCursor;

    
    // positions
    int oscPos[4]; // x,y,w,h
    int componentW1; // width of osc and filter
    int componentW2; // width of adsr and modadsr
    int componentH; // height of components
    int trainBoxPos[4]; // x,y,w,h
    int genBoxPos[4]; // x,y,w,h

    // buttons
    int btnW, btnH; // button width and button height
    int playbackBtnPos[2]; // playback button (x,y)
    int stopBtnPos[2]; // stop button (x,y)
    int saveBtnPos[2]; // phrase save button (x,y)
    int btnW2, btnH2; // button width and button height for smaller button
    int penBtnPos[2]; // phrase generation button (x,y)
    int selectBtnPos[2]; // phrase generation button (x,y)
    int clearBtnPos[2]; // melody clear button (x,y)

    // piano roll GUI parameters
    int padding_x, padding_y;
    int pr_width, pr_height;
    int pr_pos[2]; // piano roll position [x, y]
    int cursorWidth = 5;

    // state (bool)
    bool isPlaying;
    bool isSave;
    bool isPen;
    bool isSelect;
    bool isHighlighted{ false };

    std::string savePath = "";
    std::string fileName = "training_melody";
    std::string modelPath = "";
    std::string exePath = "C:\\Users\\train_generate.exe"; // exe file path (generated from python code via pyinstaller)
    std::string modelType = "";

    std::vector<std::vector<int>> sortSaveNotes(std::vector<std::vector<int>> saveNotes);

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NAP2023_hiraiAudioProcessorEditor)
};
