/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NAP2023_hiraiAudioProcessorEditor::NAP2023_hiraiAudioProcessorEditor(NAP2023_hiraiAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
    , osc(audioProcessor.apvts, "OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
    , keyboardComponent(p.getKeyboardState(), juce::MidiKeyboardComponent::Orientation::verticalKeyboardFacingRight)
    , playbackBtn(juce::ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize), true)
    , stopBtn(juce::ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize), true)
    , saveBtn(juce::ImageCache::getFromMemory(BinaryData::save_png, BinaryData::save_pngSize), true)
    , penBtn(juce::ImageCache::getFromMemory(BinaryData::pen2_png, BinaryData::pen2_pngSize), true)
    , selectBtn(juce::ImageCache::getFromMemory(BinaryData::select1_png, BinaryData::select1_pngSize), true)
    , clearBtn(juce::ImageCache::getFromMemory(BinaryData::clear_png, BinaryData::clear_pngSize), true)
    , progressCursor(juce::ImageCache::getFromMemory(BinaryData::gen_png, BinaryData::gen_pngSize), false)
    , pianoRoll(p.getKeyboardState())
{
    setSize(1200, 800);
    this->setWantsKeyboardFocus(true);

    startTimer(30); // execute timerCallback every 30 ms
    

    // make visible
    addAndMakeVisible(osc);
    addAndMakeVisible(trainComponent);
    addAndMakeVisible(genComponent);
    addAndMakeVisible(pianoRoll);
    addAndMakeVisible(playbackBtn);
    addAndMakeVisible(stopBtn);
    addAndMakeVisible(saveBtn);
    addAndMakeVisible(penBtn);
    addAndMakeVisible(selectBtn);
    addAndMakeVisible(clearBtn);
    addAndMakeVisible(progressCursor);
    addAndMakeVisible(keyboardComponent);

}

NAP2023_hiraiAudioProcessorEditor::~NAP2023_hiraiAudioProcessorEditor()
{
}

//==============================================================================
void NAP2023_hiraiAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void NAP2023_hiraiAudioProcessorEditor::resized()
{

    // GUI setting
    padding_x = getWidth() / 48;
    padding_y = getHeight() / 27;

    componentW1 = padding_x * 9;
    componentW2 = padding_x * 14;
    componentH = padding_y * 7;
    oscPos[0] = padding_x / 2;
    oscPos[1] = 0;
    oscPos[2] = componentW1;
    oscPos[3] = componentH;
    trainBoxPos[0] = oscPos[0] + oscPos[2] + padding_x / 2;
    trainBoxPos[1] = oscPos[1];
    trainBoxPos[2] = (getWidth() - padding_x / 2 - trainBoxPos[0]) / 2 + padding_x/2;
    trainBoxPos[3] = oscPos[3] - oscPos[1] - trainBoxPos[1];
    genBoxPos[0] = trainBoxPos[0] + trainBoxPos[2] + padding_x / 2;
    genBoxPos[1] = oscPos[1];
    genBoxPos[2] = getWidth() - padding_x / 2 - genBoxPos[0];
    genBoxPos[3] = trainBoxPos[3];

    // piano roll
    pr_width = getWidth() - 2 * padding_x;
    pr_height = getHeight() - (oscPos[1] + oscPos[3] + padding_y);
    pr_pos[0] = 1.5 * padding_x;
    pr_pos[1] = oscPos[1] + oscPos[3] + padding_y / 2;
    pianoRoll.wkey_h = 28;
    //keyboardComponent.setBlackNoteWidthProportion(0.5);
    keyboardComponent.setBlackNoteWidthProportion(0.66);
    pianoRoll.blackKeyWidthProportion = keyboardComponent.getBlackNoteWidthProportion();
    pianoRoll.bkey_h = pianoRoll.wkey_h * pianoRoll.blackKeyWidthProportion;
    pianoRoll.key_w = padding_x / 1.5f;
    pianoRoll.numKeys = 35; // number of keys on piano roll
    pianoRoll.startKeyNum = 60; // should start from C note (36, 48, 60).

    // progress cursor
    progressCursor.setBounds(pr_pos[0] + pianoRoll.curPosition * pr_width, pr_pos[1], cursorWidth, pr_height);

    // buttons
    btnW = padding_x * 3;
    btnH = btnW;
    saveBtnPos[0] = getWidth() - padding_x - btnW;
    saveBtnPos[1] = pr_pos[1] + padding_y / 2;
    stopBtnPos[0] = saveBtnPos[0] - btnW - padding_x / 2;
    stopBtnPos[1] = saveBtnPos[1];
    playbackBtnPos[0] = stopBtnPos[0] - btnW - padding_x / 2;
    playbackBtnPos[1] = saveBtnPos[1];


    // buttons 2
    btnW2 = padding_x * 2;
    btnH2 = btnW2;
    clearBtnPos[0] = pr_pos[0] + pr_width - btnW2 - padding_x;
    clearBtnPos[1] = pr_pos[1] + pr_height - btnH2 - padding_y / 2;
    selectBtnPos[0] = clearBtnPos[0] - padding_x - btnW2;
    selectBtnPos[1] = clearBtnPos[1];
    penBtnPos[0] = selectBtnPos[0] - padding_x - btnW2;
    penBtnPos[1] = clearBtnPos[1];


    // --------------------------------------------------------------
    // set bounds ---------------------------------------------------
    osc.setBounds(oscPos[0], oscPos[1], oscPos[2], oscPos[3]);
    trainComponent.setBounds(trainBoxPos[0], trainBoxPos[1], trainBoxPos[2], trainBoxPos[3]);
    genComponent.setBounds(genBoxPos[0], genBoxPos[1], genBoxPos[2], genBoxPos[3]);

    //keyboardComponent.setBlackNoteWidthProportion(1.0);
    keyboardComponent.setKeyWidth(pianoRoll.wkey_h);
    keyboardComponent.setBlackNoteLengthProportion(0.95);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setBounds(0.5 * padding_x, pr_pos[1], padding_x, pr_height);
    keyboardComponent.setAvailableRange(pianoRoll.startKeyNum, pianoRoll.startKeyNum+ pianoRoll.numKeys);
    pianoRoll.setBounds(pr_pos[0], pr_pos[1], pr_width, pr_height);

    // buttons
    playbackBtn.setBounds(playbackBtnPos[0], playbackBtnPos[1], btnW, btnH);
    playbackBtn.setAlwaysOnTop(true);
    stopBtn.setBounds(stopBtnPos[0], stopBtnPos[1], btnW, btnH);
    stopBtn.setAlwaysOnTop(true);
    saveBtn.setBounds(saveBtnPos[0], saveBtnPos[1], btnW, btnH);
    saveBtn.setAlwaysOnTop(true);

    // buttons 2
    penBtn.setBounds(penBtnPos[0], penBtnPos[1], btnW2, btnH2);
    penBtn.setAlwaysOnTop(true);
    selectBtn.setBounds(selectBtnPos[0], selectBtnPos[1], btnW2, btnH2);
    selectBtn.setAlwaysOnTop(true);
    clearBtn.setBounds(clearBtnPos[0], clearBtnPos[1], btnW2, btnH2);
    clearBtn.setAlwaysOnTop(true);
        
    
    // --------------------------------------------------------------
    // --------------------------------------------------------------
}


void NAP2023_hiraiAudioProcessorEditor::mouseMove(const juce::MouseEvent& m)
{

}

bool NAP2023_hiraiAudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    if (key.getKeyCode() == 'Z') { // erase
        if (pianoRoll.inputNotes.size() > 0) {
            pianoRoll.inputNotes.pop_back();
            pianoRoll.noteOnFlags.pop_back();
            pianoRoll.noteOffFlags.pop_back();
            pianoRoll.repaint();
        }
    }

    return true;
}

void NAP2023_hiraiAudioProcessorEditor::timerCallback()
{
    pianoRoll.curPosition = float((pianoRoll.curPosition * pr_width) + progressCursor.dragDx) / float(pr_width);
    if (pianoRoll.curPosition < 0.0) pianoRoll.curPosition = 0.0;
    if (pianoRoll.curPosition > 1.0) pianoRoll.curPosition = 1.0;
    if(progressCursor.dragDx!=0) this->resized();
    progressCursor.dragDx = 0;

    if ((!saveBtn.isActive)&&(pianoRoll.inputNotes.size() > 0)) {
        saveBtn.isActive = true;
        clearBtn.isActive = true;
        saveBtn.repaint();
    }
    else if((saveBtn.isActive) && (pianoRoll.inputNotes.size() == 0)){
        saveBtn.isActive = false;
        clearBtn.isActive = false;
        saveBtn.repaint();
        trainComponent.addTrainBtn.isActive = false;
        trainComponent.addTrainBtn.repaint();
    }
    if ((savePath.size() > 0) && (!trainComponent.addTrainBtn.isActive) && (pianoRoll.inputNotes.size() > 0)) {
        trainComponent.addTrainBtn.isActive = true;
        trainComponent.addTrainBtn.repaint();
    }

	if (isPlaying != playbackBtn.isPressed) {
		if (!isPlaying) {
			playbackBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize));
		}
        else {
            playbackBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize));
            for (int j = 0; j < pianoRoll.maxMidiChannels; ++j) {
                pianoRoll.state.allNotesOff(j);
            }
        }
	}
    isPlaying = playbackBtn.isPressed;    

    if (isPlaying) {
        int progressPerFrame = 3;
        pianoRoll.curPosition = float((pianoRoll.curPosition * pr_width) + progressPerFrame) / float(pr_width);
        for (int i = 0; i < pianoRoll.inputNotes.size(); i++) {
            // note on
            if ((pianoRoll.curPosition * pr_width > pianoRoll.key_w * pianoRoll.inputNotes[i][1]) && (!pianoRoll.noteOnFlags[i])) {
                pianoRoll.state.noteOn(i % pianoRoll.maxMidiChannels+1, pianoRoll.inputNotes[i][0], pianoRoll.velocity);
                pianoRoll.noteOnFlags[i] = true;
            }
            // note off
            if ((pianoRoll.curPosition * pr_width > pianoRoll.key_w * pianoRoll.inputNotes[i][2]) && (!pianoRoll.noteOffFlags[i])) {
                pianoRoll.state.noteOff(i % pianoRoll.maxMidiChannels+1, pianoRoll.inputNotes[i][0], pianoRoll.velocity);
                pianoRoll.noteOffFlags[i] = true;
            }
        }
        this->resized();
    }

    if (stopBtn.isPressed) {
        pianoRoll.curPosition = 0.0f;
        stopBtn.isPressed = false;
        playbackBtn.isPressed = false;
        this->resized();
        for (int i = 0; i < pianoRoll.inputNotes.size(); i++) {
            for (int j = 0; j < pianoRoll.maxMidiChannels; ++j) {
                pianoRoll.state.allNotesOff(j);
            }
            pianoRoll.noteOnFlags[i] = false;
            pianoRoll.noteOffFlags[i] = false;
        }
    }

    if (saveBtn.isPressed) { // save
        saveBtn.isPressed = false;

        std::string outputFile;
        // you need to set
        // #define JUCE_MODAL_LOOPS_PERMITTED 1
        // to use this function
        juce::FileChooser chooser("save output midi file...",
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "*.mid", true, false);
        if (chooser.browseForFileToSave(false)) {
            auto tmp = chooser.getResult();
            outputFile = tmp.getFullPathName().toStdString();
        }
        saveMidiFile(pianoRoll.inputNotes, outputFile);
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
        	.withIconType(juce::MessageBoxIconType::InfoIcon)
        	.withTitle("Saved...")
        	.withMessage("Saved: " + outputFile)
        	.withButton("OK"),
        	nullptr);
    }

    if (trainComponent.addTrainBtn.isPressed) { // add training data button
        trainComponent.addTrainBtn.isPressed = false;
        if (pianoRoll.inputNotes.size() > 0) { // if input note is available
            int count = 0;
            std::string outputFileName = savePath + fileName + "_" + std::to_string(count) + ".txt";
            while (std::filesystem::exists(outputFileName)) {
                count++;
                outputFileName = savePath + fileName + "_" + std::to_string(count) + ".txt";
            }
            std::vector<std::vector<int>> tmpSaveNotes;
            if (pianoRoll.highlight.size() > 0) { // save the highlighted notes
                tmpSaveNotes = sortSaveNotes(pianoRoll.saveNotes);
            }
            else { // save all the piano roll notes
                tmpSaveNotes = sortSaveNotes(pianoRoll.inputNotes);
            }

            // save transposed 12 melody -----
            // get min and max note number for determining shift range
            int maxNoteNum = -1, minNoteNum = pianoRoll.startKeyNum + pianoRoll.numKeys;
            for (int i = 0; i < tmpSaveNotes.size(); ++i) {
                if (tmpSaveNotes[i][0] > maxNoteNum) maxNoteNum = tmpSaveNotes[i][0];
                if (tmpSaveNotes[i][0] < minNoteNum) minNoteNum = tmpSaveNotes[i][0];
            }
            int shiftdown = -6;
            int shiftup = 6;
            if (maxNoteNum + shiftup > pianoRoll.startKeyNum + pianoRoll.numKeys) {
                shiftdown -= (maxNoteNum + shiftup) - (pianoRoll.startKeyNum + pianoRoll.numKeys);
            }
            if (minNoteNum + shiftdown < pianoRoll.startKeyNum) {
                shiftup += pianoRoll.startKeyNum - (minNoteNum + shiftdown);
            }
            if (maxNoteNum + shiftup > pianoRoll.startKeyNum + pianoRoll.numKeys) {
                shiftup = (pianoRoll.startKeyNum + pianoRoll.numKeys) - maxNoteNum;
            }
            if (minNoteNum + shiftdown < pianoRoll.startKeyNum) {
                shiftdown = pianoRoll.startKeyNum - minNoteNum;
            }
            // save all 12 transposed version of melody
            for (int shift = shiftdown; shift < shiftup; ++shift) {
                if (shift != 0)
                    outputFileName = savePath + fileName + "_" + std::to_string(count) + "_shift_" + std::to_string(shift) + ".txt";
                else
                    outputFileName = savePath + fileName + "_" + std::to_string(count) + ".txt";
                std::ofstream outputfile(outputFileName);
                for (int i = 0; i < tmpSaveNotes.size(); i++) {
                    outputfile << (tmpSaveNotes[i][0] + shift);
                    outputfile << "\t";
                    outputfile << tmpSaveNotes[i][1];
                    outputfile << "\t";
                    outputfile << tmpSaveNotes[i][2] << std::endl;
                }
                outputfile.close();
            }
        }
        // count files in training data folder
        int count = 0;
        for (const auto& file : std::filesystem::directory_iterator(savePath)) {
            count++;
        }
        trainComponent.nTrainData = count;
        std::string tmpStr = "Number of training data: " + std::to_string(count) + "\n\t(estimated time: about " + std::to_string(count / 150 + 1) + "min. w/ GPU)";
        trainComponent.trainingLabel.setText(tmpStr, juce::dontSendNotification);
        trainComponent.resized();
    }

    if (trainComponent.trainBtn.isPressed) { // start training
        trainComponent.trainBtn.isPressed = false;
        modelType = trainComponent.trainingModelType.getItemText(trainComponent.trainingModelType.getSelectedItemIndex()).toStdString();
        if ((modelType == "LSTM") || (modelType == "BiLSTM")) {
            juce::FileChooser chooser("save output model file...",
                juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                "*.model", true, false);
            if (chooser.browseForFileToSave(false)) {
                auto tmp = chooser.getResult();
                modelPath = tmp.getFullPathName().toStdString();
                juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                    .withIconType(juce::MessageBoxIconType::InfoIcon)
                    .withTitle("Start training...")
                    .withMessage("Executing:" + exePath + "\nSaving: " + modelPath + "\nPlease wait for a while.")
                    .withButton("OK"),
                    nullptr);
            }
            int epoch = 100;
            std::string evalStr = exePath + " train GPU " + savePath + " "
                + modelPath + " " + modelType + " " + std::to_string(epoch);
            DBG(evalStr);
            std::system(evalStr.c_str());

            // prepare for generation
            genComponent.modelPath = modelPath;
            genComponent.modelPathChosen = false;
            genComponent.modelLabel.setText(modelPath, juce::dontSendNotification);
            genComponent.genLabel1.setText("melody generation model: " + modelPath, juce::dontSendNotification);
            genComponent.visualizeGenLabel2 = true;
            genComponent.resized();
            if ((modelPath.size() > 0) && (pianoRoll.highlight.size() > 0)) {
                genComponent.genBtn.isActive = true;
                genComponent.genBtn.repaint();
            }
        }
        else {
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::InfoIcon)
                .withTitle("Warning...")
                .withMessage("The model type you selected is currently under construction. Select other model type.")
                .withButton("OK"),
                nullptr);
        }
    }

    if (genComponent.genBtn.isPressed) {
        genComponent.genBtn.isPressed = false;
        std::string deskTopDir = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getFullPathName().toStdString();
        std::string inputMelodyFile = deskTopDir + "\\inputMelody.txt";
        if (pianoRoll.inputNotes.size() > 0) {
            int count = 0;
            std::vector<std::vector<int>> tmpSaveNotes;
			tmpSaveNotes = sortSaveNotes(pianoRoll.inputNotes);
			std::ofstream outputfile(inputMelodyFile);
			for (int i = 0; i < tmpSaveNotes.size(); i++) {
				outputfile << (tmpSaveNotes[i][0]);
				outputfile << "\t";
				outputfile << tmpSaveNotes[i][1];
				outputfile << "\t";
				outputfile << tmpSaveNotes[i][2] << std::endl;
			}
			outputfile.close();
        }

        std::string outputMelodyFile = deskTopDir + "\\outputMelody.txt";
        int genLength = pianoRoll.highlight[1] - pianoRoll.highlight[0];
        std::string evalStr = exePath + " generate GPU " + inputMelodyFile
            + " " + modelPath + " " + std::to_string(genLength) + " " + outputMelodyFile;
        DBG(evalStr);
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::InfoIcon)
            .withTitle("Generating melody for highlight area...")
            .withMessage("Executing:" + exePath + "\nPlease wait a second.")
            .withButton("OK"),
            nullptr);
        std::system(evalStr.c_str());

        // read output melody and reflect to the pianoroll
        std::ifstream ifs(outputMelodyFile);
        if (ifs.fail()) {
            std::cerr << "Failed to open file." << std::endl;
        }
        std::string line;
        int generatedLength = 0;
        while (getline(ifs, line)) {
            std::string data;
            std::istringstream str(line);
            std::vector<int> tmpData;
            while (getline(str, data, '\t')) {
                tmpData.push_back(stoi(data));
            }
            if (tmpData.size() == 3) {
                tmpData[1] += pianoRoll.highlight[0];
                tmpData[2] += pianoRoll.highlight[0];
                generatedLength += tmpData[2] - tmpData[1];
                if (generatedLength > genLength) { // if the generated melody is longer than highlight
                    tmpData[2] -= (generatedLength - genLength); // shorten
                }
                pianoRoll.inputNotes.push_back(tmpData);
                pianoRoll.noteOnFlags.push_back(false);
                pianoRoll.noteOffFlags.push_back(false);
            }
            if (generatedLength >= genLength) break;
        }
        pianoRoll.repaint();
        if (generatedLength == 0) {
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::InfoIcon)
                .withTitle("No result generated....")
                .withMessage("Maybe, all generated results are rests.\nTry with other setting.")
                .withButton("OK"),
                nullptr);
        }
    }

    if ((!isPen) && (penBtn.isPressed)) { // change to pen mode
        selectBtn.isPressed = false;
        penBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::pen2_png, BinaryData::pen2_pngSize));
        selectBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::select1_png, BinaryData::select1_pngSize));
        if (pianoRoll.highlight.size() > 0) {
            pianoRoll.highlight.clear();
            pianoRoll.repaint();
        }
        this->resized();
        pianoRoll.mode = 1;
    }

    if ((!isSelect) && (selectBtn.isPressed)) { // select to pen mode
        penBtn.isPressed = false;
        penBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::pen1_png, BinaryData::pen1_pngSize));
        selectBtn.changeImage(juce::ImageCache::getFromMemory(BinaryData::select2_png, BinaryData::select2_pngSize));
        this->resized();
        pianoRoll.mode = 2;
    }

    if (clearBtn.isPressed) { // clear button
        clearBtn.isPressed = false;
        pianoRoll.inputNotes.clear();
        pianoRoll.noteOnFlags.clear();
        pianoRoll.noteOffFlags.clear();
        pianoRoll.saveNotes.clear();
        pianoRoll.repaint();
    }

    if (trainComponent.trainDirChosen) {
        savePath = trainComponent.savePath;
        trainComponent.trainDirChosen = false;
        trainComponent.trainDirChooseLabel.setText(savePath, juce::dontSendNotification);
        if ((savePath.size() > 0)&&(trainComponent.nTrainData>0)) {
            trainComponent.trainBtn.isActive = true;
            trainComponent.trainBtn.repaint();
        }
    }

    if (genComponent.modelPathChosen) {
        modelPath = genComponent.modelPath;
        genComponent.modelPathChosen = false;
        genComponent.modelLabel.setText(modelPath, juce::dontSendNotification);
        genComponent.genLabel1.setText("melody generation model: "+modelPath, juce::dontSendNotification);
        genComponent.visualizeGenLabel2 = true;
        genComponent.resized();
        if ((modelPath.size() > 0)&&(pianoRoll.highlight.size()>0)) {
            genComponent.genBtn.isActive = true;
            genComponent.genBtn.repaint();
        }
    }

    if ((!isHighlighted) && (pianoRoll.highlight.size() > 0)) {
        isHighlighted = true;
        if (modelPath.size() > 0) {
            genComponent.genBtn.isActive = true;
            genComponent.genBtn.repaint();
        }
    }
    else if ((isHighlighted) && (pianoRoll.highlight.size() == 0)) {
        genComponent.genBtn.isActive = false;
        genComponent.genBtn.repaint();
        isHighlighted = false;
    }

    isPen = penBtn.isPressed;
    isSelect = selectBtn.isPressed;
}

void NAP2023_hiraiAudioProcessorEditor::saveMidiFile(std::vector<std::vector<int>> melody, std::string outputFileName)
{
    juce::MidiFile outMidifile;
    juce::MidiMessageSequence myTrack;

    float tick = 360;
    outMidifile.setTicksPerQuarterNote(tick);

	myTrack.addEvent(juce::MidiMessage::timeSignatureMetaEvent(tick*melody.size(), 4));
	myTrack.addEvent(juce::MidiMessage::tempoMetaEvent(tick));
	myTrack.addEvent(juce::MidiMessage::midiChannelMetaEvent(1));

    for (int i = 0; i < melody.size(); i++) {
        double starttime = 0;
        double endtime = 360;
        juce::MidiMessage noteOnMsg = juce::MidiMessage::noteOn(1, melody[i][0], pianoRoll.velocity);
        myTrack.addEvent(noteOnMsg, tick * starttime / 4.0);
        myTrack.updateMatchedPairs();
        juce::MidiMessage noteOffMsg = juce::MidiMessage::noteOff(1, melody[i][0]);
        myTrack.addEvent(noteOffMsg, tick * endtime / 4.0);
    }

    myTrack.addEvent(juce::MidiMessage::endOfTrack(), tick * melody.size());
    outMidifile.addTrack(myTrack);
    juce::File file = outputFileName;
	juce::FileOutputStream fos(file);

	outMidifile.writeTo(fos);
}

// sort notes by onset timing
std::vector<std::vector<int>> NAP2023_hiraiAudioProcessorEditor::sortSaveNotes(std::vector<std::vector<int>> saveNotes)
{
    std::vector<std::vector<int>> sortedSaveNotes;
    std::vector<int> sortValue;
    std::vector<int> index(saveNotes.size());
    std::iota(index.begin(), index.end(), 0);
    for (int i = 0; i < saveNotes.size(); i++) {
        sortValue.push_back(saveNotes[i][1]);
    }
    std::sort(
        index.begin(),
        index.end(),
        [&](int x, int y) {return sortValue[x] < sortValue[y]; }
    );
    for (auto& i : index) {
        sortedSaveNotes.push_back(saveNotes[i]);
    }
    return sortedSaveNotes;
}

