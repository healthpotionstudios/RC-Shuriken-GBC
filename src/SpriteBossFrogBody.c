#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time


void Start_SpriteBossFrogBody() {
    THIS->coll_x = 3;
    THIS->coll_y = 3;
    THIS->coll_w = 10;
    THIS->coll_h = 10;
    THIS->lim_x = 0xff;
    THIS->lim_y = 0xff;

}

void Update_SpriteBossFrogBody() {
    

} 

void Destroy_SpriteBossFrogBody() {
}

