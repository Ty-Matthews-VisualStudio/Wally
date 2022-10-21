#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Texture flags; these are stored in the flags entry of struct q2_miptex_s
#define TF_LIGHT		0x1
#define TF_SLICK		0x2
#define TF_SKY			0x4
#define TF_WARP			0x8
#define TF_TRANS33		0x10
#define TF_TRANS66		0x20
#define TF_FLOWING		0x40
#define TF_NODRAW		0x80
#define TF_HINT			0x100
#define TF_SKIP			0x200

// Texture contents; these are stored in the contents entry of struct q2_miptex_s
#define TC_SOLID		0x1
#define TC_WINDOW		0x2
#define TC_AUX			0x4
#define TC_LAVA			0x8
#define TC_SLIME		0x10
#define TC_WATER		0x20
#define TC_MIST			0x40
#define TC_AREAPORTAL	0x8000
#define TC_PLAYERCLIP	0x10000
#define TC_MONSTERCLIP	0x20000
#define TC_CURRENT_0	0x40000
#define TC_CURRENT_90	0x80000
#define TC_CURRENT_180	0x100000
#define TC_CURRENT_270	0x200000
#define TC_CURRENT_UP	0x400000
#define TC_CURRENT_DN	0x800000
#define TC_ORIGIN		0x1000000
#define TC_MONSTER		0x2000000
#define TC_CORPSE		0x4000000
#define TC_DETAIL		0x8000000
#define TC_TRANSLUCENT	0x10000000
#define TC_LADDER		0x20000000

#define SIN_TF_LIGHT	0x1			// value will hold the light strength
#define SIN_TF_MASKED   0x2			// surface texture is masked
#define SIN_TF_SKY		0x4			// don't draw, but add to skybox
#define SIN_TF_WARP		0x8			// turbulent water warp
#define SIN_TF_NONLIT	0x10		// surface is not lit
#define SIN_TF_NOFILTER	0x20		// surface is not lit
#define SIN_TF_CONVEYOR	0x40		// surface is not lit
#define SIN_TF_NODRAW	0x80		// don't bother referencing the texture
#define SIN_TF_HINT		0x100		// make a primary bsp splitter
#define SIN_TF_SKIP		0x200		// completely ignore, allowing non-closed brushes

#define SIN_TF_WAVY				0x400		// surface has waves
#define SIN_TF_RICOCHET			0x800		// projectiles bounce literally bounce off this surface
#define SIN_TF_PRELIT			0x1000		// surface has intensity information for pre-lighting
#define SIN_TF_MIRROR			0x2000		// surface is a mirror
#define SIN_TF_CONSOLE			0x4000		// surface is a console
#define SIN_TF_USECOLOR			0x8000		// surface is lit with non-lit * color
#define SIN_TF_HWONLY			0x10000		// surface has been damaged
#define SIN_TF_DAMAGE			0x20000		// surface can be damaged
#define SIN_TF_WEAK				0x40000		// surface has weak hit points
#define SIN_TF_NORMAL			0x80000		// surface has normal hit points
#define SIN_TF_ADDBLEND			0x100000	// surface has strong hit points
#define SIN_TF_ENVMAP			0x200000	// environment map
#define SIN_TF_RANDOMANIMATE	0x400000	// surface start animating on a random frame
#define SIN_TF_ANIMATE			0x800000	// surface animates
#define SIN_TF_RNDTIME			0x1000000	// time between animations is random
#define SIN_TF_TRANSLATE		0x2000000	// surface translates
#define SIN_TF_NOMERGE			0x4000000	// surface is not merged in csg phase
#define SIN_TF_TYPE_BIT0		0x8000000	// 0 bit of surface type
#define SIN_TF_TYPE_BIT1		0x10000000	// 1 bit of surface type
#define SIN_TF_TYPE_BIT2		0x20000000	// 2 bit of surface type
#define SIN_TF_TYPE_BIT3		0x40000000	// 3 bit of surface type

#define SIN_TC_SOLID			0x1
#define SIN_TC_WINDOW			0x2
#define SIN_TC_FENCE			0x4
#define SIN_TC_LAVA				0x8
#define SIN_TC_SLIME			0x10
#define SIN_TC_WATER			0x20
#define SIN_TC_MIST				0x40
#define SIN_TC_AREAPORTAL		0x8000
#define SIN_TC_PLAYERCLIP		0x10000
#define SIN_TC_MONSTERCLIP		0x20000
#define SIN_TC_CURRENT_0		0x40000
#define SIN_TC_CURRENT_90		0x80000
#define SIN_TC_CURRENT_180		0x100000
#define SIN_TC_CURRENT_270		0x200000
#define SIN_TC_CURRENT_UP		0x400000
#define SIN_TC_CURRENT_DN		0x800000
#define SIN_TC_ORIGIN			0x1000000
#define SIN_TC_MONSTER			0x2000000
#define SIN_TC_CORPSE			0x4000000
#define SIN_TC_DETAIL			0x8000000
#define SIN_TC_TRANSLUCENT		0x10000000
#define SIN_TC_LADDER			0x20000000

#define	SIN_SURF_START_BIT		27
#define SIN_SURFACETYPE_FROM_FLAGS(x) ( ( x >> (SIN_SURF_START_BIT) ) & 0xf )

#define SIN_SURF_TYPE_SHIFT(x) ( ( x ) << (SIN_SURF_START_BIT) ) // macro for getting proper bit mask

#define SIN_SURF_TYPE_NONE			SIN_SURF_TYPE_SHIFT(0)
#define SIN_SURF_TYPE_WOOD			SIN_SURF_TYPE_SHIFT(1)
#define SIN_SURF_TYPE_METAL			SIN_SURF_TYPE_SHIFT(2)
#define SIN_SURF_TYPE_STONE			SIN_SURF_TYPE_SHIFT(3)
#define SIN_SURF_TYPE_CONCRETE		SIN_SURF_TYPE_SHIFT(4)
#define SIN_SURF_TYPE_DIRT			SIN_SURF_TYPE_SHIFT(5)
#define SIN_SURF_TYPE_FLESH			SIN_SURF_TYPE_SHIFT(6)
#define SIN_SURF_TYPE_GRILL			SIN_SURF_TYPE_SHIFT(7)
#define SIN_SURF_TYPE_GLASS			SIN_SURF_TYPE_SHIFT(8)
#define SIN_SURF_TYPE_FABRIC		SIN_SURF_TYPE_SHIFT(9)
#define SIN_SURF_TYPE_MONITOR		SIN_SURF_TYPE_SHIFT(10)
#define SIN_SURF_TYPE_GRAVEL		SIN_SURF_TYPE_SHIFT(11)
#define SIN_SURF_TYPE_VEGETATION	SIN_SURF_TYPE_SHIFT(12)
#define SIN_SURF_TYPE_PAPER			SIN_SURF_TYPE_SHIFT(13)
#define SIN_SURF_TYPE_DUCT			SIN_SURF_TYPE_SHIFT(14)
#define SIN_SURF_TYPE_WATER			SIN_SURF_TYPE_SHIFT(15)
