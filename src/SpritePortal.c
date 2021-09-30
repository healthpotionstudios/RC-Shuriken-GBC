#include "Banks/SetBank2.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "Scroll.h"
#include "ZGBMain.h" // get access to sprite types, delta_time

const UINT8 anim_portal[] = {4, 0,1,2,3};

void Start_SpritePortal() {
    THIS->lim_x = 0xfff;
    THIS->lim_y = 0xfff;
    THIS->coll_h = 20;

    SetSpriteAnim(THIS,anim_portal, 20);

}

void Update_SpritePortal() {
    

} 

void Destroy_SpritePortal() {
}

