#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"




extern UINT8 spikeRestrictMovementTimer;
extern UINT8 spikeSlowdown;


struct BossSnakeBody
{
     BYTE isHurtbox;
};

extern void SnakeTakeDamage();

void Start_SpriteBossSnakeBody() {
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;


    //SetSpriteAnim(THIS,anim_bossSnakeBodyLeft, 15);

}

void Update_SpriteBossSnakeBody() {
    
 (struct BossSnakeBody*)THIS->custom_data;
    
    
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
                        PlayFx(CHANNEL_1, 10, 0x8f, 0xc7, 0xf3, 0x73, 0xdd);
                    }
                }
            }
        }
    }
} 

void Destroy_SpriteBossSnakeBody() {
}

