#include "ZGBMain.h"
#include "Math.h"

const UINT8 EMPTY_HEART_TILE = 12;
const UINT8 HEART_TILE_HALF = 13;
const UINT8 HEART_TILE_FULL = 14;

UINT8 next_state = StateTitle;

void SetHealth(UINT8 curr, UINT8 max);

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == StateGame) {
		if(U_LESS_THAN(255 - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
			*tile = 0;
			return 255 - (UINT16)*tile_ptr;
		}

		*tile = *tile_ptr;
	}

	return 255u;
}


void SetHealth(UINT8 curr, UINT8 max)
{
    UINT8 i;
    for(i = 0; i < curr/2; ++i) {
		set_win_tiles(6 + i, 0, 1, 1, &HEART_TILE_FULL);
	}
    for(i = (curr+1)/2; i != max/2; ++i) {
		set_win_tiles(6 + i, 0, 1, 1, &EMPTY_HEART_TILE);
	}
    if (curr % 2 == 1)
    {
        set_win_tiles(6 + (curr-1)/2, 0, 1, 1, &HEART_TILE_HALF);
    }
}