#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <malloc.h>
#include "rumfmt2019.h"
#include "erreurs2019.h"

int RUM_V_2019(const char* bufin,char VERS[])
{
 if (strncmp(bufin+OffsetVersionRUM_2019,VERS,LgVersionRUM_2019)==0) return(0);
 else return(1);
 }

/*Fonction retournant une chaine de caractères allouée dynamiquement*/
/*commençant à Rum+Offset et de longueur Longueur, terminant par le caractère 0*/
/*L'appelant doit désallouer la chaine*/
/*
char* ReturnChar_2019(const char* Rum,int Offset,int Longueur)
{
	char* Resultat;
	Resultat = malloc(Longueur+1);
    memcpy(Resultat,&Rum[Offset],Longueur);
	Resultat[Longueur]=0;
	return Resultat;
};
*/
const char* Get_RUM_V_2019(const char* Rum)
{
	/*return ReturnChar_2019(Rum,OffsetVersionRUM_2019,LgVersionRUM_2019);*/
    return Rum+OffsetVersionRUM_2019;
}

int GetVersion_2019(const char* Rum, int Format)
{
	if(Format!=-1)
		return Format;
/*FG10c*/
    if(RUM_V_2019(Rum,"013")==0)
		return 9;
/*FG11*/
    if(RUM_V_2019(Rum,"014")==0)
		return 10;
/*FG11b*/
    if(RUM_V_2019(Rum,"015")==0)
		return 11;
/*FG13*/
    if(RUM_V_2019(Rum,"016")==0)
		return 12;
/*format 017*/
	if ((RUM_V_2019(Rum, "018") == 0) || (RUM_V_2019(Rum, "017") == 0))
		return 13;
/*format 019*/
	if (RUM_V_2019(Rum, "019") == 0)
		return 14;
	return -1;
};

int GetNombreDiagnosticsNum_2019(const char* Rum, int Format)
{
	char tps[3];
	int Resultat;
	
	tps[2] = 0;
	switch (GetVersion_2019(Rum, Format))
	{
/*format 016*/
		case 12: strncpy(tps, Rum + 118, 2);
			break;
/*format 017*/
		case 13: strncpy(tps, Rum + 118, 2);
			break;
/*format 019*/
		case 14: strncpy(tps, Rum + 118, 2);
			break;
		default:
			return -1;
	};
	Resultat=atoi(tps);
	return Resultat;
};

/*CD MAJ FG9*/
int GetNombreDADNum_2019(const char* Rum, int Format)
{
	char tps[3];
	int Resultat;

	tps[2] = 0;
	switch(GetVersion_2019(Rum,Format))
	{
/*format 016*/
	case 12: strncpy(tps, Rum + 120, 2);
			break;
/*format 017*/
	case 13: strncpy(tps, Rum + 120, 2);
			break;
/*format 019*/
	case 14: strncpy(tps, Rum + 120, 2);
		break;
	default:
			return -1;
	};
	Resultat=atoi(tps);
	return Resultat;
};


int GetNombreActesNum_2019(const char* Rum, int Format)
{
	char tps[4];
	int Resultat;

	tps[3] = 0;
	switch(GetVersion_2019(Rum,Format))
	{
/*format 016*/
	case 12: strncpy(tps, Rum + 122, 3);
			break;
/*format 017*/
	case 13: strncpy(tps, Rum + 122, 3);
			break;
/*format 019*/
	case 14: strncpy(tps, Rum + 122, 3);
		break;
	default:
			return -1;
	};
	Resultat=atoi(tps);
	return Resultat;
};


char* TrimDiagnostic_2019(char* Diagnostic)
{
	int i;

	if(Diagnostic[5]==' ')
	{
		i=5;
		while((i>-1) && (Diagnostic[i]==' '))
			Diagnostic[i--]=' ';
	}
	else
	{
		i=5;
		while((i>-1) && (Diagnostic[i]=='+'))
			Diagnostic[i--]=' ';
	};
	return Diagnostic;
};

char* GetDiagnosticPrincipal_2019(const char* Rum, int Format)
{
	char tps[7];

	tps[6] = 0;
	switch(GetVersion_2019(Rum,Format))
	{
/*format 016*/
	case 12: strncpy(tps, Rum + 125, 6); break;
/*format 017*/
	case 13: strncpy(tps, Rum + 125, 6); break;
/*format 019*/
	case 14: strncpy(tps, Rum + 125, 6); break;
	default:
			return NULL;
	};
	return TrimDiagnostic_2019(tps);
};

/*char* CopyDest_2019(char* out,char* in)
{
	strcpy(out,in);
	free(in);
	return out;
};*/

int GetTaille_2019(const char* Rum, int Format)
{
	int tps;
    char zone[3];
	zone[2] = 0;

	switch(GetVersion_2019(Rum,Format))
	{

		/*format 016*/
	case 12:
		strncpy(zone, Rum + 120, 2);
		tps = 177 + 8 * GetNombreDiagnosticsNum_2019(Rum, 12) + 26 * GetNombreActesNum_2019(Rum, 12) + 8 * atoi(zone);
		return tps;
		/*format 017 ou 018*/
	case 13:
		strncpy(zone, Rum + 120, 2);
		tps = 177 + 8 * GetNombreDiagnosticsNum_2019(Rum, 13) + 29 * GetNombreActesNum_2019(Rum, 13) + 8 * atoi(zone);
		return tps;
		/*format 019*/
	case 14:
		strncpy(zone, Rum + 120, 2);
		tps = 177 + 8 * GetNombreDiagnosticsNum_2019(Rum, 14) + 29 * GetNombreActesNum_2019(Rum, 14) + 8 * atoi(zone);
		return tps;
	default:
			return -1;
	};
};


char* RumToInterne_2019(const char* Rum, int Format)
{
	char* tps, TpsDAD[3];
	int Taille ;
	int NBDiags,NBActes;
	int i004,i005,Fin;
	char tpsDiag1[9];
	char *tpsDiag2;
	int n;

    Format = GetVersion_2019(Rum, Format);
    Taille = GetTaille_2019(Rum, Format);
	NBDiags=GetNombreDiagnosticsNum_2019(Rum,Format);
	NBActes=GetNombreActesNum_2019(Rum,Format);
	if((tps=(char*)malloc(Taille+1))==NULL)
	{
		AddErreur_2019(IMPLEMENTATION,3,0);
		return NULL;
	}

	switch(Format)
	{
/*format 016*/
        case 12:
			memcpy(tps,Rum,32);
			memcpy(tps+32,Rum+52,45);

			memcpy(tps+77,"  ",2);	/* les positions 57 et 58 doivent être à " ", "1" ou "2",  */

			memcpy(tps+79,Rum+97,21);

			memcpy(tps+100,"    ",4); /* On remplis la zone réservée par des espaces		  */

			memcpy(tps+104,Rum+118,2);  /* on met le nbre de diags à NDAS */

			memcpy(tps+106,Rum+122,9); /* on passe le nDAD, on traite le nombre d'actes et le DP*/

			strncpy(tpsDiag1,Rum+125,8);
            tpsDiag2=TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps+109,tpsDiag2,6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			strncpy(tpsDiag1,Rum+133,8);
            tpsDiag2=TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps+121,tpsDiag2,6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			memcpy(tps+115,Rum+141,3);      /* on traite l'IGS2         */
			memcpy(tps+118,"    ",3);  /* On remplis la zone réservée par des espaces */

            memcpy(tps+127,Rum+144,3);
            
            /*innovation*/
            memcpy(tps+130,Rum+147,15);
			
            /* Traitement des diagnostics */
			Fin=177+NBDiags*8;
			for(i004=145,i005=177;i005<Fin;i004+=6,i005+=8)
			{
				strncpy(tpsDiag1,Rum+i005,8);
                tpsDiag2=TrimDiagnostic_2019(tpsDiag1);
				memcpy(tps+i004,tpsDiag2,6);   /* on copie les 6 premiers chars pour obtenir un format compatible */
			};

			/* On passe les DAD */
			memcpy(TpsDAD,Rum+120,2);
			TpsDAD[2]=0;
			i005+=8*atoi(TpsDAD);

			/* On traite les actes, en recopiant la fin du rum */
			/*memcpy(tps+i004,Rum+i005,Taille-i004);*/
			for(n=0;n<NBActes;n++)
			{
				memcpy(tps+i004,Rum+i005+8,8);
				memcpy(tps+i004+8,Rum+i005+24,2);
				i004+=10;
				i005+=26;
			}
			break;
/*format 017 ou 018*/
		case 13:
			memcpy(tps, Rum, 32);
			memcpy(tps + 32, Rum + 52, 45);

			memcpy(tps + 77, "  ", 2);	/* les positions 57 et 58 doivent être à " ", "1" ou "2",  */

			memcpy(tps + 79, Rum + 97, 21);

			memcpy(tps + 100, "    ", 4); /* On remplis la zone réservée par des espaces		  */

			memcpy(tps + 104, Rum + 118, 2);  /* on met le nbre de diags à NDAS */

			memcpy(tps + 106, Rum + 122, 9); /* on passe le nDAD, on traite le nombre d'actes et le DP*/

			strncpy(tpsDiag1, Rum + 125, 8);
			tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps + 109, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			strncpy(tpsDiag1, Rum + 133, 8);
			tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps + 121, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			memcpy(tps + 115, Rum + 141, 3);      /* on traite l'IGS2         */
			memcpy(tps + 118, "    ", 3);  /* On remplis la zone réservée fil2 par des espaces */

			memcpy(tps + 127, Rum + 144, 3);

			/*innovation*/
			memcpy(tps + 130, Rum + 147, 15);

			memcpy(tps + 145, Rum + 162, 12);

			/* Traitement des diagnostics */
			Fin = 177 + NBDiags * 8;
			for (i004 = 157, i005 = 177; i005<Fin; i004 += 6, i005 += 8)
			{
				strncpy(tpsDiag1, Rum + i005, 8);
				tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
				memcpy(tps + i004, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */
			};

			/* On passe les DAD */
			memcpy(TpsDAD, Rum + 120, 2);
			TpsDAD[2] = 0;
			i005 += 8 * atoi(TpsDAD);

			/* On traite les actes*/
			for (n = 0; n<NBActes; n++)
			{
				memcpy(tps + i004, Rum + i005 + 8, 7);
				memcpy(tps + i004 + 7, Rum + i005 + 8 + 10, 1);
				memcpy(tps + i004 + 8, Rum + i005 + 27, 2);
				i004 += 10;
				i005 += 29;
			}
			break;
/*format 019*/
		case 14:
			memcpy(tps, Rum, 32);
			memcpy(tps + 32, Rum + 52, 45);

			memcpy(tps + 77, "  ", 2);	/* les positions 57 et 58 doivent être à " ", "1" ou "2",  */

			memcpy(tps + 79, Rum + 97, 21);

			memcpy(tps + 100, "    ", 4); /* On remplis la zone réservée par des espaces		  */

			memcpy(tps + 104, Rum + 118, 2);  /* on met le nbre de diags à NDAS */

			memcpy(tps + 106, Rum + 122, 9); /* on passe le nDAD, on traite le nombre d'actes et le DP*/

			strncpy(tpsDiag1, Rum + 125, 8);
			tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps + 109, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			strncpy(tpsDiag1, Rum + 133, 8);
			tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
			memcpy(tps + 121, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */

			memcpy(tps + 115, Rum + 141, 3);      /* on traite l'IGS2         */
			memcpy(tps + 118, "    ", 3);  /* On remplis la zone réservée fil2 par des espaces */

			memcpy(tps + 127, Rum + 144, 3);

			/*innovation + conversion HP=>HC + indicateur RAAC*/
			memcpy(tps + 130, Rum + 147, 17);

			memcpy(tps + 147, Rum + 164, 10);/*filler*/

			/* Traitement des diagnostics */
			Fin = 177 + NBDiags * 8;
			for (i004 = 157, i005 = 177; i005<Fin; i004 += 6, i005 += 8)
			{
				strncpy(tpsDiag1, Rum + i005, 8);
				tpsDiag2 = TrimDiagnostic_2019(tpsDiag1);
				memcpy(tps + i004, tpsDiag2, 6);   /* on copie les 6 premiers chars pour obtenir un format compatible */
			};

			/* On passe les DAD */
			memcpy(TpsDAD, Rum + 120, 2);
			TpsDAD[2] = 0;
			i005 += 8 * atoi(TpsDAD);

			/* On traite les actes*/
			for (n = 0; n<NBActes; n++)
			{
				memcpy(tps + i004, Rum + i005 + 8, 7);
				memcpy(tps + i004 + 7, Rum + i005 + 8 + 10, 1);
				memcpy(tps + i004 + 8, Rum + i005 + 27, 2);
				i004 += 10;
				i005 += 29;
			}
			break;
		default:
			free(tps);
            tps=NULL;
	}
	return tps;
}
