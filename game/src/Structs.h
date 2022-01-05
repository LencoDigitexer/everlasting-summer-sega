typedef struct {
	f32 x;
	f32 y;
} fixPoint;

typedef struct {
	s16 x;
	s16 y;
} intPoint;

typedef struct {
	bool alive;
	intPoint firstPos;
	intPoint lastPos;
	u8 trigger_type;
	u8 trigger_value;
} Trigger;

typedef struct {
	SpriteDefinition* sprDef;
	char* str;
	u8 pcmId;
	u16 posX;
} Message;

typedef struct {
	u8 size;
	u8* MessageArr;
} MessagePack;

typedef struct {
	char* str;
} String;

typedef struct {
	bool moving;
	bool is_on_floor;
	fixPoint pos;
	fixPoint spd;
	intPoint size;
} Player;

typedef struct {
	MapDefinition* foregroundMap;
	MapDefinition* backgroundMap;
	TileSet* foregroundTileset;
	TileSet* backgroundTileset;
	Palette* foregroundPallete;
	Palette* backgroundPallete;
	intPoint startPos;
	u8* collArr;
	intPoint sizeinPx;
	intPoint sizeinTiles;
} Level;