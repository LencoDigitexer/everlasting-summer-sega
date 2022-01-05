#include "Game.h"

String strArr[10];

bool paused = FALSE;
u16 palette_full[64];
u16 tile_id_init;
u16 tile_id = 0;
u16 tile_id_LAST = 0;

f16 shake_x_radius = 0;
f16 shake_timer = 0;
s16 curBgA_posX = 0;

u16 tile_id_MsgSign;
u16 tile_id_AnswerSign;

u16 bgA_tile_id = 0;

void VDP_drawTextOffset(VDPPlane plane, const u16 *vram_offsets, u16 len, u16 first_tile, u16 x, u16 y)
{
    u16 i, pw, ph, curX;

    // get the horizontal plane size (in cell)
    pw = (plane == WINDOW)?windowWidth:planeWidth;
    ph = (plane == WINDOW)?32:planeHeight;

    // string outside plane --> exit
    if ((x >= pw) || (y >= ph))
        return;

    //len = array_u16len(vram_offsets);// иногда работает иногда нет, забил на эту функцию теперь ввожу длину ручками.

    //len = sizeof(vram_offsets);

    // if string don't fit in plane, we cut it
    if (len > (pw - x))
        len = pw - x;
    i = 0;
    curX = 1;
    u16 curTileInd = 0;
    VDP_setPaletteColor(16+1,RGB24_TO_VDPCOLOR(0xffffff));
    while(i < len) {
        curTileInd = first_tile-1 + vram_offsets[i]; 
	//KLog_S1("curTile: ", curTileInd);
		
	VDP_setTileMapXY(plane, TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,curTileInd), curX, y);
	i++;
	curX++;
    }
}

void sumimasen_screen(){
	VDP_drawImage(BG_A, &soviet_games, 0, 0);
	SYS_doVBlankProcess();
	XGM_setPCM(70, sumimasen_sfx, sizeof(sumimasen_sfx));
	//XGM_startPlayPCM(70, 15, SOUND_PCM_CH2);
	waitMs(300);
	VDP_drawImage(BG_A, &sumimasen_img, 0, 0);
	SYS_doVBlankProcess();
	XGM_setPCM(70, sumimasen_sfx, sizeof(sumimasen_sfx));
	//XGM_startPlayPCM(70, 15, SOUND_PCM_CH2);
	waitMs(600);
	VDP_resetScreen();
}

u8 chooseDialog(char* question_str, String* str_arr, u8 len, Image* bg1){
	//VDP_setWindowVPos(FALSE, 3);
	VDP_setHorizontalScroll(BG_A, 0);
	VDP_setVerticalScroll(BG_A, 0);
	
	u8 cur_x = 0;
	u8 cur_y = 4;
	u8 start_y = cur_y;
	u8 timer = 10;
	bool showArrow = TRUE;
	bool prev_pressed = FALSE;
	bool pressed = FALSE;
	
	s16 cur_choose = 0;
	s16 prev_choose = cur_choose;
	
	//u8 minus_y = len/2;
	u8 minus_y = len;

	char* str;

	if(question_str) {
		VDP_drawImageEx(BG_A, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18, FALSE, TRUE);
		VDP_drawImageEx(BG_A, &question_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_AnswerSign), 0, 0, FALSE, TRUE);
		VDP_drawTextBG(BG_A, question_str, 20 - strlen(question_str)/2, 1);
	}
	
	u8 joy_value = 0;
	joy_value = JOY_readJoypad(JOY_1);
		SYS_doVBlankProcess();
	
	while(joy_value) {
		joy_value = JOY_readJoypad(JOY_1);
		SYS_doVBlankProcess();
	}
	
	for(u8 i=0; i<len; i++) {
			
		str = str_arr[i].str;
			
		VDP_drawTextBG(BG_A, str, 20 - strlen(str)/2, cur_y);
		if(i == cur_choose) {
			if(showArrow) {
				VDP_drawTextBG(BG_A, ">", (20 - strlen(str)/2)-1, cur_y);
			}
		}
		cur_y += 2;
	}
	
	while(1) {
		cur_y = start_y;
		if (joy_value & BUTTON_DOWN) {
			if(pressed && !prev_pressed) {
				cur_choose++;
			}
		} else if(joy_value & BUTTON_UP){
			if(pressed && !prev_pressed) {
				cur_choose--;
			}
		} else if(joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C){
			if(pressed && !prev_pressed) {
				
				if(bg1){
					VDP_clearPlane(BG_A, TRUE);
					PAL_setPalette(PAL1, bg1->palette->data, CPU);
					VDP_drawImageEx(BG_A, bg1, TILE_ATTR_FULL(PAL1, 255, FALSE, FALSE, bgA_tile_id), 0, 0, FALSE, TRUE);
				}
				
				
				return cur_choose;
			}
		}
		
		if(cur_choose > len-1) {
			cur_choose = 0;
		} else if(cur_choose < 0) {
			cur_choose = len-1;
		}
		
		for(u8 i=0; i<len; i++) {
			
			str = str_arr[i].str;
			VDP_drawTextBG(BG_A, " ", (20 - strlen(str)/2)-1, cur_y);
			if(i == cur_choose) {
				if(showArrow) {
					VDP_drawTextBG(BG_A, ">", (20 - strlen(str)/2)-1, cur_y);
				}
			}
			cur_y += 2;
		}
		
		prev_pressed = pressed;
		joy_value = JOY_readJoypad(JOY_1);
		SYS_doVBlankProcess();
		
		
		if(joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_DOWN || joy_value & BUTTON_UP)
			pressed = TRUE;
		else {
			pressed = FALSE;
		}
		
		timer--;
		
		
		if(!timer){
			timer = 10;
			showArrow = !showArrow;
		}
	}
}

void printMessage_BOX(char* str, u8 x1, u8 y1, u8 x2, u8 y2, Image* bg_img) {
	VDP_clearPlane(BG_A, TRUE);
	if(bg_img) VDP_drawImageEx(BG_A, bg_img, TILE_ATTR_FULL(PAL0, 255, FALSE, FALSE, curTileInd), 0, 0, FALSE, TRUE);
	
	u32 str_len = strlen(str);
	char str_char[1];
	char str_line[35];
	u8 start_pos_x = x1;
	u8 start_pos_y = y1;
	
	bool skip = FALSE;
	
	u8 cur_y = 0;
	u8 max_y = y2;
	u8 cur_x = 0;
	u8 max_x = x2;
	u8 max_x_line = max_x;
	
	u8 joy_value = 0;
	joy_value = JOY_readJoypad(JOY_1);
	SYS_doVBlankProcess();
	
	bool can_skip = TRUE;
	bool prev_joy_pressed = FALSE;
	bool joy_pressed = FALSE;

	
	if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) {
		can_skip = FALSE;
	}

	
	strncpy(&str_line, 0, max_x);
	
	for(u32 i=0;i<str_len;i++) {
		strncpy(&str_char, str+i, 1);
		
		
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed && can_skip) {
				skip = TRUE;
			} else {
				joy_pressed = FALSE;
			}
		} else {
			can_skip = TRUE;
		}

		if(i != 0 && (cur_x > max_x_line)) {
			cur_x = 0;
			cur_y += 2;
			
			if(cur_y >= max_y) { //if text doesn't fit in the rectangle, then
				while(1) {
					joy_value = JOY_readJoypad(JOY_1);
					SYS_doVBlankProcess();
					//waiting for just_pressed action from player
					if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) 
					{
						
						joy_pressed = TRUE;
						if(joy_pressed && !prev_joy_pressed) {
							cur_x = 0;
							cur_y = 0;
							skip = FALSE;
							can_skip = FALSE;
							VDP_clearPlane(BG_A, TRUE);
							if(bg_img) VDP_drawImageEx(BG_A, bg_img, TILE_ATTR_FULL(PAL0, 255, FALSE, FALSE, curTileInd), 0, 0, FALSE, TRUE);
							
							break;
						}
					} else {
						joy_pressed = FALSE;
					}
					prev_joy_pressed = joy_pressed;
				}
			}
		}
		VDP_drawTextBG(BG_A, str_char, start_pos_x+cur_x, start_pos_y+cur_y);
		if(cur_x == 0) {
			strncpy(&str_line, str+i, max_x);
			bool result_found = FALSE;
			u8 str_line_len = strlen(str_line);
			//if((str_line_len < max_x-1) && FALSE) {
			if(str_line_len < max_x) {
				max_x_line = max_x;
			}
			else {
				for(u8 k=0; k<max_x; k++)
				{
					if(str_line[k] == '\n') {
						max_x_line = k;
						result_found = TRUE;
						break;
					}
				}
				if(!result_found){
					for(u8 j=max_x; j>0; j--)
					{
						if(str_line[j] == ' '){
							max_x_line = j;
							
							break;
						}	
					}
				}
			}
			
			strncpy(&str_line, str+i, max_x_line);
			KDebug_Alert(str_line);
			KLog_U1("max_x_line: ", max_x_line);
			KLog_U1("str_line_len: ", str_line_len);
			KLog_U1("i: ", i);
		}
		
		

		//if skip, then disabling text appear animation
		if (!skip) {	
			waitMs(20);
			SYS_doVBlankProcess();
		}
		prev_joy_pressed = joy_pressed;

		shakeAnimation(25);
		cur_x++;
		
	}
	prev_joy_pressed = TRUE;
	skip = FALSE;
	//waiting for last press
	while(1) {
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed) {
				VDP_clearPlane(BG_A, TRUE);
				if(bg_img) VDP_drawImageEx(BG_A, bg_img, TILE_ATTR_FULL(PAL0, 255, FALSE, FALSE, curTileInd), 0, 0, FALSE, TRUE);
				break;
			}
		} else {
			joy_pressed = FALSE;
		}
		prev_joy_pressed = joy_pressed;
		shakeAnimation(25);
		SYS_doVBlankProcess();	
	}
}

void printMessageName(char* str, char* name, SpriteDefinition* characterSpr_def, u16 posX, u8 pcm_id) {
	if(pcm_id) {
		XGM_startPlayPCM(pcm_id, 15, SOUND_PCM_CH2);
	}
	
	VDP_setWindowVPos(TRUE, 18);
	VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18 , FALSE, TRUE);
	VDP_drawTextBG(WINDOW, name, 1,19);

	u32 str_len = strlen(str);
	char str_char[1];
	char str_char_line[1];
	
	str_char_line[0] = " ";
	
	char str_line[35];
	u8 start_pos_x = 2;
	u8 start_pos_y = 21;
	
	bool skip = FALSE;
	
	u8 cur_y = 0;
	u8 max_y = 6;
	u8 cur_x = 0;
	u8 max_x = 35;
	u8 max_x_line = max_x;
	
	s16 timer_init = 15;
	s16 timer = timer_init;
	bool timer_trigger = FALSE;
	
	Sprite* SpriteCharacter;
	
	u8 joy_value = 0;
	joy_value = JOY_readJoypad(JOY_1);
	SYS_doVBlankProcess();
	
	bool can_skip = TRUE;
	bool prev_joy_pressed = FALSE;
	bool joy_pressed = FALSE;

	
	if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) {
		can_skip = FALSE;
	}
	
	if(characterSpr_def){
		SpriteCharacter = SPR_addSprite(characterSpr_def, posX-(characterSpr_def->w/2), 160-characterSpr_def->h, TILE_ATTR(PAL3, 244, FALSE, FALSE));
		SPR_update();
	}
	
	strncpy(&str_line, 0, max_x);
	
	for(u32 i=0;i<str_len;i++) {
		strncpy(&str_char, str+i, 1);
		
		
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed && can_skip) {
				skip = TRUE;
			} else {
				joy_pressed = FALSE;
			}
		} else {
			can_skip = TRUE;
		}

		if(i != 0 && (cur_x > max_x_line)) {
			cur_x = 0;
			cur_y += 2;
			
			if(cur_y >= max_y) { //if text doesn't fit in the rectangle, then
				while(1) {
					joy_value = JOY_readJoypad(JOY_1);
					SYS_doVBlankProcess();
					//waiting for just_pressed action from player
					if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) 
					{
						
						joy_pressed = TRUE;
						if(joy_pressed && !prev_joy_pressed) {
							cur_x = 0;
							cur_y = 0;
							skip = FALSE;
							can_skip = FALSE;
							VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18, FALSE, TRUE);
							break;
						}
					} else {
						joy_pressed = FALSE;
					}
					prev_joy_pressed = joy_pressed;
				}
			}
		}
		VDP_drawTextBG(WINDOW, str_char, start_pos_x+cur_x, start_pos_y+cur_y);
		if(cur_x == 0) {
			strncpy(&str_line, str+i, max_x);
			u8 str_line_len = strlen(str_line);
			//if((str_line_len < max_x-1) && FALSE) {
			if(str_line_len < max_x) {
				max_x_line = max_x;
			}
			else {
				for(u8 j=0; j<max_x; j++)
				{
					//KLog_U1("dasdsa1: ", *str_char_line);
					//KLog_U1("dasdsa2: ", ' ');
					if(str_line[j] == ' '){
						//max_x_line = j+1;
						max_x_line = j;
						//break;
					}	
				}
			}
			
			strncpy(&str_line, str+i, max_x_line);
			KDebug_Alert(str_line);
			KLog_U1("max_x_line: ", max_x_line);
			KLog_U1("str_line_len: ", str_line_len);
			KLog_U1("i: ", i);
		}
		
		

		//if skip, then disabling text appear animation
		if (!skip) {	
			waitMs(20);
			SYS_doVBlankProcess();
		}
		prev_joy_pressed = joy_pressed;

		shakeAnimation(25);
		cur_x++;
		
	}
	prev_joy_pressed = TRUE;
	skip = FALSE;
	//waiting for last press
	while(1) {
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed) {
				//VDP_setWindowVPos(FALSE, 0); //deleting message from screen
				break;
			}
		} else {
			joy_pressed = FALSE;
		}
		prev_joy_pressed = joy_pressed;
		shakeAnimation(25);
		SYS_doVBlankProcess();	
	}
	if(characterSpr_def){
		SPR_releaseSprite(SpriteCharacter);
	}
	VDP_setWindowVPos(FALSE, 0);
}

void printMessage(char* str, SpriteDefinition* characterSpr_def, u16 posX, u8 pcm_id) {
	if(pcm_id) {
		XGM_startPlayPCM(pcm_id, 15, SOUND_PCM_CH2);
	}
	
	VDP_setWindowVPos(TRUE, 18);
	VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18 , FALSE, TRUE);
	

	u32 str_len = strlen(str);
	char str_char[1];
	char str_char_line[1];
	
	str_char_line[0] = " ";
	
	char str_line[35];
	u8 start_pos_x = 2;
	u8 start_pos_y = 21;
	
	bool skip = FALSE;
	
	u8 cur_y = 0;
	u8 max_y = 6;
	u8 cur_x = 0;
	u8 max_x = 35;
	u8 max_x_line = max_x;
	
	s16 timer_init = 15;
	s16 timer = timer_init;
	bool timer_trigger = FALSE;
	
	Sprite* SpriteCharacter;
	
	u8 joy_value = 0;
	joy_value = JOY_readJoypad(JOY_1);
	SYS_doVBlankProcess();
	
	bool can_skip = TRUE;
	bool prev_joy_pressed = FALSE;
	bool joy_pressed = FALSE;

	
	if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) {
		can_skip = FALSE;
	}
	
	if(characterSpr_def){
		SpriteCharacter = SPR_addSprite(characterSpr_def, posX-(characterSpr_def->w/2), 160-characterSpr_def->h, TILE_ATTR(PAL3, 244, FALSE, FALSE));
		SPR_update();
	}
	
	strncpy(&str_line, 0, max_x);
	
	for(u32 i=0;i<str_len;i++) {
		strncpy(&str_char, str+i, 1);
		
		
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed && can_skip) {
				skip = TRUE;
			} else {
				joy_pressed = FALSE;
			}
		} else {
			can_skip = TRUE;
		}

		if(i != 0 && (cur_x > max_x_line)) {
			cur_x = 0;
			cur_y += 2;
			
			if(cur_y >= max_y) { //if text doesn't fit in the rectangle, then
				while(1) {
					joy_value = JOY_readJoypad(JOY_1);
					SYS_doVBlankProcess();
					//waiting for just_pressed action from player
					if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START) 
					{
						
						joy_pressed = TRUE;
						if(joy_pressed && !prev_joy_pressed) {
							cur_x = 0;
							cur_y = 0;
							skip = FALSE;
							can_skip = FALSE;
							VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18, FALSE, TRUE);
							break;
						}
					} else {
						joy_pressed = FALSE;
					}
					prev_joy_pressed = joy_pressed;
				}
			}
		}
		VDP_drawTextBG(WINDOW, str_char, start_pos_x+cur_x, start_pos_y+cur_y);
		if(cur_x == 0) {
			strncpy(&str_line, str+i, max_x);
			u8 str_line_len = strlen(str_line);
			//if((str_line_len < max_x-1) && FALSE) {
			if(str_line_len < max_x) {
				max_x_line = max_x;
			}
			else {
				for(u8 j=0; j<max_x; j++)
				{
					//KLog_U1("dasdsa1: ", *str_char_line);
					//KLog_U1("dasdsa2: ", ' ');
					if(str_line[j] == ' '){
						//max_x_line = j+1;
						max_x_line = j;
						//break;
					}	
				}
			}
			
			strncpy(&str_line, str+i, max_x_line);
			KDebug_Alert(str_line);
			KLog_U1("max_x_line: ", max_x_line);
			KLog_U1("str_line_len: ", str_line_len);
			KLog_U1("i: ", i);
		}
		
		

		//if skip, then disabling text appear animation
		if (!skip) {	
			waitMs(20);
			SYS_doVBlankProcess();
		}
		prev_joy_pressed = joy_pressed;

		shakeAnimation(25);
		cur_x++;
		
	}
	prev_joy_pressed = TRUE;
	skip = FALSE;
	//waiting for last press
	while(1) {
		joy_value = JOY_readJoypad(JOY_1);
		if (joy_value & BUTTON_A || joy_value & BUTTON_B || joy_value & BUTTON_C || joy_value & BUTTON_START)  {
			joy_pressed = TRUE;
			if(joy_pressed && !prev_joy_pressed) {
				//VDP_setWindowVPos(FALSE, 0); //deleting message from screen
				break;
			}
		} else {
			joy_pressed = FALSE;
		}
		prev_joy_pressed = joy_pressed;
		shakeAnimation(25);
		SYS_doVBlankProcess();	
	}
	if(characterSpr_def){
		SPR_releaseSprite(SpriteCharacter);
	}
	VDP_setWindowVPos(FALSE, 0);
}

void transFadeIn(){
	PAL_getColors(0, &palette_full, 64);
	PAL_fadeOutAll(60, FALSE);
	PAL_setPalette(0,palette_black, DMA);
	PAL_setPalette(1,palette_black, DMA);
	PAL_setPalette(2,palette_black, DMA);
	PAL_setPalette(3,palette_black, DMA);
}

void transFadeOut(){
	PAL_fadeInAll(&palette_full, 60, FALSE);
}

void updateImage(Image* bg1, Image* bg2, u8 trandition_id, u8 transition_time){
	tile_id = tile_id_init;
	
	if(trandition_id){
		if(trandition_id == 1){
			PAL_getColors(0, &palette_full[0], 64);
			if(bg1) memcpy(&palette_full[16],  bg1->palette->data, 16 * 2);
			if(bg2) memcpy(&palette_full[0],  bg2->palette->data, 16 * 2);

			PAL_fadeOutAll(transition_time, FALSE);
			PAL_setPalette(0,palette_black, DMA);
			PAL_setPalette(1,palette_black, DMA);
			PAL_setPalette(2,palette_black, DMA);
			PAL_setPalette(3,palette_black, DMA);
			
			
		}
	}
	if(bg2){
		if(!bg1){
			VDP_clearPlane(BG_A, TRUE);
		}
		//VDP_clearPlane(BG_B, TRUE);
		VDP_drawImageEx(BG_B, bg2, TILE_ATTR_FULL(PAL0, 255, FALSE, FALSE, tile_id), 0, 0, FALSE, TRUE);
		tile_id += bg2->tileset->numTile;
		bgA_tile_id = tile_id;
		if(!trandition_id) PAL_setPalette(PAL0, bg2->palette->data, DMA);
		
	}
	if(bg1){
		VDP_clearPlane(BG_A, TRUE);
		VDP_drawImageEx(BG_A, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18, FALSE, TRUE);
		if(!bg2){
			tile_id += bgA_tile_id;
		}
		
		if(!trandition_id) PAL_setPalette(PAL1, bg1->palette->data, DMA);
		VDP_drawImageEx(BG_A, bg1, TILE_ATTR_FULL(PAL1, 255, FALSE, FALSE, tile_id), 0, 0, FALSE, TRUE);
		tile_id += bg1->tileset->numTile;
	}
	
	if(trandition_id){
		if(trandition_id == 1){
			PAL_fadeInAll(&palette_full, transition_time, FALSE);
		}
		waitMs(10);
	}
	SYS_doVBlankProcess();
	
}

void shakeAnimation(u16 spd){
	if(shake_x_radius) {
		curBgA_posX = fix16Mul(sinFix16(shake_timer), shake_x_radius);
		VDP_setHorizontalScroll(BG_A, curBgA_posX);
		shake_timer += spd;
	}
}

void moveBGA_ToPos(s16 toPosX, u8 spd){
	VDP_setWindowVPos(TRUE, 20);
	VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 18, FALSE, TRUE);
	
	s16 destPosX_rel = toPosX - curBgA_posX;
	if(destPosX_rel == 0){
		return;
	}
	if(destPosX_rel > 0){
		while(curBgA_posX < toPosX) {
			VDP_setHorizontalScroll(BG_A, curBgA_posX);
			SYS_doVBlankProcess();
			curBgA_posX+=spd;
		}
	} else {
		while(curBgA_posX > toPosX) {
			VDP_setHorizontalScroll(BG_A, curBgA_posX);
			SYS_doVBlankProcess();
			curBgA_posX-=spd;
		}
	}
	
}

void start_menu(){
	XGM_startPlay(blow_with_the_fires);
	//VDP_resetScreen();
	while(1) {
		
		u8 answ_id = 0;
		PAL_setPalette(PAL0, mainmenu_ground.palette->data, DMA);
		//VDP_drawImageEx(BG_B, &logo, TILE_ATTR_FULL(PAL0, 255, FALSE, FALSE, curTileInd), 0, 0, FALSE, TRUE);
		updateImage(NULL, &mainmenu_ground, NULL, NULL);
		strArr[0].str = "Jdqas]";
		strArr[1].str = "Jnuo";
		answ_id = chooseDialog(NULL, strArr, 2, NULL);
		if(answ_id == 0) {
			VDP_clearPlane(BG_B, TRUE);
			prologue();
			XGM_startPlay(blow_with_the_fires);
		} else if(answ_id == 1) {
			printMessage_BOX("Reflal lfnkoejdjsfkrfq.\n Jrpol]iocalr@ rous: RDEK, arfpqas, mjejcdm, uosoyopjk.\n Kaqsjnkj: aqrfnjVR.\n Pfqronahj: RoqaRoqa", 2, 2, 35, 23, NULL);
		} else if(answ_id == 2) {
			printMessage_BOX("Reflal lfnkoejdjsfkrfq.\n Jrpol]iocalr@ rous: RDEK, arfpqas, mjejcdm, uosoyopjk.\n Kaqsjnkj: aqrfnjVR.\n Pfqronahj: RoqaRoqa", 2, 2, 35, 23, NULL);
		}
	}
	
		
}


void prologue() {
	XGM_setLoopNumber(-1);
	XGM_startPlay(&a_promise_from_distant_days_v2);
	u8 answ_id = 0;
	bool renaHelp1 = FALSE;

	updateImage(NULL, &ext_camp_entrance_night, NULL, NULL);
	printMessage("Mnf op@s] rnjlr@ ron.", NULL, NULL, NULL);
	printMessage("^sos ron...", NULL, NULL, NULL);
	printMessage("Kahet& nox] oeno j so hf.", NULL, NULL, NULL);
	printMessage("No natsqo, kak ob\\xno, crg iabtefsr@.", NULL, NULL, NULL);
	printMessage("Mohfs b\\s], ono j k ltxyfmt...", NULL, NULL, NULL);
	printMessage("Orsantsr@ sol]ko stmann\\f corpomjnanj@ o pqjoskqрs\\v, rlocno pqjdlaya&zjv ktea-so coqosav, q@eom r kosoqтmj c kamnf iarsзlj eca pjonfqa.", NULL, NULL, NULL);
	printMessage("A fzg rsqanna@ efcoxka... kosoqa@ porso@nno rpqayjcafs:", NULL, NULL, NULL);


	printMessageName("ТS\\ pojegy] ro mnoj?", "Efcoxka", NULL, NULL, NULL);



	
}

int main()
{
	tile_id_init = curTileInd;
	sumimasen_screen();
	
	tile_id_MsgSign = 1249-message_sign.tileset->numTile;
	tile_id_AnswerSign = tile_id_MsgSign-question_sign.tileset->numTile;
	//VDP_loadTileSet(&font_custom, TILE_FONTINDEX, DMA);
	VDP_setPlaneSize(128, 32, TRUE);
	PAL_setPalette(PAL3, message_sign.palette->data, CPU);
	VDP_setTextPalette(PAL3);
	tile_id = curTileInd;
	
	VDP_loadFont(&font_custom_ru, CPU);

	
	start_menu();
	
    return (0);
}
