#include "Banks/SetBank1.h"

#include "..\res\src\tilesTutorial.h"
#include "..\res\src\tutorial1.h"
#include "..\res\src\tutorial2.h"
#include "..\res\src\tutorial3.h"

#include "../res/src/player.h"
#include "../res/src/spike.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Keys.h"

#include "Print.h"


#include "Palette.h"

UINT8 page = 1;
UINT8 tutCameraTimer = 15;

struct Sprite* tutSpr[2];


extern BYTE controllingPlayer;


const UINT16 bg_palette_Tutorial[] = {PALETTE_FROM_HEADER(tilesTutorial)}; //file name

UINT16 sprites_palette_tutorial[] = {
	PALETTE_INDEX(player, 0),
	PALETTE_INDEX(spike, 1),
	PALETTE_INDEX(player,  2),
	PALETTE_INDEX(player, 3),
	PALETTE_INDEX(player, 4),
	PALETTE_INDEX(player, 5),
	PALETTE_INDEX(player, 6),
	PALETTE_INDEX(player, 7),
};


void Start_StateTutorial() {
	//UINT8 i;

    NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
    NR50_REG = 0x77; //Max volume

    
    SPRITES_8x16;
    SpriteManagerLoad(0); //player
    SpriteManagerLoad(1); //spike
    SpriteManagerLoad(2); //camera
    SHOW_SPRITES;
    
    if (page == 1)
    {
        SpriteManagerAdd(SpriteSpike, 3*8, 6*8);
        SpriteManagerAdd(SpriteSpike, 12*8, 6*8);
        
        InitScroll(&tutorial1, 0, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Tutorial, bank_StateTutorial);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_tutorial, bank_StateTutorial);

        SHOW_BKG;
    }
    else if (page == 2)
    {
        SpriteManagerAdd(SpriteSpike, 3*8, 10*8);
        SpriteManagerAdd(SpriteSpike, 12*8, 10*8);
        
        InitScroll(&tutorial2, 0, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Tutorial, bank_StateTutorial);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_tutorial, bank_StateTutorial);

        SHOW_BKG;
    }
    else if (page == 3)
    {
        tutSpr[0] = SpriteManagerAdd(SpriteSpike, 6*8, 9*8);
        tutSpr[1] = SpriteManagerAdd(SpritePlayer, 22*8, 9*8);
        scroll_target = SpriteManagerAdd(SpriteCamera, 6*8, 9*8);

        // SetSpriteAnim(tutSpr[0],anim_none,0);
        // SetSpriteAnim(tutSpr[1],anim_none,0);
        //tutSpr[0]->anim_speed=0;
        tutSpr[1]->anim_speed=0;
        controllingPlayer = 0;
        
        InitScroll(&tutorial3, 0, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Tutorial, bank_StateTutorial);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_tutorial, bank_StateTutorial);

        SHOW_BKG;
    }

}

void Update_StateTutorial() {
    if (KEY_TICKED(J_A))
    {
        page++;
        if (page == 4)
        {
            SetState(StateGame);
        }
        else
        {
            SetState(StateTutorial);
        }
    }

    if (page == 3)
    {
        tutCameraTimer--;
        if (tutCameraTimer == 0)
        {
            controllingPlayer = !controllingPlayer;
            if (controllingPlayer == 0)
            {
                tutSpr[0]->anim_speed=20;
                tutSpr[1]->anim_speed=0;
            }
            else
            {
                tutSpr[0]->anim_speed=0;
                tutSpr[1]->anim_speed=15;
            }
            tutCameraTimer = 120;
        }
    }   
}



