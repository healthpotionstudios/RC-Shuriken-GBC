#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time
#include "Sound.h"



void Start_SpriteBossEyeHHurtbox() {
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    // THIS->coll_x = 0;
    // THIS->coll_y = 3;
    // THIS->coll_w = 16;
    // THIS->coll_h = 28;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;
}

void Update_SpriteBossEyeHHurtbox() {
    
} 

void Destroy_SpriteBossEyeHHurtbox() {
}

