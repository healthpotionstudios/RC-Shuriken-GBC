#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "rand.h"
#include "Sound.h"

#include "Palette.h"
#include "../res/src/frog.h"


const UINT8 anim_frogFace[] = {1,0};
const UINT8 anim_frogBody[] = {1,1};
const UINT8 anim_frogBodyJump[] = {1,2};
const UINT8 anim_frogLeg[] = {1,3};

const UINT8 anim_frogFaceRight[] = {1,4};
const UINT8 anim_frogBodyRight[] = {1,5};
const UINT8 anim_frogBodyJumpRight[] = {1,6};
const UINT8 anim_frogLegRight[] = {1,7};

struct Sprite* frogSprFaceRight;
struct Sprite* frogSprBodyLeft;
struct Sprite* frogSprBodyRight;
struct Sprite* frogSprLegsLeft;
struct Sprite* frogSprLegsRight;

struct Sprite* tongueSprites[5];

extern struct Sprite* flyPlayerSpr;

const UINT16 frogBossBasePalette[] = {PALETTE_INDEX(frog,2)};
UINT16 frogBossHurtPalette[] = {PALETTE_INDEX(frog,2)};
UINT8 frogBossHealth;
const UINT8 frogBossHealthMax = 8;
extern UINT8 boss2Beat;
extern UINT8 level;

extern UINT8 spikeRestrictMovementTimer;
extern UINT8 spikeSlowdown;

//attack details
enum frogStates{Idle,
          HopInit, HopJumping, HopEnd, //1
          TongueInit, TongueLick, TongueEnd} frogAttackState; //2
UINT8 frogDirection; // 0down 1right 2up 3left
INT8 tonguePosX;
INT8 tonguePosY;
UINT16 tongueTargetPosX;
UINT16 tongueTargetPosY;
UINT8 frogHurtPaletteTimer = 0;

//attack timers
UINT16 frogTimerGeneric;
UINT8 frogDelayTongueTimer;


void FrogDelayBetweenAttacks();
UINT16 Lerp(INT16 a, INT16 b, UINT8 segments, UINT8 which);
void FrogHopInit();
void FrogHopJumping();
void FrogHopEnd();
void FrogTongueInit();
void FrogTongueLick();
void FrogTongueEnd();
UINT8 NewDirection();
void FrogTakeDamage();

extern void SetHealth(UINT8 curr, UINT8 max);
extern BYTE triggerWinJingle;

void Start_SpriteBossFrog() {
    THIS->coll_x = 0;
    THIS->coll_y = -16;
    THIS->coll_w = 32;
    THIS->coll_h = 32;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;

    frogSprFaceRight = SpriteManagerAdd(SpriteBossFrogBody, THIS->x+16, THIS->y);
    frogSprBodyLeft = SpriteManagerAdd(SpriteBossFrogBody, THIS->x, THIS->y-16);
    frogSprBodyRight = SpriteManagerAdd(SpriteBossFrogBody, THIS->x+16, THIS->y-16);
    

    SetSpriteAnim(THIS,anim_frogFace, 15);
    SetSpriteAnim(frogSprFaceRight, anim_frogFace, 15);
    SetSpriteAnim(frogSprBodyLeft, anim_frogBody, 15);
    SetSpriteAnim(frogSprBodyRight, anim_frogBody, 15);
    
    frogSprFaceRight->first_tile = THIS->first_tile;
    frogSprBodyLeft->first_tile = THIS->first_tile;
    frogSprBodyRight->first_tile = THIS->first_tile;

    SPRITE_SET_VMIRROR(frogSprFaceRight);
    SPRITE_SET_VMIRROR(frogSprBodyRight);

    frogBossHurtPalette[1] = RGB2(30,18,18);
    frogBossHurtPalette[2] = RGB2(28,4,4);
    
    frogAttackState = Idle;
    frogTimerGeneric = 60;
    frogDirection = 0;
    frogDelayTongueTimer = 0;

    frogBossHealth = frogBossHealthMax;
    SetHealth(frogBossHealth, frogBossHealthMax); 
}

void Update_SpriteBossFrog() {
        
    if (frogTimerGeneric != 0)
    {
        frogTimerGeneric--;
    }

    if (frogDelayTongueTimer != 0)
    {
        frogDelayTongueTimer--;
        if (frogDelayTongueTimer == 0)
        {
            for (UINT8 i = 1; i < 5; i++)
            {
                tongueSprites[i] = SpriteManagerAdd(SpriteBossFrogTongue,THIS->x + tonguePosX, THIS->y + tonguePosY);
            }
        }
    }

    if (frogTimerGeneric == 0 && frogAttackState == Idle) //pick a new attack
    {
        UINT8 randAttack = randw() % 4; //0,1,2,3
        if (randAttack == 3)
        {
            FrogTongueInit();
        }
        else
        {
            FrogHopInit();
        }
    }
    else if (frogTimerGeneric == 0 && (frogAttackState == TongueInit || frogAttackState == HopInit))
    {
        if (frogAttackState == TongueInit)
        {
            FrogTongueLick();
        }
        else if (frogAttackState == HopInit)
        {
            FrogHopJumping();
        }
    }
    else if (frogTimerGeneric == 0 && (frogAttackState == TongueLick || frogAttackState == HopJumping))
    {
        if (frogAttackState == TongueLick)
        {
            FrogTongueEnd();
        }
        else if (frogAttackState == HopJumping)
        {
            FrogHopEnd();
        }
    }

    if (frogAttackState == TongueLick && frogDelayTongueTimer == 0)
    {
        if (frogTimerGeneric < 60)
        {
            tongueSprites[4]->x = Lerp(THIS->x + tonguePosX, tongueTargetPosX, 7, frogTimerGeneric >> 3);
            tongueSprites[4]->y = Lerp(THIS->y + tonguePosY, tongueTargetPosY, 7, frogTimerGeneric >> 3);
        }
        else
        {
            tongueSprites[4]->x = Lerp(THIS->x + tonguePosX, tongueTargetPosX, 7, 7-((frogTimerGeneric - 60) >> 3));
            tongueSprites[4]->y = Lerp(THIS->y + tonguePosY, tongueTargetPosY, 7, 7-((frogTimerGeneric - 60) >> 3));
        }

        tongueSprites[1]->x = Lerp(THIS->x + tonguePosX, tongueSprites[4]->x, 4, 1); //25%
        tongueSprites[1]->y = Lerp(THIS->y + tonguePosY, tongueSprites[4]->y, 4, 1); //25%

        tongueSprites[2]->x = Lerp(THIS->x + tonguePosX, tongueSprites[4]->x, 4, 2); //50%
        tongueSprites[2]->y = Lerp(THIS->y + tonguePosY, tongueSprites[4]->y, 4, 2); //50%

        tongueSprites[3]->x = Lerp(THIS->x + tonguePosX, tongueSprites[4]->x, 4, 3); //75%
        tongueSprites[3]->y = Lerp(THIS->y + tonguePosY, tongueSprites[4]->y, 4, 3); //75%
    }
    else if (frogAttackState == HopJumping)
    {
        if (frogDirection == 0) //d
        {
            if (TranslateSprite(THIS,0,2))
                frogTimerGeneric = 0;
            
            frogSprLegsLeft->x = THIS->x;
            frogSprLegsLeft->y = THIS->y-32;
            frogSprLegsRight->x = THIS->x+16;
            frogSprLegsRight->y = THIS->y-32;
        }
        else if (frogDirection == 1) //r
        {
            if (TranslateSprite(THIS,2,0))
                frogTimerGeneric = 0;

            frogSprLegsLeft->x = THIS->x-16;
            frogSprLegsLeft->y = THIS->y-16;
            frogSprLegsRight->x = THIS->x-16;
            frogSprLegsRight->y = THIS->y;
        }
        else if (frogDirection == 2) //u
        {
            if (TranslateSprite(THIS,0,-2))
                frogTimerGeneric = 0;

            frogSprLegsLeft->x = THIS->x;
            frogSprLegsLeft->y = THIS->y+16;
            frogSprLegsRight->x = THIS->x+16;
            frogSprLegsRight->y = THIS->y+16;
        }
        else if (frogDirection == 3) //l
        {
            if (TranslateSprite(THIS,-2,0))
                frogTimerGeneric = 0;

            frogSprLegsLeft->x = THIS->x+32;
            frogSprLegsLeft->y = THIS->y-16;
            frogSprLegsRight->x = THIS->x+32;
            frogSprLegsRight->y = THIS->y;
        }
        frogSprFaceRight->x = THIS->x+16;
        frogSprFaceRight->y = THIS->y;
        frogSprBodyLeft->x = THIS->x;
        frogSprBodyLeft->y = THIS->y-16;
        frogSprBodyRight->x = THIS->x+16;
        frogSprBodyRight->y = THIS->y-16;

    }


    if (frogHurtPaletteTimer != 0)
    {
        frogHurtPaletteTimer--;
        if (frogHurtPaletteTimer == 0)
        {
            set_sprite_palette(2,1,frogBossBasePalette); 
        }
    }

    UINT8 i;
    struct Sprite* spr;
    SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
    {
        if (spr->type == SpriteSpike)
        {
            if (CheckCollision(THIS,spr))
            {
                if (spikeRestrictMovementTimer == 0 && spikeSlowdown == 1) //spike not in hitstun and is moving at max speed
                {
                    FrogTakeDamage();
                    PlayFx(CHANNEL_1, 10, 0x8f, 0xc7, 0xf3, 0x73, 0xdd);
                }
            }
        }
    }

} 

void Destroy_SpriteBossFrog() {
}

UINT16 Lerp(INT16 a, INT16 b, UINT8 segments, UINT8 which)
{
    return (UINT16)(a + (((b - a) / segments)) * which);
}

void FrogDelayBetweenAttacks()
{
    frogTimerGeneric = (rand() & 0b11111111) + 100; //how long to wait before next attack
}

void FrogHopInit()
{
    PlayFx(CHANNEL_1, 5, 0x22, 0x89, 0xf2, 0x1e, 0x85);
    SPRITE_UNSET_VMIRROR(THIS);
    SPRITE_UNSET_VMIRROR(frogSprFaceRight);
    SPRITE_UNSET_VMIRROR(frogSprBodyLeft);
    SPRITE_UNSET_VMIRROR(frogSprBodyRight);
    SPRITE_UNSET_HMIRROR(THIS);
    SPRITE_UNSET_HMIRROR(frogSprFaceRight);
    SPRITE_UNSET_HMIRROR(frogSprBodyLeft);
    SPRITE_UNSET_HMIRROR(frogSprBodyRight);

    frogDirection = NewDirection();
    if (frogDirection == 0) //facing down
    {
        frogSprLegsLeft = SpriteManagerAdd(SpriteBossFrogBody,THIS->x,THIS->y-32); //above frog
        frogSprLegsRight = SpriteManagerAdd(SpriteBossFrogBody,THIS->x+16,THIS->y-32);

        frogSprLegsLeft->first_tile = THIS->first_tile;
        frogSprLegsRight->first_tile = THIS->first_tile;

        SetSpriteAnim(THIS,anim_frogFace,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogFace,15);
        SetSpriteAnim(frogSprBodyLeft,anim_frogBodyJump,15);
        SetSpriteAnim(frogSprBodyRight,anim_frogBodyJump,15);
        SetSpriteAnim(frogSprLegsLeft,anim_frogLeg,15);
        SetSpriteAnim(frogSprLegsRight,anim_frogLeg,15);
        

        SPRITE_SET_VMIRROR(frogSprLegsRight);
        SPRITE_SET_VMIRROR(frogSprBodyRight);
        SPRITE_SET_VMIRROR(frogSprFaceRight);
    }
    else if (frogDirection == 2) //facing up
    {
        frogSprLegsLeft = SpriteManagerAdd(SpriteBossFrogBody,THIS->x,THIS->y+16); //above frog
        frogSprLegsRight = SpriteManagerAdd(SpriteBossFrogBody,THIS->x+16,THIS->y+16);

        frogSprLegsLeft->first_tile = THIS->first_tile;
        frogSprLegsRight->first_tile = THIS->first_tile;

        SetSpriteAnim(THIS,anim_frogBodyJump,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogBodyJump,15);
        SetSpriteAnim(frogSprBodyLeft,anim_frogFace,15);
        SetSpriteAnim(frogSprBodyRight,anim_frogFace,15);
        SetSpriteAnim(frogSprLegsLeft,anim_frogLeg,15);
        SetSpriteAnim(frogSprLegsRight,anim_frogLeg,15);
        

        SPRITE_SET_VMIRROR(frogSprLegsRight);
        SPRITE_SET_VMIRROR(frogSprBodyRight);
        SPRITE_SET_VMIRROR(frogSprFaceRight);
        SPRITE_SET_HMIRROR(THIS);
        SPRITE_SET_HMIRROR(frogSprFaceRight);
        SPRITE_SET_HMIRROR(frogSprBodyLeft);
        SPRITE_SET_HMIRROR(frogSprBodyRight);
        SPRITE_SET_HMIRROR(frogSprLegsLeft);
        SPRITE_SET_HMIRROR(frogSprLegsRight);
    }
    else if (frogDirection == 1) //facing right
    {
        frogSprLegsLeft = SpriteManagerAdd(SpriteBossFrogBody,THIS->x-16,THIS->y-16); //above frog
        frogSprLegsRight = SpriteManagerAdd(SpriteBossFrogBody,THIS->x-16,THIS->y);

        frogSprLegsLeft->first_tile = THIS->first_tile;
        frogSprLegsRight->first_tile = THIS->first_tile;

        SetSpriteAnim(THIS,anim_frogBodyJumpRight,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogFaceRight,15);
        SetSpriteAnim(frogSprBodyLeft,anim_frogBodyJumpRight,15);
        SetSpriteAnim(frogSprBodyRight,anim_frogFaceRight,15);
        SetSpriteAnim(frogSprLegsLeft,anim_frogLegRight,15);
        SetSpriteAnim(frogSprLegsRight,anim_frogLegRight,15);
        

        SPRITE_SET_HMIRROR(frogSprLegsLeft);
        SPRITE_SET_HMIRROR(frogSprBodyLeft);
        SPRITE_SET_HMIRROR(frogSprBodyRight);
    }
    else if (frogDirection == 3) //facing left
    {
        frogSprLegsLeft = SpriteManagerAdd(SpriteBossFrogBody,THIS->x+32,THIS->y-16); //above frog
        frogSprLegsRight = SpriteManagerAdd(SpriteBossFrogBody,THIS->x+32,THIS->y);

        frogSprLegsLeft->first_tile = THIS->first_tile;
        frogSprLegsRight->first_tile = THIS->first_tile;

        SetSpriteAnim(THIS,anim_frogFaceRight,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogBodyJumpRight,15);
        SetSpriteAnim(frogSprBodyLeft,anim_frogFaceRight,15);
        SetSpriteAnim(frogSprBodyRight,anim_frogBodyJumpRight,15);
        SetSpriteAnim(frogSprLegsLeft,anim_frogLegRight,15);
        SetSpriteAnim(frogSprLegsRight,anim_frogLegRight,15);
        

        SPRITE_SET_HMIRROR(frogSprLegsLeft);
        SPRITE_SET_HMIRROR(frogSprBodyLeft);
        SPRITE_SET_HMIRROR(frogSprBodyRight);
        SPRITE_SET_VMIRROR(THIS);
        SPRITE_SET_VMIRROR(frogSprFaceRight);
        SPRITE_SET_VMIRROR(frogSprBodyLeft);
        SPRITE_SET_VMIRROR(frogSprBodyRight);
        SPRITE_SET_VMIRROR(frogSprLegsLeft);
        SPRITE_SET_VMIRROR(frogSprLegsRight);
    }


    frogTimerGeneric = 15;
    frogAttackState = HopJumping;
}
void FrogHopJumping()
{
    frogTimerGeneric = 1;
    frogAttackState = HopEnd;
}
void FrogHopEnd()
{
    SpriteManagerRemoveSprite(frogSprLegsLeft);
    SpriteManagerRemoveSprite(frogSprLegsRight);
    if (frogDirection == 0) //down
    {
        SetSpriteAnim(frogSprBodyLeft,anim_frogBody,15);
        SetSpriteAnim(frogSprBodyRight,anim_frogBody,15);
    }
    else if (frogDirection == 2) //up
    {
        SetSpriteAnim(THIS,anim_frogBody,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogBody,15);
    }
    else if (frogDirection == 1) //right
    {
        SetSpriteAnim(frogSprBodyLeft,anim_frogBodyRight,15);
        SetSpriteAnim(THIS,anim_frogBodyRight,15);
    }
    else if (frogDirection == 3) //left
    {
        SetSpriteAnim(frogSprBodyRight,anim_frogBodyRight,15);
        SetSpriteAnim(frogSprFaceRight,anim_frogBodyRight,15);
    }
    frogTimerGeneric = 90;
    frogAttackState = Idle;
}

void FrogTongueInit()
{
    PlayFx(CHANNEL_1, 5, 0x72, 0x89, 0xf2, 0x1e, 0x85);
    if (frogDirection == 0)
    {
        tonguePosX = 8;
        tonguePosY = 14;
    }
    else if (frogDirection == 1)
    {
        tonguePosX = 32;
        tonguePosY = -8;
    }
    else if (frogDirection == 2)
    {
        tonguePosX = 8;
        tonguePosY = -30;
    }
    else if (frogDirection == 3)
    {
        tonguePosX = -14;
        tonguePosY = -8;
    }

        tongueSprites[0] = SpriteManagerAdd(SpriteBossFrogTongue,THIS->x + tonguePosX, THIS->y + tonguePosY);
    tongueTargetPosX = flyPlayerSpr->x;
    tongueTargetPosY = flyPlayerSpr->y;
    frogTimerGeneric = 120;
    frogDelayTongueTimer = 60;
    frogAttackState = TongueLick;
}
void FrogTongueLick()
{
    frogAttackState = TongueEnd;
    frogTimerGeneric = 1;
}
void FrogTongueEnd()
{
    for (UINT8 i = 0; i < 5; i++)
    {
        SpriteManagerRemoveSprite(tongueSprites[i]);
    }
    
    frogTimerGeneric = 90;
    frogAttackState = Idle;
}

UINT8 NewDirection()
{
    UINT8 r = randw() % 4; //change later to target player
    if (r == 0 && THIS->y > 24*8-4)
    {
        r = NewDirection();
    }
    else if (r == 1 && THIS->x > 23*8-4)
    {
        r = NewDirection();
    }
    else if (r == 2 && THIS->y < 5*8+4)
    {
        r = NewDirection();
    }
    else if (r == 3 && THIS->x < 3*8+4)
    {
        r = NewDirection();
    }


    return r;
}



void FrogTakeDamage()
{
    set_sprite_palette(2,1,frogBossHurtPalette); 
    frogBossHealth--;
    frogHurtPaletteTimer = 20;
    // if (frogBossHealth == 2)
    // {
    //     //TODO play sound

    // }
    SetHealth(frogBossHealth, frogBossHealthMax); 

    if (frogBossHealth == 0)
    {
        UINT8 i;
        struct Sprite* spr;
        SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
        {
            if (spr->type == SpriteBossFrogBody || spr->type == SpriteBossFly || spr->type == SpriteBossFrogTongue)
            {
                SpriteManagerRemove(i);
            }
        }
        SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
        {
            if (spr->type == SpriteBossFrog)
            {
                triggerWinJingle = 1;
                SpriteManagerRemove(i);
                //TODO trigger win
                
            }
        }
    }
}
