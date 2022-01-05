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
		VDP_drawImageEx(BG_A, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 20, FALSE, TRUE);
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



void printMessage(char* str, SpriteDefinition* characterSpr_def, u16 posX, u8 pcm_id) {
	if(pcm_id) {
		XGM_startPlayPCM(pcm_id, 15, SOUND_PCM_CH2);
	}
	
	VDP_setWindowVPos(TRUE, 20);
	VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 20, FALSE, TRUE);
	

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
							VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 20, FALSE, TRUE);
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
		VDP_drawImageEx(BG_A, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 20, FALSE, TRUE);
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
	VDP_drawImageEx(WINDOW, &message_sign, TILE_ATTR_FULL(PAL3, 255, FALSE, FALSE, tile_id_MsgSign), 0, 20, FALSE, TRUE);
	
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
	XGM_startPlay(mus_default);
	u8 answ_id = 0;
	bool renaHelp1 = FALSE;

	updateImage(NULL, &Train_Day, NULL, NULL);
	printMessage("Mnf op@s] rnjlr@ ron.", NULL, NULL, NULL);
	printMessage("^sos ron...", NULL, NULL, NULL);
	printMessage("Kahet& nox] oeno j so hf.", NULL, NULL, NULL);
	printMessage("No natsqo, kak ob\\xno, crg iabtefsr@.", NULL, NULL, NULL);
	printMessage("Mohfs b\\s], ono j k ltxyfmt...", NULL, NULL, NULL);
	printMessage("Orsantsr@ sol]ko stmann\\f corpomjnanj@ o pqjoskqрs\\v, rlocno pqjdlaya&zjv ktea-so coqosav, q@eom r kosoqтmj c kamnf iarsзlj eca pjonfqa.", NULL, NULL, NULL);
	printMessage("A fzg rsqanna@ efcoxka... kosoqa@ porso@nno rpqayjcafs:", NULL, NULL, NULL);


	printMessage("PQJCFS, MOG JM@ LFNA", NULL, NULL, NULL);
	updateImage(&PinkH_angry2, &Train_Day, NULL, NULL);
	shake_x_radius = FIX16(0.08);
	XGM_stopPlay();
	SYS_doVBlankProcess();
	XGM_startPlay(xgm_danger); 
	
	printMessage("Wait, what are you doing?", NULL, NULL, NULL);
	printMessage("You must give me that bag, if you don;t want to get a TROUBLES!", NULL, NULL, NULL);
	printMessage("Someone, please help!", NULL, NULL, NULL);
	printMessage("Nobody will help you, there is nothing to shout!", NULL, NULL, NULL);
	printMessage("...", NULL, NULL, NULL);
	printMessage("She's been screaming like that for 10 seconds now, why hasn't anyone helped yet? Should I help her?", NULL, NULL, NULL);
	strArr[0].str = "Yes";
	strArr[1].str = "No";
	answ_id = chooseDialog("Help her?" , strArr, 2, &PinkH_angry2);
	if(answ_id == 0){
		//TODO, change palette when new character says
		//VDP_setPaletteColor(49,RGB24_TO_VDPCOLOR(0xFFBADE));
		//VDP_setPaletteColor(55,RGB24_TO_VDPCOLOR(0x7F0037));
		printMessage("Get away from her. *took the hand*", NULL, NULL, NULL);
		printMessage("Hey, get off.", NULL, NULL, NULL);
		printMessage("Return what you stole, NOW! *Clenched his hand tightly*", NULL, NULL, NULL);
		printMessage("Aaaagh.... mph.. okay, you are crazy.", NULL, NULL, NULL);
		
		renaHelp1 = TRUE;
		
		XGM_startPlay(mus_default);
		updateImage(&PinkH_smile1, NULL, NULL, NULL);
		shake_x_radius = 0;
		printMessage("Thanks for helping, what's your name?", NULL, NULL, NULL);
		printMessage("My name is Korosu.", NULL, NULL, NULL);
		printMessage("Okay, I'll remember you.", NULL, NULL, NULL);
		moveBGA_ToPos(-300, 3);
	} else if(answ_id == 1){
		printMessage("Someone will clearly help her, the police for example. Am I some kind of superman to you or what? This is a real life, it's not a game.", NULL, NULL, NULL);
		XGM_startPlay(mus_default);
	}
	shake_x_radius = 0;
	//updateImage(NULL, NULL);
	printMessage("... After 20 minutes ...", NULL, NULL, NULL);
	VDP_clearPlane(BG_A, TRUE);
	VDP_setHorizontalScroll(BG_A, 0);
	printMessage("Finally I got there.", NULL, NULL, NULL);
	updateImage(&aiko_hana_talk, &school_01, 1, 15);
	printMessage("On the way, I saw two girls chatting sweetly about TV shows.", NULL, NULL, NULL);
	updateImage(NULL, &school_03, 1, 15);
	printMessage("If I am not mistaken, I was assigned to class 2-1.", NULL, NULL, NULL);
	updateImage(&PinkH_smile1, &school_04, 1, 15);
	printMessage("New student, please introduce yourself.", NULL, NULL, NULL);
	printMessage("!!!", NULL, NULL, NULL);
	printMessage("You, that girl from train!", NULL, NULL, NULL);
	updateImage(&PinkH_neutral, NULL, NULL, NULL);
	printMessage("Please introduce yourself, we'll talk about this later.", NULL, NULL, NULL);
	printMessage("Khm...", NULL, NULL, NULL);
	printMessage("Hello. My name is Korosu, I am 16 years old. I came from Japan, I will be glad to study with you. Please take care of me.", NULL, NULL, NULL);
	printMessage("Please take a seat over there.", NULL, NULL, NULL);
	printMessage("Before I had time to sit down, a pink-haired girl spoke to me.", NULL, NULL, NULL);
	
	XGM_startPlay(xgm_music);
	updateImage(&Rin_Casual_OpenSmile, NULL, NULL, NULL);
	printMessage("Hi, I'm Rin, let's get to know each other.", NULL, NULL, NULL);
	printMessage("Korosu, nice to meet you.", NULL, NULL, NULL);
	printMessage("Tell me what is it like to live in Japan? There is a lot of fun for the lover.", NULL, NULL, NULL);
	printMessage("Well, it's pretty ...", NULL, NULL, NULL);
	printMessage("Please don't talk in classroom.", NULL, NULL, NULL);
	updateImage(&Rin_Casual_Smile, NULL, NULL, NULL);
	printMessage("We'll talk later.", NULL, NULL, NULL);
	
	XGM_startPlay(mus_default);
	updateImage(&PinkH_neutral, NULL, NULL, NULL);
	printMessage("All introduced themselves, now it's my turn. My name is Rena, and as of today I am your homeroom teacher and also an English teacher.", NULL, NULL, NULL);
	printMessage("Later in the afternoon.", NULL, NULL, NULL);
	updateImage(NULL, &school_04, 1, 15);
	updateImage(&PinkH_neutral, NULL, NULL, NULL);
	printMessage("This is where we are done, do not forget to do your homework in time.", NULL, NULL, NULL);
	updateImage(NULL, &school_04, NULL, NULL);
	printMessage("Fuh, it was hard. Fortunately, now everything is over.", NULL, NULL, NULL);
	XGM_startPlay(xgm_music);
	updateImage(&Rin_Casual_Smile, NULL, NULL, NULL);
	printMessage("Korosu! Come with me, I'll show you something.", NULL, NULL, NULL);
	if(renaHelp1) {
		printMessage("?!?", NULL, NULL, NULL);
		printMessage("What to do, go to Rena or Rin?", NULL, NULL, NULL);
		strArr[0].str = "Rena";
		strArr[1].str = "Rin";
		answ_id = chooseDialog("Rena or Rin?" , strArr, 2, &Rin_Casual_Smile);
	} else {
		answ_id = 1;
	}
	if(answ_id == 0){
		printMessage("Sorry, I have things to do, you can show me later.", NULL, NULL, NULL);
	} else if(answ_id == 1) {
		printMessage("Come on, already.", NULL, NULL, NULL);
		printMessage("Rin pulled my hand. *", NULL, NULL, NULL);
		updateImage(NULL, &school_06, 1, 15);
		waitMs(120);
		updateImage(NULL, &school_05, 1, 15);
		waitMs(120);
		updateImage(&Rin_Casual_Smile, &school_09, 1, 15);
		waitMs(120);
		printMessage("So what did you want to show me?", NULL, NULL, NULL);
		printMessage("Nothing.", NULL, NULL, NULL);
		printMessage("Um .. what?", NULL, NULL, NULL);
		printMessage("I just really like you, so I decided to pick you up before the others had time. You are handsome, Korosu!", NULL, NULL, NULL);
		printMessage("Do girls really like the appearance of an ordinary Japanese schoolboy so much?", NULL, NULL, NULL);
		printMessage("*knock, knock*", NULL, NULL, NULL);
		updateImage(&Rin_Casual_Smile_Blush_Face, NULL, NULL, NULL);
		printMessage("Well, do you like me?", NULL, NULL, NULL);
		strArr[0].str = "Yes";
		strArr[1].str = "No";
		answ_id = chooseDialog("Would you like to date with Rin?" , strArr, 2, &Rin_Casual_Smile_Blush_Face);
		if(answ_id == 0){
			printMessage("Yes, you are an amazing girl. Let's be a couple.", NULL, NULL, NULL);
			printMessage("Yaaaaaayy!!!", NULL, NULL, NULL);
			printMessage("Rin jumped into my arms.", NULL, NULL, NULL);
			printMessage("I'm so glad.", NULL, NULL, NULL);
			printMessage("I'm, too glad *", NULL, NULL, NULL);
			printMessage("Let's go home together!", NULL, NULL, NULL);
			printMessage("Yes, let's go.", NULL, NULL, NULL);
			updateImage(NULL, &Modern_School1, 1, 15);
			waitMs(400);
			updateImage(NULL, &Modern_Street1, 1, 15);
			waitMs(400);
			updateImage(NULL, &Modern_Street_night, 1, 15);
			waitMs(400);
			
			printMessage("We talked to her on various topics. Time passed quickly, and here we are at Rin's house. *", NULL, NULL, NULL);
			printMessage("Do you want to come in?", NULL, NULL, NULL);
			printMessage("Can i?", NULL, NULL, NULL);
			printMessage("Of course, we're a couple now.", NULL, NULL, NULL);
			printMessage("I can't believe this is happening to me, everything is so sudden. My God, how happy I am! *", NULL, NULL, NULL);
			updateImage(&Rin_Casual_Smile, &Apartment_Exterior_Night, 1, 15);
			printMessage("We will go to my room.", NULL, NULL, NULL);
			updateImage(NULL, &Modern_Dormroom2_sumimasen, 1, 15);
			printMessage("...", NULL, NULL, NULL);
			
			XGM_startPlay(xgm_you_stupid);
			printMessage("Oh my god, no, it can't be. It is not true.", NULL, NULL, NULL);
			printMessage("This is reality, dear.", NULL, NULL, NULL);
			printMessage("You! I thought you were normal!", NULL, NULL, NULL);
			printMessage("You are too naive Korosu. Did you really think that SUMIMASEN TEAM's game could be normal? This is the whole point of SIMIMASEN TEAM, making stupid trash games. Yes, even the name of the team speaks for itself.", NULL, NULL, NULL);
			printMessage("And also, our main and only developer, so far, sitting on the toilet. He is currently developing the game 'Wonderful date'.", NULL, NULL, NULL);
			printMessage("Okay, now to the main thing.", NULL, NULL, NULL);
			
			printMessage("That's not all?", NULL, NULL, NULL);

			updateImage(&terryAnim1, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim2, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim3, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim4, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim5, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim6, NULL, NULL, NULL);
			waitMs(500);
			updateImage(&terryAnim7, NULL, NULL, NULL);
			waitMs(500);
			
			printMessage("Oh god, you're a trap, and Terry in the same time. How it's can be possible?", NULL, NULL, NULL);
			printMessage("Yes, that's right. And now to the main thing.", NULL, NULL, NULL);
			XGM_setPCM(100, s_assmine, sizeof(s_assmine));
			printMessage("Nooooooooooooooooooooo", NULL, NULL, 100);
			printMessage("Ending: caught in a Terry Trap.", NULL, NULL, NULL);
			return;
		} else if(answ_id == 1) {
			printMessage("It's so sudden, I'm sorry I can't start dating you. I do not even know you.", NULL, NULL, NULL);
			updateImage(&Rin_Casual_Smile, NULL, NULL, NULL);
			printMessage("Okay, okay .. Then, let's get to know each other better. We will walk together, communicate. And then I hope you can love me.", NULL, NULL, NULL);
			printMessage("Rin is a sweet girl, but I don't know her, so I turned it down. I hope I made the right choice. *", NULL, NULL, NULL);
		}
	}
	
	if(renaHelp1){
		XGM_startPlay(mus_default);
		updateImage(&PinkH_neutral, &school_04, NULL, NULL);
		printMessage("Come with me.", NULL, NULL, NULL);
		updateImage(&PinkH_neutral, &school_05, 1, 15);
		printMessage("Listen. I am grateful to you that you helped me. But please, keep it a secret. Classmates should not know about this, it damages my business reputation. Nevertheless, I will not remain in debt.", NULL, NULL, NULL);
		updateImage(&PinkH_smile3, NULL, NULL, NULL);
		printMessage("Here, take my phone number, if you need anything, I'll help you.", NULL, NULL, NULL);
		printMessage("Okay, I understand you.", NULL, NULL, NULL);
		updateImage(&PinkH_smile1, NULL, NULL, NULL);
		printMessage("Good.", NULL, NULL, NULL);
	}
	updateImage(NULL, &canteen, NULL, NULL);
	printMessage("Well, next one is the cafeteria.", NULL, NULL, NULL);
	printMessage("I ordered a combo lunch. It tastes good.", NULL, NULL, NULL);
	printMessage("After a while, a girl that I saw at the entrance of school came up to me.", NULL, NULL, NULL);
	updateImage(&Aiko_Shout, NULL, NULL, NULL);
	printMessage("Hi. My name is Aiko, nice to meet you.", NULL, NULL, NULL);
	printMessage("Korosu, same to you.", NULL, NULL, NULL);
	printMessage("Listen Korosu, do you do any sport?", NULL, NULL, NULL);
	printMessage("Not really, although I keep myself in shape.", NULL, NULL, NULL);
	printMessage("Do you like basketball?", NULL, NULL, NULL);
	printMessage("Yes, I like it.", NULL, NULL, NULL);
	printMessage("Okay, then come to our additional activities. We look forward to your company.", NULL, NULL, NULL);
	updateImage(&Aiko_Smile, NULL, NULL, NULL);
	printMessage("Are there many participants there?", NULL, NULL, NULL);
	updateImage(&Aiko_Shout_Side, NULL, NULL, NULL);
	printMessage("Well, a lot ...", NULL, NULL, NULL);
	updateImage(&Aiko_Shout, NULL, NULL, NULL);
	printMessage("In any case, come to us, I think you will like it. Activities will start tomorrow, so, don't miss it. Bye Bye.", NULL, NULL, NULL);
	updateImage(NULL, &canteen, NULL, NULL);
	printMessage("I finished my lunch and went home.", NULL, NULL, NULL);
	updateImage(NULL, &Modern_Street_night, 1, 15);
	waitMs(500);
	updateImage(NULL, &Small_Apartment_Kitchen_Night, 1, 15);
	printMessage("What a big day it was today. I can't wait to know what will happen tomorrow.", NULL, NULL, NULL);
	printMessage("End of the Demo: Created by bolon667 from SUMIMASEN TEAM", NULL, NULL, NULL);
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
