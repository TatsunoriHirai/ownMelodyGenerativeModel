# ownMelodyGenerativeModel

This repository is for Neural Audio Plugin Competition 2023.

- Requirement
  - JUCE
  - PyTorch
  - PyInstaller

- Procedure for execution
  1. Generate "train_generate.exe" using PyInstaller with python files (train_generate.exe).
  2. compile C++ codes with JUCE (need to specify the path of "train_generate.exe" in "PluginEditor.h").
  3. if the system has launched successfully, you need to create training data by your own (The more data, the better).
  4. By pressing "train button," your own generative model will be trained (takes few minutes for training).
  5. Your original generative model helps you by suggesting a subsequent melody by pressing "gen button"


- Screen capture of the GUI:


![GUI_capture](https://user-images.githubusercontent.com/3185518/225596122-a8c30f61-961e-43d1-8ca2-0171472278da.png)


---

- Video: 

https://youtu.be/zENPSFc5lxs


---

These codes are implemented by Tatsunori Hirai, Komazawa University, Japan.
Part of the code for the synthesizer implementation was referred to the audio programmer's tutorial.
Thank you!
