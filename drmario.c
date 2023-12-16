//*******************************************************
//*                 DRMARIO92+ by RØLy!                 *
//*-----------------------------------------------------*
//*                version :  1.0 Beta (Revision 0)     *
//*                created :  16/12/2001                *
//*            last update :  10/01/2002                *
//*-----------------------------------------------------*
//*        [ www.roly.fr.st ] -=- [ roly@fr.st ]        *
//*                                                     *
//*                     HAVE FUN !!!                    *
//*******************************************************

#define OPTIMIZE_ROM_CALLS
#define SAVE_SCREEN
#include <tigcclib.h>

short _ti92plus;

//Enumerations
enum T_SENS {HORIZONTAL=0, VERTICAL=1};
enum T_ROTATION {CLOCK=0, COUNTER=1};
enum T_COLOR {BLACK=0, GREY=1, WHITE=2, TRANSPARENT=3};
enum T_KEYS {DOWN=0, UP=1, LEFT=2, RIGHT=3, ESC=4, MODE=5};

//Environnement de jeu
enum T_BLOCK {BLANK=0, VIRUS=1, PILL=2, PILL_L=3, PILL_U=4, PILL_R=5, PILL_D=6};
struct T_ITEM {
	enum T_BLOCK Type;
	enum T_COLOR Col;
};

//écran virtuel
unsigned char* virtual1=0;
unsigned char* virtual2=0;

//variables de jeu
static unsigned char x, y, level, viruses[3];
static unsigned long int score;
static enum T_SENS sens;
static enum T_COLOR col1, col2, next_col1, next_col2;
static enum Bool game_over, stage_cleared, quit, in_game=FALSE;
static struct T_ITEM table[8][15];
  
//gestion du timer et des animations  
static unsigned char timer_state, anim_frame;
const char timer_fall=5;
  
//tableau des records
static unsigned long int best_score[3] = {0, 0, 0};
static unsigned char best_player[3][15] = {"Dr. Mario 92+","by RØLy!","roly@fr.st"};

//parametres
static enum Bool autosave=TRUE, gridback=TRUE;
static unsigned char backqual=2, viranim=2, speed=3;


/////////////////////////////////DEFINITION DES SPRITES/////////////////////////////////////		

//Sprites pillules (4 direction + 1 unaire) (3 couleurs + 1 masque commun)
static unsigned char pill_mask[] ={ 0xFF, 0xC1, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC1};		//block
static unsigned char pill_0[] ={ 0x0, 0x3E, 0x7B, 0x7D, 0x7D, 0x7D, 0x7F, 0x3E};					//noir
static unsigned char pill_1[] ={ 0x0, 0x3E, 0x6B, 0x55, 0x6B, 0x55, 0x6B, 0x3E};					//gris
static unsigned char pill_2[] ={ 0x0, 0x3E, 0x63, 0x41, 0x41, 0x41, 0x63, 0x3E};					//noir
static unsigned char pill_L_mask[] ={ 0xFF, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0};	//gauche
static unsigned char pill_L0[] ={ 0x0, 0x3F, 0x60, 0x5F, 0x7F, 0x7F, 0x7F, 0x3F};					//noir
static unsigned char pill_L1[] ={ 0x0, 0x3F, 0x6A, 0x55, 0x6A, 0x55, 0x6A, 0x3F};					//gris
static unsigned char pill_L2[] ={ 0x0, 0x3F, 0x60, 0x40, 0x40, 0x40, 0x60, 0x3F};					//blanc
static unsigned char pill_U_mask[] ={ 0xFF, 0xC1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};	//dessus
static unsigned char pill_U0[] ={ 0x0, 0x3E, 0x7B, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D};					//noir
static unsigned char pill_U1[] ={ 0x0, 0x3E, 0x6B, 0x55, 0x6B, 0x55, 0x6B, 0x55};					//gris
static unsigned char pill_U2[] ={ 0x0, 0x3E, 0x63, 0x41, 0x41, 0x41, 0x41, 0x41};					//blanc
static unsigned char pill_R_mask[] ={ 0xFF, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1};					//droite
static unsigned char pill_R0[] ={ 0x0, 0xFE, 0x83, 0xFD, 0xFF, 0xFF, 0xFF, 0xFE};					//noir
static unsigned char pill_R1[] ={ 0x0, 0xFE, 0xAB, 0xD5, 0xAB, 0xD5, 0xAB, 0xFE};					//gris
static unsigned char pill_R2[] ={ 0x0, 0xFE, 0x83, 0x81, 0x81, 0x81, 0x83, 0xFE};					//blanc
static unsigned char pill_D_mask[] ={ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC1};	//dessous
static unsigned char pill_D0[] ={ 0x7F, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7B, 0x3E};				//noir
static unsigned char pill_D1[] ={ 0x7F, 0x55, 0x6B, 0x55, 0x6B, 0x55, 0x6B, 0x3E};				//gris
static unsigned char pill_D2[] ={ 0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x63, 0x3E};				//blanc

//Sprites petits virus (2 frames avec masque)
static unsigned char virus_A0[] ={ 0x0, 0x22, 0x3E, 0x7F, 0x7F, 0x3A, 0x45, 0x3E};			//virus noir
static unsigned char virus_A0_mask[] ={ 0xFF, 0xDD, 0xC1, 0x80, 0x80, 0x80, 0x80, 0xC1};
static unsigned char virus_B0[] ={ 0x0, 0x14, 0x3E, 0x7F, 0x7F, 0x26, 0x59, 0x3E};
static unsigned char virus_B0_mask[] ={ 0xFF, 0xEB, 0xC1, 0x80, 0x80, 0x80, 0x80, 0xC1};
static unsigned char virus_A1[] ={ 0x0, 0x77, 0x3E, 0x6B, 0x55, 0x6B, 0x55, 0x3E};			//virus gris
static unsigned char virus_A1_mask[] ={ 0xFF, 0x88, 0xC1, 0x80, 0x80, 0x80, 0x80, 0xC1};
static unsigned char virus_B1[] ={ 0x0, 0x0, 0x14, 0x3E, 0x6B, 0x55, 0x6B, 0x3E};
static unsigned char virus_B1_mask[] ={ 0xFF, 0xFF, 0xEB, 0xC1, 0x80, 0x80, 0x80, 0xC1};
static unsigned char virus_A2[] ={ 0x0, 0x2A, 0x36, 0x41, 0x41, 0x41, 0x41, 0x2A};			//virus blanc
static unsigned char virus_A2_mask[] ={ 0xFF, 0xD5, 0xC1, 0x80, 0x80, 0x80, 0x80, 0xD5};
static unsigned char virus_B2[] ={ 0x0, 0x49, 0x36, 0x22, 0x22, 0x22, 0x22, 0x14};
static unsigned char virus_B2_mask[] ={ 0xFF, 0xB6, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xEB};

//Sprites gros virus animés (noir, gris, blancs) (3 frames) (2 planes)
static unsigned short bigvirus_A0_light[] ={ 0x0, 0x2004, 0x324C, 0x3FFC, 0xDC3B, 0xD00B, 0x324C, 0x799E, 0x7FFE, 0x7FFE, 0x6006, 0x3FFC, 0x7FFE, 0x4FF2, 0x6006, 0xE007};
static unsigned short bigvirus_A0_dark[] ={ 0x0, 0x2004, 0x324C, 0x3FFC, 0xDC3B, 0xD00B, 0x324C, 0x799E, 0x7FFE, 0x7FFE, 0x6606, 0x3FFC, 0x7FFE, 0x4FF2, 0x6006, 0xE007};
static unsigned short bigvirus_B0_light[] ={ 0x10, 0xD030, 0xDFF0, 0x70F8, 0x2038, 0x493B, 0x667B, 0x7FFC, 0x400C, 0x600C, 0x201C, 0x3038, 0x18F6, 0x2FC7, 0x6000, 0xE000};
static unsigned short bigvirus_B0_dark[] ={ 0x10, 0xD530, 0xDFF0, 0x70F8, 0x2038, 0x493B, 0x667B, 0x7FFC, 0x4C0C, 0x6C0C, 0x201C, 0x3038, 0x18F6, 0x2FC7, 0x6000, 0xE000};
static unsigned short bigvirus_C0_light[] ={ 0x800, 0xC0B, 0xFFB, 0x1F0E, 0x1C04, 0xDC92, 0xDE66, 0x3FFE, 0x3002, 0x3806, 0x3804, 0x1C0C, 0x6F18, 0xE3F4, 0x6, 0x7};
static unsigned short bigvirus_C0_dark[] ={ 0x800, 0xCAB, 0xFFB, 0x1F0E, 0x1C04, 0xDC92, 0xDE66, 0x3FFE, 0x3182, 0x3986, 0x3804, 0x1C0C, 0x6F18, 0xE3F4, 0x6, 0x7};
static unsigned short bigvirus_A1_light[] ={ 0x0, 0x0, 0x3838, 0x7EFC, 0xE7CE, 0x8102, 0x280, 0x0, 0x100, 0x3FF8, 0x440, 0x7C0, 0x0, 0x0, 0x5014, 0xF01E};
static unsigned short bigvirus_A1_dark[] ={ 0x0, 0x0, 0x3838, 0x7EFC, 0xF7DE, 0xA10A, 0x4284, 0x4004, 0x6AAC, 0x7FFC, 0x5454, 0x2FE8, 0x1550, 0x2AA8, 0x701C, 0xF01E};
static unsigned short bigvirus_B1_light[] ={ 0x3C00, 0x7E38, 0x8E7C, 0x23CE, 0x4102, 0x280, 0x0, 0x100, 0x3FF8, 0x442, 0x7C6, 0x2, 0x6, 0x0, 0x5000, 0xF000};
static unsigned short bigvirus_B1_dark[] ={ 0x3C00, 0x7E38, 0x8E7C, 0x37DE, 0x610A, 0x4284, 0x4004, 0x6AAC, 0x7FFC, 0x5452, 0x2FEE, 0x155A, 0x2AB6, 0x2000, 0x5000, 0xF000};
static unsigned short bigvirus_C1_light[] ={ 0x78, 0x38FC, 0x7CE2, 0xE788, 0x8104, 0x280, 0x0, 0x100, 0x3FF8, 0x8440, 0xC7C0, 0x8000, 0xC000, 0x0, 0x14, 0x1E};
static unsigned short bigvirus_C1_dark[] ={ 0x78, 0x38FC, 0x7CE2, 0xF7D8, 0xA10C, 0x4284, 0x4004, 0x6AAC, 0x7FFC, 0x9454, 0xEFE8, 0xB550, 0xDAA8, 0x8, 0x14, 0x1E};
static unsigned short bigvirus_A2_light[] ={ 0x6246, 0x75AE, 0x381C, 0x1008, 0x17F8, 0xDDBB, 0xF3C7, 0x4422, 0x4002, 0x5FFA, 0x2004, 0x1008, 0xFF0, 0x1C38, 0x3E7C, 0x3E7C};
static unsigned short bigvirus_A2_dark[] ={ 0x6246, 0x57EA, 0x2814, 0x1008, 0x17F8, 0xDDBB, 0xB3C5, 0x4422, 0x4002, 0x5FFA, 0xE007, 0xD00B, 0xFF0, 0x1428, 0x2244, 0x3E7C};
static unsigned short bigvirus_B2_light[] ={ 0x0, 0x0, 0x6246, 0x75AE, 0x381C, 0x1008, 0x17E8, 0x3DBC, 0x43C2, 0x8001, 0xB81D, 0x47E2, 0x300C, 0x1FF8, 0x2E7C, 0x3E7C};
static unsigned short bigvirus_B2_dark[] ={ 0x0, 0x0, 0x6246, 0x57EA, 0x2814, 0x1008, 0x17E8, 0x3DBC, 0x43C2, 0x8001, 0xB81D, 0x47E2, 0xF00F, 0xDFFB, 0x3244, 0x3E7C};
static unsigned short bigvirus_C2_light[] ={ 0xC7E3, 0xE817, 0x700E, 0x37EC, 0xDDBB, 0xF3CF, 0x6426, 0x2004, 0x2FF4, 0x1008, 0x1008, 0xC30, 0x1FF8, 0x1C38, 0x3E7C, 0x3E7C};
static unsigned short bigvirus_C2_dark[] ={ 0xC5A3, 0xA815, 0x500A, 0x37EC, 0xDDBB, 0xB3CD, 0x6426, 0x2004, 0xEFF7, 0xD00B, 0x1008, 0xC30, 0x17E8, 0x1428, 0x2244, 0x3E7C};

//Sprites gros mario (2 planes)
static unsigned long bigmario_A_light[] ={ 0x00C00000, 0x013F8000, 0x029FC000, 0x03302000, 0x01FFE000, 0x0127F000, 0x0173F800, 0x0173F800, 0x01FFF800, 0x03FFF800, 0x03FFF800, 0x01FFF000, 0x00FFE000, 0x017F9000, 0x3E3E0800, 0x78AA0800, 0x791C4400, 0x79104400, 0x0FD3C400, 0x0110FC00, 0x0090FC00, 0x00887C00, 0x007FF800, 0x00242000, 0x0074E000, 0x008F1000, 0x01040800, 0x01FFF800};
static unsigned long bigmario_A_dark[] ={ 0x00C00000, 0x013F8000, 0x021FC000, 0x03302000, 0x01FFE000, 0x0104F000, 0x0150C800, 0x0151A800, 0x01002800, 0x020C4800, 0x021F3800, 0x01FE3000, 0x00B06000, 0x01419000, 0x3E3E0800, 0x68AA0800, 0x491C4400, 0x79104400, 0x0FD3C400, 0x0110FC00, 0x00908400, 0x00884400, 0x007FF800, 0x00242000, 0x0074E000, 0x00FFF000, 0x01FFF800, 0x01FFF800};
static unsigned long bigmario_D_light[] ={ 0x001E0000, 0x00798000, 0x00F0C000, 0x0119A000, 0x01FFE000, 0x03C9F000, 0x079CF800, 0x079CF800, 0x07FFF800, 0x07FFF800, 0x03FFF000, 0x03FFF000, 0x11FFE200, 0x7AFFD7C0, 0xFD7F0FE0, 0xF15D13C0, 0x222A0900, 0x121C0E00, 0x0EEBC800, 0x02081800, 0x02081000, 0x01082000, 0x00FFC000, 0x00444000, 0x00E5C000, 0x011E2000, 0x02041000, 0x03FFF000};
static unsigned long bigmario_D_dark[] ={ 0x001E0000, 0x00798000, 0x00F2C000, 0x0119A000, 0x01FFE000, 0x03417000, 0x05144800, 0x0594C800, 0x04000800, 0x05412800, 0x02E39000, 0x027F1000, 0x11002200, 0x6AC1D5C0, 0x8D7F0C20, 0xF15D13C0, 0x222A0900, 0x121C0E00, 0x0EEBC800, 0x02081800, 0x02081000, 0x01082000, 0x00FFC000, 0x00444000, 0x00E5C000, 0x01FFE000, 0x03FFF000, 0x03FFF000};
//Sprites: animation main de mario pour le jeter de pillule (3 frames) (2 planes)
static unsigned char mario_A_light[] ={ 0x0, 0x1, 0x2, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0x3, 0x1, 0x0, 0x1, 0x3E, 0x78, 0x79, 0x79, 0xF};
static unsigned char mario_A_dark[] ={ 0x0, 0x1, 0x2, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x1, 0x0, 0x1, 0x3E, 0x68, 0x49, 0x79, 0xF};
static unsigned char mario_B_light[] ={ 0x0, 0x1, 0x2, 0x3, 0x1, 0x1, 0x69, 0xFD, 0xFD, 0xFF, 0x73, 0x21, 0x10, 0xC, 0x2, 0x1, 0x1, 0x1, 0x1};
static unsigned char mario_B_dark[] ={ 0x0, 0x1, 0x2, 0x3, 0x1, 0x1, 0x69, 0x95, 0x85, 0x8E, 0x52, 0x21, 0x10, 0xC, 0x2, 0x1, 0x1, 0x1, 0x1};
static unsigned char mario_C_light[] ={ 0x0, 0x1, 0x2, 0x3, 0x3, 0x7, 0x7, 0x7, 0x5, 0x7, 0x7, 0x5, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1};
static unsigned char mario_C_dark[] ={ 0x0, 0x1, 0x2, 0x3, 0x3, 0x5, 0x5, 0x7, 0x5, 0x6, 0x6, 0x5, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1};

//Sprites additionnels pour la grille
static unsigned char pill_dis[] ={ 0x0, 0x2A, 0x41, 0x0, 0x41, 0x0, 0x41, 0x2A};		//pillule en train de disparaitre
static unsigned char virus_dis[] ={ 0x0, 0x8, 0x2A, 0x1C, 0x77, 0x1C, 0x2A, 0x8};		//virus en train de disparaitre
static unsigned char spr_blank[] ={ 0x80, 0x2, 0x8, 0x20, 0x80, 0x2, 0x8, 0x20};		//case vide (fond points clairs)


//matrice-pointeur vers les sprites de type pill
static unsigned char *pill_sprite[5][4]={						// pill_sprite[T_BLOCK-2][T_COLOR]
	{pill_0 , pill_1 , pill_2 , pill_mask  },
	{pill_L0, pill_L1, pill_L2, pill_L_mask},
	{pill_U0, pill_U1, pill_U2, pill_U_mask},
	{pill_R0, pill_R1, pill_R2, pill_R_mask},
	{pill_D0, pill_D1, pill_D2, pill_D_mask}};

//matrice-pointeur vers les sprites de type viru
static unsigned char *virus_sprite[2][3][2]={				// virus_sprite[ANIM_STATE][T_COLOR][MASK]
	{{virus_A0, virus_A0_mask}, {virus_A1, virus_A1_mask}, {virus_A2, virus_A2_mask}},
	{{virus_B0, virus_B0_mask}, {virus_B1, virus_B1_mask}, {virus_B2, virus_B2_mask}}};

//matrice-pointeur vers les sprites de type bigvirus
static unsigned short *bigvirus_sprite[3][3][2]={		// bigvirus_sprite[ANIM_STATE][T_COLOR][PLANE]
	{{bigvirus_A1_light, bigvirus_A1_dark}, {bigvirus_A0_light, bigvirus_A0_dark}, {bigvirus_A2_light, bigvirus_A2_dark}},
	{{bigvirus_B1_light, bigvirus_B1_dark}, {bigvirus_B0_light, bigvirus_B0_dark}, {bigvirus_B2_light, bigvirus_B2_dark}},
	{{bigvirus_C1_light, bigvirus_C1_dark}, {bigvirus_C0_light, bigvirus_C0_dark}, {bigvirus_C2_light, bigvirus_C2_dark}}};

//matrice-pointeur vers les sprites de type mario hand
static unsigned char *mario_sprite[3][2]						// mario_sprite[ANIM_STATE][PLANE]
	={{mario_A_dark, mario_A_light}, {mario_B_dark, mario_B_light}, {mario_C_dark, mario_C_light}};



/////////////////////////////////FONCTIONS GRAPHIQUES/////////////////////////////////////		


//=====Panel==> Remplit une zone rectangulaire blanche (2 planes) aux bords arrondis et en relief
	void Panel(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
	{
		unsigned char i;
  	PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		for(i=y1;i<=y2;i++) DrawLine (x1, i, x2, i, A_REVERSE);
		DrawLine (x1+1, y2, x2-1, y2, A_NORMAL);
		DrawLine (x2, y1+1, x2, y2-1, A_NORMAL);
	  PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		for(i=y1;i<=y2;i++) DrawLine (x1, i, x2, i, A_REVERSE);
		DrawLine (x1+1, y1, x2-1, y1, A_NORMAL);
		DrawLine (x1, y1+1, x1, y2-1, A_NORMAL);
	}


//=====Rectangle==> Trace un rectangle sur la plane en cours
	void Rectangle (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
	{
		DrawLine (x1, y1, x2, y1, A_NORMAL);		
		DrawLine (x2, y1, x2, y2, A_NORMAL);		
		DrawLine (x2, y2, x1, y2, A_NORMAL);		
		DrawLine (x1, y2, x1, y1, A_NORMAL);		
	}


//=====Filled_Rectangle==> Remplit une zone rectangulaire de couleur col
	void Filled_Rectangle(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char col)
	{
		unsigned char i;
  	PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		for(i=y1;i<=y2;i++) DrawLine (x1, i, x2, i, col<2);
	  PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		for(i=y1;i<=y2;i++) DrawLine (x1, i, x2, i, col%2==0);
	}


//=====Background==> Rempli le fond d'un écran en gris clair
	void Background(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
	{
		if (!backqual) return;
		unsigned char x ,y;
		//virtual1=malloc(3840);
		memset(virtual1,0,LCD_SIZE);
		PortSet (virtual1, LCD_WIDTH-1, LCD_HEIGHT-1);
		//PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		if (backqual==2) for(y=y1;y<y2+1;y+=2) for(x=x1;x<x2+1;x+=2)	DrawLine(x,y,x+1,y+1,A_NORMAL);
  	else if (backqual==1) for(x=x1;x<x2+1;x+=2)	DrawLine(x,y1,x,y2,A_SHADE_PS);
		memcpy (GetPlane(LIGHT_PLANE), virtual1, LCD_SIZE);
		//free(virtual1);
  }


//=====Window==> Affiche une fenêtre semi-transparence avec transition
	void Window(int largeur, int hauteur)
	{
		unsigned char p, i;
		PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		for(p=0;p<=50;p++){
			for(i=64-(p*hauteur/100);i<=64+(p*hauteur/100);i++){
				DrawLine (120-(p*largeur/100), i, 120+(p*largeur/100), i, A_REVERSE);	}
			DrawLine (120-p*largeur/100,64-p*hauteur/100,120+p*largeur/100,64-p*hauteur/100,A_NORMAL);
			DrawLine (120+p*largeur/100,64-p*hauteur/100,120+p*largeur/100,64+p*hauteur/100,A_NORMAL);
			DrawLine (120+p*largeur/100,64+p*hauteur/100,120-p*largeur/100,64+p*hauteur/100,A_NORMAL);
			DrawLine (120-p*largeur/100,64+p*hauteur/100,120-p*largeur/100,64-p*hauteur/100,A_NORMAL);
  }	}


/////////////////////////////FONCTIONS NECESSAIRES AU JEU//////////////////////////////


//=====Xor_Sprite==> Affichage d'un sprite en XOR
	void Xor_Sprite(unsigned char x0, unsigned char y0, unsigned char sprite[])
	{
	  Sprite8 (3+x0*8, 3+y0*8, 8, sprite, GetPlane(DARK_PLANE), SPRT_XOR);
    Sprite8 (3+x0*8, 3+y0*8, 8, sprite, GetPlane(LIGHT_PLANE), SPRT_XOR);
  }


//=====Xor_Pill==> Affichage de la pillule en XOR
	void Xor_Pill(unsigned char x, unsigned char y, enum T_SENS sens, enum T_COLOR col1, enum T_COLOR col2)
	{
		Xor_Sprite(x, y, pill_sprite[PILL_U-2-!sens][col1]);
		Xor_Sprite(x+!sens, y+sens, pill_sprite[PILL_D-2-!sens][col2]);
  }


//=====Put_Sprite==> Affichage d'un sprite sur la grille de jeu
	void Put_Sprite(unsigned char x0, unsigned char y0, unsigned char sprite[], unsigned char mask[])
	{
	  Sprite8 (3+x0*8, 3+y0*8, 8, mask, GetPlane(DARK_PLANE), SPRT_AND);
	  Sprite8 (3+x0*8, 3+y0*8, 8, sprite, GetPlane(DARK_PLANE), SPRT_OR);
  	Sprite8 (3+x0*8, 3+y0*8, 8, mask, GetPlane(LIGHT_PLANE), SPRT_AND);
    Sprite8 (3+x0*8, 3+y0*8, 8, sprite, GetPlane(LIGHT_PLANE), SPRT_OR);
  }


//=====Clear_Sprite==> Efface un sprite de la grille de jeu et retrace le fond d'écran
	void Clear_Sprite(unsigned char oldx, unsigned char oldy)
	{
		Filled_Rectangle(3+oldx*8, 3+oldy*8, 10+(oldx)*8, 10+(oldy)*8, 3);
    if (gridback) Sprite8 (3+oldx*8, 3+oldy*8, 8, spr_blank, GetPlane(LIGHT_PLANE), SPRT_OR);
	}


//=====Put_Virus==> Affiche les virus animés
	void Put_Virus(unsigned char etape)
	{
		unsigned char i;
		
		etape=etape%10;
		if (etape==1 || etape==3) etape=0; //sprites A
		else if (etape==2) etape=1; 			 //sprites B
		else if (etape==4) etape=2; 			 //sprites C
		else return;
		
		for (i=0;i<3;i++){
		  Sprite16 (84+i*18, 96, 16, bigvirus_sprite[etape][i][1], GetPlane(DARK_PLANE), SPRT_XOR);
   		Sprite16 (84+i*18, 96, 16, bigvirus_sprite[etape][i][0], GetPlane(LIGHT_PLANE), SPRT_XOR);
		}
	}


//=====Animation==> Animation permanente (par timer) des virus
	void Animation(void)
	{
		unsigned char x0,y0;
		enum T_COLOR color;
		enum Bool etape_paire=!(anim_frame%4); //une fois sur 4
		
		timer_state++;
		if (timer_state==210) timer_state=0;	//210=2*3*5*7, un bon ppcm pour les timers

		if (!viranim || timer_state%2) return;
		//on anime les virus qu'une fois sur 2, quand ils sont activés

		//petits virus
		if (anim_frame%2==0) {										//une fois sur 2 à partir d'ici
			for (x0=0;x0<8;x0++){
				for (y0=0;y0<15;y0++){
					if (table[x0][y0].Type!=VIRUS) continue;
					color=table[x0][y0].Col;
					Clear_Sprite(x0,y0);
					Put_Sprite(x0,y0,	virus_sprite[!etape_paire][color][0],
														virus_sprite[!etape_paire][color][0]);
		} } }

		//gros virus
		if (viranim>1) Put_Virus(anim_frame);																//on efface l'ancien sprite
		if (anim_frame==4) anim_frame=1; else anim_frame++;		//on incrémente l'étape
		if (viranim>1) Put_Virus(anim_frame);																//on affiche le nouveau sprite
  }


//=====Clear_Pill==> Efface la pillule courante de la grille de jeu (->Clear_sprite)
	void Clear_Pill (void)
	{
		Clear_Sprite(x,y);
		Clear_Sprite(x+!sens,y+sens);
	}


//=====Put_Pill==> Affichage d'une capsule sur la grille de jeu
	void Put_Pill(unsigned char x0, unsigned char y0, enum T_BLOCK capsule, enum T_COLOR color)
	{
		if (capsule==BLANK) Clear_Sprite(x0,y0);
		else if (capsule==VIRUS) Put_Sprite(x0,y0,virus_sprite[0][color][0],virus_sprite[0][color][1]);
		else Put_Sprite(x0,y0,pill_sprite[capsule-2][color],pill_sprite[capsule-2][3]);
	}


//=====Show_Pill==> Affiche la pillule courante sur la grille de jeu
	void Show_Pill (void)
	{
		Put_Pill(x, y, PILL_U-!sens, col1);
		Put_Pill(x+!sens, y+sens, PILL_D-!sens, col2);
	}


//=====Set_Pill==> Efface, modifie puis affiche une capsule à des coords données
	void Set_Pill(unsigned char x0, unsigned char y0, enum T_BLOCK block)
	{
		Clear_Sprite(x0,y0); 				//d'abord on efface
		table[x0][y0].Type=block;	//puis on modifie la table
		Put_Pill(x0,y0,block,table[x0][y0].Col);	//enfin on affiche
	}


//=====Cut_Pill==> Détache une pillule de ses voisins
	void Cut_Pill(unsigned char x0, unsigned char y0)
	{	
		if (x0>0 && table[x0-1][y0].Type==PILL_L) Set_Pill(x0-1,y0,PILL);
		else if (x0<7 && table[x0+1][y0].Type==PILL_R) Set_Pill(x0+1,y0,PILL);
		else if (y0>0 && table[x0][y0-1].Type==PILL_U) Set_Pill(x0,y0-1,PILL);
		else if (y0<14 && table[x0][y0+1].Type==PILL_D) Set_Pill(x0,y0+1,PILL);
	}
	

//====Hide_Pill==> Fait disparaitre progessivement un sprite + troncature des voisins
	unsigned long int Hide_Pill(unsigned char x0, unsigned char y0, enum T_COLOR col0)
	{	
		unsigned long int local_score;
		local_score=10;
		Clear_Sprite(x0,y0);
		if (table[x0][y0].Type==VIRUS){
			viruses[col0]--;
			local_score=100;

			//activer la destruction du gros virus !!!

		  Sprite8 (3+x0*8, 3+y0*8, 8, virus_dis, GetPlane(DARK_PLANE), SPRT_OR);
		} else Sprite8 (3+x0*8, 3+y0*8, 8, pill_dis, GetPlane(DARK_PLANE), SPRT_OR);
		table[x0][y0].Type=BLANK;
		table[x0][y0].Col=TRANSPARENT;
		return local_score;
  }


//=====Draw_Text==> Affiche un texte en mode niveaux de gris
	void Draw_Text (unsigned char x, unsigned char y, char *text)
	{
	  PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		printf_xy(x, y, text);
	  PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		printf_xy(x, y, text);
	}


//=====Draw_Score==> Affiche le score et les statistiques
	void Draw_Score (void)
	{
		char buffer[8];
		memset(buffer, 0, 8);
		//score
		sprintf(buffer,"%07lu", score);
		Draw_Text(133,59,buffer);
		//niveau
		sprintf(buffer,"%02u", level);
		Draw_Text(204,59,buffer);
		//virus
		sprintf(buffer,"%02u %02u %02u", viruses[GREY], viruses[BLACK], viruses[WHITE]);
		Draw_Text(85,115,buffer);
	}
	

//=====Draw_Table==> Affichage de la table des sprites sur la grille de jeu
	void Draw_Table ()
	{
		unsigned char x0,y0;
		for (x0=0;x0<8;x0++){
			for (y0=0;y0<15;y0++) Put_Pill(x0, y0, table[x0][y0].Type, table[x0][y0].Col); }
	}


//=====Virus_Table==> Ajoute qty*3 virus à la table
	void Virus_Table (int qty)	//en fait qty=level
	{
		unsigned char i,x0,y0,col0,max;
		for (i=0;i<3;i++) viruses[i]=qty;
		max=5+qty/3;
		for (i=0;i<qty*3;i++){
			col0=i%3;
			do{
				x0=random(8);
				y0=random(max)+(15-max);
			}while(table[x0][y0].Type);	//on cherche une case vide
			table[x0][y0].Type=VIRUS;
			table[x0][y0].Col=col0;
	}	}


//=====Clear_Table==> Efface la table des sprites
	void Clear_Table ()
	{
		unsigned char x0,y0;
		for (x0=0;x0<8;x0++){
			for (y0=0;y0<15;y0++){
				table[x0][y0].Type=BLANK;
				table[x0][y0].Col=TRANSPARENT; } }
	}


//=====Wait==>Attend [times] pulsations timer en animant les gros virus
	void Wait (unsigned char times)
	{
		unsigned char i;
		for (i=0;i<times;i++)
		{
			while (!OSTimerExpired(6));
			Animation();
		}
	}


//=====Next_Pill==> Choisit et affiche la pillule suivante
	void Next_Pill (void)
	{
		next_col1=random(3);
		next_col2=random(3);
		Xor_Pill(10,6,HORIZONTAL,next_col1,next_col2);
	}
	

//=====Xor_Mario==> Affiche la main de mario pendant un lancer de pillule
	void Xor_Mario (unsigned char etape)
	{
	  Sprite8 (97, 42, 19, mario_sprite[etape][0], GetPlane(DARK_PLANE), SPRT_XOR); 
	  Sprite8 (97, 42, 19, mario_sprite[etape][1], GetPlane(LIGHT_PLANE), SPRT_XOR); 
	}


//=====Anim_Xor_Pill==> Affiche puis efface une pillule XOR
	void Anim_Xor_Pill(unsigned char x, unsigned char y, enum T_SENS sens)
	{
		Xor_Pill(x,y,sens,col1,col2);
		Wait(1);
		Xor_Pill(x,y,sens,col1,col2);
	}

//=====Give_Pill==> Donne la pillule suivante au joueur
	void Give_Pill (void)
	{
	  col1=next_col1;
	  col2=next_col2;
		//animation: jeter de pillule
		Xor_Pill(10,6,HORIZONTAL,col1,col2);
	  	Xor_Mario(0);
	  	Xor_Mario(1);
			Anim_Xor_Pill(10,5,VERTICAL);
	  	Xor_Mario(1);
	  	Xor_Mario(2);
			Anim_Xor_Pill(9,4,HORIZONTAL);
			Anim_Xor_Pill(9,2,VERTICAL);
			Anim_Xor_Pill(8,2,HORIZONTAL);
			Anim_Xor_Pill(8,1,VERTICAL);
			Anim_Xor_Pill(6,1,HORIZONTAL);
			Anim_Xor_Pill(5,0,VERTICAL);
			sens=HORIZONTAL;
			x=3;
			y=0;
	  	Xor_Mario(2);
	  	Xor_Mario(0);
		Next_Pill();
	}


//==========Possible_Move==> Vérifie qu'un bloc voisin (coords relatives) est libre
	enum Bool Possible_Move(signed char xdir, signed char ydir)
	{
		return (y+ydir<15-(signed char)sens && x+xdir>=0 && x+xdir<7+(signed char)sens 
			&& table[x+xdir][y+ydir].Type==BLANK && table[x+xdir+!sens][y+ydir+sens].Type==BLANK);
	}


//=====Fall_Pill==> Fait descendre une pillule (table+grille)
	void Fall_Pill(unsigned char x0, unsigned char y0)
	{
		table[x0][y0+1]=table[x0][y0];
		table[x0][y0].Type=BLANK;
		table[x0][y0].Col=TRANSPARENT;
		Clear_Sprite(x0,y0);
		Put_Pill(x0, y0+1, table[x0][y0+1].Type, table[x0][y0+1].Col);
	}
	
	
//==========Check_Gravity==> Fait "tomber" les pillules de la table qui flottent encore
	enum Bool Check_Gravity (void)
	{
		signed char x,y;
		enum Bool movement=FALSE;
		for (y=13;y>=0;y--){
			for (x=0;x<8;x++){
				if (table[x][y].Type==PILL_L && table[x][y+1].Type==BLANK && table[x+1][y+1].Type==BLANK){
					Fall_Pill(x,y);
					Fall_Pill(x+1,y);
					movement=TRUE; }
				else if (table[x][y].Type==PILL_U && y<13 && table[x][y+2].Type==BLANK){
					Fall_Pill(x,y+1);
					Fall_Pill(x,y);
					movement=TRUE; }
				else if (table[x][y].Type==PILL && table[x][y+1].Type==BLANK){
					Fall_Pill(x,y);
					movement=TRUE; }
		}	}
		return movement;
	}
		
		
//====Check_Associations==> Vérifie si 4 capsules identiques sont cote à cote
	long unsigned int Check_Associations (unsigned char x0, unsigned char y0)
	{
		signed char x1, xmin=x0, xmax=x0;
		signed char y1, ymin=y0, ymax=y0;
		long unsigned int local_score;
		enum T_COLOR col0=table[x0][y0].Col;
		
		local_score=0;
				
		if (col0==TRANSPARENT) return 0; //sinon on est dans la merde!
		
		//associations horizontales
		for (x1=x0;x1>=0;x1--) if (table[x1][y0].Col==col0) {xmin=x1;} else break;
		for (x1=x0;x1<8;x1++) if (table[x1][y0].Col==col0) {xmax=x1;} else break;
		if (xmax-xmin>2) {
			for (x1=xmin;x1<=xmax;x1++)	Cut_Pill(x1, y0);
			for (x1=xmin;x1<=xmax;x1++)	local_score+=Hide_Pill(x1, y0, col0);
			Wait(2);
			for (x1=xmin;x1<=xmax;x1++)	Clear_Sprite(x1,y0);
			Wait(1); }

		//associations verticales
		for (y1=y0;y1>=0;y1--) if (table[x0][y1].Col==col0) {ymin=y1;} else break;
		for (y1=y0;y1<15;y1++) if (table[x0][y1].Col==col0) {ymax=y1;} else break;
		if (ymax-ymin>2) {
			for (y1=ymin;y1<=ymax;y1++) Cut_Pill(x0, y1);
			for (y1=ymin;y1<=ymax;y1++) local_score+=Hide_Pill(x0, y1, col0);
			Wait(2);
			for (y1=ymin;y1<=ymax;y1++) Clear_Sprite(x0,y1);
			Wait(1); }

		return local_score;	//renvoie zéro si aucune association
	}


//=====Check_Blocks==> Boucle jusqu'à ce que la table soit tassée et marrie les capsules
	void Check_Blocks (void)
	{
		unsigned char x0,y0;
		long unsigned int local_score;
		enum Bool check_again;
		check_again=TRUE;
		while (check_again){
			check_again=FALSE;			
			for (x0=0;x0<8;x0++){	//on vérifie toute la table case par case !!!
				for (y0=0;y0<15;y0++){
					if ((local_score=Check_Associations(x0,y0))>0){
						score+=local_score;
						Draw_Score();
						if (!(check_again=!(stage_cleared=(viruses[0]+viruses[1]+viruses[2]==0)))) return;
						//si ya plus de virus RETURN sinon CHECK_AGAIN
			}	} }
			if (check_again) {
				while (Check_Gravity()){
					check_again=TRUE;
					Wait(2);
	}	}	} }


//=====Integrate_Pill==> Enregistre la pillule courante dans la table et vérifie les dépendances
	void Integrate_Pill (void)
	{
		table[x][y].Type=PILL_U-!sens;
		table[x][y].Col=col1;
		table[x+!sens][y+sens].Type=PILL_D-!sens;
		table[x+!sens][y+sens].Col=col2;
		Check_Blocks();
			if (stage_cleared) return;	//plus de virus !
		Give_Pill();
			if ((game_over=table[3][0].Type || table[4][0].Type)) return;				//plus de place !
	}
				

//=====Fall==> Vérifie que le bloc de dessous est libre, descend la pillule courante
	void Fall (void)
	{
			if (Possible_Move(0,1)) {Clear_Pill(); y++;}
			else Integrate_Pill(); 	//enregistre la pillule courante dans la table
	}


//=====Rotate==> Rotation (si possible) de la pilule courante
	void Rotate(enum T_ROTATION rot)
	{
		if (y<14 && x<7+(int)!sens && table[x+sens][y+1].Type==BLANK)	{
			Clear_Pill(); 			//efface la pillule de son ancienne position
      sens=!sens;					//rotation
			if (sens==rot){			//inversion des couleurs
				enum T_COLOR tmp;
				tmp=col1;
				col1=col2;
				col2=tmp; }
			if (sens==HORIZONTAL) {
				if (Possible_Move(0,1)) y++;	//copie de Fall() sans Clear_Pill()
				else Integrate_Pill(); }
	}	}
	
	
//=====Draw_Game==> Trace l'interface du jeu
	void Draw_Game (void)
	{
	  memset (GetPlane(DARK_PLANE), 0, LCD_SIZE);
	  memset (GetPlane(LIGHT_PLANE), 0, LCD_SIZE);

		//FOND
		Background(70,0,LCD_WIDTH-1,LCD_HEIGHT-1);

		//TOUTES LES PLANES
	  Panel(80,2,215,13);		//cadre du titre
	  Panel(80,39,121,72);	//cadre de mario
	  Panel(131,39,177,72);	//cadre du score
	  Panel(187,39,233,72);	//cadre du niveau
	  Panel(80,82,140,124);	//cadre des virus
	  Panel(150,82,233,124);	//cadre des infos

	  FontSetSys(F_6x8);
		Draw_Text(139,46,"Score");
		Draw_Text(192,46,"Niveau");
		Draw_Text(95,84,"Virus");
		Draw_Text(166,84 ,"Contrôles");

	  FontSetSys(F_4x6);
		Draw_Text(152,96 ,"* Clavier directionnel");
		Draw_Text(152,106,"* [MODE] Pause/Menu");
		Draw_Text(152,115,"* [ESC]  Quitter");

	  Draw_Text(85,15,"Version 1.0 beta. Auteur: ROLY@FR.ST");

		//mario
	  Sprite32 (97, 42, 28, bigmario_A_dark, GetPlane(DARK_PLANE), SPRT_XOR); 
	  Sprite32 (97, 42, 28, bigmario_A_light, GetPlane(LIGHT_PLANE), SPRT_XOR); 

		Put_Virus(1);																//gros virus

		//DARK_PLANE
	  PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);

		//cadre de jeu
	  Rectangle(1,1,69,125);
	  
	  FontSetSys(F_6x8);
	  DrawStr(82,4,"Dr. Mario 92+ by R0Ly!", A_NORMAL);
	  
		//LIGHT_PLANE
	  PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
	  
	  Rectangle(0,0,70,126);
	  Rectangle(2,2,68,124);
	
	  DrawStr(82,4,"Dr. Mario 92+ by R0Ly!", A_NORMAL);
	  DrawStr(83,5,"Dr. Mario 92+ by R0Ly!", A_NORMAL);	//ombre

		//capture de l'affichage standart
		memcpy (virtual1, GetPlane(DARK_PLANE), LCD_SIZE);
		memcpy (virtual2, GetPlane(LIGHT_PLANE), LCD_SIZE);
	}


//=====Game_Menu==> menu "pause"
	void Game_Menu (void)	
	{
		unsigned char keys0, keys6, item, i;
		item=0;
		
		Filled_Rectangle(3,3,67,123,2);	//censure de la grille de jeu
	  FontSetSys(F_6x8);
  	Draw_Text(5,3,"PAUSE MENU");

		Window(200,102);

	  for (i=0;i<2;i++)	//affichage sur les deux planes
	  {
		  PortSet (GetPlane(i), LCD_WIDTH-1, LCD_HEIGHT-1);
	  	DrawStr(100,18,"OPTIONS",A_NORMAL);
	  	DrawStr(40, 30,"Retour au jeu",A_NORMAL);
	  	DrawStr(40, 40,"Abandonner la partie",A_NORMAL);
	  	DrawStr(40, 50,"Quitter le jeu",A_NORMAL);
	  	DrawStr(40, 64,"Sauvegarde auto  ...... [   ]",A_NORMAL);
	  	DrawStr(40, 74,"Fond grille de jeu .... [   ]",A_NORMAL);
	  	DrawStr(40, 84,"Qualité fond d'écran .. [   ]",A_NORMAL);
	  	DrawStr(40, 94,"Virus animés .......... [   ]",A_NORMAL);
	  	DrawStr(40,104,"Vitesse du jeu ........ [   ]",A_NORMAL);
	  }
	  
		while (_rowread(0x2FF)&0x20);			//faut relacher mode d'abord

	  do
	  {

		  Filled_Rectangle(190,64,209,70,3);
		  Filled_Rectangle(190,74,209,80,3);
		  Filled_Rectangle(190,84,209,90,3);
		  Filled_Rectangle(190,94,209,100,3);
		  Filled_Rectangle(190,104,209,110,3);

		  PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		  if (autosave) {DrawLine(192,64,207,70,A_NORMAL); DrawLine(192,70,207,64,A_NORMAL);}
		  if (gridback) {DrawLine(192,74,207,80,A_NORMAL); DrawLine(192,80,207,74,A_NORMAL);}
		  
		  Filled_Rectangle(191,89,196,90,1); 	//19 = 1+ 4 +1+ 4 +1+ 4 +2
		  if (backqual) Filled_Rectangle(198,86,202,90,1);
		  if (backqual>1) Filled_Rectangle(204,84,208,90,1);

		  Filled_Rectangle(191,99,196,100,1); 	//19 = 1+ 4 +1+ 4 +1+ 4 +2
		  if (viranim) Filled_Rectangle(198,96,202,100,1);
		  if (viranim>1) Filled_Rectangle(204,94,208,100,1);

		  Filled_Rectangle(191,109,193,110,1); 	//19 = 3 +1+ 3 +1+ 3 +1+ 3 +1+ 3 +1
		  if (speed<5) Filled_Rectangle(195,108,197,110,1);
		  if (speed<4) Filled_Rectangle(199,107,201,110,1);
		  if (speed<3) Filled_Rectangle(203,106,205,110,1);
		  if (speed<2) Filled_Rectangle(207,105,209,110,1);

	  	Draw_Text(24,30+4*(item>2)+10*item,">");	//curseur

			do
			{ keys0=_rowread(0x3FE);	//on attend que le clavier soit libre
				keys6=_rowread(0x3BF); }
			while (keys6&0x40 || keys0&0x10 || keys0&0x40 || keys0&0x20 || keys0&0x80);

			do
			{	keys0=_rowread(0x3FE);	//on attend l'appui d'une touche
				keys6=_rowread(0x3BF); }
			while (!(keys6&0x40 || keys0&0x10 || keys0&0x40 || keys0&0x20 || keys0&0x80));
	
			//on efface le curseur
	  	Filled_Rectangle(24,30+4*(item>2)+10*item,30,38+4*(item>2)+10*item,2);

		  if (keys0&0x10)																						//gauche
		  {
		  	if (item==3) autosave=FALSE;																	//sauvegarde auto
		  	else if (item==4) gridback=FALSE;															//fond de grille
		  	else if (item==5) backqual=backqual-(backqual>0);							//qualité fond
		  	else if (item==6) viranim=viranim-(viranim>0);								//animation
		  	else if (item==7) speed=speed+(speed<5);											//vitesse
		  }
		  else if (keys0&0x40 || keys6&0x40)												//droite ou entrée
		  {
		  	if (item==3) autosave=!autosave;																	//sauvegarde auto
		  	else if (item==4) gridback=!gridback;															//fond de grille
		  	else if (item==5) {if (backqual<2) backqual++; else backqual=0;}	//qualité fond
		  	else if (item==6) {if (viranim<2) viranim++; else viranim=0;}			//animation
		  	else if (item==7) {if (speed>1) speed--; else speed=5;}						//vitesse
		  	else break;
		  }
			else if (keys0&0x20) {if (item==0) item=7; else item--;}	//haut
			else if (keys0&0x80) {if (item==7) item=0; else item++;}	//bas

	  }
	  while (1);
	  
	  if (item==1) game_over=TRUE;
	  else if (item==2) quit=TRUE;
	  else{
		  OSFreeTimer(6);
		  OSRegisterTimer(6,speed);
	  }

		//restauration de l'affichage
		memcpy (GetPlane(DARK_PLANE), virtual1, LCD_SIZE);
		memcpy (GetPlane(LIGHT_PLANE), virtual2, LCD_SIZE);
		
		//mise à jour de la grille (fond activé/désactivé)
		//Filled_Rectangle(3,3,67,123,3);
		Draw_Score();
		Draw_Table();
		anim_frame=1;
		timer_state=0;
		Show_Pill();
		Xor_Pill(10,6,HORIZONTAL,next_col1,next_col2);
	}


/////////////////////////////////FONCTION PRINCIPALE DU JEU/////////////////////////////////////		


//====Game==> Déroullement général d'une partie
	void Game (void)
	{
	  static enum Bool key_pressed[6], keep_pressing[4], refresh, falltime, high_speed;
	  static unsigned char i, keys0, keys8;
		
		if (!in_game) score=0;

		quit=FALSE;
	  timer_state=0;
		anim_frame=1;				//valeur initiale 0: multiple de 10 => écran vide
	
	  INT_HANDLER save_int_1;
	  save_int_1=GetIntVec(AUTO_INT_1);
	  SetIntVec(AUTO_INT_1,DUMMY_HANDLER);

	  GrayOn();

		if (in_game) 
			Draw_Game();
		else
		{	//affichage graphique
			memcpy (GetPlane(DARK_PLANE), virtual1, LCD_SIZE);
			memcpy (GetPlane(LIGHT_PLANE), virtual2, LCD_SIZE);
		}
	
	  OSFreeTimer(6);
	  OSRegisterTimer(6,speed);
	  
	  //BOUCLE PRINCIPALE DU JEU => EVOLUTION DU NIVEAU
	  do
	  {
	  
			for(i=0;i<4;i++) key_pressed[i]=FALSE;
			refresh=TRUE;
			high_speed=FALSE;
			game_over=0;
			stage_cleared=0;

			if (!in_game){		//réinitialisation du niveau
			  Clear_Table();
			  Virus_Table(level);	}
			  
			Draw_Score();
		  Draw_Table();
		  
			if (!in_game){
			  Next_Pill();
				Give_Pill();		//donne une nouvelle pillule
			} else Xor_Pill(10,6,HORIZONTAL,next_col1,next_col2);
			
			in_game=TRUE;
			
			for(i=0;i<4;i++) key_pressed[i]=FALSE;

			//BOUCLE DU NIVEAU EN COURS
		  do
		  {
		
				if (refresh) Show_Pill();
				refresh=FALSE;

				for(i=0;i<4;i++) keep_pressing[i]=key_pressed[i];
				falltime=FALSE;
				
				if (OSTimerExpired(6)) 	//O,25 secondes se sont écoulées
				{	Animation();
					falltime=high_speed || !(timer_state%timer_fall);	}
				
				keys0=_rowread(0x3FE);						//on ne lit qu'une seule fois le clavier
				keys8=_rowread(0x2FF);
	
				key_pressed[DOWN]  = keys0&0x80;
				key_pressed[UP]    = keys0&0x20;
		    key_pressed[LEFT]  = keys0&0x10;
		    key_pressed[RIGHT] = keys0&0x40;
		    key_pressed[ESC]   = keys8&0x40;
		    key_pressed[MODE]  = keys8&0x20;
	
	      high_speed=keep_pressing[DOWN]; 	//accélération si flèche du bas maintenue
	
				//flèche du bas ou timer descente
	      if (key_pressed[DOWN] || falltime){
	      	if (!high_speed || falltime){		//attendre le timer si touche maintenue
	      		Fall();
	      		refresh=TRUE;
	      	}
	
				//autres touches fléchées      
	      }	else {
	
		      if(key_pressed[UP]){								//flèche du haut
		        if (!keep_pressing[UP]) {
		        	Rotate(CLOCK);			 //rotation sens des aiguilles d'une montre
		        	refresh=TRUE;
		      } }
					else if(key_pressed[LEFT]){ 					//flèche de gauche
		        if (!keep_pressing[LEFT] && Possible_Move(-1,0)) {
							Clear_Pill();
		        	x--;
		        	refresh=TRUE;
		      } }
		      else if(key_pressed[RIGHT]){						//flèche de droite
		        if (!keep_pressing[RIGHT] && Possible_Move(1,0)) {
							Clear_Pill();
		        	x++;
		        	refresh=TRUE;
		      } }
		      
		      //touches spéciales
					else if (key_pressed[ESC]) quit=TRUE;
					else if (key_pressed[MODE]) Game_Menu();
			  }
				
		  } while(!quit && !stage_cleared && !game_over);	//fin du niveau en cours?
		  
			Xor_Pill(10,6,HORIZONTAL,next_col1,next_col2);

			in_game=autosave;
			
			if (!quit) {
				in_game=FALSE;				
				Filled_Rectangle(3,68,67,123,3);
			  FontSetSys(F_4x6);
				Draw_Text(13,98,"Press ENTER");
				Draw_Text(13,106,"to continue!");
			  FontSetSys(F_6x8); }

		  if (stage_cleared && level<20){
		  	if (level<20){
			  	Draw_Text(5,82,"NEXT LEVEL");
					level++;		//on passe au niveau suivant
		  	} else Draw_Text(5,82,"BONUS PTS.");
		  }
		  else if (game_over){
			  Draw_Text(5,82,"GAME OVER!");
			  Sprite32 (97, 42, 28, bigmario_A_dark, GetPlane(DARK_PLANE), SPRT_XOR); 
		  	Sprite32 (97, 42, 28, bigmario_A_light, GetPlane(LIGHT_PLANE), SPRT_XOR); 
			  Sprite32 (97, 42, 28, bigmario_D_dark, GetPlane(DARK_PLANE), SPRT_XOR); 
		  	Sprite32 (97, 42, 28, bigmario_D_light, GetPlane(LIGHT_PLANE), SPRT_XOR); 
		  }

		  if (!quit) while (!(_rowread(0x3BF)&0x40));	//on attend ENTREE

			Filled_Rectangle(3,3,67,123,3);
		  
		} while(!quit && !game_over);
	
		//capture de l'affichage
		//virtual1=malloc(3840); memcpy (virtual1, GetPlane(DARK_PLANE), LCD_SIZE);
		//virtual2=malloc(3840); memcpy (virtual2, GetPlane(LIGHT_PLANE), LCD_SIZE);

		//FIN DU JEU
	  OSFreeTimer(6);

	  GrayOff();
	  SetIntVec(AUTO_INT_1,save_int_1);
	}



/////////////////////////////////FONCTIONS DE MENUS/SCORES/////////////////////////////////////		


//=====Check_Score==> Ajoute éventuellement un score au tableau
	void Check_Score (void)
	{
		short key;
		unsigned char curseur, position, i, plane;
		char buffer[17];
			memset(buffer, 0, 17);
			position=0;
		
		if (score>best_score[0]) position=1;
		else if (score>best_score[1]) position=2;
		else if (score>best_score[2]) position=3;
      
		if (!position) return;
      
		//décalage des scores
		for(i=2;i>position-1;i--){
			strcpy (best_player[i], best_player[i-1]); 
			best_score[i]=best_score[i-1]; }
			
		//réinitialisation du score
		i=position-1;
    memset(best_player[i], 0, 15);
		best_score[i]=score;

				
		//affichage
	  GrayOn();
		memcpy (GetPlane(DARK_PLANE), virtual1, LCD_SIZE);
		memcpy (GetPlane(LIGHT_PLANE), virtual2, LCD_SIZE);
		Window(200,102);
		Filled_Rectangle(46,54,193,89, 3);	//scores
	  FontSetSys(F_6x8);

	  for(plane=0;plane<2;plane++){
		  PortSet (GetPlane(plane), LCD_WIDTH-1, LCD_HEIGHT-1);
	  	DrawStr(61,18,"TABLEAU DES RECORDS",A_NORMAL);
	  	DrawStr(38,32,"Félicitations ! Vous entrez",A_NORMAL);
	  	DrawStr(36,40,"dans le tableau des records.",A_NORMAL);
	  	DrawStr(71,93,"Tapez votre nom",A_NORMAL);
	  	DrawStr(45,101,"puis validez avec ENTREE.",A_NORMAL);
	  	Rectangle(46+plane,54+plane,193-plane,89-plane);	//cadre meilleurs scores
	    printf_xy(146,58+i*10,"%7lu", best_score[i]);
		}
		
		//score grisés
	  PortSet (GetPlane(DARK_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
  	for(i=0;i<3;i++){
    	printf_xy(50,58+i*10,"%s", best_player[i]);
      printf_xy(146,58+i*10,"%7lu", best_score[i]);}
		
    //on encadre le nom du joueur à saisir
	  PortSet (GetPlane(LIGHT_PLANE), LCD_WIDTH-1, LCD_HEIGHT-1);
		i=position-1;
		DrawLine(49,57+i*10,140,57+i*10,A_NORMAL);
		DrawLine(140,57+i*10,140,66+i*10,A_NORMAL);
		DrawLine(140,66+i*10,49,66+i*10,A_NORMAL);
		DrawLine(49,66+i*10,49,57+i*10,A_NORMAL);
		GKeyFlush();

    //saisie du nom de joueur
		curseur=0;
		do
		{
			sprintf(buffer,"%s_  ", best_player[i]);
			Draw_Text(50,58+10*i,buffer);
			DrawLine(140,57+i*10,140,66+i*10,A_NORMAL);
			key = ngetchx();
			if (key == KEY_BACKSPACE && curseur) {
				curseur--;
				best_player[i][curseur] = 0; }
			else if (key >= ' ' && key <= '~' && curseur < 14) {
				best_player[i][curseur] = key;
				curseur++; }
		} while (key != KEY_ENTER);
		
		GrayOff();
		
	}



//==========Menu==> Trace l'interface du menu 
	enum Bool Menu (void)
	{
		char buffer[17];
			memset(buffer, 0, 17);
		unsigned char i,plane;

	  GrayOn();

		//affichage graphique
		memcpy (GetPlane(DARK_PLANE), virtual1, LCD_SIZE);
		memcpy (GetPlane(LIGHT_PLANE), virtual2, LCD_SIZE);
		
		Window(200,102);

		Filled_Rectangle(47,75,192,110,3);	//scores
		Filled_Rectangle(30,38,210,51,3);	//niveau

	  for(plane=0;plane<2;plane++){

		  PortSet (GetPlane(plane), LCD_WIDTH-1, LCD_HEIGHT-1);

		  FontSetSys(F_4x6);
	  	DrawStr(30,32,"Séléction du niveau de difficulté:",A_NORMAL);
	  	DrawStr(30,52,"Appuyez sur ENTREE pour jouer, ECHAP pour quitter.",A_NORMAL);
		  FontSetSys(F_6x8);
	  	DrawStr(71,18,"NOUVELLE PARTIE",A_NORMAL);
	  	DrawStr(61,66,"TABLEAU DES RECORDS",A_NORMAL);
	  	Rectangle(47+plane,75+plane,192-plane,110-plane);	//cadre meilleurs scores
	  	Rectangle(30+plane,38+plane,210-plane,51-plane);	//cadre niveau

	  	for(i=0;i<3;i++){
	  		sprintf(buffer, "%s", best_player[i]);
  	  	DrawStr(51,79+i*10,buffer,A_NORMAL);
	  		sprintf(buffer, "%7lu", best_score[i]);
    	  DrawStr(147,79+i*10,buffer,A_NORMAL);
		}	}

		unsigned char action, etape;
		short k;
		action=0;
		level=1;
		etape=0;
		
	  Draw_Text(33,41,"-");	
	  Draw_Text(201,41,"+");	

	  OSFreeTimer(6);
	  OSRegisterTimer(6,8);
		
	  while (!action)
	  {
		  for(plane=0;plane<2;plane++){
			  PortSet (GetPlane(plane), LCD_WIDTH-1, LCD_HEIGHT-1);
				for(i=40;i<=49;i++) DrawLine (40, i, 200, i, A_REVERSE);
				for (i=0;i<level;i++)	Sprite8 (40+i*8, 40, 8, virus_sprite[etape][i%3][0], GetPlane(plane), SPRT_XOR);
			}
			etape=(etape+1)%2;
			
		  while (!(i=kbhit()) && !OSTimerExpired(6));
		  if (i)
		  {
			  k=ngetchx();
			  if (k==KEY_ENTER) action=2;
			  else if (k==KEY_ESC)	action=1;
			  else if (k==KEY_LEFT)	level=level-(level>1);
			  else if (k==KEY_RIGHT) level=level+(level<20);
		  }
		  
	  }
	  
	  GrayOff();
	  return action-1;
	}
	


/////////////////////////////////FONCTION PRINCIPALE _MAIN/////////////////////////////////////		


//===_main==> PROGRAMME PRINCIPAL = MENU
void _main(void)
{
	randomize();
	
		virtual1=malloc(3840); 
		virtual2=malloc(3840); 

	if (!in_game){
		GrayOn();
		Draw_Game();
		GrayOff();
	}
	
	while (in_game || Menu())	//tant qu'on accepte de jouer
	{	Game();
		if (in_game) break;
		if (score) Check_Score();
		GKeyFlush();
	}

	  PortRestore();
		free(virtual1);
		free(virtual2);
}
