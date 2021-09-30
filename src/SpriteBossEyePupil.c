#include "Banks/SetBank2.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"
#include "Palette.h"
#include "Trig.h"
#include "Rand.h"

#include "../res/src/eyePupil.h"



const UINT8 anim_bossEyeIdle[] = {1, 0};
const UINT8 anim_bossEyeLeft[] = {1, 1};
const UINT8 anim_bossEyeUp[] = {1, 2};


UINT8 eyeBallOriginX;
UINT8 eyeBallOriginY;
UINT8 eyePupilAng;
INT8 eyePupilRotationDirection;

UINT8 eyeBossHealth;
const UINT8 eyeBossHealthMax = 12;
const UINT16 eyeBossBasePalette[] = {PALETTE_INDEX(eyePupil,2)};
UINT16 eyeBossHurtPalette[] = {PALETTE_INDEX(eyePupil,2)};
UINT8 eyeHurtPaletteTimer = 0;
extern const UINT8 EMPTY_HEART_TILE;
extern const UINT8 HEART_TILE_HALF;
extern const UINT8 HEART_TILE_FULL;

extern UINT8 spikeRestrictMovementTimer;
extern UINT8 spikeSlowdown;

struct Sprite* eyeHHurtBox;
UINT8 eyeHitboxIndex;
const UINT8 eyeHitboxX[] = {8,16,8,0};
const UINT8 eyeHitboxY[] = {0,8,16,8};
//struct Sprite* eyeVHurtBox;
struct Sprite* eyePlayerSpr;


const unsigned char eyeTiles[] =       {15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
const unsigned char eyeTiles2[] =      {31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
const unsigned char eyeTargetTiles[] = {47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62};
unsigned char originalTilesBeforeEyeTiles[16];
unsigned char originalTilesBeforeTargetTiles[16];
UINT8 eyeTilesAnimTimer;

//attack details
UINT8 eyeTargetX, eyeTargetY;
enum eyeStates{Idle,
          ShotgunInit, ShotgunTeleport, ShotgunFire, //1
          BlastInit, BlastTeleport, BlastFire, //2
          TelePlayerInit, TelePlayerTeleport, TelePlayerFire, //3
          RotatingEye} eyeAttackState; //4

UINT8 eyeLastAttack;
UINT8 eyeLastShotgunPosition;
BYTE eyePupilIsCentered;

//attack timers
UINT16 eyeTimerGeneric;

struct LaserInfo
{
    INT8 angle;
};

extern UINT8 boss3Beat;
extern UINT8 level;
    
void SetEyeSprite();
void EyeTakeDamage();
extern void SetHealth(UINT8 curr, UINT8 max);
void EyeShotgunInit();
void EyeShotgunTeleport();
void EyeShotgunFire();
void EyeBlastInit();
void EyeBlastTeleport();
void EyeBlastFire();
void EyeTelePlayerInit();
void EyeTelePlayerTeleport();
void EyeTelePlayerFire();
void EyeTeleportCode();
void SpawnLaser();
void DelayBetweenAttacks();
void EyeRotatePupil();

extern BYTE triggerWinJingle;

void Start_SpriteBossEyePupil() {
    THIS->coll_x = 2;
    THIS->coll_y = 2;
    THIS->coll_w = 12;
    THIS->coll_h = 12;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;

    eyeBallOriginX = (THIS->x >> 3) - 1;
    eyeBallOriginY = (THIS->y >> 3) - 1;
    eyePupilAng = rand();

    eyeHHurtBox = SpriteManagerAdd(SpriteBossEyeHHurtbox, 8*eyeBallOriginX + 8,8*eyeBallOriginY);
    //eyeVHurtBox = SpriteManagerAdd(SpriteBossEyeVHurtbox, 8*eyeBallOriginX, 8*eyeBallOriginY + 8 - 16);
    UINT8 i;
    struct Sprite* spr;
    SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
    {
        if (spr->type == SpritePlayer)
        {
            eyePlayerSpr = spr;
        }
    }

    eyeBossHealth = eyeBossHealthMax;
    eyeBossHurtPalette[1] = RGB2(30,18,18);
    eyeBossHurtPalette[2] = RGB2(28,4,4);

    SetSpriteAnim(THIS,anim_bossEyeIdle, 15);

    eyeTilesAnimTimer = 0;
    get_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeEyeTiles);
    get_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeTargetTiles);
    set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,eyeTiles);

    SetHealth(eyeBossHealth,eyeBossHealthMax);

    DelayBetweenAttacks(); //how long to wait before next attack
    eyeAttackState = Idle;
    eyeLastAttack = 0;
    eyePupilIsCentered = 0;
}

void Update_SpriteBossEyePupil()
{
    // you must reset the map before moving the eye otherwise it'll copy its own tiles
    // set_bkg_tiles((THIS->x >> 3) - 1,(THIS->y >> 3) - 1,4,4,originalTilesBeforeEyeTiles);

    // THIS->x++;
    // get_bkg_tiles((THIS->x >> 3) - 1,(THIS->y >> 3) - 1,4,4,originalTilesBeforeEyeTiles);
    

    //controls hitbox location
    eyeHitboxIndex++;
    eyeHHurtBox->x = eyeHitboxX[eyeHitboxIndex % 4] + 8*eyeBallOriginX;
    eyeHHurtBox->y = eyeHitboxY[eyeHitboxIndex % 4] + 8*eyeBallOriginY;



    if (eyeTimerGeneric != 0)
    {
        eyeTimerGeneric--;
    }

    if (eyeTimerGeneric == 0 && eyeAttackState == Idle) //pick a new attack
    {
        UINT8 randAttack = rand() & 0b111;
        while (randAttack == eyeLastAttack)
        {
            randAttack = rand() & 0b1111111;
        }
        eyeLastAttack = randAttack;
        if (randAttack == 1 || randAttack == 2)
        {
            EyeShotgunInit();
        }
        else if (randAttack == 3 || randAttack == 4)
        {
            EyeBlastInit();
        }
        else if (randAttack == 5 || randAttack == 6)
        {
            EyeTelePlayerInit();
        }
        else //only 7 I hope
        {
            EyeRotatePupil();
        }
    }
    else if (eyeTimerGeneric == 0 && (eyeAttackState == ShotgunInit || eyeAttackState == BlastInit || eyeAttackState == TelePlayerInit))
    {
        if (eyeAttackState == ShotgunInit)
        {
            EyeShotgunTeleport();
        }
        else if (eyeAttackState == BlastInit)
        {
            EyeBlastTeleport();
        }
        else if (eyeAttackState == TelePlayerInit)
        {
            EyeTelePlayerTeleport();
        }
    }
    else if (eyeTimerGeneric == 0 && (eyeAttackState == ShotgunTeleport || eyeAttackState == BlastTeleport || eyeAttackState == TelePlayerTeleport))
    {
        if (eyeAttackState == ShotgunTeleport)
        {
            EyeShotgunFire();
        }
        else if (eyeAttackState == BlastTeleport)
        {
            EyeBlastFire();
        }
        else if (eyeAttackState == TelePlayerTeleport)
        {
            EyeTelePlayerFire();
        }
    }
    else if (eyeTimerGeneric == 0 && eyeAttackState == RotatingEye)
    {
        eyeAttackState = Idle;
        eyePupilIsCentered = 1;
        eyeTimerGeneric = (rand() & 0b111111);
    }

    if (eyeAttackState == RotatingEye)
    {
        if (eyePupilRotationDirection == 1)
        {
            eyePupilAng++;
        }
        else
        {
            eyePupilAng--;
        }
    }


    if(eyeAttackState == ShotgunInit || eyeAttackState == BlastInit || eyeAttackState == TelePlayerInit)
    {
        set_bkg_tiles(eyeTargetX,eyeTargetY,4,4,eyeTargetTiles);
    }



    eyeTilesAnimTimer++;
    if (eyeTilesAnimTimer < 20)
    {
        set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,eyeTiles);
    }
    else
    {
        set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,eyeTiles2);
        if (eyeTilesAnimTimer == 40)
        {
            eyeTilesAnimTimer = 0;
        }
    }

    //eyePupilAng++;

    if (eyePupilIsCentered == 0)
    {
        THIS->x = 8*eyeBallOriginX + 16 - 8 + (SIN(eyePupilAng) / 9); //-4 to center the cross, +8 to center in the sprite
        THIS->y = 8*eyeBallOriginY + 16 - 8 + (COS(eyePupilAng) / 9); // >>3 controls the distance
        SetEyeSprite();
    }
    else
    {
        THIS->x = 8*eyeBallOriginX + 8;
        THIS->y = 8*eyeBallOriginY + 8;
        SetSpriteAnim(THIS,anim_bossEyeIdle, 15);
    }


    


    if (eyeHurtPaletteTimer != 0)
        {
            eyeHurtPaletteTimer--;
            if (eyeHurtPaletteTimer == 0)
            {
                set_sprite_palette(2,1,eyeBossBasePalette); 
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
                    EyeTakeDamage();
                    PlayFx(CHANNEL_1, 10, 0x8f, 0xc7, 0xf3, 0x73, 0xdd);
                }
            }
        }
    }
    
} 

void Destroy_SpriteBossEyePupil() {
}


void SetEyeSprite()
{
    if (eyePupilAng > 32 && eyePupilAng <= 96)
    {
        SetSpriteAnim(THIS,anim_bossEyeLeft, 15);
        SPRITE_UNSET_VMIRROR(THIS);
        SPRITE_UNSET_HMIRROR(THIS);
    }
    else if (eyePupilAng > 159 && eyePupilAng <= 223)
    {
        SetSpriteAnim(THIS,anim_bossEyeLeft, 15);
        SPRITE_SET_VMIRROR(THIS);
        SPRITE_UNSET_HMIRROR(THIS);
    }
    else if (eyePupilAng > 96 && eyePupilAng <= 159)
    {
        SetSpriteAnim(THIS,anim_bossEyeUp, 15);
        SPRITE_SET_HMIRROR(THIS);
        SPRITE_UNSET_VMIRROR(THIS);
    }
    else if (eyePupilAng > 223 || eyePupilAng <= 32)
    {
        SetSpriteAnim(THIS,anim_bossEyeUp, 15);
        SPRITE_UNSET_HMIRROR(THIS);
        SPRITE_UNSET_VMIRROR(THIS);
    }
}

void EyeTakeDamage()
{
    set_sprite_palette(2,1,eyeBossHurtPalette); 
    eyeBossHealth--;
    eyeHurtPaletteTimer = 20;
    // if (eyeBossHealth == 2)
    // {
    //     //play sound
    //     snakeMoveDelayMax = 1;
    // }
    SetHealth(eyeBossHealth, eyeBossHealthMax); 

    if (eyeBossHealth == 2 || eyeBossHealth == 1)
    {
        PlayFx(CHANNEL_1,10,0x2c,0x89,0xf2,0x3f,0x85);
        eyeTimerGeneric = 0; //trigger an attack
        //get faster
    }

    if (eyeBossHealth == 0)
    {
        UINT8 i;
        struct Sprite* spr;
        SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
        {
            if (spr->type == SpriteBossEyePupil)
            {
                triggerWinJingle =1;
                SpriteManagerRemove(i); //remove eye
                //trigger win
                
            }
        }
    }
}



void EyeShotgunInit()
{
    PlayFx(CHANNEL_1,10,0x26,0xf2,0x5d,0x85);
    eyePupilIsCentered = 1;

    UINT8 lastPos = rand() & 0b1111111;
    while (lastPos % 4 == eyeLastShotgunPosition)
    {
        UINT8 lastPos = rand() & 0b1111111;
    }
    eyeLastShotgunPosition = lastPos % 4;

    if (eyeLastShotgunPosition == 0)
    {
        eyeTargetX = 4;
        eyeTargetY = 4;
    }
    else if (eyeLastShotgunPosition == 1)
    {
        eyeTargetX = 22;
        eyeTargetY = 4;
    }
    else if (eyeLastShotgunPosition == 2)
    {
        eyeTargetX = 4;
        eyeTargetY = 22;
    }
    else
    {
        eyeTargetX = 22;
        eyeTargetY = 22;
    }
    set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeTargetTiles); //set tiles
    get_bkg_tiles(eyeTargetX,eyeTargetY,4,4,originalTilesBeforeTargetTiles); //store new tiles

    eyeTimerGeneric = 90; //how long target indicator is active
    eyeAttackState = ShotgunInit;
}
void EyeShotgunTeleport()
{
    EyeTeleportCode();
    eyeTimerGeneric = 10; //how long to wait before firing
    eyeAttackState = ShotgunTeleport;
}
void EyeShotgunFire()
{
    //fire

    PlayFx(CHANNEL_1,10,0x27,0x89,0xf2,0x35,0x85);
    if (eyeBallOriginX == 4 && eyeBallOriginY == 4)
    {
        eyePupilAng = 64-14;
    }
    else if (eyeBallOriginX == 22 && eyeBallOriginY == 4)
    {
        eyePupilAng = 128-14;
    }
    else if (eyeBallOriginX == 4 && eyeBallOriginY == 22)
    {
        eyePupilAng = 0-14;
    }
    else
    {
        eyePupilAng = 191-14;
    }

    UINT8 i;
    for (i = 0; i < 5; i++)
    {
        SpawnLaser();
        eyePupilAng+= 92/5;
    }

    DelayBetweenAttacks(); //how long to wait before next attack
    eyeAttackState = Idle;
    eyePupilIsCentered = 0;
}

void EyeBlastInit()
{
    PlayFx(CHANNEL_1,10,0x27,0x09,0xf2,0xe4,0x85);
    eyePupilIsCentered = 1;

    eyeTargetX = 13;
    eyeTargetY = 13;

    set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeTargetTiles); //set tiles
    get_bkg_tiles(eyeTargetX,eyeTargetY,4,4,originalTilesBeforeTargetTiles); //store new tiles

    eyeTimerGeneric = 120; //how long target indicator is active
    eyeAttackState = BlastInit;
}
void EyeBlastTeleport()
{
    EyeTeleportCode();
    eyeTimerGeneric = 10; //how long to wait before firing
    eyeAttackState = BlastTeleport;
}
void EyeBlastFire()
{
//fire
PlayFx(CHANNEL_1,10,0x27,0x89,0xf2,0x35,0x85);
    eyePupilAng = rand() & 0b11111111;
    UINT8 i;
    for (i = 0; i < 8; i++)
    {
        SpawnLaser();
        eyePupilAng+= 32;
    }

    DelayBetweenAttacks(); //how long to wait before next attack
    eyeAttackState = Idle;
    eyePupilIsCentered = 0;
}

void EyeTelePlayerInit()
{
    PlayFx(CHANNEL_1,10,0x2f,0x89,0xf2,0x3f,0x85);
    eyePupilIsCentered = 1;
    eyeTargetX = (eyePlayerSpr->x >> 3) - 1;
    eyeTargetY = (eyePlayerSpr->y >> 3) - 1;
    set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeTargetTiles); //set tiles
    get_bkg_tiles(eyeTargetX,eyeTargetY,4,4,originalTilesBeforeTargetTiles); //store new tiles

    eyeTimerGeneric = 120; //how long target indicator is active
    eyeAttackState = TelePlayerInit;
}
void EyeTelePlayerTeleport()
{
    EyeTeleportCode();
    eyeTimerGeneric = 10; //how long to wait before firing
    eyeAttackState = TelePlayerTeleport;
}
void EyeTelePlayerFire()
{
//fire

PlayFx(CHANNEL_1,10,0x27,0x89,0xf2,0x35,0x85);
    eyePupilAng = rand() & 0b11111111;
    UINT8 i;
    for (i = 0; i < 3; i++)
    {
        SpawnLaser();
        eyePupilAng+= 255/3;
    }

    DelayBetweenAttacks(); //how long to wait before next attack
    eyeAttackState = Idle;
    eyePupilIsCentered = 0;
}

void EyeTeleportCode()
{
    PlayFx(CHANNEL_1, 10, 0x25, 0x89, 0xf2, 0x35, 0x85);
    set_bkg_tiles(eyeBallOriginX,eyeBallOriginY,4,4,originalTilesBeforeEyeTiles); //set old tiles back
    set_bkg_tiles(eyeTargetX,eyeTargetY,4,4,originalTilesBeforeTargetTiles); //store new tiles
    
    eyeBallOriginX = eyeTargetX;
    eyeBallOriginY = eyeTargetY;

    get_bkg_tiles(eyeTargetX,eyeTargetY,4,4,originalTilesBeforeEyeTiles); //store new tiles
    
    eyeHHurtBox->x = 8*eyeBallOriginX + 8;
    eyeHHurtBox->y = 8*eyeBallOriginY;
    //eyeVHurtBox->x = 8*eyeBallOriginX;
    //eyeVHurtBox->y = 8*eyeBallOriginY + 8 - 16;

    THIS->x = eyeTargetX + 8;
    THIS->y = eyeTargetY + 8;
}

void SpawnLaser()
{
    ((struct LaserInfo*)(SpriteManagerAdd(SpriteBossLaserBall, 8*eyeBallOriginX + 16 - 8 + (SIN(eyePupilAng) / 6), 8*eyeBallOriginY + 16 - 8 + (COS(eyePupilAng) / 6))->custom_data))->angle = eyePupilAng;
}

void DelayBetweenAttacks()
{
    eyeTimerGeneric = (rand() & 0b11111111) + 100; //how long to wait before next attack
}


void EyeRotatePupil()
{
    eyePupilIsCentered = 0;
    eyePupilRotationDirection  = rand() & 0b1;
    eyeTimerGeneric = (rand() & 0b11111111) + 90;
    eyeAttackState = RotatingEye;
}

