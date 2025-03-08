#ifndef CIRCULARFIELD_H
#define CIRCULARFIELD_H
#include"Field.h"

class CircularField : public Field 
{
public:
	CircularField(const double& creationTime,const Point& s_Position, const double& s_Amplitude, const double& s_Frequency, const double& s_InitialPhase, const double& s_Speed);
	virtual ~CircularField() {};
	
	void ActivateMedium (const double& timeOfNow, const double& frameTime) override;
};
#endif //CIRCULARFIELD_H
