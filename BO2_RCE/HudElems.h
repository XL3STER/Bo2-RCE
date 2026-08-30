
enum he_type_t {
	HE_TYPE_FREE = 0x0,
	HE_TYPE_TEXT = 0x1,
	HE_TYPE_VALUE = 0x2,
	HE_TYPE_PLAYERNAME = 0x3,
	HE_TYPE_MAPNAME = 0x4,
	HE_TYPE_GAMETYPE = 0x5,
	HE_TYPE_DAMAGE_INDICATOR = 0x6,
	HE_TYPE_SCORE = 0x7,
	HE_TYPE_MATERIAL = 0x8,
	HE_TYPE_TIMER_DOWN = 0x9,
	HE_TYPE_TIMER_UP = 0xA,
	HE_TYPE_TENTHS_TIMER_DOWN = 0xB,
	HE_TYPE_TENTHS_TIMER_UP = 0xC,
	HE_TYPE_CLOCK_DOWN = 0xD,
	HE_TYPE_CLOCK_UP = 0xE,
	HE_TYPE_WAYPOINT = 0xF,
	HE_TYPE_PERKS = 0x10,
	HE_TYPE_COUNT = 0x11,
};


struct color_t
{
	unsigned char r, g, b, a;
};

struct hudelem_s {
	float x; //0
	float y; //4
	float z; //8
	float fontSize; //c
	float fromFontScale; //10
	int fontScaleStartTime; //14
	color_t color; //18
	color_t fromColor; //1c
	int fadeStartTime; //20
	int scaleStartTime; //24
	float fromX; //28
	float fromY; //2c
	int moveStartTime; //30
	int time;
	int duration;
	float value;
	float sort;
	color_t glowColor;
	int fxBirthTime;
	int flags;
	short targetEntNum;
	short fontScaleTime;
	short fadeTime;
	short label;
	short width;
	short height;
	short fromWidth;
	short fromHeight;
	short scaleTime;
	short moveTime;
	short text;
	unsigned short fxLetterTime;
	unsigned short fxDecayStartTime;
	unsigned short fxDecayDuration;
	unsigned short fxRedactDecayStartTime;
	unsigned short fxRedactDecayDuration;
	char type; //0x70
	char Font;
	char alignOrg;
	char alignScreen;
	char materialIndex;
	char offscreenMaterialIndex;
	char fromAlignOrg;
	char fromAlignScreen;
	char soundID;
	char ui3dWindow;
	short flag2;
};

struct game_hudelem_s {
	hudelem_s elem;
	int clientNum;
	int teamNum;
	int archived;
};


int GetMaterialIndex(const char* Material);
int GetLocalizedStringIndex(const char* String);
int GetAlloc_HudElemen(int clientNum, int teamNum);
game_hudelem_s* setText(int clientNum, const char* text, int font, float fontScale, float x, float y, int allign, float sort, color_t color, color_t glowColor);
game_hudelem_s* setShader(int clientNum, const char* material, int width, int height, float x, float y, float sort, color_t color);