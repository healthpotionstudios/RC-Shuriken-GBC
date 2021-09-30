#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Keys.h"
#include "../res/src/player.h"

struct Sprite *playerSpr;
struct Sprite *spikeSpr;

//UINT8 state = 1;

extern BYTE controllingPlayer;


void Start_SpriteCamera() {
    if (current_state == StateTutorial)
    {
        UINT8 i;
        struct Sprite* spr;
        SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
        {
            if (spr->type == SpritePlayer)
            {
                playerSpr = spr;
            }
            if (spr->type == SpriteSpike)
            {
                spikeSpr = spr;
            }
        }
        THIS->lim_x = 0xff;
        THIS->lim_y = 0xff;
        return;
    }
    playerSpr = SpriteManagerAdd(SpritePlayer, THIS->x, THIS->y);
    spikeSpr = SpriteManagerAdd(SpriteSpike, THIS->x, THIS->y-16);
    //scroll_target = THIS;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;
}

void Update_SpriteCamera() {
    
    // if (KEY_TICKED(J_B))
    // {
    //     state++;
    //     if (state == 3)
    //     {
    //         state = 0;
    //     }
    // }
    if (current_state == StateGame)
    {
        if (controllingPlayer == 1)
        {
            // THIS->x = playerSpr->x; 
            // THIS->y = playerSpr->y; 
            // THIS->x = (THIS->x + playerSpr->x) / 2; //smooth move towards player
            // THIS->y = (THIS->y + playerSpr->y) / 2;
            THIS->x = (UINT16)(THIS->x + ((((INT16)playerSpr->x) - (INT16)THIS->x) / 5));
            THIS->y = (UINT16)(THIS->y + ((((INT16)playerSpr->y) - (INT16)THIS->y) / 5));
        }
        else
        {
            // THIS->x = spikeSpr->x; 
            // THIS->y = spikeSpr->y;
            // THIS->x = (THIS->x + spikeSpr->x) / 2;
            // THIS->y = (THIS->y + spikeSpr->y) / 2;
            THIS->x = (UINT16)(THIS->x + ((((INT16)spikeSpr->x) - (INT16)THIS->x) / 5));
            THIS->y = (UINT16)(THIS->y + ((((INT16)spikeSpr->y) - (INT16)THIS->y) / 5));
        }
    }
    else
    {
        if (controllingPlayer == 1)
        {
            // THIS->x = playerSpr->x; 
            // THIS->y = playerSpr->y; 
            // THIS->x = (THIS->x + playerSpr->x) / 2; //smooth move towards player
            // THIS->y = (THIS->y + playerSpr->y) / 2;
            THIS->x = (UINT16)(THIS->x + ((((INT16)playerSpr->x) - (INT16)THIS->x) / 10));
            THIS->y = (UINT16)(THIS->y + ((((INT16)playerSpr->y) - (INT16)THIS->y) / 10));
        }
        else
        {
            // THIS->x = spikeSpr->x; 
            // THIS->y = spikeSpr->y;
            // THIS->x = (THIS->x + spikeSpr->x) / 2;
            // THIS->y = (THIS->y + spikeSpr->y) / 2;
            THIS->x = (UINT16)(THIS->x + ((((INT16)spikeSpr->x) - (INT16)THIS->x) / 10));
            THIS->y = (UINT16)(THIS->y + ((((INT16)spikeSpr->y) - (INT16)THIS->y) / 10));
        }
    }

} 

void Destroy_SpriteCamera() {
}

