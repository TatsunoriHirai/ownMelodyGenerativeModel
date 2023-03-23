
to use these python codes in JUCE.
you have to generate exe file using pyinstaller.
generate "train_generate.exe" using "train_generate.py"

---
pyinstaller command is:
pyinstaller train_generate.py --onefile
---

use the generated exe file for JUCE project.

# usage of train_generate.exe:

- first argument: "train" or "generate"
- second argument: cpu or gpu
- third argument: training folder path (train mode) / input melody path (gen mode)
- fourth argument: model file path (output model file (train) or using model file (gen) )
- fifth argument: # of epoch (train mode) / # of output note length (gen mode)
- sixth argument (only for gen mode): output melody file path

# arg example: 
args = ["train_generate.exe","train", "GPU","C:/Users/hirait/Desktop/melodyData/", 
	"C:/Users/hirait/Desktop/output.model", "LSTM", "100"]
args = ["train_generate.exe","generate","GPU", "C:/Users/hirait/Desktop/inputmelody_0.txt", 
        "C:/Users/hirait/Desktop/output.model", "16","C:/Users/hirait/Desktop/outputmelody.txt"]

# training command sample:
"C:/Users/hirait/train_generate/train_generate.exe train GPU C:/Users/hirait/Desktop/melodyData/ C:/Users/hirait/Desktop/output.model LSTM 100"

# generation command sample:
"C:/Users/hirait/train_generate/train_generate.exe generate GPU C:/Users/hirait/Desktop/inputmelody_0.txt C:/Users/hirait/Desktop/output.model 16 C:/Users/hirait/Desktop/outputmelody.txt"


****
Author's environment:
Windows 11
Visual Studio 2022
JUCE v.7.0.5
pyTorch 1.9.0


****
for latest version of pyTorch (as of 12th, March, 2023),
use "LSTM_forNewerPyTorch.py" instead of "LSTM.py"
works on pyTorch ver. 1.13.1

Haven't checked for the pyTorch 2.0 or later.



