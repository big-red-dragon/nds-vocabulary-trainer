#ifndef VOKABELTRAINER_H
#define VOKABELTRAINER_H

#include "defines.h"
#include "VocDB.h"
#include "TyFile.h"
#include "TyColor.h"
#include "TyString.h"

#define NUM_LEVELS 10
#define VOCS_AT_ONCE 7
#define POS_NUMVOCS 8
#define POS_CURLIST POS_NUMVOCS + NUM_LEVELS * 4
#define POS_DATA POS_CURLIST + VOCS_AT_ONCE*4

#define MODE_FRONT true
#define MODE_BACK false
#define STATE_NORMAL 0
#define STATE_WRONG 1
#define STATE_RIGHT 2
#define STATE_HELP 3
#define FLAG_NONE 0
#define FLAG_WASWRONG 1

extern const ushort VocOrder[];

class Vokabeltrainer
{
public:
    Vokabeltrainer();

    void onKeyEvent(uint k);

private:
    enum LevelDirection {
        Level_Up,
        Level_Down
    };

    struct VocEntry {
        ushort id;
        uchar levelP;

        ushort pos;
        uchar level;
        uchar flags;
    };


    VocDB vdb;
    TyFile vocFile;
    bool mode;
    bool isWrong;
    ushort question;
    ushort answers[4];
    uchar answerStates[4];
    uchar answerEndStates[4];
    ushort vocPos;
    bool bottomScreen;
    bool saved;
    ushort * sIdData;
    uchar * sLevelPData;
    uint seed;

    ushort numVocsLevel[NUM_LEVELS];
    ushort numVocsLevelP[NUM_LEVELS]; // pseudo

    VocEntry curList[VOCS_AT_ONCE];
    ushort posInList;

    ushort * buttons[4];

    void init();

    void selectAnswer(uchar ans);

    VocEntry getNewVoc();
    void setAnswers(ushort ansId);
    bool isLevelUnlocked(uchar lvl);
    void noMoreVocs();
    void showCurrentVoc();
    void showFinishScreen();

    // saves:
    void loadSave();
    void createNewSave();
    void editSave(ushort numVocs);
    void save();
    void saveListEntry(uchar p);
    void saveListPos();
    void changeLevel(VocEntry& entry, LevelDirection dir);
    void swapVoc(ushort pOld, ushort pNew);
    void makeSeed();

    // rendering:
    void swapDisplays();
    void createInterface();
    void drawButton(uchar p);
    void drawQuestionBox(int id);
    void drawEntry(int id, int y, int w, bool front, ushort *buffer);
    void showStatistics();
    static void renderBox(uchar y);
    static void renderBackground();
    static void renderBackBuffer();
};

#endif // VOKABELTRAINER_H
