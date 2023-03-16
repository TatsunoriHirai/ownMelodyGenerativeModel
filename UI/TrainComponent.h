/*
  ==============================================================================

    TrainComponent.h
    Created: 13 Mar 2023 3:33:32pm
    Author:  hirait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ButtonComponent.h"
#include <filesystem>

//==============================================================================
/*
*/
class TrainComponent  : public juce::Component
{
public:
    TrainComponent();
    ~TrainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::TextButton trainDirChooseBtn;
    void trainDirChooseBtnClicked();
    ButtonComponent addTrainBtn;
    ButtonComponent trainBtn;

    bool trainDirChosen{ false };
    std::string savePath;
    juce::Label trainDirChooseLabel;
    juce::Label trainingLabel; // label for # of data in the folder
    juce::ComboBox trainingModelType; // LSTM, RNN, etc. selection

    int nTrainData{ 0 };

private:
    // training components
    juce::Label trainingModelTypeLabel;

    // training positions
    int trainDirLabelPos[4]; // x,y,w,h
    int trainDirChooseBtnPos[4]; // x,y,w,h
    int trainingModelTypePos[4]; // x,y,w,h
    int trainingModelTypeLabelPos[4]; // x,y,w,h
    int trainingLabelPos[4]; // x,y,w,h
    int addTrainBtnPos[2]; // add training bustton (x,y)
    int trainBtnPos[2]; // training bustton (x,y)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrainComponent)
};
