#include "Banks/SetBank1.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"
#include "BankManager.h"

UINT8 direction;
const UINT8 anim_idle_forward[] = {1, 0};
const UINT8 anim_walk_forward[] = {2, 1, 2};
const UINT8 anim_idle_right[] = {1, 3};
const UINT8 anim_walk_right[] = {2, 4, 5};
const UINT8 anim_idle_up[] = {1, 6};
const UINT8 anim_walk_up[] = {2, 7, 8};

BYTE controllingPlayer;

const unsigned char playerSelectedUiTiles[] = {4, 5, 6, 7};
const unsigned char spikeSelectedUiTiles[] = {8, 9, 10, 11};

extern UINT8 level;
extern UINT8 nameTimer;


void UpdateUI();
void RefreshScreen();

void Start_SpritePlayer()
{
    if (current_state == StateTutorial)
    {
        SetSpriteAnim(THIS, anim_walk_forward, 15);
        THIS->lim_x = 0xff;
        THIS->lim_y = 0xff;
        return;
    }

    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;
    controllingPlayer = 1;
    UpdateUI();
    direction = 0; //forward

    //scroll_target = THIS;
}

void Update_SpritePlayer()
{
    
    if (current_state == StateTutorial)
    {
        return;
    }

    //delta time is used to prevent lag - if there's lag, speed will be multiplied by 2

    if (KEY_TICKED(J_A))
    {
        if (level != 0)
        {
            controllingPlayer = !controllingPlayer;
            UpdateUI();
        }
    }
    
    if (KEY_TICKED(J_SELECT))
    {
        level = 0;
        SetState(StateGame);
    }

    if (controllingPlayer)
    {
        if (KEY_PRESSED(J_UP))
        {
            direction = 2;
            TranslateSprite(THIS, 0, -1 << delta_time); //THIS, X, Y
            SPRITE_UNSET_VMIRROR(THIS);
        }
        if (KEY_PRESSED(J_DOWN))
        {
            direction = 0;
            TranslateSprite(THIS, 0, 1 << delta_time);
            SPRITE_UNSET_VMIRROR(THIS);
        }
        if (KEY_PRESSED(J_LEFT))
        {
            direction = 3;
            TranslateSprite(THIS, -1, 0 << delta_time);
            SPRITE_SET_VMIRROR(THIS);
        }
        if (KEY_PRESSED(J_RIGHT))
        {
            direction = 1;
            TranslateSprite(THIS, 1, 0 << delta_time);
            SPRITE_UNSET_VMIRROR(THIS);
        }

        if (KEY_PRESSED(J_UP) || KEY_PRESSED(J_DOWN) || KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_LEFT))
        {
            switch (direction)
            {
            case 0:
                SetSpriteAnim(THIS, anim_walk_forward, 15);
                break;
            case 1:
                SetSpriteAnim(THIS, anim_walk_right, 15);
                break;
            case 2:
                SetSpriteAnim(THIS, anim_walk_up, 15);
                break;
            case 3:
                SetSpriteAnim(THIS, anim_walk_right, 15);
                break;

            default:
                break;
            }
        }
    }
    if (keys == 0 || controllingPlayer == 0)
    {
        switch (direction)
        {
        case 0:
            SetSpriteAnim(THIS, anim_idle_forward, 15);
            break;
        case 1:
            SetSpriteAnim(THIS, anim_idle_right, 15);
            break;
        case 2:
            SetSpriteAnim(THIS, anim_idle_up, 15);
            break;
        case 3:
            SetSpriteAnim(THIS, anim_idle_right, 15);
            break;

        default:
            break;
        }
    }

    UINT8 i;
    struct Sprite *spr;
    SPRITEMANAGER_ITERATE(i, spr) //iterates over active sprites
    {
        if (spr->type == SpriteBossSnakeHead || spr->type == SpriteBossSnakeBody ||
            spr->type == SpriteBossEyePupil || spr->type == SpriteBossEyeHHurtbox || spr->type == SpriteBossEyeVHurtbox || spr->type == SpriteBossLaserBall ||
            spr->type == SpritePortal ||
            spr->type == SpriteBossFly || spr->type == SpriteBossFrog || spr->type == SpriteBossFrogBody || spr->type == SpriteBossFrogTongue)
        {
            if (CheckCollision(THIS, spr))
            {
                if (spr->type != SpritePortal)
                {
                    PlayFx(CHANNEL_1, 10, 0x4f, 0xc7, 0xf3, 0x73, 0x86);
                    SetState(StateGame);
                }
                else
                {
                    PlayFx(CHANNEL_1,10,0x17,0x09,0xf2,0xe4,0x85);
                    nameTimer = 120;
                    if (spr->y == 42 * 8)
                    {
                        level = 4;
                    }
                    else if (spr->y == 18 * 8)
                    {
                        level = 5;
                    }
                    else if (spr->y == 7 * 8)
                    {
                        level = 6;
                    }
                    SetState(StateGame);
                }
            }
        }
    }
}

void Destroy_SpritePlayer()
{
}

void UpdateUI()
{
    if (controllingPlayer == 1)
    {
        set_win_tiles(0, 0, 2, 2, playerSelectedUiTiles);
    }
    else
    {
        set_win_tiles(0, 0, 2, 2, spikeSelectedUiTiles);
    }
}

/*

// adapted from https://github.com/bbbbbr/plutoscorner/blob/7e362c33bc80b0de24cbb547a05325f4bfa01040/src/zgb_utils.c
// Copied from Scroll.h  >> used in RefreshScreen
#define SCREEN_TILES_W       20 // 160 >> 3 = 20
#define SCREEN_TILES_H       18 // 144 >> 3 = 18
#define SCREEN_PAD_LEFT   1
#define SCREEN_PAD_RIGHT  2
#define SCREEN_PAD_TOP    1
#define SCREEN_PAD_BOTTOM 2

#define SCREEN_TILE_REFRES_W (SCREEN_TILES_W + SCREEN_PAD_LEFT + SCREEN_PAD_RIGHT)
#define SCREEN_TILE_REFRES_H (SCREEN_TILES_H + SCREEN_PAD_TOP  + SCREEN_PAD_BOTTOM)
/////////

//unused for heavy lag
void RefreshScreen()
{
    UINT8 i;
    INT16 y;

    // scroll_x = scroll_target->x - (SCREENWIDTH >> 1);
	// scroll_y = scroll_target->y - scroll_bottom_movement_limit; //Move the camera to its bottom limit

    //PUSH_BANK(scroll_bank);
    y = scroll_y >> 3;
    for(i = 0u; i != (SCREEN_TILE_REFRES_H) && y != scroll_h; ++i, y ++) {
        ScrollUpdateRow((scroll_x >> 3) - SCREEN_PAD_LEFT,  y - SCREEN_PAD_TOP);
    }
    //POP_BANK;
}*/