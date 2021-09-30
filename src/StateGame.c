#include "Banks/SetBank4.h"

#include "..\res\src\tilesWorld.h"
#include "..\res\src\tilesUI.h"
#include "..\res\src\tilesDungeon.h"
#include "..\res\src\tilesSnake.h"
#include "..\res\src\tilesEye.h"
#include "..\res\src\mapSnake.h"
#include "..\res\src\mapEye.h"
#include "..\res\src\mapFrog.h"
#include "..\res\src\mapWorld.h"

#include "..\res\src\nameSnake.h"
#include "..\res\src\nameFrog.h"
#include "..\res\src\nameEye.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

#include "Print.h"
#include "../res/src/font.h"
#include "../res/src/fontName.h"
#include "../res/src/player.h" //used for palettes
#include "../res/src/portal.h"
#include "../res/src/spike.h"
#include "../res/src/snake.h"
#include "../res/src/eyePupil.h"
#include "../res/src/fly.h"
#include "../res/src/frog.h"
#include "../res/src/tongue.h"
#include "Palette.h"

extern INT8 scroll_h_border;

UINT8 level = 0;
UINT8 boss1Beat = 0;
UINT8 boss2Beat = 0;
UINT8 boss3Beat = 0;

extern UINT8* bossFight_mod_Data[];
extern UINT8* jingle_mod_Data[];
extern UINT8* overworld_mod_Data[];


const UINT16 bg_palette_Snake[] = {PALETTE_FROM_HEADER(tilesSnake)}; //file name
const UINT16 bg_palette_Eye[] = {PALETTE_FROM_HEADER(tilesEye)}; //file name
const UINT16 bg_palette_Frog[] = {PALETTE_FROM_HEADER(tilesDungeon)}; //file name
const UINT16 bg_palette_World[] = {PALETTE_FROM_HEADER(tilesWorld)}; //file name
const UINT16 bg_palette_Name[] = {PALETTE_FROM_HEADER(fontName)}; //file name

const UINT16 sprites_palette_snake[] = {
	PALETTE_INDEX(player, 0),
	PALETTE_INDEX(spike, 1),
	PALETTE_INDEX(snake,  2),
	PALETTE_INDEX(player, 3),
	PALETTE_INDEX(portal, 4),
	PALETTE_INDEX(player, 5),
	PALETTE_INDEX(player, 6),
	PALETTE_INDEX(player, 7),
};
const UINT16 sprites_palette_eye[] = {
	PALETTE_INDEX(player, 0),
	PALETTE_INDEX(spike, 1),
	PALETTE_INDEX(eyePupil,  2),
	PALETTE_INDEX(player, 3),
	PALETTE_INDEX(player, 4),
	PALETTE_INDEX(player, 5),
	PALETTE_INDEX(player, 6),
	PALETTE_INDEX(player, 7),
};
const UINT16 sprites_palette_frog[] = {
	PALETTE_INDEX(player, 0),
	PALETTE_INDEX(spike, 1),
	PALETTE_INDEX(frog,  2),//frog 
	PALETTE_INDEX(fly, 3),
	PALETTE_INDEX(tongue, 4),//tongue
	PALETTE_INDEX(player, 5),
	PALETTE_INDEX(player, 6),
	PALETTE_INDEX(player, 7),
};


UINT8 collision_tiles[] = {1,84,85,94,100, 0}; //collides with tile 1 - MUST END IN ZERO
UINT8 collision_tiles_world[] = {19,20,21,22,23,24,25,26,27,28,29,30,31,32,43,44,45,46,48,50,51,52,53,54,55,56,57,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85, 0}; //collides with tile 1 - MUST END IN ZERO

UINT8 waterTimer;
UINT8 nameTimer;
UINT8 winTimer;
BYTE triggerWinJingle;

void WinJingle();

void Start_StateGame() {
	//UINT8 i;

    NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
    NR50_REG = 0x77; //Max volume
    
    

    scroll_bottom_movement_limit = 72; //center player in scroll
    scroll_top_movement_limit = 72;

    
    if (level == 0) // world map
    {
        
        SPRITES_8x16;
        SpriteManagerLoad(0); //player
        SpriteManagerLoad(9); //portal
        SHOW_SPRITES;

        scroll_target = SpriteManagerAdd(SpritePlayer, 18*8, 50*8);
        SpriteManagerAdd(SpritePortal, 11*8, 42*8);//1
        SpriteManagerAdd(SpritePortal, 31*8, 18*8);//2
        SpriteManagerAdd(SpritePortal, 6*8, 7*8);//3
        
        InitScroll(&mapWorld, collision_tiles_world, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_World, bank_StateGame);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_snake, bank_StateGame);

        if (boss1Beat == 1)
        {
            set_bkg_data(59,9,tilesWorld.data->data  + (16u * 134)); //check mark
        }
        if (boss2Beat == 1)
        {
            set_bkg_data(68,9,tilesWorld.data->data  + (16u * 134)); //check mark
        }
        if (boss3Beat == 1)
        {
            set_bkg_data(77,9,tilesWorld.data->data  + (16u * 134)); //check mark
        }

        SHOW_BKG;
        HIDE_WIN; 
        waterTimer = 0;

        PlayMusic(overworld_mod_Data,7,1);

    }
    else if (level == 1) //snake boss
    {
        SPRITES_8x16;
        SpriteManagerLoad(0); //player
        SpriteManagerLoad(1); //spike
        SpriteManagerLoad(2); //camera
        SpriteManagerLoad(3); //snake head
        SpriteManagerLoad(4); //snake body
        SHOW_SPRITES;

    //now set from player
        scroll_target = SpriteManagerAdd(SpriteCamera, 9*8, 13*8);
        
        InitScroll(&mapSnake, collision_tiles, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Snake, bank_StateGame);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_snake, bank_StateGame);

        SHOW_BKG;


        WX_REG = 7;
        WY_REG = 128;
        scroll_h_border = 8*2;
        SHOW_WIN;    

        PlayMusic(bossFight_mod_Data,7,1);    //boss music

    }
    else if (level == 3) //eye boss
    {
        SPRITES_8x16;
        SpriteManagerLoad(0); //player
        SpriteManagerLoad(1); //spike
        SpriteManagerLoad(2); //camera
        SpriteManagerLoad(5); //eye pupil
        SpriteManagerLoad(6); //eye H hurtbox
        SpriteManagerLoad(7); //eye V hurtbox
        SpriteManagerLoad(8); //laser
        SHOW_SPRITES;

    //now set from player
        scroll_target = SpriteManagerAdd(SpriteCamera, 9*8, 15*8);
        
        InitScroll(&mapEye, collision_tiles, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Eye, bank_StateGame);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_eye, bank_StateGame);

        SHOW_BKG;


        WX_REG = 7;
        WY_REG = 128;
        scroll_h_border = 8*2;
        SHOW_WIN;  

        PlayMusic(bossFight_mod_Data,7,1);    //boss music
        
    }
    else if (level == 2) //frog boss
    {
        SPRITES_8x16;
        SpriteManagerLoad(0); //player
        SpriteManagerLoad(1); //spike
        SpriteManagerLoad(2); //camera
        SpriteManagerLoad(10); //fly
        SpriteManagerLoad(11); //frog
        SpriteManagerLoad(12); //frog body
        SpriteManagerLoad(13); //frog body
        SHOW_SPRITES;

    //now set from player
        scroll_target = SpriteManagerAdd(SpriteCamera, 9*8, 13*8);
        
        InitScroll(&mapFrog, collision_tiles, 0);

        SetPalette(BG_PALETTE, 0, 8, bg_palette_Frog, bank_StateGame);
        SetPalette(SPRITES_PALETTE, 0, 8, sprites_palette_frog, bank_StateGame);

        SHOW_BKG;


        WX_REG = 7;
        WY_REG = 128;
        scroll_h_border = 8*2;
        SHOW_WIN;  

        PlayMusic(bossFight_mod_Data,7,1);    //boss music
    }
    else if (level == 4)
    {
        gbt_stop();
        InitScroll(&nameSnake, 0, 0);
        SetPalette(BG_PALETTE, 0, 8, bg_palette_Name, bank_StateGame);
        SHOW_BKG;
    }
    else if (level == 5)
    {
        gbt_stop();
        InitScroll(&nameFrog, 0, 0);
        SetPalette(BG_PALETTE, 0, 8, bg_palette_Name, bank_StateGame);
        SHOW_BKG;
    }
    else if (level == 6)
    {
        gbt_stop();
        InitScroll(&nameEye, 0, 0);
        SetPalette(BG_PALETTE, 0, 8, bg_palette_Name, bank_StateGame);
        SHOW_BKG;
    }
    

    if (level != 0 && level < 4)
    {
        INIT_FONT(font, PRINT_WIN); 
        set_bkg_data(4,11,tilesUI.data->data  + (16u * 4)); //load UI tiles into memory
        set_bkg_data(80,47,tilesDungeon.data->data  + (16u * 80)); //load dungeon tiles into memory
    }

}

void Update_StateGame() {
    if (level == 1 || level == 2 ||level == 3)
    {
        PRINT_POS(3,0);
        Printf("HP:");
    }
    else
    {
        if (level < 4)
        {
            if (waterTimer == 0)
            {
                //frame 1 
                set_bkg_data(23,6,tilesWorld.data->data  + (16u * 23)); //wall waves
                set_bkg_data(31,4,tilesWorld.data->data  + (16u * 31)); //corner out waves
                set_bkg_data(35,4,tilesWorld.data->data  + (16u * 35)); //corner in waves
                set_bkg_data(40,3,tilesWorld.data->data  + (16u * 40)); //surface waves
            }
            else if (waterTimer == 30)
            {
                //frame 2
                set_bkg_data(23,6,tilesWorld.data->data  + (16u * 100)); //wall waves
                set_bkg_data(31,4,tilesWorld.data->data  + (16u * 112)); //corner out waves
                set_bkg_data(35,4,tilesWorld.data->data  + (16u * 120)); //corner in waves
                set_bkg_data(40,3,tilesWorld.data->data  + (16u * 128)); //surface waves
            }
            else if (waterTimer == 60)
            {
                //frame 3
                set_bkg_data(23,6,tilesWorld.data->data  + (16u * 106)); //wall waves
                set_bkg_data(31,4,tilesWorld.data->data  + (16u * 116)); //corner out waves
                set_bkg_data(35,4,tilesWorld.data->data  + (16u * 124)); //corner in waves
                set_bkg_data(40,3,tilesWorld.data->data  + (16u * 131)); //surface waves
            }
            else if (waterTimer == 90)
            {
                //frame 2
                set_bkg_data(23,6,tilesWorld.data->data  + (16u * 100)); //wall waves
                set_bkg_data(31,4,tilesWorld.data->data  + (16u * 112)); //corner out waves
                set_bkg_data(35,4,tilesWorld.data->data  + (16u * 120)); //corner in waves
                set_bkg_data(40,3,tilesWorld.data->data  + (16u * 128)); //surface waves
            }

            if (waterTimer != 120)
            {
                waterTimer++;
            }
            else
            {
                waterTimer = 0;
            }
        }
    }

    if (nameTimer != 0)
    {
        nameTimer--;
        if (nameTimer == 0)
        {
            level = level - 3;
            SetState(StateGame);
        }
    }

    if (winTimer != 0)
    {
        winTimer--;
        if (winTimer == 0)
        {
            if (level == 1)
            {
                boss1Beat = 1;
                level = 0;
                SetState(StateGame);
            }
            else if (level == 2)
            {
                boss2Beat = 1;
                level = 0;
                SetState(StateGame);
            }
            else if (level == 3)
            {
                boss3Beat = 1;
                level = 0;
                SetState(StateGame);
            }
        }
    }
    
    if (triggerWinJingle == 1)
    {
        WinJingle();
        triggerWinJingle = 0;
    }
    
}


void WinJingle()
{
    winTimer = 60*3;
    PlayMusic(jingle_mod_Data,7,0);    //win music
}
