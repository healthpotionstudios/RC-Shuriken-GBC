#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define SPRITE_SET_HMIRROR(SPRITE)   (SPRITE->flags |= 64)
#define SPRITE_UNSET_HMIRROR(SPRITE) (SPRITE->flags &= ~64)
#define SPRITE_GET_HMIRROR(SPRITE)   (SPRITE->flags & 64)

#define STATES \
_STATE(StateGame)\
_STATE(StateTutorial)\
_STATE(StateTitle)\
STATE_DEF_END

//associate .c sprite file with art (file name without .b3.gbr)
#define SPRITES \
_SPRITE_DMG(SpritePlayer, player)\
_SPRITE_DMG(SpriteSpike, spike)\
_SPRITE_DMG(SpriteCamera, camera)\
_SPRITE_DMG(SpriteBossSnakeHead, snake)\
_SPRITE_DMG(SpriteBossSnakeBody, snake)\
_SPRITE_DMG(SpriteBossEyePupil, eyePupil)\
_SPRITE_DMG(SpriteBossEyeHHurtbox, camera)\
_SPRITE_DMG(SpriteBossEyeVHurtbox, camera)\
_SPRITE_DMG(SpriteBossLaserBall, eyePupil)\
_SPRITE_DMG(SpritePortal, portal)\
_SPRITE_DMG(SpriteBossFly, fly)\
_SPRITE_DMG(SpriteBossFrog, frog)\
_SPRITE_DMG(SpriteBossFrogBody, fly)\
_SPRITE_DMG(SpriteBossFrogTongue, tongue)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif