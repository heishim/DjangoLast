#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "erreurs2019.h"



static int posErrCtrl_2019=0;
static int posErrArb_2019=0;
static int posErrImpl_2019=0;


void InitErreur_2019(int ErrCtrl[],int ErrArb[],int ErrImpl[],int nbsej)
{
    int i;
    for(i=0;i<MAXERRCTRL_2019;i++)ErrCtrl[i]=0;
    ErrCtrl[0] = nbsej ;
    posErrCtrl_2019=nbsej+1;
    posErrImpl_2019=0;
    posErrArb_2019=0;
    for(i=0;i<MAXERRARB_2019;i++)ErrArb[i]=0;
    for(i=0;i<MAXERRIMPL_2019;i++)ErrImpl[i]=0;
};

void AddErreur_2019(int FormatErr,int Erreur,int norum)
{

	switch(FormatErr)
	{
		case   IMPLEMENTATION:
                       if ( posErrImpl_2019 < MAXERRIMPL_2019 ) ErrImpl_2019[posErrImpl_2019++]=Erreur;
                       else ErrImpl_2019[posErrImpl_2019]=2;
		       break;
		case   GROUPAGE:
                       if ( posErrArb_2019 < MAXERRARB_2019 ) ErrArb_2019[posErrArb_2019++]=Erreur;
                       else {
                          if ( posErrImpl_2019 < MAXERRIMPL_2019 ) ErrImpl_2019[posErrImpl_2019++]=Erreur;
                          else ErrImpl_2019[posErrImpl_2019]=2;
                        }
		       break;
		case   CONTROLE:
                       if ( posErrCtrl_2019 < MAXERRCTRL_2019 ) ErrCtrl_2019[posErrCtrl_2019++]=Erreur;
                       else {
                          if ( posErrImpl_2019 < MAXERRIMPL_2019 ) ErrImpl_2019[posErrImpl_2019++]=Erreur;
                          else ErrImpl_2019[posErrImpl_2019]=2;
                        }
                        ErrCtrl_2019[norum+1]++;  /* Ajout d'une erreur pour le compteur de RUM */
			break;
		default : if ( posErrImpl_2019 < MAXERRIMPL_2019 )
					  ErrImpl_2019[posErrImpl_2019++]=Erreur;
                  else
					  ErrImpl_2019[posErrImpl_2019]=1;
  			  break;
	}
}
