#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"



void Start_SpriteBossEyeVHurtbox() {
    THIS->coll_x = 3;
    THIS->coll_y = 0+16;
    THIS->coll_w = 28;
    THIS->coll_h = 16;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;
}

void Update_SpriteBossEyeVHurtbox() {
    
} 

void Destroy_SpriteBossEyeVHurtbox() {
}

