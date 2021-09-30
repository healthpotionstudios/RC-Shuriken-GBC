#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"
#include "Trig.h"
#include "Print.h"


const UINT8 anim_fly[] = {2,0,1};
struct Sprite* flyPlayerSpr;

UINT8 flyAng;
fixed flyAccum_x;
fixed flyAccum_y;
INT16 flySpeed_x;
INT16 flySpeed_y;
UINT8 flyDelay;


void Start_SpriteBossFly() {
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;


    SetSpriteAnim(THIS,anim_fly, 15);

    UINT8 i;
    struct Sprite* spr;
    SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
    {
        if (spr->type == SpritePlayer)
        {
            flyPlayerSpr = spr;
        }
    }

}

void Update_SpriteBossFly()
{
    flyAng = AngleTowards(THIS->x+8, THIS->y+8,flyPlayerSpr->x+8,flyPlayerSpr->y+8);

    if (flyDelay == 0)
    {
        flySpeed_x = SIN(flyAng) << 2;
        flySpeed_y = COS(flyAng) << 2;

        flyAccum_x.w += flySpeed_x << delta_time;
        flyAccum_y.w += flySpeed_y << delta_time;

        TranslateSprite(THIS, flyAccum_x.b.h, flyAccum_y.b.h);
        flyDelay = 4;
    }
    else
    {
        flyDelay--;
    }
    


 /*(struct BossSnakeBody*)THIS->custom_data;
    
    
    if (((struct BossSnakeBody*)THIS->custom_data)->isHurtbox)//only the boss tail
    {
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
                        SnakeTakeDamage();
                        PlayFx(CHANNEL_1, 3, 0x8f, 0xc7, 0xf3, 0x73, 0xdd);
                    }
                }
            }
        }
    }*/

    flyAccum_x.b.h = 0;
	flyAccum_y.b.h = 0;
} 

void Destroy_SpriteBossFly() {
}

