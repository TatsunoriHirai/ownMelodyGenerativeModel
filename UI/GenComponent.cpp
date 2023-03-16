/*
  ==============================================================================

    GenComponent.cpp
    Created: 13 Mar 2023 3:33:48pm
    Author:  hirait

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GenComponent.h"

//==============================================================================
GenComponent::GenComponent():
    genBtn(juce::ImageCache::getFromMemory(BinaryData::gen_png, BinaryData::gen_pngSize), true)
    , modelChooseBtn("select a model")
{
    modelChooseBtn.onClick = [this] { modelChooseBtnClicked();  };

    addAndMakeVisible(genBtn);
    modelLabel.setText("select a model for melody generation.", juce::dontSendNotification);
    modelLabel.setFont(15.0f);
    modelLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
    modelLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    genLabel1.setText("melody generation model: ", juce::dontSendNotification);
    genLabel1.setFont(15.0f);
    genLabel1.setColour(juce::Label::textColourId, juce::Colours::white);
    genLabel2.setText("Next step: Specify the range for melody generation.", juce::dontSendNotification);
    genLabel2.setFont(15.0f);
    genLabel2.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(modelLabel);
    addAndMakeVisible(modelChooseBtn);
    addAndMakeVisible(genLabel1);
    addAndMakeVisible(genLabel2);

    genBtn.isActive = false;
}

GenComponent::~GenComponent()
{
}

void GenComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    //auto bounds = getLocalBounds();
    auto labelSpace = bounds.removeFromTop(20.0f);

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Melody generation settings", labelSpace.withX(5), juce::Justification::left);
    g.drawRoundedRectangle(bounds.toFloat(), 5.0f, 2.0f);
}

void GenComponent::resized()
{
    int padding_x = getWidth() / 10;
    int padding_y = getHeight() / 10;

    modelLabelPos[0] = padding_x / 2;
    modelLabelPos[1] = 2.5 * padding_y;
    modelLabelPos[2] = getWidth() / 2 + padding_x / 2;
    modelLabelPos[3] = 30;
    modelChooseBtnPos[0] = modelLabelPos[0] + modelLabelPos[2] + padding_x / 4;
    modelChooseBtnPos[1] = modelLabelPos[1];
    modelChooseBtnPos[2] = modelLabelPos[2] / 2;
    modelChooseBtnPos[3] = modelLabelPos[3];
    genLabel1Pos[0] = modelLabelPos[0];
    genLabel1Pos[1] = modelLabelPos[1] + modelLabelPos[3] + padding_y;
    genLabel1Pos[2] = modelLabelPos[2] + 2 * padding_x;
    genLabel1Pos[3] = modelLabelPos[3];
    genLabel2Pos[0] = genLabel1Pos[0];
    genLabel2Pos[1] = genLabel1Pos[1] + genLabel1Pos[3] + padding_y;
    genLabel2Pos[2] = genLabel1Pos[2];
    genLabel2Pos[3] = genLabel1Pos[3];

    modelLabel.setBounds(modelLabelPos[0], modelLabelPos[1], modelLabelPos[2], modelLabelPos[3]);
    modelChooseBtn.setBounds(modelChooseBtnPos[0], modelChooseBtnPos[1], modelChooseBtnPos[2], modelChooseBtnPos[3]);
    genLabel1.setBounds(genLabel1Pos[0], genLabel1Pos[1], genLabel1Pos[2], genLabel1Pos[3]);
    if (visualizeGenLabel2) {
        genLabel2.setBounds(genLabel2Pos[0], genLabel2Pos[1], genLabel2Pos[2], genLabel2Pos[3]);
    }

    int btnW = 3 * padding_y;
    int btnH = btnW;
    genBtnPos[0] = getWidth() - btnW - padding_x / 2;
    genBtnPos[1] = getHeight() - btnH - 1.5 * padding_y;

    genBtn.setBounds(genBtnPos[0], genBtnPos[1], btnW, btnH);
    genBtn.setAlwaysOnTop(true);
}

void GenComponent::modelChooseBtnClicked()
{
    // you need to set
    // #define JUCE_MODAL_LOOPS_PERMITTED 1
    // to use this function
    juce::FileChooser chooser("Choose a model file...",
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        "*.model", true, false);
    if (chooser.browseForFileToOpen()) {
        auto tmp = chooser.getResult();
        modelPath = tmp.getFullPathName().toStdString();
    }
    modelPathChosen = true;
}
