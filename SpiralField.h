#ifndef SPIRALFIELD_H
#define SPIRALFIELD_H
#include"Field.h"
#include"Medium.h"

class SpiralField : public Field
{
public:
	SpiralField(const double& creationTime, const Point& s_Position, const double& s_Amplitude, const double& s_Frequency, const double& s_InitialPhase, const double& s_Speed);
	virtual ~SpiralField() {};

	void ActivateMedium(const double& timeOfNow, const double& frameTime)override;
};
#endif //SPIRALFIELD_H

