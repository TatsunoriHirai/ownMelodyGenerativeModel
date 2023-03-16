/*
  ==============================================================================

    pianoRollComponent.cpp
    Created: 21 Feb 2023 6:59:29pm
    Author:  hirait

  ==============================================================================
*/

#include <JuceHeader.h>
#include "pianoRollComponent.h"

//==============================================================================
pianoRollComponent::pianoRollComponent(juce::MidiKeyboardState& stateToUse)
    : state(stateToUse)
{
    state.addListener(this);

}

pianoRollComponent::~pianoRollComponent()
{
    state.removeListener(this);
}

void pianoRollComponent::paint (juce::Graphics& g)
{
    // piano roll background color
    g.fillAll (juce::Colours::ghostwhite);

    // ---------------------------------------------------
    // lines for piano roll ------------------------------

    // horizontal lines interval
    if (keyInterval.size() == 0) {
        keyInterval = { 0.0, 3.0 / 5.0, 6.0 / 5.0, 9.0 / 5.0, 12.0/5.0,
        3.0, 3.0 + 4.0 / 7.0, 3.0 + 8.0 / 7.0, 3.0 + 12.0 / 7.0, 3.0 + 16.0 / 7.0, 3.0 + 20.0 / 7.0, 3.0 + 24.0 / 7.0 }; // equidistant
        //keyInterval = { 0, 1 - blackKeyWidthProportion / 2.0f, 1 + blackKeyWidthProportion / 2.0f,
        //2 - blackKeyWidthProportion / 2.0f, 2 + blackKeyWidthProportion / 2.0f,
        //3, 4 - blackKeyWidthProportion / 2.0f, 4 + blackKeyWidthProportion / 2.0f,
        //5 - blackKeyWidthProportion / 2.0f, 5 + blackKeyWidthProportion / 2.0f,
        //6 - blackKeyWidthProportion / 2.0f, 6 + blackKeyWidthProportion / 2.0f }; // same as piano roll
        for (int i = keyInterval.size(); i < numKeys; i++) {
            keyInterval.push_back(7 * (i / 12) + keyInterval[i % 12]);
        }
    }

    // blackkey background
    for (int i = numKeys - 1; i >= 0; --i) {
        if (i < numKeys - 1) {
            g.setColour(juce::Colours::lightgrey);
            int curKey_h = int(keyInterval[i + 1] * wkey_h) - int(keyInterval[i] * wkey_h);
            if ((i % 12 == 1) || (i % 12 == 3) || (i % 12 == 6) || (i % 12 == 8) || (i % 12 == 10))
                g.fillRect(0, getHeight() - int(keyInterval[i + 1] * wkey_h), getWidth(), curKey_h);
        }
    }

    // highlighting current drag timing
    g.setColour(juce::Colours::yellow);
    if (highlight.size() > 0) {
        g.fillRect(highlight[0] * key_w, 0, highlight[1] * key_w - highlight[0] * key_w, getHeight());
    }
    else if ((curDragX != -1) && (mode == 2)) {
        if (curDragX * key_w > noteOnTiming * key_w) {
            g.fillRect(noteOnTiming * key_w, 0, curDragX * key_w - noteOnTiming * key_w, getHeight());
        }
        else if (curDragX* key_w < noteOnTiming * key_w) {
            g.fillRect(curDragX * key_w, 0, noteOnTiming * key_w - curDragX * key_w, getHeight());
        }
    }

    // pointed note highlighting
    if ((pointedNote != -1) && (mode == 1)) {
        g.setColour(juce::Colours::lightpink);
        int highlight_h = int(keyInterval[pointedNote + 1] * wkey_h) - int(keyInterval[pointedNote] * wkey_h);
        g.fillRect(0, getHeight() - int(keyInterval[pointedNote + 1] * wkey_h), getWidth(), highlight_h);
    }

    // horizontal lines
    for (int i = numKeys - 1; i >= 0; --i) {
        if (i > 0) {
            g.setColour(juce::Colours::darkgrey);
            if ((numKeys - i) % 12 == 0) {
                g.drawLine(0, getHeight() - keyInterval[numKeys - i] * wkey_h, getWidth(), getHeight() - keyInterval[numKeys - i] * wkey_h, 4);
            }
            else {
                g.drawLine(0, getHeight() - keyInterval[numKeys - i] * wkey_h, getWidth(), getHeight() - keyInterval[numKeys - i] * wkey_h, 1);
            }
        }
    }

    // vertical lines
    int i = 0;
    while (i*key_w < getWidth()) {
        if(i%4==0) g.setColour(juce::Colours::black);
        else g.setColour(juce::Colours::grey);
        
        if(i%16==0) g.drawLine(i * key_w, 0, i * key_w, getHeight(), 2);
        else g.drawLine(i*key_w, 0, i * key_w, getHeight(), 1);
        i++;
    }

    // current drag note
    if ((pointedNote != -1)&&(curDragX > noteOnTiming)&& (mode == 1)) {
        g.setColour(juce::Colours::red);
        int dragging_h = int(keyInterval[pointedNote + 1] * wkey_h) - int(keyInterval[pointedNote] * wkey_h);
        g.fillRect(noteOnTiming*key_w, getHeight() - int(keyInterval[pointedNote + 1] * wkey_h),curDragX*key_w- noteOnTiming * key_w, dragging_h);
        g.setColour(juce::Colours::black);
        g.drawRect(noteOnTiming * key_w, getHeight() - int(keyInterval[pointedNote + 1] * wkey_h), curDragX * key_w - noteOnTiming * key_w, dragging_h, 1.0);
    }

    // input notes
    for (int i = 0; i < inputNotes.size(); i++) {
        int curKey_h = int(keyInterval[inputNotes[i][0] - startKeyNum + 1] * wkey_h) - int(keyInterval[inputNotes[i][0] - startKeyNum] * wkey_h);
        if(i==pointingInputeNote) g.setColour(juce::Colours::deeppink);
        else g.setColour(juce::Colours::red);
        g.fillRect(inputNotes[i][1] * key_w, getHeight() - int(keyInterval[inputNotes[i][0] - startKeyNum + 1] * wkey_h),
            inputNotes[i][2] * key_w - inputNotes[i][1] * key_w, curKey_h);
        g.setColour(juce::Colours::black);
        g.drawRect(inputNotes[i][1] * key_w, getHeight() - int(keyInterval[inputNotes[i][0] - startKeyNum + 1] * wkey_h),
            inputNotes[i][2] * key_w - inputNotes[i][1] * key_w, curKey_h, 1.0);
        if ((dragInNote4LenMod)&&(i==pointingInputeNote)) { // drag for input note length change
            g.setColour(juce::Colours::darkblue );
            g.drawLine(inputNotes[i][2] * key_w, getHeight() - int(keyInterval[inputNotes[i][0] - startKeyNum + 1] * wkey_h),
                inputNotes[i][2] * key_w, getHeight() - int(keyInterval[inputNotes[i][0] - startKeyNum + 1] * wkey_h) + curKey_h,
                5.0f);
        }
    }

    // highlight pointing timing
    if ((mode == 2) && (curDragX == -1)) {
        g.setColour(juce::Colours::lightyellow);
        g.fillRect(int(float(mouseX) / float(key_w) + 0.5) * key_w, 0, 3, getHeight());
    }
     
    // ---------------------------------------------------
    // ---------------------------------------------------

}

void pianoRollComponent::resized()
{

}

void pianoRollComponent::mouseMove(const juce::MouseEvent& m)
{
    pointingInputeNote = -1;
    dragInNote4LenMod = false;

    for (int i = numKeys - 1; i > 0; --i) {
        if (m.getPosition().y > getHeight() - keyInterval[numKeys - i] * wkey_h) {
            if (pointedNote != (numKeys - i - 1)) {
                pointedNote = numKeys - i - 1;
                repaint();
            }
            break;
        }
    }
    if (mode == 1) { // pen mode
        for (int i = 0; i < inputNotes.size(); ++i) {
            if ((pointedNote + startKeyNum == inputNotes[i][0])
                && (m.getPosition().x / key_w >= inputNotes[i][1])
                && (m.getPosition().x / key_w < inputNotes[i][2])){
                pointingInputeNote = i;
                break;
            }
        }
        if (pointingInputeNote != -1) {
            int dragInNote4LenMode_x = inputNotes[pointingInputeNote][1] * key_w
                + 0.95 * key_w * (inputNotes[pointingInputeNote][2] - inputNotes[pointingInputeNote][1]);
            if (m.getPosition().x >= dragInNote4LenMode_x) dragInNote4LenMod = true;
        }
        repaint();
    }
    if (mode == 2) { // select mode
        mouseX = m.getPosition().x;
        repaint();
    }

    isDragged = false;
}

void pianoRollComponent::mouseEnter(const juce::MouseEvent& m)
{
}

void pianoRollComponent::mouseExit(const juce::MouseEvent& m)
{
    pointedNote = -1;
    repaint();
}

void pianoRollComponent::mouseDown(const juce::MouseEvent& m)
{
    int midiNoteNumber = pointedNote + startKeyNum;
    if(mode==1) state.noteOn(midiChannel, midiNoteNumber, velocity);
    noteOnTiming = int(float(m.getPosition().x) / float(key_w) + 0.5);
    if(mode == 2) highlight.clear();

    if ((mode == 1)&&(pointingInputeNote!=-1)) {
        prevX = m.getPosition().x;
    }
}

void pianoRollComponent::mouseUp(const juce::MouseEvent& m)
{
    int midiNoteNumber = pointedNote + startKeyNum;
    if (pointingInputeNote != -1) midiNoteNumber = inputNotes[pointingInputeNote][0];
    if (mode == 1) state.noteOff(midiChannel, midiNoteNumber, velocity);
    noteOffTiming = (m.getPosition().x + key_w) / key_w;
    if ((noteOnTiming < noteOffTiming)&&(mode==1)) {
        std::vector<int> noteOnOff = { midiNoteNumber, noteOnTiming, noteOffTiming};
        inputNotes.push_back(noteOnOff);
        inputNotes = duplicateCheck(inputNotes);
        if (noteOnFlags.size() < inputNotes.size()) {
            noteOnFlags.push_back(false);
            noteOffFlags.push_back(false);
        }
        repaint();
    }
    if ((isDragged) && (mode == 2)) { // highligting
        saveNotes.clear();
        if (noteOnTiming < noteOffTiming) {
            highlight.push_back(noteOnTiming);
            highlight.push_back(noteOffTiming);
        }
        else {
            highlight.push_back(noteOffTiming-1);
            highlight.push_back(noteOnTiming);
        }
        for (int i = 0; i < inputNotes.size(); i++) {
            if (((inputNotes[i][1] >= highlight[0]) && (inputNotes[i][1] <= highlight[1])) ||
                ((inputNotes[i][2] >= highlight[0]) && (inputNotes[i][2] <= highlight[1])) ||
                ((inputNotes[i][1] <= highlight[0]) && (inputNotes[i][2] >= highlight[1]))){
                saveNotes.push_back(inputNotes[i]);
                saveNotes[saveNotes.size() - 1][1] = inputNotes[i][1] - highlight[0];
                if (saveNotes[saveNotes.size() - 1][1] < 0) saveNotes[saveNotes.size() - 1][1] = 0;
                saveNotes[saveNotes.size() - 1][2] = inputNotes[i][2] - highlight[0];
                if (saveNotes[saveNotes.size() - 1][2] > highlight[1]-highlight[0])
                    saveNotes[saveNotes.size() - 1][2] = highlight[1] - highlight[0];
            }
        }
    }

    // delete input note (by single click)
    if ((pointingInputeNote != -1) && (!isDragged)) {
        inputNotes.erase(inputNotes.begin() + pointingInputeNote);
        noteOnFlags.erase(noteOnFlags.begin() + pointingInputeNote);
        noteOffFlags.erase(noteOffFlags.begin() + pointingInputeNote);
        repaint();
    }

    noteOnTiming = -1;
    noteOffTiming = -1;
    curDragX = -1;
}

void pianoRollComponent::mouseDrag(const juce::MouseEvent& m)
{
    isDragged = true;
    if (dragInNote4LenMod) { // dragging input note for note length change (dragging right edge)
        int shift = (m.getPosition().x - prevX) / key_w;
        inputNotes[pointingInputeNote][2] += shift;
        if (shift != 0) prevX = m.getPosition().x;
    }
    else if (pointingInputeNote!=-1){ // dragging input note
        // x axis drag
		int shift = (m.getPosition().x - prevX) / key_w;
		inputNotes[pointingInputeNote][1] += shift;
		inputNotes[pointingInputeNote][2] += shift;
        if(shift != 0) prevX = m.getPosition().x;

        // y axis drag
        int curYpos = -1;
        for (int i = numKeys - 1; i > 0; --i) {
            if (m.getPosition().y > getHeight() - keyInterval[numKeys - i] * wkey_h) {
                curYpos = numKeys - i - 1;
                break;
            }
        }
        if (curYpos + startKeyNum != inputNotes[pointingInputeNote][0]) {
            state.noteOff(midiChannel, inputNotes[pointingInputeNote][0], velocity);
            inputNotes[pointingInputeNote][0] = curYpos + startKeyNum;
            state.noteOn(midiChannel, inputNotes[pointingInputeNote][0], velocity);
        }
    }
    else { // piano roll drag
        curDragX = int(float(m.getPosition().x) / float(key_w) + 0.5);
    }
    repaint();
}


void pianoRollComponent::handleNoteOn(juce::MidiKeyboardState*, int, int, float)
{
}


void pianoRollComponent::handleNoteOff(juce::MidiKeyboardState*, int, int, float)
{
}

std::vector<std::vector<int>> pianoRollComponent::duplicateCheck(std::vector<std::vector<int>> inputNotes)
{
    std::vector<int> addedNote = inputNotes[inputNotes.size() - 1];
    for (int i = 0; i < inputNotes.size() - 1; ++i) {
        std::vector<int> tmpNote = inputNotes[i];
        if (tmpNote[0] == addedNote[0]) {
            if ((tmpNote[2] > addedNote[1]) && (tmpNote[1] < addedNote[2])) {
                inputNotes.pop_back();
                break;
            }
        }
    }
    return inputNotes;
}
