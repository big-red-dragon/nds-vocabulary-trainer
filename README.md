[Screenshot](https://i.imgur.com/HQrY92L.png)

# What it is and how to use
I use this program to learn vocabulary for Japanese. It supports any characters (like Kanji and Hiragana but also most other languages), and (simple) furigana (for when you don't know the pronunciation of the Kanji). It's a simple flash card program. You get to select answers until you found the right one. It will give you a maximum of 7 different words at the same time in its buffer so it never becomes overwhelming and the times between repetitions stay low. If you manage to get the right answer on first try, it will level up the word which makes its appearence less common. Words from higher levels will be always interspersed between the new words that you learn, so you'll always repeat. The upper screen displays how many words are at which level and your overal total progress.

Instead of using the touchscreen, you can also press X Y A B to select Answer 1 2 3 4.

You must save your progress by hitting the `Start` button. Due to performance, battery and flash storage lifetime concerns the program does not save automatically.

# How it works
In order to display the text on the screen a custom text rendering engine had to be built, which is the majority of the code. When the `nds-trainer-tool` converts a .txt file into a .voc file, it actually renders a bitmap of every different character and stores it together with information about its Y-Position, character width and left and right bearings in order to place the characters (especially latin ones) at the proper positions so the text is nicely readable. It also applies font anti-aliasing which is especially important for readability of small font sizes.
This also means that the *.voc file only contains the characters that are used instead of just loading in all of the Japanese / Chinese characters (which would cost way too much memory).


# Build Instructions
First, you need to build Qt project in the nds-trainer-tool. Then execute it with the command `nds-trainer-tool --help` to see all the options. 
Generally you need a picture for the background (found in nds-trainer-tool/assets/BG.png) which needs to be 256x192px. 
Then you need an image for the button (also found in the same folder). 
Lastly you'll need a text file for the vocabulary (see below for the format). 

Example command:  
```
nds-trainer-tool -x assets/Button_Raw.png -b assets/BG.png -v assets/TextDB/sample.txt
```

This should leave you with 3 files:
- `background.bin` - Move this file into `nds-vocabulary-trainer/data`
- `box.bin` - Move this into the same folder as `background.bin`
- `sample.voc` - Put this file into a folder of your choice. I create a `filesystem` folder where I put it. This one will be loaded at runtime by the .nds game. If you're running it from a card like M3, put it into the root directory there.

Next you need to build the .nds file. For that you need to download and setup devkitpro, then run `devkitPro\msys2\usr\bin\make` against the Makefile. If everything went right, it should build a `nds-vocabulary-trainer.nds` which you can copy together with the `sample.voc` file to your device or use in an emulator.

# Run in emulator

You need DeSmuME. Go to Emulation > GBA slot and select `MPCF Flash Card Device` from the dropdown. If your *.voc file is in the same directory as the *.nds file you can tick the option at the bottom that says `use path of the loaded ROM`, otherwise tick `folder` and Browse for the folder that contains the `*.voc` file.
Then you should be all set up! Keep in mind that the game creates a `*.sav` file with the same name as the `*.voc` file to save your progress.

# TextDB vocabulary format

The first 3 lines should start with the `#` character and each of them contains the configuration of a font on your PC. This font will be used to display the different languages. First font is for the first language, second and third fonts are for the second language and its furigana version if applicable. The string here is the format returned from `QFont::toString()`.

Then there are 2~3 columns separated by tab characters for the actual vocabulary. Although present in the `sample.txt` file, the system currently does not support HTML. But you can enter `\n` in order to create line breaks.