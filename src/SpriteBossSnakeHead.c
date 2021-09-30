#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"
#include "Trig.h"
#include "Rand.h"
#include "Palette.h"

#include "../res/src/snake.h"

const UINT8 anim_bossSnakeLeft[] = {1, 0};
const UINT8 anim_bossSnakeUp[]   = {1, 1};

const UINT8 anim_bossSnakeBodyLeft[] = {1, 2};
const UINT8 anim_bossSnakeBodyUp[] = {1, 3};
const UINT8 anim_bossSnakeBodyX[]   = {1, 4};

fixed snakeAccum_x;
fixed snakeAccum_y;
INT16 snakeSpeed_x;
INT16 snakeSpeed_y;
UINT8 snakeAng;
UINT8 snakeMoveDelay;
UINT8 snakeMoveDelayMax = 2;
UINT8 snakeTurnDelay;
UINT8 snakeTurnTimer;
UINT8 snakeBounceDelay;

UINT8 snakeOldPosIndex;
#define OLDLIMIT 90
#define OFFSETINCREMENT 14
#define OFFSETINCREMENTFASTER 7 
UINT8 offsetIncrementDivisor;
UINT16 snakeXOldPos[OLDLIMIT];
UINT16 snakeYOldPos[OLDLIMIT];
UINT8  snakeOldAng[OLDLIMIT];

UINT8 snakeBossHealth;
const UINT8 snakeBossHealthMax = 6;
const UINT16 snakeBossBasePalette[] = {PALETTE_INDEX(snake,2)};
UINT16 snakeBossHurtPalette[] = {PALETTE_INDEX(snake,2)};
UINT8 snakeHurtPaletteTimer = 0;
extern const UINT8 EMPTY_HEART_TILE;
extern const UINT8 HEART_TILE_HALF;
extern const UINT8 HEART_TILE_FULL;
extern UINT8 boss1Beat;
extern UINT8 level;

extern BYTE triggerWinJingle;

struct BossSnakeBody
{
     BYTE isHurtbox;
};
struct Sprite* snakeArrayOfSegments[5];

void SnakeBounce();
void SnakeTakeDamage();
extern void SetHealth(UINT8 curr, UINT8 max);

void Start_SpriteBossSnakeHead() {
    //TODO don't spawn two snakes after the first dies
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;
    snakeAng = 0;

    snakeBossHealth = snakeBossHealthMax;
    snakeMoveDelayMax = 2;
    snakeBossHurtPalette[1] = RGB2(30,18,18);
    snakeBossHurtPalette[2] = RGB2(28,4,4);
    offsetIncrementDivisor = 1;

    snakeOldPosIndex = 0;
    UINT8 i;
    for (i = 0; i != OLDLIMIT; i++)
    {
        snakeXOldPos[i] = THIS->x;
        snakeYOldPos[i] = THIS->y;
        snakeOldAng[i] = 0;
    }
    for (i = 0; i != 5; i++)
    {
        snakeArrayOfSegments[i] = SpriteManagerAdd(SpriteBossSnakeBody,THIS->x, THIS->y);
    }
    snakeArrayOfSegments[4]->anim_data = anim_bossSnakeBodyX;
    ((struct BossSnakeBody*)snakeArrayOfSegments[4]->custom_data)->isHurtbox = 1; //set tail to be the hurtbox

    snakeTurnDelay = rand() & 0b1111111;
    snakeTurnTimer = rand() & 0b1111111;
    snakeMoveDelay = snakeMoveDelayMax;
    snakeBounceDelay = 0;

    SetSpriteAnim(THIS,anim_bossSnakeLeft, 15);
    //scroll_target = THIS;

    SetHealth(snakeBossHealth, snakeBossHealthMax); 
	
}

void Update_SpriteBossSnakeHead() {
    // UINT8 i;
    // struct Sprite* spr;

        if (snakeTurnDelay != 0)
        {
            snakeTurnDelay --;
        }
        if (snakeTurnDelay == 0)
        {
            snakeAng += 2;
            snakeTurnTimer--;
            if (snakeTurnTimer == 0)
            {
                snakeTurnDelay = rand() & 0b1111111;
                snakeTurnTimer = rand() & 0b111111;
            }
        }


        if (snakeAng > 32 && snakeAng <= 96)
        {
            SetSpriteAnim(THIS,anim_bossSnakeLeft, 15);
            SPRITE_UNSET_VMIRROR(THIS);
            SPRITE_UNSET_HMIRROR(THIS);
        }
        else if (snakeAng > 159 && snakeAng <= 223)
        {
            SetSpriteAnim(THIS,anim_bossSnakeLeft, 15);
            SPRITE_SET_VMIRROR(THIS);
            SPRITE_UNSET_HMIRROR(THIS);
        }
        else if (snakeAng > 96 && snakeAng <= 159)
        {
            SetSpriteAnim(THIS,anim_bossSnakeUp, 15);
            SPRITE_SET_HMIRROR(THIS);
            SPRITE_UNSET_VMIRROR(THIS);
        }
        else if (snakeAng > 223 || snakeAng <= 32)
        {
            SetSpriteAnim(THIS,anim_bossSnakeUp, 15);
            SPRITE_UNSET_HMIRROR(THIS);
            SPRITE_UNSET_VMIRROR(THIS);
        }

        if (snakeHurtPaletteTimer != 0)
        {
            snakeHurtPaletteTimer--;
            if (snakeHurtPaletteTimer == 0)
            {
                set_sprite_palette(2,1,snakeBossBasePalette); 
            }
        }

        snakeSpeed_x = (SIN(snakeAng)) << 2;
        snakeSpeed_y = (COS(snakeAng)) << 2;



        snakeAccum_x.w += snakeSpeed_x << delta_time;
        snakeAccum_y.w += snakeSpeed_y << delta_time;

        snakeMoveDelay--;
        if (snakeMoveDelay == 0)
        {
            snakeMoveDelay = snakeMoveDelayMax;
            
            if (snakeBounceDelay != 0)
            {
                snakeBounceDelay--;
            }
            if(TranslateSprite(THIS, snakeAccum_x.b.h, snakeAccum_y.b.h))
            {
                SnakeBounce();
            }
        }

        //updated the old positions array
        for (UINT8 i = 0; i != 5; i++)
        {
            UINT8 offset = 0;
            if (snakeMoveDelayMax == 2)
            {
                offset = snakeOldPosIndex - ((i+1) * OFFSETINCREMENT);
            }
            else
            {
                offset = snakeOldPosIndex - ((i+1) * OFFSETINCREMENTFASTER);
            }

            if (offset > OLDLIMIT)
            {
                offset = offset + OLDLIMIT;
            }
            snakeArrayOfSegments[i]->x = snakeXOldPos[offset];
            snakeArrayOfSegments[i]->y = snakeYOldPos[offset];
            if (i != 4)
            {
                if (snakeOldAng[offset] > 32 && snakeOldAng[offset] <= 96)
                {
                    SetSpriteAnim(snakeArrayOfSegments[i],anim_bossSnakeBodyLeft, 15);
                    SPRITE_UNSET_VMIRROR(snakeArrayOfSegments[i]);
                    SPRITE_UNSET_HMIRROR(snakeArrayOfSegments[i]);
                }
                else if (snakeOldAng[offset] > 159 && snakeOldAng[offset] <= 223)
                {
                    SetSpriteAnim(snakeArrayOfSegments[i],anim_bossSnakeBodyLeft, 15);
                    SPRITE_SET_VMIRROR(snakeArrayOfSegments[i]);
                    SPRITE_UNSET_HMIRROR(snakeArrayOfSegments[i]);
                }
                else if (snakeOldAng[offset] > 96 && snakeOldAng[offset] <= 159)
                {
                    SetSpriteAnim(snakeArrayOfSegments[i],anim_bossSnakeBodyUp, 15);
                    SPRITE_SET_HMIRROR(snakeArrayOfSegments[i]);
                    SPRITE_UNSET_VMIRROR(snakeArrayOfSegments[i]);
                }
                else if (snakeOldAng[offset] > 223 || snakeOldAng[offset] <= 32)
                {
                    SetSpriteAnim(snakeArrayOfSegments[i],anim_bossSnakeBodyUp, 15);
                    SPRITE_UNSET_HMIRROR(snakeArrayOfSegments[i]);
                    SPRITE_UNSET_VMIRROR(snakeArrayOfSegments[i]);
                }
            }
        }
        snakeXOldPos[snakeOldPosIndex] = THIS->x;
        snakeYOldPos[snakeOldPosIndex] = THIS->y;
        snakeOldAng[snakeOldPosIndex] = snakeAng;
        snakeOldPosIndex++;
        if (snakeOldPosIndex == OLDLIMIT)
        {
            snakeOldPosIndex = 0;
        }

    snakeAccum_x.b.h = 0;
	snakeAccum_y.b.h = 0;
    

    /*SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
    {
        if (spr->type == SpriteEnemy)
        {
            if (CheckCollision(THIS,spr))
            {
                PlayFx(CHANNEL_1, 10, 0x4f, 0xc7, 0xf3, 0x73, 0x86);
                SetState(StateGame);
            }
        }
    }*/

} 

void Destroy_SpriteBossSnakeHead() {
}

void SnakeBounce()
{
    if (snakeBounceDelay == 0)
    {
        if (snakeMoveDelayMax == 2)
        {
            snakeBounceDelay = 10;
        }
        else
        {
            snakeBounceDelay = 20;
        }
        //snakeTurnDelay = 120;
        //snakeTurnTimer = 0;
        PlayFx(CHANNEL_4, 4, 0x0c, 0x41, 0x30, 0xc0);
        if (snakeAng > 159 && snakeAng <= 223 || snakeAng < 96 && snakeAng >= 32)
        {
            snakeAng = 0 - snakeAng;
        }
        if (snakeAng > 96 && snakeAng <= 159 || snakeAng > 233 | snakeAng < 32)
        {
            snakeAng = 128 - snakeAng;
        }
    }
}

void SnakeTakeDamage()
{
    set_sprite_palette(2,1,snakeBossHurtPalette); 
    snakeBossHealth--;
    snakeHurtPaletteTimer = 20;
    if (snakeBossHealth == 2)
    {
        PlayFx(CHANNEL_1,10,0x2c,0x89,0xf2,0x3f,0x85);
        snakeMoveDelayMax = 1;
    }
    SetHealth(snakeBossHealth, snakeBossHealthMax); 

    if (snakeBossHealth == 0)
    {
        UINT8 i;
        struct Sprite* spr;
        for (i = 0; i < 5; i++)
        {
            SpriteManagerRemoveSprite(snakeArrayOfSegments[i]);
        }

        SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
        {
            if (spr->type == SpriteBossSnakeHead)
            {
                triggerWinJingle = 1;
                SpriteManagerRemove(i); //remove head
                
            }
        }
    }
}
