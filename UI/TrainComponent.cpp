/*
  ==============================================================================

    TrainComponent.cpp
    Created: 13 Mar 2023 3:33:32pm
    Author:  hirait

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TrainComponent.h"

//==============================================================================
TrainComponent::TrainComponent()
    : trainDirChooseBtn("select a folder")
    , addTrainBtn(juce::ImageCache::getFromMemory(BinaryData::add_train_png, BinaryData::add_train_pngSize), true)
    , trainBtn(juce::ImageCache::getFromMemory(BinaryData::train_png, BinaryData::train_pngSize), true)
{

    trainDirChooseLabel.setText("select a folder to save the training data.", juce::dontSendNotification);
    trainDirChooseLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
    trainDirChooseLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    trainDirChooseBtn.onClick = [this] { trainDirChooseBtnClicked();  };
    addAndMakeVisible(trainDirChooseLabel);
    addAndMakeVisible(trainDirChooseBtn);
    juce::StringArray choices{ "LSTM", "BiLSTM", "RNN (N/A)", "Transformer (N/A)"};
    trainingModelType.addItemList(choices, 1);
    trainingModelType.setSelectedItemIndex(0);
    addAndMakeVisible(trainingModelType);
    trainingModelTypeLabel.setText("Training model type: ", juce::dontSendNotification);
    trainingModelTypeLabel.setFont(15.0f);
    trainingModelTypeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    trainingModelTypeLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(trainingModelTypeLabel);
    trainingLabel.setText("Number of training data: ", juce::dontSendNotification);
    trainingLabel.setFont(15.0f);
    trainingLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    trainingLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(trainingLabel);

    addAndMakeVisible(addTrainBtn);
    addAndMakeVisible(trainBtn);

    addTrainBtn.isActive = false;
    trainBtn.isActive = false;
}

TrainComponent::~TrainComponent()
{
}

void TrainComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    //auto bounds = getLocalBounds();
    auto labelSpace = bounds.removeFromTop(20.0f);

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Training settings", labelSpace.withX(5), juce::Justification::left);
    g.drawRoundedRectangle(bounds.toFloat(), 5.0f, 2.0f);
}

void TrainComponent::resized()
{
    int padding_x = getWidth() / 10;
    int padding_y = getHeight() / 10;
    trainDirLabelPos[0] = padding_x / 2;
    trainDirLabelPos[1] = 2.5*padding_y;
    trainDirLabelPos[2] = getWidth() / 2 + padding_x / 2;
    trainDirLabelPos[3] = 30;
    trainDirChooseBtnPos[0] = trainDirLabelPos[0] + trainDirLabelPos[2] + padding_x / 4;
    trainDirChooseBtnPos[1] = trainDirLabelPos[1];
    trainDirChooseBtnPos[2] = trainDirLabelPos[2] / 2;
    trainDirChooseBtnPos[3] = trainDirLabelPos[3];
    trainingModelTypeLabelPos[0] = trainDirLabelPos[0];
    trainingModelTypeLabelPos[1] = trainDirLabelPos[1] + trainDirLabelPos[3] + padding_y;
    trainingModelTypeLabelPos[2] = trainDirChooseBtnPos[2] + padding_x / 4;
    trainingModelTypeLabelPos[3] = trainDirLabelPos[3];
    trainingModelTypePos[0] = trainingModelTypeLabelPos[0] + trainingModelTypeLabelPos[2];
    trainingModelTypePos[1] = trainingModelTypeLabelPos[1];
    trainingModelTypePos[2] = trainDirLabelPos[2] / 2;
    trainingModelTypePos[3] = trainDirLabelPos[3];
    trainingLabelPos[0] = trainDirLabelPos[0];
    trainingLabelPos[1] = trainingModelTypeLabelPos[1] + trainingModelTypeLabelPos[3] + padding_y;
    trainingLabelPos[2] = trainDirLabelPos[2] + 2 * padding_x;
    trainingLabelPos[3] = trainDirLabelPos[3];


    int btnW = 3*padding_y;
    int btnH = btnW;
    trainBtnPos[0] = getWidth() - btnW - padding_x / 2;
    trainBtnPos[1] = getHeight() - btnH - 1.5*padding_y;
    addTrainBtnPos[0] = trainBtnPos[0] - btnW - padding_x / 4;
    addTrainBtnPos[1] = trainBtnPos[1];


    trainDirChooseBtn.setBounds(trainDirChooseBtnPos[0], trainDirChooseBtnPos[1],
        trainDirChooseBtnPos[2], trainDirChooseBtnPos[3]);
    trainDirChooseLabel.setBounds(trainDirLabelPos[0], trainDirLabelPos[1],
        trainDirLabelPos[2], trainDirLabelPos[3]);

    trainingModelTypeLabel.setBounds(trainingModelTypeLabelPos[0], trainingModelTypeLabelPos[1],
        trainingModelTypeLabelPos[2], trainingModelTypeLabelPos[3]);
    trainingModelType.setBounds(trainingModelTypePos[0], trainingModelTypePos[1],
        trainingModelTypePos[2], trainingModelTypePos[3]);
    trainingLabel.setBounds(trainingLabelPos[0], trainingLabelPos[1],
        trainingLabelPos[2], trainingLabelPos[3]);

    addTrainBtn.setBounds(addTrainBtnPos[0], addTrainBtnPos[1], btnW, btnH);
    addTrainBtn.setAlwaysOnTop(true);
    trainBtn.setBounds(trainBtnPos[0], trainBtnPos[1], btnW, btnH);
    trainBtn.setAlwaysOnTop(true);
}

void TrainComponent::trainDirChooseBtnClicked()
{
    // you need to set
    // #define JUCE_MODAL_LOOPS_PERMITTED 1
    // to use this function
    juce::FileChooser chooser("Choose a directory...",
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        "*", true, false);
    if (chooser.browseForDirectory()) {
        auto tmp = chooser.getResult();
        savePath = tmp.getFullPathName().toStdString() + "\\";
    }
    trainDirChosen = true;
    // count files in training data folder
    int count = 0;
    for (const auto& file : std::filesystem::directory_iterator(savePath)) {
        count++;
    }
    nTrainData = count;
    std::string tmpStr = "Number of training data: " + std::to_string(nTrainData) + "\n\t(estimated time: about " + std::to_string(count / 150 + 1) + "min. w/ GPU)";
    trainingLabel.setText(tmpStr, juce::dontSendNotification);
    repaint();
}
