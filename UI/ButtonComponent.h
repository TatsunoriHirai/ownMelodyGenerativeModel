/*
  ==============================================================================

    ButtonComponent.h
    Created: 21 Feb 2023 10:58:54pm
    Author:  hirait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ButtonComponent  : public juce::Component
{
public:
    ButtonComponent(juce::Image img, bool useImg);
    ~ButtonComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& m) override;
    void mouseUp(const juce::MouseEvent& m) override;
    void mouseEnter(const juce::MouseEvent& m) override;
    void mouseExit(const juce::MouseEvent& m) override;
    void mouseDrag(const juce::MouseEvent& m) override;
    void changeImage(juce::Image img);
    juce::Image paintImg;
    int dragDx = 0;
    bool isPressed{ false };
    bool isActive{ true };

private:
    bool isMouseDown = false;
    bool isMouseEnter = false;
    bool useBGImg;
    int prevX = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonComponent)
};
