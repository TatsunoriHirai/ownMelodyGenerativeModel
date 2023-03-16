/*
  ==============================================================================

    ButtonComponent.cpp
    Created: 21 Feb 2023 10:58:54pm
    Author:  hirait

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ButtonComponent.h"

//==============================================================================
ButtonComponent::ButtonComponent(juce::Image img, bool useImg)
{
    paintImg = img;
    useBGImg = useImg;
}

ButtonComponent::~ButtonComponent()
{
}

void ButtonComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::transparentBlack);
    if(useBGImg) g.drawImageWithin(paintImg, 0, 0, getWidth(), getHeight(),juce::RectanglePlacement::centred);
    if (isMouseEnter&& isActive) {
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 128));
        if(useBGImg) g.fillEllipse(0, 0, getWidth(), getHeight());
        else g.fillRect(0, 0, getWidth(), getHeight());
    }
    if (isMouseDown&& isActive) {
        g.setColour(juce::Colour::fromRGBA(255, 255, 255, 128));
        if (useBGImg) g.fillEllipse(0, 0, getWidth(), getHeight());
        else g.fillRect(0, 0, getWidth(), getHeight());
    }
    if (!isActive) {
        g.setColour(juce::Colour::fromRGBA(0, 0, 0, 200));
        g.fillEllipse(0, 0, getWidth(), getHeight());
    }

    if (!useBGImg) { // for progress cursor
        g.setColour(juce::Colour::fromRGBA(255, 50, 100, 200));
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColour(juce::Colours::black);
        g.drawRect(0, 0, getWidth(), getHeight());
    }
}

void ButtonComponent::resized()
{
}

void ButtonComponent::mouseDown(const juce::MouseEvent& m)
{
    //DBG("mouse down");
    isMouseDown = true;
    prevX = m.getPosition().x;
    repaint();
}

void ButtonComponent::mouseUp(const juce::MouseEvent& m)
{
    //DBG("mouse up");
    if (isMouseDown) {
        isPressed = !isPressed;
    }
    isMouseDown = false;
    repaint();
}

void ButtonComponent::mouseEnter(const juce::MouseEvent& m)
{
    //DBG("mouse enter");
    isMouseEnter = true;
    repaint();
}

void ButtonComponent::mouseExit(const juce::MouseEvent& m)
{
    //DBG("mouse exit");
    isMouseEnter = false;
    repaint();
}

void ButtonComponent::mouseDrag(const juce::MouseEvent& m)
{
    //DBG("mouse drag");
    dragDx = m.getPosition().x - prevX;
    repaint();
}

void ButtonComponent::changeImage(juce::Image img)
{
    paintImg = img;
    repaint();
}
