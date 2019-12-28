#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "../gba/gba.h"

#define CLOUD_WIDTH		96
#define CLOUD_HEIGHT 	32

void LoadCloudImages();

class Cloud
{
	public:
		Cloud()	{ Init(); }
		~Cloud() {}
		void Init();
		void Create(s16 nPosX, s16 nPosY);
		void SetPos(s16 nPosX, s16 nPosY);
		void SetVel(float fVelX, float fVelY);
		void Update();
		void Delete();

	private:
		//u16 m_nCharNums[3];      // character numbers where cloud image data are stored
		//s16 m_nFirstSpriteIndex; // index of first (leftmost) cloud sprite
		s16 m_nCloudSpriteIndexes[3]; // index of cloud sprites

		// position of first (leftmost) cloud sprite
		s16 m_nFirstSpriteX;
		s16 m_nFirstSpriteY;

		// position of first (leftmost) cloud sprite stored as fixed float
		FIXED m_ffixGradualPosX;

		float m_fVelX;
		float m_fVelY;
};

#endif
