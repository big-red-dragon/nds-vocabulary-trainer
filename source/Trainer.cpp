#include "Trainer.h"
#include <nds.h>
#include <fat.h>
#include <time.h>
#include "TyColor.h"
#include "TyString.h"

#include "background_bin.h"
#include "box_bin.h"
#define VOCNAME "sample"

const uchar buttonWidth = box_bin[0];
const uchar buttonHeight = box_bin[1];

Trainer::Trainer()
{
    init();
}

void Trainer::init()
{
    lcdMainOnBottom();
    bottomScreen = true;
    saved = false;

    videoSetMode(MODE_FB0);
    vramSetBankA(VRAM_A_LCD);
    consoleDemoInit();

    fatInitDefault();

    vocFile.setFileName(VOCNAME ".voc");
    if(!vocFile.open("rb"))
        dmaFillHalfWords(RGB15(0, 31, 0) | BIT(15), back_buffer, 256*192*2);
    vdb.setFile(&vocFile);

    mode = false;

    for(int i = 0; i < 4; i++) {
        buttons[i] = new ushort[box_bin[0]*box_bin[1]];
    }

    loadSave();
    showCurrentVoc();
}

void Trainer::onKeyEvent(uint k)
{
    if(k & KEY_LEFT) {
        swapDisplays();
    }
    if(k & KEY_TOUCH && bottomScreen) {
        touchPosition touch;
        touchRead(&touch);
        uchar x = (256-buttonWidth)/2;
        for(int i = 0; i < 4; i++) { // for every button
            uchar y = 50 + i * 35;
            if(touch.px >= x && touch.px < x+buttonWidth) {
                if(touch.py >= y && touch.py < y+buttonHeight) {
                    selectAnswer(i);
                    break;
                }
            }
        }
    }
    if(k & KEY_X)
        selectAnswer(0);
    if(k & KEY_Y)
        selectAnswer(1);
    if(k & KEY_A)
        selectAnswer(2);
    if(k & KEY_B)
        selectAnswer(3);
    if(k & KEY_START)
        save();
}

void Trainer::selectAnswer(uchar ans)
{
    drawButton(ans);
    renderBackBuffer();

    if(answerEndStates[ans] == STATE_WRONG) {
        isWrong = true;
        return;
    }

    // if curVoc is finished:

    if(isWrong) {
        curList[posInList].flags |= FLAG_WASWRONG;
    } else {
        LevelDirection direction;
        VocEntry &entry = curList[posInList];
        if(entry.flags & FLAG_WASWRONG) {
            direction = Level_Down;
        } else {
            direction = Level_Up;
        }
        changeLevel(curList[posInList], direction);
        curList[posInList].flags = -1;
        curList[posInList].id = -1;
        curList[posInList].pos = -1;
        curList[posInList].level = -1;
        curList[posInList].levelP = -1;
        curList[posInList] = getNewVoc();
    }
    posInList++;
    if(posInList == VOCS_AT_ONCE)
        posInList = 0;

    saved = false;
    showCurrentVoc();
}

void Trainer::showCurrentVoc()
{
    showStatistics();
    answerStates[0] = STATE_NORMAL;
    answerStates[1] = STATE_NORMAL;
    answerStates[2] = STATE_NORMAL;
    answerStates[3] = STATE_NORMAL;

    isWrong = false;
    question = curList[posInList].id;
    int numTries = VOCS_AT_ONCE+1;
    while(question == 0xffff) {
        posInList++;
        if(numTries == 0) {
            showFinishScreen();
            return;
        }
        numTries--;
        if(posInList == VOCS_AT_ONCE)
            posInList = 0;
        question = curList[posInList].id;
    }
    setAnswers(question);

    for(int i = 0; i < 4; i++) {
        ushort ansId = answers[i];
        if(question == answers[i]) {
            answerEndStates[i] = STATE_RIGHT;
            continue;
        }
        TyString s1 = vdb.getEntry(question, 0);
        TyString s2 = vdb.getEntry(ansId, 0);
        TyString s3 = vdb.getEntry(question, 1);
        TyString s4 = vdb.getEntry(ansId, 1);
        if(!(s1 == s2)) {
            if(!(s3 == s4)) {
                // Answer Wrong!
                s1.destroy();
                s2.destroy();
                s3.destroy();
                s4.destroy();
                answerEndStates[i] = STATE_WRONG;
                continue;
            }
        }
        // Answer Right!
        s1.destroy();
        s2.destroy();
        s3.destroy();
        s4.destroy();
        answerEndStates[i] = STATE_RIGHT;
    }

    createInterface();
    renderBackBuffer();
}

void Trainer::makeSeed()
{
    time_t t = time(0);
    uint seed = t;
    for(uchar i = 0; i < NUM_LEVELS; i++) {
        seed += 100000*numVocsLevel[i];
    }
    srand(seed);
}

Trainer::VocEntry Trainer::getNewVoc()
{
    VocEntry entry;
    uchar level = -1;
    int numLevelsUnlocked = 0;

    for(uchar x = 0; x != NUM_LEVELS; x++) {
        if(isLevelUnlocked(x)) {
            numLevelsUnlocked++;
        }
    }    

    if(numLevelsUnlocked == 0) {
        entry.id = 0xffff;
        entry.pos = -1;
        return entry;
    }

    int maxRnd = 1 << (numLevelsUnlocked);
    int r = rand() % (maxRnd-1);
    if(r == 0) r = maxRnd-1;

    int pos = numLevelsUnlocked;
    for(uchar x = 0; x != NUM_LEVELS; x++) {
        if(isLevelUnlocked(x)) {
            pos--;
            if(r >= (1 << pos)) {
                level = x;
                break;
            }
        }
    }

    int vp = 0;
    for(uchar i = NUM_LEVELS-1; i > level; i--) {
        vp += numVocsLevel[i];
    }

    int vpEnd = vp + numVocsLevel[level];
    int vp2 = rand() % numVocsLevel[level];

    int numTries = VOCS_AT_ONCE;
    bool retry = true;
    do {
        if(numTries == 0) {
            entry.id = 0xffff;
            return entry;
        }
        retry = false;
        for(int i = 0; i < VOCS_AT_ONCE; i++) {
            if(curList[i].pos == vp + vp2) {
                retry = true;
                break;
            }
        }
        if(retry) {
            vp2++;
            if(vp+vp2 == vpEnd) {
                vp2 = 0;
            }
        }

        numTries--;
    } while(retry);

    int vpt = vp + vp2;

    entry.id = sIdData[vpt];
    entry.levelP = sLevelPData[vpt];
    entry.level = level;
    entry.pos = vpt;
    entry.flags = FLAG_NONE;
    return entry;
}

bool Trainer::isLevelUnlocked(uchar lvl)
{
    if(numVocsLevel[lvl] == 0)
        return false;

    int v = 0;
    for(int i = 0; i < VOCS_AT_ONCE; i++) {
        if(curList[i].id != 0xffff && curList[i].level == lvl) {
            v++;
        }
    }
    if(v == numVocsLevel[lvl])
        return false;

    if(lvl == 0) {
        if(numVocsLevel[lvl+1] < 20)
            return true;
        else
            return false;
    }

    return true;
}

void Trainer::setAnswers(ushort ansId)
{
    uchar posTruth = rand() % 4;
    for(uchar p = 0; p < 4; p++) {
        if(p == posTruth) {
            answers[p] = ansId;
            continue;
        }
        bool reshuffle = true;
        ushort vocid;
        int maxReshuffle = 100; // prevent infinity loop
        do {
            maxReshuffle--;
            vocid = rand() % vdb.numEntries;
            if(vocid == ansId)
                continue;
            reshuffle = false;
            for(uchar x = 0; x < p; x++) {
                if(vocid == answers[x]) {
                    reshuffle = true;
                    break;
                }
            }
        } while(reshuffle && maxReshuffle != 0);
        answers[p] = vocid;
    }
}

void noMoreVocs() {

}

void Trainer::loadSave()
{
    TyFile saveFile(VOCNAME ".sav");
    if(!saveFile.open("rb")) {
        createNewSave();
    } else {
        ushort numVocsTotal = saveFile.readShort();
        if(numVocsTotal != vdb.numEntries) {
            editSave(numVocsTotal);
            saveFile.seek(4);
        }
        vocPos = saveFile.readShort();
        posInList = saveFile.readShort();
        saveFile.readShort(); // skip reserved
        for(uchar i = 0; i < NUM_LEVELS; i++) {
            numVocsLevel[i] = saveFile.readShort();
            numVocsLevelP[i] = saveFile.readShort();
        }
        for(uchar i = 0; i < VOCS_AT_ONCE; i++) {
            curList[i].pos = saveFile.readShort();
            curList[i].level = saveFile.readChar();
            curList[i].flags = saveFile.readChar();
        }
        sIdData = (ushort*)malloc(vdb.numEntries*4);
        sLevelPData = (uchar*)malloc(vdb.numEntries*4);
        for(ushort i = 0; i < vdb.numEntries; i++) {
            sIdData[i] = saveFile.readShort();
            sLevelPData[i] = saveFile.readShort();
        }

        for(uchar i = 0; i < VOCS_AT_ONCE; i++) {
            saveFile.seek(POS_DATA + curList[i].pos * 4);
            curList[i].id = saveFile.readShort();
            curList[i].levelP = saveFile.readShort();
        }



        saveFile.close();
        makeSeed();
    }
}

void Trainer::createNewSave()
{
    // reset data

    vocPos = 0;
    posInList = 0;

    for(uchar i = 1; i < NUM_LEVELS; i++) {
        numVocsLevel[i] = 0;
        numVocsLevelP[i] = 0;
    }
    numVocsLevel[0] = vdb.numEntries;
    numVocsLevelP[0] = vdb.numEntries;

    sIdData = (ushort*)malloc(vdb.numEntries*2);
    sLevelPData = (uchar*)malloc(vdb.numEntries);

    for(ushort i = 0; i < vdb.numEntries; i++) {
        sIdData[i] = i; // id
        sLevelPData[i] = 0; // shown level
    }

    for(uchar i = 0; i < VOCS_AT_ONCE; i++) {
        curList[i].flags = -1;
        curList[i].id = -1;
        curList[i].pos = -1;
        curList[i].level = -1;
        curList[i].levelP = -1;
    }

    makeSeed();

    for(uchar i = 0; i < VOCS_AT_ONCE; i++) {
        curList[i] = getNewVoc();
    }
}

void Trainer::editSave(ushort numVocs)
{
    if(numVocs < vdb.numEntries) {
        // add the vocs
        sIdData = (ushort*)realloc(sIdData, vdb.numEntries*2);
        sLevelPData = (uchar*)realloc(sLevelPData, vdb.numEntries);
        for(ushort i = numVocs; i < vdb.numEntries; i++) {
            numVocsLevel[0]++;
            numVocsLevelP[0]++;
            sIdData[i] = i;
            sLevelPData[i] = 0;
        }
    } else {
        // too difficult to implement for now
    }
}

void Trainer::save()
{
    if(saved)
        return;
    saved = true;
    TyFile saveFile(VOCNAME ".sav");
    saveFile.open("wb");
    saveFile.writeShort(vdb.numEntries);
    saveFile.writeShort(vocPos);
    saveFile.writeShort(posInList);
    saveFile.writeShort(0); // reserved

    for(uchar i = 0; i < NUM_LEVELS; i++) {
        saveFile.writeShort(numVocsLevel[i]);
        saveFile.writeShort(numVocsLevelP[i]);
    }

    for(uchar i = 0; i < VOCS_AT_ONCE; i++) {
        saveFile.writeShort(curList[i].pos); //pos
        saveFile.writeChar(curList[i].level);
        saveFile.writeChar(curList[i].flags);
    }

    for(ushort i = 0; i < vdb.numEntries; i++) {
        saveFile.writeShort(sIdData[i]); // id
        saveFile.writeShort(sLevelPData[i]); // shown level
    }
    saveFile.close();

    printf("Saved!\n");
}

void Trainer::changeLevel(VocEntry &entry, LevelDirection dir)
{
    int spos = entry.pos;
    int lvl = entry.level;
    int lvlP = entry.levelP;

    int vp = 0;
    for(uchar i = NUM_LEVELS-1; i > lvl; i--) {
        vp += numVocsLevel[i];
    }

    int vpLast = vp + numVocsLevel[lvl]-1;

    if(dir == Level_Up || lvl == 0) {
        if(lvl == NUM_LEVELS-1) {
            return;
        }
        numVocsLevel[lvl]--;
        numVocsLevel[lvl+1]++;
        if(lvl == lvlP) {
            numVocsLevelP[lvl]--;
            numVocsLevelP[lvl+1]++;
            sLevelPData[spos] = lvlP+1;
        }
        swapVoc(vp, spos);
    } else {
        if(lvl == 1) {
            return;
        }
        numVocsLevel[lvl]--;
        numVocsLevel[lvl-1]++;

        swapVoc(vpLast, spos);
    }
}

void Trainer::swapVoc(ushort pOld, ushort pNew)
{
    if(pOld == pNew)
        return;
    // Check if we break something and fix it!
    for(int i = 0; i < VOCS_AT_ONCE; i++) {
        if(curList[i].pos == pOld) {
            curList[i].pos = pNew;
            break;
        }
    }
    ushort tempLevelP;
    ushort tempId;

    tempId = sIdData[pOld];
    tempLevelP = sLevelPData[pOld];

    sIdData[pOld] = sIdData[pNew];
    sLevelPData[pOld] = sLevelPData[pNew];

    sIdData[pNew] = tempId;
    sLevelPData[pNew] = tempLevelP;
}

void Trainer::createInterface()
{
    uchar x = (256-buttonWidth)/2;

    // Background
    renderBackground();

    drawQuestionBox(question);

    // Boxes
    for(int i = 0; i < 4; i++) { // bg for buttons
        uchar y = 50 + i * 35;
        renderBox(y);

        // make buttons darker when pressed:
        for(uchar py = 0; py < buttonHeight; py++) {
            for(uchar px = 0; px < buttonWidth; px++) {
                buttons[i][py*buttonWidth+px] = back_buffer[x+px+(y+py)*256];
            }
        }
        TyColor newColor(0, 0, 0);
        if(answerEndStates[i] == STATE_RIGHT)
            newColor = TyColor(0, 22, 0);
        else if(answerEndStates[i] == STATE_WRONG)
            newColor = TyColor(31, 0, 0);
        else
            newColor = TyColor(31, 23, 0);
        for(int dy = 0; dy < buttonHeight; dy++) {
            for(int dx = 0; dx < buttonWidth; dx++) {
                int p = 256*(y+dy)+dx+x;
                TyColor Color = TyColor(back_buffer[p]);
                uchar alpha = box_bin[2 + dy*buttonWidth + dx];
                Color.blend(newColor, alpha);

                buttons[i][dy*buttonWidth+dx] = Color.toRGB15();
            }
        }

        // font
        drawEntry(answers[i], 0, buttonWidth, mode, buttons[i]);
        drawEntry(answers[i], y, 256, mode, back_buffer);
    }
}

void Trainer::drawButton(uchar p)
{
    uchar x = (256-buttonWidth)/2;
    uchar y = 50 + p * 35;

    for(uchar py = 0; py < buttonHeight; py++) {
        for(uchar px = 0; px < buttonWidth; px++) {
            back_buffer[x+px+(y+py)*256] = buttons[p][py*buttonWidth+px];
        }
    }
}

void Trainer::drawQuestionBox(int id)
{
    int y = 0;
    renderBox(y);
    drawEntry(id, y, 256, !mode, back_buffer);
}

void Trainer::drawEntry(int id, int y, int w, bool front, ushort * buffer)
{
    if(front) {
        TyString str = vdb.getEntry(id, 0);
        if(str.numLines() == 1)
            vdb.renderEntry(0, y+19, w, id, 0, buffer);
        else
            vdb.renderEntry(0, y+14, w, id, 0, buffer);
    } else {
        TyString str = vdb.getEntry(id, 2);
        if(str.length() == 0) {
            vdb.renderEntry(0, y+24, w, id, 1, buffer);
        } else { // 10 27
            vdb.renderEntry(0, y+17, w, id, 1, buffer);
            vdb.renderEntry(0, y+29, w, id, 2, buffer);
        }
        str.destroy();
    }
}

/********* static functions *********/

void Trainer::renderBox(uchar y)
{
    uchar width = box_bin[0];
    uchar x = (256-width)/2;
    uchar height = box_bin[1];
    TyColor newColor(0, 0, 0);
    for(int dy = 0; dy < height; dy++) {
        for(int dx = 0; dx < width; dx++) {
            int p = 256*(y+dy)+dx+x;
            TyColor Color = TyColor(back_buffer[p]);
            uchar alpha = box_bin[2 + dy*width + dx];
            if(alpha < 255 - 30)
                alpha += 30;
            else
                alpha = 255;
            Color.blend(newColor, alpha);

            back_buffer[p] = Color.toRGB15();
        }
    }
}


void Trainer::renderBackground()
{
    dmaCopyWords(0, background_bin, back_buffer, 256*192*2);
}

void Trainer::renderBackBuffer()
{
    swiWaitForVBlank();
    for(int i = 0; i < 256*192; i++) {
        VRAM_A[i] = back_buffer[i];
    }
}

void Trainer::showStatistics()
{
    swiWaitForVBlank();
    consoleClear();
    uint percent = 0;
    for(int i = 0; i < NUM_LEVELS; i++) {
        printf("%i: %i\n", i, numVocsLevel[i]);
        percent += float(numVocsLevel[i])/vdb.numEntries*float(i)/(NUM_LEVELS-1)*100;
    }
    printf("Progress: %i%%\n", percent);
}

void Trainer::showFinishScreen()
{
    printf("You have FINISHED!!!\n");
}

void Trainer::swapDisplays()
{
    bottomScreen = !bottomScreen;
    lcdSwap();
}
