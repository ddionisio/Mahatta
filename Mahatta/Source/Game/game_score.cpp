#include "game_score.h"

Score* Score::pinstance = NULL;

Score::Score(){

}

Score::~Score(){
	
}

Score* Score::getMe(){
	if(pinstance == NULL)
		pinstance = new Score();
	return pinstance;
}

void Score::Init(ScoreType oscoretype){
	scoretype = oscoretype;
	scoreside1 = scoreside2 = consecutivesside1 = consecutivesside2 = 0;
	urre3side1 = urre3side2 = 0;
	urre7side1 = urre7side2 = 0;
}