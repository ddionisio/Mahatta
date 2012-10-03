#ifndef _GAME_SCORE_H
#define _GAME_SCORE_H

#include "game_share.h"

typedef enum{
	SCORETYPE_MODERN,
	SCORETYPE_SEMITRADITIONAL,
	SCORETYPE_TRADITIONAL,

	SCORETYPE_MAX
}ScoreType;

class Score{
	public:
		// singleton thing, call this to get the object then call its methods
		static Score* getMe();
		~Score();

		void Init(ScoreType);

	protected:
		// declared here to avoid instancing
		Score();			
		Score& operator= (const Score&);

	private:
		static Score* pinstance;
		u8 scoretype,										
		   scoreside1, scoreside2,							// score
		   consecutivesside1, consecutivesside2,
		   urre3side1, urre3side2,							// treat as boolean
		   urre7side1, urre7side2;							// treat as boolean
		
};

#endif