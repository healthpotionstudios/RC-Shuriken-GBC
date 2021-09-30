#include "Banks/SetBank2.h"
#include "SpriteManager.h"
#include "ZGBMain.h" //delta_time
#include "Keys.h"
#include "Trig.h"
#include "../res/src/player.h"

fixed accum_x;
fixed accum_y;
INT16 speed_x;
INT16 speed_y;
UINT8 spikeAng;
// UINT8 desiredspikeAng;
//                               //0 1  2   3  4   5  6   7   8    10  11 12 13  14  15
// const UINT8 movementAngles[] = {0,64,191,64,128,96,159,128,0,32,223,0, 0, 64, 191, 0};

const UINT8 anim_spikeSpinCCW[] = {3, 0, 1, 2};
const UINT8 anim_spikeSpinCW[] = {3, 2, 1, 0};

UINT8 spikeSlowdown = 10;
UINT8 timer = 0;
const UINT8 timerMax = 10;
UINT8 spikeRestrictMovementTimer;

extern BYTE controllingPlayer;

struct EnemyInfo
{
    INT8 vy;
};

void Bounce(BYTE restrictMovement);

void Start_SpriteSpike() {  
    if (current_state == StateTutorial || current_state == StateTitle)
    {
        SetSpriteAnim(THIS, anim_spikeSpinCCW, 20);
        THIS->lim_x = 0xff;
        THIS->lim_y = 0xff;
        return;
    }

    struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
    data->vy = 1;

    speed_x = 0;
	speed_y = 0;
	accum_x.w = 0;
	accum_y.w = 0;
    spikeSlowdown = 20;
    timer = timerMax;
    spikeRestrictMovementTimer = 0;
    THIS->coll_x = 4;
    THIS->coll_y = 4;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;

    SetSpriteAnim(THIS, anim_spikeSpinCCW, 0);
}

void Update_SpriteSpike() {
    
    if (current_state == StateTutorial  || current_state == StateTitle)
    {
        return;
    }

    UINT8 increment = 0;
  
    if (spikeRestrictMovementTimer != 0)
    {
        spikeRestrictMovementTimer--;
    }

    if((KEY_PRESSED(J_LEFT) || KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_UP) || KEY_PRESSED(J_DOWN)) && !controllingPlayer && spikeRestrictMovementTimer == 0)
    {
        if (spikeSlowdown > 2)
        {
            increment = 5;
        }
        else if (spikeSlowdown == 2)
        {
            increment = 4;
        }
        else if (spikeSlowdown == 1)
        {
            increment = 3;
        }

        if (spikeSlowdown != 1)
        {
            timer--;
            if (timer == 0) 
            {
                spikeSlowdown--;
                timer = timerMax >> 2;
            }
        }
        //desiredspikeAng = movementAngles[keys & 0xf ^ 0b1100]; //XOR because I messed up the up and down a few times in the array
        if (spikeSlowdown > 3)
        {
            if(KEY_PRESSED(J_UP)) {
                spikeAng = 0;
            }
            if(KEY_PRESSED(J_LEFT)) {
                spikeAng = 191;
            }
            if(KEY_PRESSED(J_RIGHT)) {
                spikeAng = 64;
            }
            
            if(KEY_PRESSED(J_DOWN)) {
                spikeAng = 128;
            }
        }
        else
        {
            if(KEY_PRESSED(J_LEFT)) {
                if (spikeAng != 191)
                {
                    if (spikeAng < 64 || spikeAng > 191)
                    {
                        spikeAng-=increment;
                    }
                    else
                    {
                        spikeAng+=increment;
                    }
                }
            }
            if(KEY_PRESSED(J_RIGHT)) {
                if (spikeAng != 64)
                {
                    if (spikeAng < 64 || spikeAng > 191)
                    {
                        spikeAng+=increment;
                    }
                    else
                    {
                        spikeAng-=increment;
                    }
                }
            }
            if(KEY_PRESSED(J_UP)) {
                if (spikeAng != 0)
                {
                    if (spikeAng < 128)
                    {
                        spikeAng-=increment;
                    }
                    else
                    {
                        spikeAng+=increment;
                    }
                }
            }
            if(KEY_PRESSED(J_DOWN)) {
                if (spikeAng != 128)
                {
                    if (spikeAng > 128)
                    {
                        spikeAng-=increment;
                    }
                    else
                    {
                        spikeAng+=increment;
                    }
                }
            }
        }
    }
    else
    {
        if (spikeSlowdown != 20)
        {
            timer--;
            if (timer == 0) 
            {
                spikeSlowdown++;
                timer = timerMax;
            }
            
        }
    }

        
    UINT8 i;
    struct Sprite* spr;

    SPRITEMANAGER_ITERATE(i,spr) //iterates over active sprites
    {
        if (spr->type == SpriteBossSnakeHead || spr->type == SpriteBossSnakeBody ||
            spr->type == SpriteBossEyePupil || spr->type == SpriteBossEyeHHurtbox || spr->type == SpriteBossEyeVHurtbox || spr->type == SpriteBossLaserBall ||
            spr->type == SpriteBossFly || spr->type == SpriteBossFrog || spr->type == SpriteBossFrogBody || spr->type == SpriteBossFrogTongue)
        {
            if (CheckCollision(THIS,spr))
            {
                if (spikeRestrictMovementTimer == 0)
                {
                    Bounce(1);
                }
            }
        }
    }


    speed_x = (SIN(spikeAng) << 2) / spikeSlowdown;
    speed_y = (COS(spikeAng) << 2) / spikeSlowdown;

    
    THIS->anim_speed = 20-spikeSlowdown;




    if (spikeSlowdown < 19)
    {
        accum_x.w += speed_x << delta_time;
        accum_y.w += speed_y << delta_time;

        if(TranslateSprite(THIS, accum_x.b.h, accum_y.b.h))
        {
            Bounce(0);
        }
    }

    accum_x.b.h = 0;
	accum_y.b.h = 0;


}

void Destroy_SpriteSpike() {
}

void Bounce(BYTE restrictMovement)
{
    if (restrictMovement == 1)
    {
        spikeSlowdown = 0;
        spikeRestrictMovementTimer = 30;
    }
    // else
    // {
    //     //wall
    // }

    if (spikeAng > 159 && spikeAng <= 223 || spikeAng < 96 && spikeAng >= 32)
    {
        spikeAng = 0 - spikeAng;
    }
    if (spikeAng > 96 && spikeAng <= 159 || spikeAng > 233 | spikeAng < 32)
    {
        spikeAng = 128 - spikeAng;
    }
    //spikeSlowdown = 3; //tenative
}