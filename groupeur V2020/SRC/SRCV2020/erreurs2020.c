#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "erreurs2020.h"



static int posErrCtrl_2020=0;
static int posErrArb_2020=0;
static int posErrImpl_2020=0;


void InitErreur_2020(int ErrCtrl[],int ErrArb[],int ErrImpl[],int nbsej)
{
    int i;
    for(i=0;i<MAXERRCTRL_2020;i++)ErrCtrl[i]=0;
    ErrCtrl[0] = nbsej ;
    posErrCtrl_2020=nbsej+1;
    posErrImpl_2020=0;
    posErrArb_2020=0;
    for(i=0;i<MAXERRARB_2020;i++)ErrArb[i]=0;
    for(i=0;i<MAXERRIMPL_2020;i++)ErrImpl[i]=0;
};

void AddErreur_2020(int FormatErr,int Erreur,int norum)
{

	switch(FormatErr)
	{
		case   IMPLEMENTATION:
                       if ( posErrImpl_2020 < MAXERRIMPL_2020 ) ErrImpl_2020[posErrImpl_2020++]=Erreur;
                       else ErrImpl_2020[posErrImpl_2020]=2;
		       break;
		case   GROUPAGE:
                       if ( posErrArb_2020 < MAXERRARB_2020 ) ErrArb_2020[posErrArb_2020++]=Erreur;
                       else {
                          if ( posErrImpl_2020 < MAXERRIMPL_2020 ) ErrImpl_2020[posErrImpl_2020++]=Erreur;
                          else ErrImpl_2020[posErrImpl_2020]=2;
                        }
		       break;
		case   CONTROLE:
                       if ( posErrCtrl_2020 < MAXERRCTRL_2020 ) ErrCtrl_2020[posErrCtrl_2020++]=Erreur;
                       else {
                          if ( posErrImpl_2020 < MAXERRIMPL_2020 ) ErrImpl_2020[posErrImpl_2020++]=Erreur;
                          else ErrImpl_2020[posErrImpl_2020]=2;
                        }
                        ErrCtrl_2020[norum+1]++;  /* Ajout d'une erreur pour le compteur de RUM */
			break;
		default : if ( posErrImpl_2020 < MAXERRIMPL_2020 )
					  ErrImpl_2020[posErrImpl_2020++]=Erreur;
                  else
					  ErrImpl_2020[posErrImpl_2020]=1;
  			  break;
	}
}
