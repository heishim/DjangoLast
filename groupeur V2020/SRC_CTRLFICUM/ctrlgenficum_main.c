#include <stdio.h>
#include <stdlib.h>
#include "ctrlgenficum.h"


int main(int argc, char *argv[])
{
    int res;
    char* msgerr_ium;
    if(argc<7)
	{
        printf("syntaxe : %s {nom complet du fichier d'UM source} {nom complet du fichier d'UM a generer} {repertoire cible (finissant par /)} {fichier log 0:non 1:oui} {gestion portee globale 0:non 1:oui} {inclure FINESS geographique 0:non 1:oui}\n",argv[0]);
		return(1);
	}
    res = CtrlFicUM(argv[1],argv[2],argv[3],atoi(argv[4]),atoi(argv[5]),atoi(argv[6]));
    if(res==0)
        fprintf(stdout,"generation du fichier ficum.txt reussie.\n");
    else
    {
        switch(res)
        {
            case 1 : msgerr_ium="impossible d'ouvrir le fichier";break;
            case 2 : msgerr_ium="impossible de creer le fichier de travail";break;
            case 3 : msgerr_ium="impossible de creer le fichier de rapport";break;
            case 4 : msgerr_ium="erreurs de format ou dates incoherentes detectees";break;
            case 5 : msgerr_ium="au moins deux dates de debut d''autorisation identiques";break;
            case 6 : msgerr_ium="erreur système";break;
            case 8 : msgerr_ium="Type d'autorisation non acceptee avec l'UM reservee $$$$";break;
            case 9 : msgerr_ium="fichier de références des autorisations d'UM corrompu";break;
			case 10: msgerr_ium="au moins deux dates de début d''autorisation identiques pour une autorisation globale au finess géographique"; break;
			case 11: msgerr_ium = "Le type d'autorisation 72 IVG n'est plus accepté au 1er mars 2019."; break;
			default: msgerr_ium="erreur inconnue.";
        }
        fprintf(stdout,"Le fichier ficum.txt n'a pas ete genere car :  %s\n",msgerr_ium);
    }
 }
