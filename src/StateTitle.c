#include "Banks/SetBank1.h"
#include "rand.h"
#include "../res/src/spike.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Keys.h"

#include "Print.h"

#include "..\res\src\tilesTitle.h"
#include "..\res\src\title.h"
#include "Palette.h"


const UINT16 bg_palette_Title[] = {PALETTE_FROM_HEADER(tilesTitle)}; //file name

const UINT16 sprites_palette_title[] = {
	PALETTE_INDEX(spike, 0),
	PALETTE_INDEX(spike, 1),
	PALETTE_INDEX(spike,  2),
	PALETTE_INDEX(spike, 3),
	PALETTE_INDEX(spike, 4),
	PALETTE_INDEX(spike, 5),
	PALETTE_INDEX(spike, 6),
	PALETTE_INDEX(spike, 7),
};


void Start_StateTitle() {
	//UINT8 i;

    SPRITES_8x16;
    SpriteManagerLoad(1); //spike
    SHOW_SPRITES;

    NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
    NR50_REG = 0x77; //Max volume

    SpriteManagerAdd(SpriteSpike, 2*8, 10*8);
    SpriteManagerAdd(SpriteSpike, 16*8, 10*8);

    InitScroll(&title, 0, 0);

    SetPalette(BG_PALETTE, 0, 8, bg_palette_Title, bank_StateTitle);
    SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_title, bank_StateTitle);


}

void Update_StateTitle() {
    if (KEY_TICKED(J_START))
    {
        //seed random
        UINT16 seed = LY_REG;
        seed |= (UINT16)DIV_REG << 8;
        initrand(seed);

        SetState(StateTutorial);
    }

}



