/*
  ==============================================================================

    GenComponent.h
    Created: 13 Mar 2023 3:33:48pm
    Author:  hirait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ButtonComponent.h"

//==============================================================================
/*
*/
class GenComponent  : public juce::Component
{
public:
    GenComponent();
    ~GenComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    ButtonComponent genBtn;
    void modelChooseBtnClicked();
    bool visualizeGenLabel2{false};

    bool modelPathChosen{ false };
    std::string modelPath{""};
    juce::Label modelLabel;
    juce::Label genLabel1, genLabel2;

private:
    // generation components
    juce::TextButton modelChooseBtn;

    // generation positions
    int modelLabelPos[4]; // x,y,w,h
    int modelChooseBtnPos[4]; // x,y,w,h
    int genLabel1Pos[4], genLabel2Pos[4]; // x,y,w,h

    int genBtnPos[2]; // phrase generation button (x,y)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenComponent)
};
