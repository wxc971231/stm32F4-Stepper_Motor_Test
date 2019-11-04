#include "pid.h"

#define Skp 40000
#define Ski 60000
#define Skd 0
	
#define Pkp 1
#define Pki 0
#define Pkd 0

double PEk=0;
double PSk=0;
double PDk=0;
double Pout;
double PoutMax=100;
double PoutMin=-100;
double PEk_1,PEk_2;
	
double SEk=0;
double SSk=0;
double SDk=0;
double Sout;
double SoutMax=100;
double SoutMin=-100;
double SEk_1,SEk_2;	
	
float Pos_PID_cal(float REF,float FDB)
{
	PEk=REF-FDB;
	PSk+=PEk;
	Pout=1.0*(Pkp*PEk+Pki*PSk+Pkd*PDk);
//	if(Pout>PoutMax)
//		Pout=PoutMax;
//	if(Pout<PoutMin)
//		Pout=PoutMin;
	
	PEk_1=PEk_2;
	PEk_2=PEk;
	
	PDk=PEk_2-PEk_1; 
	
	return Pout/10;
}

float Speed_PID_cal(float REF,float FDB)
{
	SEk=REF-FDB;
	SSk+=SEk;
	Sout=1.0*(Skp*SEk+Ski*SSk+Skd*SDk);
//	if(Sout>SoutMax)
//		Sout=SoutMax;
//	if(Sout<SoutMin)
//		Sout=SoutMin;
	
	SEk_1=SEk_2;
	SEk_2=SEk;
	
	SDk=SEk_2-SEk_1; 
	
	return Sout/10;
}
