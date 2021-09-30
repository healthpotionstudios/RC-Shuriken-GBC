#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"
#include "Trig.h"


const UINT8 anim_Laser[] = {1, 0};



struct LaserInfo
{
    UINT8 angle;
    UINT8 laserDelay;
};

    fixed laserAccum_x;
    fixed laserAccum_y;
    INT16 laserSpeed_x;
    INT16 laserSpeed_y;

void Start_SpriteBossLaserBall() {
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;


    SetSpriteAnim(THIS,anim_Laser, 15);

}

void Update_SpriteBossLaserBall() {
    struct LaserInfo* data = (struct LaserInfo*)THIS->custom_data;
    if (data->laserDelay == 0)
    {
        laserSpeed_x = SIN(data->angle) << 2;
        laserSpeed_y = COS(data->angle) << 2;

        laserAccum_x.w += laserSpeed_x << delta_time;
        laserAccum_y.w += laserSpeed_y << delta_time;

        if (TranslateSprite(THIS, laserAccum_x.b.h, laserAccum_y.b.h))
        {
            PlayFx(CHANNEL_4, 4, 0x0c, 0x41, 0x30, 0xc0);
            SpriteManagerRemoveSprite(THIS);
        }
        data->laserDelay = 1;
    }
    else
    {
        data->laserDelay--;
    }
    laserAccum_x.b.h = 0;
	laserAccum_y.b.h = 0;
} 

void Destroy_SpriteBossLaserBall() {
}

