#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "../gba/gba.h"

#define CLOUD_WIDTH		96
#define CLOUD_HEIGHT 	32

class Cloud
{
	public:
		Cloud()	{}
		~Cloud() {}
		void Init(u8 firstSpriteNum, u16 memCharPos, u16 objRamPos, s16 xPos, s16 yPos);
		void SetPos(s16 xPos, s16 yPos);
		void SetVel(float xVel, float yVel);
		void Update();
		void Delete();

		float xVelocity;
		float yVelocity;

		u8 firstSpriteIndex;

		s16 xFirstSprite;
		s16 yFirstSprite;

		float fxGradualPos;
		float fyGradualPos;

		u16 firstMemCharPos;
		u16 firstObjRamPos;
};

#endif
