#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>


#include "grpstruct2020.h"

#include "fg2020.h"
#include "rumfmt2020.h"
#include "erreurs2020.h"

#include "ccam2020.h"

#include "gestionUM2020.h"

#define ASCII_MOD
#define INITIALCRC    0xFFFFFFFFFFFFFFFFULL
#define POLY64REV     0x95AC9329AC4BC9B5ULL

int nbactes_2020 = 0;
int nbdiags_2020 = 0;
int nbsejR_2020;

/*CD Maj FG11b*/
/*indique si un RUM a les conditions réunies pour le supplément REA dans une UM de REA (au moins un acte marqueur de la liste 1*/
/*ou au moins 3 actes de la liste 2*/
int bActesMarqueursUMREA_2020[tab_max];

/*CD FG10B*/
#define MAX_BUFFER_FIC 2000000
unsigned char buffic_2020[MAX_BUFFER_FIC];

/* CG Macro d'arrondi*/
#define   nint( x ) (x>0.0?(int)(x+0.5):(int)(x-0.5))

int puisi2_2020[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
int puis26_2020[3] = { 676, 26, 1 };

int groupeRacine;/*groupe racine en fonction du poids et de l'AG*/
int annuleActeOperGHMMed;/*permet d'ignorer l'erreur due à un acte opératoire reclassant dans un GHM médical*/

/**** FONCTIONS DE TESTS UTILISEES PAR LE CALCUL DU GHS****/
int autorisationUMGHS_2020;
/*Test n°0
  Test renvoyant toujours 1 (test réussi)*/
int _Vrai_2020(int param)
{
    return 1;
}

/*Test n°1
Renvoie 1 si il existe au moins 1 acte ayant la
caractéristique passée en paramètre param
Renvoie 0 sinon*/
int _acteCara_2020(int param)
{
    int i;
    unsigned int a, b;

    for (i = 0; i < nbac_2020; i++)
    {
        a = *(ptr_amask_2020 + (param / 256) + *(ptr_indacte_2020 + i)*_prmsize_2020);
        b = param % 256;
        if (a & b)
            return(1);
    }
    return(0);
}

/*24/01/07 modification du Test n°2:
Renvoie 1 si au moins un des jours du RSS est
autorisé en type passé en paramètre, renvoie 0 sinon
*/
int _typeUM_2020(int param)
{
    int i, nbj;
    autorisationUMGHS_2020 = param;

    if (autorisation_valide_2020(param) == 0)
        return 0;

    for (i = 0; i < nbsej_2020; i++)
    {
        if (sejourAutoriseEnTypAutor_2020(i, param, rumin_2020[i].datent, rumin_2020[i].datsor, &nbj))
            return 1;
    }
    return 0;
}

/*Test n°3
Renvoie 1 si il existe au moins 1 RUM
avec un type de lit dédié = param
Renvoie 0 sinon*/
int _typeLitdedie_2020(int param)
{
    int i;
    int typelitdednum;
    char zone[3];
    zone[2] = 0;

    for (i = 0; i < nbsej_2020; i++)
    {
        strncpy(zone, rumin_2020[i].litded, 2);
        typelitdednum = atoi(zone);
        if (typelitdednum == param)
            return 1;
    }
    return 0;
}

/*Test n°4
Renvoie 1 si la durée totale du séjour est
strictement supérieure à param
Renvoie 0 sinon*/
int _dureeTotSejStrictSup_2020(int param)
{
    if (_dsj_2020 > param)
        return 1;
    return 0;
}
/*Test n°5
Renvoie 1 si le DP à la caractéristique passée en paramètre
Renvoie 0 sinon*/
int _dpcarac_2020(int param)
{
    unsigned int a, b;
    a = *(ptr_dmask_2020 + (param / 256) + *(ptr_inddiag_2020)*_dxmsize_2020);
    b = param % 256;
    if (a & b)
        return(1);
    else
        return(0);
}

/*Test n°6
Renvoie 1 si le nombre total de journées passées dans une unité de type passé en paramètre au Test n°2
est supérieur au paramètre param passé à cette fonction
Renvoie 0 sinon
Attention : Ne marche pas pour une autorisation globale au finess géo*/
int _dureeSejPartielleStrictSup_2020(int param)
{
    int i;
    char datetemp[9];
    int dernierjourj, dernierjourm, dernierjoura;
    int nbjours_sup, nbjours_sup_sej, premjaut;

    nbjours_sup = 0;
    for (i = 0; i<nbsej_2020; i++)
    {
        if (los_2020[i]>0)
        {
            Ajout_jour_date_2020(rumin_2020[i].datent, los_2020[i] - 1, &dernierjourj, &dernierjourm, &dernierjoura);
            sprintf(datetemp, "%02d%02d%04d", dernierjourj, dernierjourm, dernierjoura);
        }
        else
        {
            strncpy(datetemp, rumin_2020[i].datsor, 8);
        }

        nbjours_sup_sej = 0;
        if (est_autorise_2020(indice_ums_2020[i], autorisationUMGHS_2020, rumin_2020[i].datent, datetemp, &nbjours_sup_sej, &premjaut))
            nbjours_sup += nbjours_sup_sej;
    }
    if (nbjours_sup > param)
        return 1;
    return 0;
}

/*Test n°7
Renvoie 1 si un des DAS à la caractéristique passée en paramètre
Renvoie 0 sinon*/
int _dascarac_2020(int param)
{
    unsigned int a, b;
    int i;
    for (i = 2; i < nbdc_2020; i++)
    {
        a = *(ptr_dmask_2020 + (param / 256) + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        b = param % 256;
        if (a & b)
            return(1);
    }
    return 0;
}
/*Test n°8
Renvoie 1 si l'age est supérieur ou égal au paramètre
Renvoie 0 sinon*/
int _agesup_2020(int param)
{
    if (age_an_2020 >= param)
        return 1;
    else
        return 0 ;
}
/*Test n°9
Renvoie 1 si les critères pour forfait diabète sont réunis
Renvoie 0 sinon*/
int _criteres_forfdiabete_2020(int param)
{
	unsigned int a, b;
	int dpok, drok;
	int numcaracdiabete;
	

	/*durée de séjour*/
	if (_dsj_2020 >= param)
		return 0;
	
	/*autorisation globale diabète*/
	if (!_typeUM_2020(TYPUM_DIABETE))
		return 0;
	
	/*DP ou DR appartient à la liste*/
	numcaracdiabete = 8224; /*liste diabète*/

	dpok = drok = 0;
	a = *(ptr_dmask_2020 + (numcaracdiabete / 256) + *(ptr_inddiag_2020)*_dxmsize_2020);
	b = numcaracdiabete % 256;
	if (a & b)
		dpok=1;
	if (!dpok)
	{
		a = *(ptr_dmask_2020 + (numcaracdiabete / 256) + *(ptr_inddiag_2020 + 1)*_dxmsize_2020);
		b = numcaracdiabete % 256;
		if (a & b)
			drok = 1;
	}
	if (!dpok && !drok)
		return 0;

	/*mode entrée domicile hors urgence, mode de sortie domicile*/
	if ((_me_2020 != 8) || (_ms_2020 != 8) || (_pr_2020 == 5))
		return 0;

	sejEliForfDiabete_2020 = 1;
	return 1;
}

/*Test n°10
le séjour n'est il pas exclus ni par la variable gradationHDJ ni par les 3 variables du RUM ?
si rien ne l'exclut, la catégorie du nombre d'interventions est elle égale à param?
param : la catégorie du nombre d'interventions
1 si catégorie A ou blanc
2 si catégorie B
3 si catégorie C
Renvoie 0 sinon*/
int _criteres_GHSgradation_2020(int param)
{
	int i;
	int catnbinterv;
	if (gradationHDJ_2020>0)
	{
		for (i = 0; i < nbsej_2020; i++)
		{
			if ((rumin_2020[i].contextSurv == '1') ||
				(rumin_2020[i].adminProdRH == '1') ||
				(rumin_2020[i].rescrit == '1'))
				break;
		}
		if (i == nbsej_2020)
		{
			/*aucune des 3 variables n'exclut la gradation*/
			/*on regarde la valeur de la catégorie du nombre d'intervention dans le (les?) RUM*/
			catnbinterv = 0;
			for (i = 0; i < nbsej_2020; i++)
			{
				if (rumin_2020[i].categNbinterventions != ' ')
				{
					catnbinterv = rumin_2020[i].categNbinterventions - 'A' + 1;
					break;
				}
			}
			if (catnbinterv < 0)
				catnbinterv = 0;

			if (param == 1)
			{
				if (catnbinterv == 0) /*catégorie non renseignée*/
				{
					AddErreur_2020(CONTROLE, ERR_INDICCATEGNBINTERV_ATTENDU, nbsej_2020 - 1);
					return 1;
				}
				else
					if (catnbinterv == 1) /*catégorie A externe*/
					{
						AddErreur_2020(CONTROLE, ERR_SEJOURNONJUSTIFIE_INCORRECT, nbsej_2020 - 1);
						sejourHdjexterne_2020 = 1;
						return 1;
					}
			}
			else
			{
				if (catnbinterv == param)
					return 1;
			}
		}
	}
	return 0;
}


/**** FIN DES FONCTIONS DE TESTS UTILISEES PAR LE CALCUL DU GHS****/

int ErreurCtrlBloquante_2020(int numerr)
{
    int i, erreur_courante;
    for (i = 0; i<MAXERRBLOQ_2020; i++)
    {
        erreur_courante = erreursbloq_2020[i];
        if (erreur_courante == -1)
            break;
        if (erreur_courante == numerr)
            return 1;
        if (erreur_courante>numerr)
            break;
    }
    return 0;
}

short my_index_2020(char s1[], char s2[], short nb)
{
    int i, j, k, l;
    l = 0;
    for (i = 0; s1[i] != '\0'; i++)
    {
        for (j = i, k = 0; s2[k] != '\0' && s1[j] == s2[k]; j++, k++);
        if (s2[k] == '\0')
        {
            l++;
            if (l == nb)return(i);
        }
    }
    return(-1);
}

/* FERMETURE DES FICHIERS */
void grpclose_2020()
{
    int i;
    if (fptrum_2020 != NULL)
    {
        fclose(fptrum_2020);
        fptrum_2020 = NULL;
    }

    for (i = 0; i < NB_TABLES_BINAIRES_2020; i++)
    {
        if (multitables_2020[i].fileptr != NULL)
        {
            fclose(multitables_2020[i].fileptr);
            multitables_2020[i].fileptr = NULL;
            /*FG 11e:TEM*/
            free(multitables_2020[i].htable);
            multitables_2020[i].htable = NULL;
        }
    }
    ptaccam_2020 = NULL;
    ptmdiag10_2020 = NULL;
    pttree_2020 = NULL;
    ghsinf_2020 = NULL;
    ptfidx_gc_2020 = NULL;
    ghminf_2020 = NULL;
    srcdgac_2020 = NULL;
    innovref_2020 = NULL;
    tabcombi_2020 = NULL;
    ptccamdesc_2020 = NULL;
    ptghsminor_2020 = NULL;

    /*TEM*/
    h_ptaccam_2020 = NULL;
    h_ptmdiag10_2020 = NULL;
    h_pttree_2020 = NULL;
    h_ghsinf_2020 = NULL;
    h_ptfidx_gc_2020 = NULL;
    h_ghminf_2020 = NULL;
    h_srcdgac_2020 = NULL;
    h_innovref_2020 = NULL;
    h_tabcombi_2020 = NULL;
    h_refauto_2020 = NULL;
    h_ptccamdesc_2020 = NULL;
    h_ptghsminor_2020 = NULL;
}

/*CD 25/01/07*/
int date_strict_sup_2020(char *date1, char *date2)
{
    char  datang1[8];
    char  datang2[8];

    strncpy(datang1, date1 + 4, 4);
    strncpy(datang1 + 4, date1 + 2, 2);
    strncpy(datang1 + 6, date1, 2);

    strncpy(datang2, date2 + 4, 4);
    strncpy(datang2 + 4, date2 + 2, 2);
    strncpy(datang2 + 6, date2, 2);

    return (strncmp(datang1, datang2, 8) > 0);
}
int comparer_dates_2020(char *date1, char *date2)
{
    char  datang1[8];
    char  datang2[8];

    strncpy(datang1, date1 + 4, 4);
    strncpy(datang1 + 4, date1 + 2, 2);
    strncpy(datang1 + 6, date1, 2);

    strncpy(datang2, date2 + 4, 4);
    strncpy(datang2 + 4, date2 + 2, 2);
    strncpy(datang2 + 6, date2, 2);

    return (strncmp(datang1, datang2, 8));
}

int optree_2020()
{
    long taille;
    long loffs;
    struct multiTable_2020 mtable;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_ARBRE_2020];
    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);
    accstb_2020(mtable.fileptr,&loffs,&_trsize_2020,&taille);
    */
    ptoffft = h_pttree_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _trsize_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _toffset_2020 = loffs + mtable.offset_table[mtable.periodecour];

    /*FG 11e:TEM : on a besoin de verc car c'est utilisé dans la fonction ChoixTables*/
    strncpy(entete_table_2020.verc, h_pttree_2020 + mtable.offset_table[mtable.periodecour] + 8, 2);

    return(0);

} /* fin de la fonction optree_2020 */



/***********************************************************
 *                                                         *
 *  fonction de chargement des tables de codes diags  cim10*
 *                                                         *
 ***********************************************************/
int opdiag10_2020()
{
    long taille, tailtot;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    unsigned char *ptoffft;

    mtable = multitables_2020[TABLE_DIAG_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);*/
    ptoffft = h_ptmdiag10_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);

    tailtot = 0;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_dsizepref_2020,&taille);*/
    loffs = carlong_2020(ptoffft + 26);
    _dsizepref_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _doffpref_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_dsize_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _dsize_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _doffnop10_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;



    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_dxmsize_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _dxmsize_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _doffpro10_2020 = loffs + mtable.offset_table[mtable.periodecour];
    _dxmsize_2020 /= 2;
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_prfsize_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _prfsize_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _doffoms10_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_excsize10_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _excsize10_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _doffexc10_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    return(0);
} /* fin de la fonction opdiag */

/***********************************************************
 *                                                         *
 *  fonction de chargement des tables de codes diags  cim10*
 *                                                         *
 ***********************************************************/
int opacccam_2020()
{
    long taille, tailtot;
    long loffs;
    struct multiTable_2020 mtable;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_ACTES_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);*/
    ptoffft = h_ptaccam_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    tailtot = 0;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_psizepref_ccam_2020,&taille);*/
    loffs = carlong_2020(ptoffft + 26);
    _psizepref_ccam_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _poffpref_ccam_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_psize_ccam_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _psize_ccam_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _poffnop_ccam_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_prmsize_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _prmsize_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _poffpro_ccam_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    return(0);
} /* fin de la fonction opacccam */



/***********************************************************
 *                                                         *
 *  fonction de chargement de la table                     *
 *  contenant les associations DP/Actes pour les           *
 *  suppléments SRC                                        *
 ***********************************************************/
int opsrcdgac_2020()
{
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_SRCDGACT_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);*/
    ptoffft = h_srcdgac_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);

    /*adultes+enfants*/
    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleSRCDGACTADENF_2020,&_TailleTotaleTableSRCDGACTADENF_2020);*/
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleSRCDGACTADENF_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableSRCDGACTADENF_2020 = carlong_2020(ptoffft + 22);

    /*FG 11e:TEM
    _NbArticlesSRCDGACTADENF_2020=entete_stab_2020.nastn[0]*256+entete_stab_2020.nastn[1];*/
    _NbArticlesSRCDGACTADENF_2020 = *(ptoffft + 18) * 256 + *(ptoffft + 19);

    _offstSRCDGACTADENF_2020 = loffs + mtable.offset_table[mtable.periodecour];

    /*enfants seulement*/
    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleSRCDGACTENFANT_2020,&_TailleTotaleTableSRCDGACTENFANT_2020);
    _NbArticlesSRCDGACTENFANT_2020=entete_stab_2020.nastn[0]*256+entete_stab_2020.nastn[1];*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleSRCDGACTENFANT_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableSRCDGACTENFANT_2020 = carlong_2020(ptoffft + 22);
    _NbArticlesSRCDGACTENFANT_2020 = *(ptoffft + 18) * 256 + *(ptoffft + 19);

    _offstSRCDGACTENFANT_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
} /* fin de la fonction opsrcdgac_2020 */


int opghm_2020()
{
    long tailtot = 0;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_GHMINFO_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);
    accstb_2020(mtable.fileptr,&loffs,&_lTailleArticleGHM_2020,&_lTailleTotaleTableGHM_2020);*/
    ptoffft = h_ghminf_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _lTailleArticleGHM_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _lTailleTotaleTableGHM_2020 = carlong_2020(ptoffft + 22);

    _offstghm_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
}

int opghs_2020()
{
    long tailtot = 0;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_GHS_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);
    accstb_2020(mtable.fileptr,&loffs,&_lTailleArticleGHS_2020,&_lTailleTotaleTableGHS_2020);*/
    ptoffft = h_ghsinf_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _lTailleArticleGHS_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _lTailleTotaleTableGHS_2020 = carlong_2020(ptoffft + 22);

    _offstghs_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
}
/*FG13 innovation*/
int opinnov_2020()
{
    long tailtot = 0;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_INNOVREFER_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleInnovref_2020,&_TailleTotaleTableInnovref_2020);*/
    ptoffft = h_innovref_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleInnovref_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableInnovref_2020 = carlong_2020(ptoffft + 22);

    _offstinnovref_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
}

/*FG1311d tableau des combinatoires*/
int optabcombi_2020()
{
    long tailtot = 0;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_COMBI_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);*/
    ptoffft = h_tabcombi_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleCombiPoidsAG_2020,&_TailleTotaleTableCombiPoidsAG_2020);*/
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleCombiPoidsAG_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableCombiPoidsAG_2020 = carlong_2020(ptoffft + 22);

    _offstcombiPoidsAG_2020 = loffs + mtable.offset_table[mtable.periodecour];

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleCombiAGNivA_2020,&_TailleTotaleTableCombiAGNivA_2020);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleCombiAGNivA_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableCombiAGNivA_2020 = carlong_2020(ptoffft + 22);

    _offstcombiAGNivA_2020 = loffs + mtable.offset_table[mtable.periodecour];

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleCombiAGNivB_2020,&_TailleTotaleTableCombiAGNivB_2020);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleCombiAGNivB_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableCombiAGNivB_2020 = carlong_2020(ptoffft + 22);

    _offstcombiAGNivB_2020 = loffs + mtable.offset_table[mtable.periodecour];

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_TailleArticleCombiAGNivC_2020,&_TailleTotaleTableCombiAGNivC_2020);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _TailleArticleCombiAGNivC_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _TailleTotaleTableCombiAGNivC_2020 = carlong_2020(ptoffft + 22);

    _offstcombiAGNivC_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
}
/*FG11g table des actes CCAM descriptive*/
int opccamdesc_2020()
{
    long taille, tailtot;
    long loffs;
    struct multiTable_2020 mtable;
    char *ptoffft;

    mtable = multitables_2020[TABLE_CCAMDESC_2020];

    ptoffft = h_ptccamdesc_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    tailtot = 0;

    loffs = carlong_2020(ptoffft + 26);
    _psizepref_ccamdesc_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _poffpref_ccamdesc_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;

    /*FG 11e:TEM
    accstb_2020(mtable.fileptr,&loffs,&_psize_ccam_2020,&taille);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _psize_ccamdesc_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    taille = carlong_2020(ptoffft + 22);

    _poffnop_ccamdesc_2020 = loffs + mtable.offset_table[mtable.periodecour];
    tailtot += taille;
    return(0);
}
int opghsminor_2020()
{
    long tailtot = 0;
    struct multiTable_2020 mtable;
    long loffs;
    /*FG 11e:TEM*/
    char *ptoffft;

    mtable = multitables_2020[TABLE_GHSMINOR_2020];

    /*FG 11e:TEM
    fseek(mtable.fileptr,mtable.offset_table[mtable.periodecour],SEEK_SET);
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,mtable.fileptr);
    accstb_2020(mtable.fileptr,&loffs,&_lTailleArticleGHS_2020,&_lTailleTotaleTableGHS_2020);*/
    ptoffft = h_ptghsminor_2020 + mtable.offset_table[mtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _lTailleArticleGHSMINOR_2020 = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _lTailleTotaleTableGHSMINOR_2020 = carlong_2020(ptoffft + 22);

    _offstghsminor_2020 = loffs + mtable.offset_table[mtable.periodecour];

    return(0);
}

int chargeRefAutorisationsCourantes_2020()
{
    int i;
    FILE *lfptr;
    long loffs;
    int lnbart;
    struct multiTable_2020 lmtable;
    unsigned char article[4];
    int _lTailleArticleREFAUTO_UM;
    int _lTailleArticleREFAUTO_LITDED;
    long _lTailleTotaleTableREFAUTO_UM;
    long _lTailleTotaleTableREFAUTO_LITDED;
    long _offsttabref_um;
    long _offsttabref_litded;
    char *ptoffft;/*FG 11e:TEM*/

    umrefs_2020.nbaut = 0;
    litdedref_2020.nbaut = 0;
    memset(&umrefs_2020, 0, sizeof(struct _autref_2020));
    memset(&litdedref_2020, 0, sizeof(struct _autref_2020));

    lmtable = multitables_2020[TABLE_REFAUTO_2020];
    lfptr = lmtable.fileptr;

    /*FG 11e:TEM
    fseek(lfptr,lmtable.offset_table[lmtable.periodecour],SEEK_SET);*/

    /*entete général*/
    /*FG 11e:TEM
    fread(entete_table_2020.sign,sizeof(struct ENTETE_2020),1,lmtable.fileptr);*/
    /*lit dédiés*/
    /*FG 11e:TEM
    accstb_2020(lfptr,&loffs,&_lTailleArticleREFAUTO_LITDED,&_lTailleTotaleTableREFAUTO_LITDED);*/

    ptoffft = h_refauto_2020 + lmtable.offset_table[lmtable.periodecour] + sizeof(struct ENTETE_2020);
    loffs = carlong_2020(ptoffft + 26);
    _lTailleArticleREFAUTO_LITDED = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _lTailleTotaleTableREFAUTO_LITDED = carlong_2020(ptoffft + 22);

    _offsttabref_litded = loffs + lmtable.offset_table[lmtable.periodecour];

    /*UM*/
    /*FG 11e:TEM
    accstb_2020(lfptr,&loffs,&_lTailleArticleREFAUTO_UM,&_lTailleTotaleTableREFAUTO_UM);*/
    ptoffft += sizeof(struct entete_2020);
    loffs = carlong_2020(ptoffft + 26);
    _lTailleArticleREFAUTO_UM = *(ptoffft + 20) * 256 + *(ptoffft + 21);
    _lTailleTotaleTableREFAUTO_UM = carlong_2020(ptoffft + 22);

    _offsttabref_um = loffs + lmtable.offset_table[lmtable.periodecour];

    /*chargement des references de lits dédiés*/
    /*FG 11e:TEM
    fseek(lfptr,_offsttabref_litded,SEEK_SET);*/
    if (_lTailleArticleREFAUTO_LITDED == 0) /*precaution*/
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return 0;
    }
    lnbart = _lTailleTotaleTableREFAUTO_LITDED / _lTailleArticleREFAUTO_LITDED;
    litdedref_2020.nbaut = lnbart;
    for (i = 0; i < lnbart; i++)
    {
        /*FG 11e:TEM
        fread(article,sizeof(char),2,lfptr);*/
        memcpy(article, h_refauto_2020 + _offsttabref_litded + i*sizeof(char)* 2, sizeof(char)* 2);
        litdedref_2020.bvalide[article[0]] = 1;
        litdedref_2020.numfoncval[article[0]] = article[1];
    }

    /*chargement des references d'autorisation d'UM*/
    /*FG 11e:TEM
    fseek(lfptr,_offsttabref_um,SEEK_SET);*/
    if (_lTailleArticleREFAUTO_UM == 0) /*precaution*/
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return 0;
    }
    lnbart = _lTailleTotaleTableREFAUTO_UM / _lTailleArticleREFAUTO_UM;
    umrefs_2020.nbaut = lnbart;
    for (i = 0; i < lnbart; i++)
    {
        /*FG 11e:TEM
        fread(article,sizeof(char),3,lfptr);*/
        memcpy(article, h_refauto_2020 + _offsttabref_um + i*sizeof(char)* 3, sizeof(char)* 3);
        umrefs_2020.bvalide[article[0]] = 1;
        umrefs_2020.numfoncval[article[0]] = article[1];
        umrefs_2020.typeportee[article[0]] = article[2];
    }
    return 0;
}

void free_ptr_2020()
{
    if (ptr_omask_2020 != NULL)
    {
        free(ptr_omask_2020);
        ptr_omask_2020 = NULL;
    }
    if (ptr_emask_2020 != NULL)
    {
        free(ptr_emask_2020);
        ptr_emask_2020 = NULL;
    }
    if (ptr_dmask_2020 != NULL)
    {
        free(ptr_dmask_2020);
        ptr_dmask_2020 = NULL;
    }
    if (ptr_diags_2020 != NULL)
    {
        free(ptr_diags_2020);
        ptr_diags_2020 = NULL;
    }
    if (ptr_inddiag_2020 != NULL)
    {
        free(ptr_inddiag_2020);
        ptr_inddiag_2020 = NULL;
    }
    if (ptr_indacte_2020 != NULL)
    {
        free(ptr_indacte_2020);
        ptr_indacte_2020 = NULL;
    }
    if (ptr_amask_2020 != NULL)
    {
        free(ptr_amask_2020);
        ptr_amask_2020 = NULL;
    }
    /*CD MAJ FG10*/
    if (ptr_typact_2020 != NULL)
    {
        free(ptr_typact_2020);
        ptr_typact_2020 = NULL;
    }
    if (ptr_datfin_2020 != NULL)
    {
        free(ptr_datfin_2020);
        ptr_datfin_2020 = NULL;
    }
    if (ptr_datdeb_2020 != NULL)
    {
        free(ptr_datdeb_2020);
        ptr_datdeb_2020 = NULL;
    }

    if (ptr_actes_2020 != NULL)
    {
        free(ptr_actes_2020);
        ptr_actes_2020 = NULL;
    }
    if (ptr_nbactes_2020 != NULL)
    {
        free(ptr_nbactes_2020);
        ptr_nbactes_2020 = NULL;
    }
    /*CD*/
    if (lrss_inf_2020 != NULL)
    {
        if (lrss_inf_2020->actlist.listzac != NULL)
        {
            free(lrss_inf_2020->actlist.listzac);
            lrss_inf_2020->actlist.listzac = NULL;
        }
        if (lrss_inf_2020->diaglist.listdiag != NULL)
        {
            free(lrss_inf_2020->diaglist.listdiag);
            lrss_inf_2020->diaglist.listdiag = NULL;
        }
    }
}
void accstb_2020(FILE *fich, long *_offset, int *_size, long *_taille)
{
    fread(entete_stab_2020.sgstn, 1, sizeof(struct entete_2020), fich);
    *_size = entete_stab_2020.lastn[0] * 256 + entete_stab_2020.lastn[1];
    *_offset = carlong_2020(entete_stab_2020.ofstn);
    *_taille = carlong_2020(entete_stab_2020.lgstn);
}
int errfic_2020(char *rum[], int ret, int type, GRP_RESULTAT_2020 *grp_res, GHS_VALORISATION_2020 *ghs_val, RSS_INFO_2020 *rss_inf)
{
    AddErreur_2020(type, ret, 0);
    grpclose_2020();
    return FreeErr_2020(rum, ret, type, grp_res, ghs_val, rss_inf);
}

/* Chargement des diagnostics :
  On met en premier le DP du RUM principal
  Puis on met les DP des RUM non principal
  Puis on met les DA dans l'ordre des RUMs
  sejp contient le numéro du RUM principal
  nbdc contiendra le nombre de diagnostics chargés
  Remplissage de ptr_inddiag_2020 */
void charged_2020(int sejp, int *nbdc)
{
    int *ptr_trav;
    int i, j;

    ptr_trav = ptr_inddiag_2020;
    *ptr_trav++ = inddiag_2020[sejp];
    (*nbdc)++;
    /* Chargement du DR */
    /* Ajout pour ne pas avoir des doublons DP DR : mise à blanc du DR en cas de doublon*/
    /* modif du 19/11 CD
        if(strncmp(ptr_diags_2020+(inddiag_2020[sejp]*lgdiag_2020),ptr_diags_2020+(inddiag_2020[sejp]+1)*lgdiag_2020,lgdiag_2020)!=0)*/
    if (strncmp(ptr_diags_2020 + (inddiag_2020[sejp] * lgdiag_2020), ptr_diags_2020 + (inddiag_2020[sejp] + 1)*lgdiag_2020, lgdiag_2020) == 0)
        strncpy(ptr_diags_2020 + (inddiag_2020[sejp] + 1)*lgdiag_2020, "        ", lgdiag_2020);

    *ptr_trav++ = inddiag_2020[sejp] + 1;
    (*nbdc)++;

    for (i = 0; i < nbsej_2020; i++)
    if (i != sejp)
        /*ptr_inddiag_2020 : liste courante d'indices retenues*/
    if (!doublon_2020(inddiag_2020[i], ptr_inddiag_2020, *nbdc, ptr_diags_2020, lgdiag_2020))
    {
        *ptr_trav++ = inddiag_2020[i];
        (*nbdc)++;
    }

    for (i = 0; i < nbsej_2020; i++)
    for (j = inddiag_2020[i] + (i == sejp ? 2 : 1); j < inddiag_2020[i + 1]; j++)
        /*CD MAJ FG10 plus de filtre
        if (!doublon_2020(j,ptr_inddiag_2020,*nbdc,ptr_diags_2020,lgdiag_2020) && *(ptr_diags_2020+j*lgdiag_2020)!='U'
        && *(ptr_diags_2020+j*lgdiag_2020)!='V' && *(ptr_diags_2020+j*lgdiag_2020)!='W' && *(ptr_diags_2020+j*lgdiag_2020)!='X'
        && *(ptr_diags_2020+j*lgdiag_2020)!='Y' && strncmp(ptr_diags_2020+j*lgdiag_2020,"      ",lgdiag_2020)!=0)*/
    if (!doublon_2020(j, ptr_inddiag_2020, *nbdc, ptr_diags_2020, lgdiag_2020) && strncmp(ptr_diags_2020 + j*lgdiag_2020, "      ", lgdiag_2020) != 0)
    {
        *ptr_trav++ = j;
        (*nbdc)++;
    }
}

int doublon_2020(int indcour, int *ptr_indice, int nb, char *ptr_diact, int lg)
{
    int i;
    for (i = 0; i < nb; i++)
    {
        if (strncmp(ptr_diact + indcour*lg, ptr_diact + *(ptr_indice + i)*lg, lg) == 0 && indcour != *(ptr_indice + i))
            return(1);
    }
    return(0);
}

int doublon_acte_2020(int indcour, int *ptr_indice, int nb, char *ptr_actes_2020, int lg)
{
    int i, nb1, nb2, nbt;

    char zone1[5];
    char zone2[5];
    char zone3[5];


    for (i = 0; i < nb; i++)
    {
        if (strncmp(ptr_actes_2020 + indcour*lg, ptr_actes_2020 + *(ptr_indice + i)*lg, lg) == 0 && indcour != *(ptr_indice + i))
        {
            strncpy(zone1, ptr_nbactes_2020 + *(ptr_indice + i) * 4, 4); zone1[4] = 0;
            if (atoi(zone1) == 0)nb1 = 1;
            else nb1 = atoi(zone1);
            strncpy(zone2, ptr_nbactes_2020 + indcour * 4, 4); zone2[4] = 0;
            if (atoi(zone2) == 0)nb2 = 1;
            else nb2 = atoi(zone2);
            /* Bug lié à l'utilisation de nb au lieu de nbt */
            if ((nbt = nb1 + nb2) > 9999)nbt = 9999;
            sprintf(zone3, "%04d", nbt);
            strncpy(ptr_nbactes_2020 + *(ptr_indice + i) * 4, zone3, 4);
            return(1);
        }
    }
    return(0);
}

/*CD MAJ FG10
int verif_date_2020(int ind)
{
int i,ns;

i=0;
while(indacte_2020[i]<=ind && i<=nbsej_2020){i++;}
ns=i-1;
if (nombre_jour_1980_2020(rumin_2020[ns].datent)> *(ptr_datfin_2020+ind)) return(0);
if (nombre_jour_1980_2020(rumin_2020[ns].datsor)< *(ptr_datdeb_2020+ind)) return(0);
return(1);
}
*/
void chargep_2020(int *nbac)
{
    int *ptr_trav;
    int i;

    ptr_trav = ptr_indacte_2020;
    for (i = 0; i < nbactes_2020; i++)
    {
        if (!doublon_acte_2020(i, ptr_indacte_2020, *nbac, ptr_actes_2020, lgacte_2020) && strncmp(ptr_actes_2020 + i*lgacte_2020, "          ", lgacte_2020) != 0)
        {
            /*CD if (verif_date_2020(i)!=0)*/
            {
                *ptr_trav++ = i;
                (*nbac)++;
            }
        }
    }
}


int convacte_ccam_2020(char *adrcod, unsigned char prof[])
{
    unsigned char *buf;
    long haccam, fpos;
    char zone[5];

    unsigned char pref[2][_tpref_ccam_2020], hcod[3];
    unsigned int j, nboff;
    int i, nba;

    for (i = 0; i < _psize_ccam_2020; i++) prof[i] = 0;
    haccam = 0;
    for (i = 0; i < 3; i++)
    {
        haccam += (long)((toupper(adrcod[i]) - 'A')*puis26_2020[i]);
    }

    if (haccam < 0 || haccam >= 17576) return 0;

    haccam *= _tpref_ccam_2020;
    for (i = 0; i < 2; i++)
    for (j = 0; j < _tpref_ccam_2020; j++)
        pref[i][j] = 0;
    if (haccam == 0)
    {
        /*FG 11e:TEM
        fseek(ptaccam_2020,_poffpref_ccam_2020+haccam,0);
        fread(pref[1],_tpref_ccam_2020,1,ptaccam_2020);*/
        memcpy(pref[1], h_ptaccam_2020 + _poffpref_ccam_2020 + haccam, _tpref_ccam_2020);
    }
    else
    {
        /*FG 11e:TEM
        fseek(ptaccam_2020,_poffpref_ccam_2020+haccam-_tpref_ccam_2020,0);
        fread(pref,_tpref_ccam_2020,2,ptaccam_2020);*/
        memcpy(pref, h_ptaccam_2020 + _poffpref_ccam_2020 + haccam - _tpref_ccam_2020, _tpref_ccam_2020 * 2);
    }
    if (memcmp(pref[0], pref[1], _tpref_ccam_2020) == 0)
        return 0;

    nboff = pref[0][0] * 256 + pref[0][1];
    nba = (pref[1][0] * 256 + pref[1][1]) - nboff;


    if ((buf = (unsigned char *)malloc(nba*_psize_ccam_2020)) == 0)
        return 0;

    fpos = _poffnop_ccam_2020 + (long)nboff*_psize_ccam_2020;
    /*FG 11e:TEM
    fseek(ptaccam_2020,fpos,0);
    fread(buf,nba*_psize_ccam_2020,1,ptaccam_2020);*/
    memcpy(buf, h_ptaccam_2020 + fpos, nba*_psize_ccam_2020);

    strncpy(zone, adrcod + 4, 4); zone[4] = 0;
    hcod[0] = toupper(adrcod[3]) - 'A';
    hcod[1] = (atoi(zone)) / 256;
    hcod[2] = (atoi(zone)) % 256;

    i = 0;
    while ((i < (nba*_psize_ccam_2020)) && (memcmp(buf + i, hcod, 3) != 0))
        i += _psize_ccam_2020;
    if (i >= nba*_psize_ccam_2020)
    {
        /*pas trouvé*/
        free(buf);
        return 0;
    }
    /*trouvé*/
    memcpy(prof, buf + i + 3, _psize_ccam_2020 - 3);
    free(buf);
    return 1;
} /* fin de la fonction convacte_ccam_2020*/

/*renvoie 1 si le code acte CCAM avec son code descripteur sur 2 car existe*/
/*codeactedesc est le code sur 10 caractères*/
int trouve_codedesc_2020(char *codeactedesc, int *extexist, int *datedeb, int *datefin)
{
    unsigned char *buf;
    long haccam, fpos;
    char zone[6];
    unsigned char codedesc;

    unsigned char pref[2][_tpref_ccam_2020], hcod[3];
    unsigned int j, nboff;
    int i, nba;
	unsigned char codesdesclu;
	int trouveext;

	if (!extexist)/*précaution*/
		return 0;
	*extexist = 0;

    haccam = 0;
    for (i = 0; i < 3; i++)
    {
        haccam += (long)((toupper(codeactedesc[i]) - 'A')*puis26_2020[i]);
    }

    if (haccam < 0 || haccam >= 17576) return 0;

    haccam *= _tpref_ccam_2020;
    for (i = 0; i < 2; i++)
    for (j = 0; j < _tpref_ccam_2020; j++)
        pref[i][j] = 0;
    if (haccam == 0)
        memcpy(pref[1], h_ptccamdesc_2020 + _poffpref_ccamdesc_2020 + haccam, _tpref_ccam_2020);
    else
        memcpy(pref, h_ptccamdesc_2020 + _poffpref_ccamdesc_2020 + haccam - _tpref_ccam_2020, _tpref_ccam_2020 * 2);

    if (memcmp(pref[0], pref[1], _tpref_ccam_2020) == 0)
        return 0;

    nboff = pref[0][0] * 256 + pref[0][1];
    nba = (pref[1][0] * 256 + pref[1][1]) - nboff;

    if ((buf = (unsigned char *)malloc(nba*_psize_ccamdesc_2020)) == 0)
        return 0;

    fpos = _poffnop_ccamdesc_2020 + (long)nboff*_psize_ccamdesc_2020;
    memcpy(buf, h_ptccamdesc_2020 + fpos, nba*_psize_ccamdesc_2020);

    strncpy(zone, codeactedesc + 4, 3); zone[4] = 0;
    zone[3] = codeactedesc[10];
    hcod[0] = toupper(codeactedesc[3]) - 'A';
    hcod[1] = (atoi(zone)) / 256;
    hcod[2] = (atoi(zone)) % 256;

    strncpy(zone, codeactedesc + 8, 2);
    zone[2] = 0;
    codedesc = atoi(zone);

    i = 0;
	trouveext = 0;
	while ((i < (nba*_psize_ccamdesc_2020)) && (memcmp(buf + i, hcod, 3) != 0))
		i += _psize_ccamdesc_2020;
	while ((i < (nba*_psize_ccamdesc_2020)) && (memcmp(buf + i, hcod, 3) == 0))
	{
		codesdesclu = *(buf + i + 3);
		if (codesdesclu == codedesc)
		{
			trouveext = 1;
			*datedeb = (int)(((*(buf + i + 4)) * 256) + *(buf + i + 5));
			*datefin = (int)(((*(buf + i + 6)) * 256) + *(buf + i + 7));
		}
		if (codesdesclu != 0)
		{
			*extexist = 1;
		}
		if ((trouveext) && (*extexist))
			break;
		i += _psize_ccamdesc_2020;
	}

    free(buf);
    /*if (i >= nba*_psize_ccamdesc_2020)*/
	if(!trouveext)
    {
        /*pas trouvé*/
        return 0;
    }
    /*trouvé*/
    return 1;
} /* fin de la fonction trouve_codedesc_2020*/


int hashcode_2020(char *acode, int *hcod)
{
    int i, m;
    char code[3];
    int hc[3] = { 132, 11, 1 };

    strncpy(code, acode, 3);
    *hcod = 0;
    for (i = 0; i < 3; i++)
    {
        switch (code[i])
        {
        case ' ': m = 0;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            m = code[i] - '0' + 1;
            break;
        case caras:
            if (i < 2)
            {
                m = 11;
                break;
            }
            else
                return(-1);
        default:
            return(-1);
        }
        *hcod += m*hc[i];
    }
    return 0; /*Pour faire plaisir au compilateur*/
}

int convdiag10_2020(char *adrcod, unsigned char prof[])
{
    char trans[4];
    unsigned char *buf;
    long hdg10, fpos;
    int hdgf10;
    unsigned char pref[2][_tpref_2020], hcod[2];
    unsigned int j, nboff;
    int i, nbd;

    for (i = 0; i < _dsize_2020; i++)
        prof[i] = 0;
    strncpy(trans, adrcod, 3); trans[3] = '\0';
    if (trans[0] == ' ')
        return 0;
    hdg10 = (long)(toupper(trans[0]) - 'A') * 100;
    hdg10 += atoi(trans + 1);
    if (hdg10 < 0 || hdg10 >= 2600)
        return 0;
    hdg10 *= _tpref_2020;
    for (i = 0; i < 2; i++)
    for (j = 0; j < _tpref_2020; j++)
        pref[i][j] = 0;
    if (hdg10 == 0)
    {
        /*FG 11e:TEM
        fseek(ptmdiag10_2020,_doffpref_2020+hdg10,0);
        fread(pref[1],_tpref_2020,1,ptmdiag10_2020);*/
        memcpy(pref[1], h_ptmdiag10_2020 + _doffpref_2020 + hdg10, _tpref_2020);
    }
    else
    {
        /*FG 11e:TEM
        fseek(ptmdiag10_2020,_doffpref_2020+hdg10-_tpref_2020,0);
        fread(pref,_tpref_2020,2,ptmdiag10_2020);*/
        memcpy(pref, h_ptmdiag10_2020 + _doffpref_2020 + hdg10 - _tpref_2020, _tpref_2020 * 2);
    }
    if (memcmp(pref[0], pref[1], _tpref_2020) == 0)
        return 0;

    nboff = pref[0][0] * 256 + pref[0][1];
    nbd = (pref[1][0] * 256 + pref[1][1]) - nboff;

    if ((buf = (unsigned char *)malloc(nbd*_dsize_2020)) == 0)
        return 0;

    fpos = _doffnop10_2020 + (long)nboff*_dsize_2020;
    /*FG 11e:TEM
    fseek(ptmdiag10_2020,fpos,0);
    fread(buf,nbd*_dsize_2020,1,ptmdiag10_2020);*/
    memcpy(buf, h_ptmdiag10_2020 + fpos, nbd*_dsize_2020);

    hashcode_2020(adrcod + 3, &hdgf10);
    hcod[0] = hdgf10 / 256;
    hcod[1] = hdgf10 % 256;

    /*il faut trouver dans buf le suffixe qui correspond au DIAG avec hcod (terminaison)*/
    i = 0;
    while ((i < nbd*_dsize_2020) && (memcmp(buf + i, hcod, 2) != 0))
        i += _dsize_2020;
    if (i >= nbd*_dsize_2020)
    {
        /*pas trouvé*/
        free(buf);
        return 0;
    }
    /*trouvé*/
    memcpy(prof, buf + i + 2, _dsize_2020 - 2);
    free(buf);
    return 1;
} /* fin de la fonction convdiag10_2020 */


/**********************************************************************
 *                                                                    *
 * recherche du sejour principal                                      *
 *                                                                    *
 **********************************************************************/

int findmax_2020()
{
    int i, j, iretenu, iZ515retenu, iSretenu, bacte, bactetherap, bactemoyen, bacteclass;
    int score, scoreprec, scoremin;
    int dp23rz;
    int dureemax, dureeZ515;
    int sansS, dureeS, maxrumS;
    char zone[5];

    for (i = 0; i <= nbsej_2020 - 1; i++)
        los_2020[i] = callos_2020(rumin_2020[i].datent, rumin_2020[i].datsor);

    /*FG11: IGS et la durée de séjour totale*/
    _igs_2020 = 0;
    _dsj_2020 = 0;
    for (i = 0; i<nbsej_2020; i++)
    {
        _dsj_2020 += los_2020[i];
        strncpy(zone, rumin_2020[i].igs, 3);
        zone[3] = 0;
        if (atoi(zone)>_igs_2020)
            _igs_2020 = atoi(zone);
        /*nombre de faisceaux*/
        /*CD supprimé en V13
        if(rumin_2020[i].nbfaisc!=' ')
        {
        if(rumin_2020[i].nbfaisc-'0'>_nbfaisc_2020)
        _nbfaisc_2020=rumin_2020[i].nbfaisc-'0';
        }*/
    }

    score = scoreprec = 0;
    iretenu = 0;
    iZ515retenu = 0;
    iSretenu = 0;
    scoremin = 99999999;
    dureeZ515 = -1;

    sansS = 0;
    dureeS = -1;
    dureemax = 0;
    maxrumS = 0;

    bacte = 0; bactetherap = 0; bactemoyen = 0; bacteclass = 0;
    for (i = 0; i <= nbsej_2020 - 1; i++)
    {
        bacte = 0; bactetherap = 0; bactemoyen = 0; bacteclass = 0;
        score = scoreprec;
        for (j = indacte_2020[i]; j < indacte_2020[i + 1]; j++)
        {
            /*actes opératoires nonR*/
            /*0:0 : acte opératoire*/
            /*23:0 : acte mineur*/
            if (*(ptr_amask_2020 + (j*_prmsize_2020)) & 128 && !(*(ptr_amask_2020 + (j*_prmsize_2020) + 23) & 128))
            {
                bacte = 1;
                break;
            }
            if (*(ptr_amask_2020 + (j*_prmsize_2020) + 38) & puisi2_2020[6])
            {
                bactetherap = 1;
            }
            if ((_dsj_2020 <= 1) && (*(ptr_amask_2020 + (j*_prmsize_2020) + 39) & puisi2_2020[0]))
            {
                bactemoyen = 1;
            }
            if ((_dsj_2020 == 0) && (*(ptr_amask_2020 + (j*_prmsize_2020) + 39) & puisi2_2020[1]))
            {
                bacteclass = 1;
            }
        }
        if (bacte)
        {
            iretenu = i;
            break;
        }
        if (bactetherap)
            score -= 999999;
        else
        if (bactemoyen)
            score -= 99999;
        else
        if (bacteclass)
            score -= 9999;

        dp23rz = 0;
        dp23rz = ((*(ptr_dmask_2020 + (inddiag_2020[i])*_dxmsize_2020 + 21)) & puisi2_2020[2]);
        if ((los_2020[i] >= 2) && (!dp23rz))
            scoreprec += 100;
        if (dp23rz)
            score += 150;

        if (los_2020[i] == 0)
            score += 2;
        if (los_2020[i] == 1)
            score += 1;

        /*DP imprécis*/
        if ((*(ptr_dmask_2020 + (inddiag_2020[i])*_dxmsize_2020 + 21)) & puisi2_2020[6])
            score += 201;

        /*DP Z515*/
        if (((strncmp(rumin_2020[i].diagp, "Z515", 4) == 0) ||
            (strncmp(rumin_2020[i].diagp, "Z502", 4) == 0) ||
            (strncmp(rumin_2020[i].diagp, "Z503", 4) == 0))
            && (los_2020[i] > dureeZ515))
        {
            dureeZ515 = los_2020[i];
            iZ515retenu = i;
        }
        if (score<scoremin)
        {
            iretenu = i;
            scoremin = score;
        }

        if (((*(ptr_dmask_2020 + (inddiag_2020[i])*_dxmsize_2020 + 21)) & puisi2_2020[5]) && (sansS == 0))
        {
            maxrumS = i;
            if (los_2020[i]>dureeS)
            {
                dureeS = los_2020[i];
                iSretenu = i;
            }
        }
        else
            sansS = 1;

        if (los_2020[i] > dureemax)
            dureemax = los_2020[i];
    }
    if (!bacte)
    {
        if (dureeZ515 == dureemax)
            iretenu = iZ515retenu;
        else
        if (iretenu <= maxrumS)
            iretenu = iSretenu;
    }
    return iretenu;
}

int ToInt_2020(char Buff[], int Len)
{
    char Buffer[6];
    int Resultat;
    strncpy(Buffer, Buff, Len);
    Buffer[Len] = 0;
    Resultat = atoi(Buffer);
    return Resultat;
}

/* Calcul le nombre de jours entre 2 dates en jours */
int	callos_2020(char *date, char *dats)
{
    int	duree;		/* zone de travail pour calcul de los */

    duree = nombre_jour_1980_2020(dats) - nombre_jour_1980_2020(date);

    /*FG11b
    if (duree > 999)
    duree = 999;*/
    return (duree);
} /* fin de la fonction callos */


int nombre_jour_1980_2020(char *date)
{
    static int tab[2][12] = {
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 } };
    int 	jj, mm, aa;	/* date entree en entier */
    int i, bis, nbj;

    convdate_2020(date, &jj, &mm, &aa);
    nbj = 0;
    for (i = 1980; i < aa; i++)
    if (i % 4 == 0 && i % 100 != 0 || i % 400 == 0)
        nbj += 366;
    else
        nbj += 365;
    bis = (aa % 4 == 0 && aa % 100 != 0 || aa % 400 == 0);
    nbj += tab[bis][mm - 1];
    nbj += jj;
    return(nbj);
}

void	convdate_2020(char *adat, int *p_jj, int *p_mm, int *p_aa)
{
    char zw[5];

    sprintf(zw, "%2.2s", adat);
    *p_jj = atoi(zw);
    sprintf(zw, "%2.2s", adat + 2);
    *p_mm = atoi(zw);
    sprintf(zw, "%4.4s", adat + 4);
    *p_aa = atoi(zw);

} /* fin de la fonction convdat */


void callage_2020(char *d1, char *d2)
{
    int aa1, aa2, mm1, mm2, jj1, jj2;


    age_jour_2020 = 0;
    convdate_2020(d1, &jj1, &mm1, &aa1);
    convdate_2020(d2, &jj2, &mm2, &aa2);
    if ((age_an_2020 = aa1 - aa2) <= 1)
    {
        if ((age_jour_2020 = callos_2020(d2, d1)) > 365)
            age_jour_2020 = 0;
        else
            age_an_2020 = 0;
    }
    else
    {
        if (mm1 < mm2)
            age_an_2020 -= 1;
        if (mm2 == mm1 && jj1 < jj2)
            age_an_2020 -= 1;
    }
}

unsigned long carlong_2020(unsigned char c[])
{
    int i;
    unsigned long l;

    l = 0;
    for (i = 0; i < 4; i++)
        l = (l * 256) + c[i];
    return(l);
}
/*CD le 08/12/03*/
int getCategorie_2020(int erreurFacultative)
{
    int i;
    /*CD MAJ FG10*/
    /*for(i=0;i<0;i++)
    {
    if(erreurFacultative==errcatdp_11b[i])
    return 1;
    }*/
	
	for (i = 0; i < 2; i++)
	{
		if (erreurFacultative == errCatGragradation_2020[i])
			return 1;
	}

	for (i = 0; i < 4; i++)
    {
        if (erreurFacultative == errcatac_2020[i])
            return 2;
    }
    /*CD FG11b
    for(i=0;i<2;i++)
    {
    if(erreurFacultative==errcatda_11b[i])
    return 3;
    }*/

    /*CD MAJ FG10*/
    /*for(i=0;i<4;i++)
    {
    if(erreurFacultative==errcatdr_11b[i])
    return 4;
    }*/
    for (i = 0; i < 2; i++)
    {
        if (erreurFacultative == errcatum_2020[i])
            return 5;
    }

    for (i = 0; i < 4; i++)
    {
        if (erreurFacultative == errcatautres_2020[i])
            return 6;
    }

    return 7;
}

/*Fonction qui renvoie 1 si au moins une table binaire est fermée
  0 sinon*/
int table_fermee_2020()
{
    int i;
    for (i = 0; i < NB_TABLES_BINAIRES_2020; i++)
    {
        if (multitables_2020[i].fileptr == NULL)
            return 1;
    }
    return 0;
}
void initRes_2020(GRP_RESULTAT_2020 *pgrp_res)
{
    if (!pgrp_res)
        return;
    pgrp_res->ns_sor = 0;
    pgrp_res->sejp = -1;
    strncpy(pgrp_res->version, "     ", 3);
    strcpy(pgrp_res->cmd, "  ");
    strcpy(pgrp_res->ghm, "    ");
    strcpy(pgrp_res->cret, "   ");
}

int grp_2020(GRP_ENTREE_2020 grp_ent, GRP_RESULTAT_2020 *grp_res, GHS_VALORISATION_2020 *ghs_val, RSS_INFO_2020 *rss_inf)
{
    int i, ok_gr, ok_co, j,s;
    int inta, intd;
    char *rum[tab_max];
    int lgr;
    int ret;
    int RUM_V_2020(char[], char[]);
    int errcat, errcat_okgr;

    /*CD MAJ FG10*/
    int offset_actes;
    char zone[3];
    int nbdad_rum;

    const char* Get_RUM_V_2020(const char* Rum);

    char cretuhcd[4];
    char cmduhcd[3];
    char ghmuhcd[5];
	
	char ghmdughs[5];

    /*CD 23/01/07*/
    /*char nomficumlong[512];*/
    int nbjoursuhcd;
    int	callos_2020(char *, char*);

    /*CD FG10B*/
    unsigned char lsignrss[21];
    unsigned int lsignfg;
    char lchsigfg[6];
	
    int confcod;/*confirmation de codage nécessaire*/
    char *masque,*tmpacte;
    int flagconfcod;
    int lnbseance, nivCMD14;
    int acteEvacCurMed;
	int presence_exclusion_radiation_avastin;
	int presence_radiation_avastin;
	unsigned char * laracineinfo;

    _igs_2020 = 0;
    agegest_renseigne_2020 = -1;
    datedernregles_renseignee_2020 = -1;
    numero_innovation_2020 = -1;
    groupeRacine = -1;
    annuleActeOperGHMMed = 0;
    nivCMD14 = 0;

    sejourprinc_2020 = -1;
	
	/*FG 2019 RAAC*/
	critereRAAC_2020 = 0;
	indicateurRAAC_2020 = 0;

	/*type d'hospitalisation du 1er RUM*/
	typehospit1erRUM_exp_2020 = ' ';
	/*séjour éligible au forfait diabète*/
	sejEliForfDiabete_2020 = 0;

	/*gradation des HDJ*/
	gradationHDJ_2020 = -1;
	sejourHdjexterne_2020 = 0;

    /*CD FG11*/
    binversionDPDR_2020 = 0;

    nbsej_2020 = grp_ent.ns;
    nbsejR_2020 = grp_ent.ns;
    nbac_2020 = 0;
    nbdc_2020 = 0;

    _dsj_2020 = 0;
    age_an_2020 = 0;
    age_jour_2020 = 0;
    memset(bActesMarqueursUMREA_2020, 0, sizeof(bActesMarqueursUMREA_2020));

    /*conservation du pointeur sur GHS_VALORISATION_2020*/
    lghs_val_2020 = ghs_val;

    /*accouchement*/
    daZ37_2020 = 0;
    daaccouch_2020 = 0;
    nbjdateaccouch_2020 = -1;
    presenceActeAccouchement_2020 = 0;


    /*initialisation de rss_inf*/
    init_info_vide_2020(rss_inf);
    /*initialisation de ghs_val*/
    init_valo_vide_2020(ghs_val);
    /*initialisation de grp_res*/
    initRes_2020(grp_res);

    if (rss_inf != NULL)
    {
        rss_inf->actlist.listzac = NULL;
        rss_inf->actlist.nbac = 0;
        rss_inf->diaglist.listdiag = NULL;
        rss_inf->diaglist.nbdiag = 0;
    }
    lrss_inf_2020 = rss_inf;

    for (i = 0; i < tab_max; i++)
    {
        rum[i] = NULL;
    };
    if (nbsej_2020 == 0)
    {
        grpclose_2020();
        return(0);
    }
    if (nbsej_2020 >= tab_max)
        nbsej_2020 = tab_max;

    /*CD FG10B*/
    /*calcul de la signature du RSS*/
    signer_RSS_2020(grp_ent.rum2, grp_ent.ns, lsignrss);
    strncpy(rss_inf->sigrss, lsignrss, 20);

    /*signature version de la fonction groupage et des tables binaires*/
    if (table_fermee_2020())
    {
        lsignfg = CalculerVersionFGTableBinaires_2020(&grp_ent);
        sprintf(lchsigfg, "%05u", lsignfg);
        strncpy(rss_inf->sigfg, lchsigfg, 5);
    }

    strcpy(grp_res->version, "  ");

    if ((ret = OuvrirTables_2020(grp_ent.dirtab, grp_res->version)) != 0)
        return errfic_2020(rum, ret, IMPLEMENTATION, grp_res, ghs_val, rss_inf);

    /*FG 11e:TEM*/
    ptaccam_2020 = multitables_2020[TABLE_ACTES_2020].fileptr;
    ptmdiag10_2020 = multitables_2020[TABLE_DIAG_2020].fileptr;
    pttree_2020 = multitables_2020[TABLE_ARBRE_2020].fileptr;
    ghsinf_2020 = multitables_2020[TABLE_GHS_2020].fileptr;
    ptfidx_gc_2020 = multitables_2020[TABLE_GESTECOMP_2020].fileptr;
    ghminf_2020 = multitables_2020[TABLE_GHMINFO_2020].fileptr;
    srcdgac_2020 = multitables_2020[TABLE_SRCDGACT_2020].fileptr;
    innovref_2020 = multitables_2020[TABLE_INNOVREFER_2020].fileptr;
    tabcombi_2020 = multitables_2020[TABLE_COMBI_2020].fileptr;
    ptccamdesc_2020 = multitables_2020[TABLE_CCAMDESC_2020].fileptr;
    ptghsminor_2020 = multitables_2020[TABLE_GHSMINOR_2020].fileptr;

    /*FG 11e:TEM*/
    h_ptaccam_2020 = multitables_2020[TABLE_ACTES_2020].htable;
    h_ptmdiag10_2020 = multitables_2020[TABLE_DIAG_2020].htable;
    h_pttree_2020 = multitables_2020[TABLE_ARBRE_2020].htable;
    h_ghsinf_2020 = multitables_2020[TABLE_GHS_2020].htable;
    h_ptfidx_gc_2020 = multitables_2020[TABLE_GESTECOMP_2020].htable;
    h_ghminf_2020 = multitables_2020[TABLE_GHMINFO_2020].htable;
    h_srcdgac_2020 = multitables_2020[TABLE_SRCDGACT_2020].htable;
    h_innovref_2020 = multitables_2020[TABLE_INNOVREFER_2020].htable;
    h_tabcombi_2020 = multitables_2020[TABLE_COMBI_2020].htable;
    h_refauto_2020 = multitables_2020[TABLE_REFAUTO_2020].htable;
    h_ptccamdesc_2020 = multitables_2020[TABLE_CCAMDESC_2020].htable;
    h_ptghsminor_2020 = multitables_2020[TABLE_GHSMINOR_2020].htable;

    InitErreur_2020(ErrCtrl_2020, ErrArb_2020, ErrImpl_2020, nbsej_2020);

    ret = 0;
    for (i = 0; i<nbsej_2020; i++)
    {
        lgdiag_2020 = 0;
        if (i>0)
        {
            if (strncmp(Get_RUM_V_2020(grp_ent.rum2[i - 1]), Get_RUM_V_2020(grp_ent.rum2[i]), 3) != 0)
            {
                ret = 2;
                AddErreur_2020(CONTROLE, 59, i);
                return FreeErr_2020(rum, 59, CONTROLE, grp_res, ghs_val, rss_inf);
            }
        }
        /*seul le format 020 est reconnu par la FG1311g*/
        if (RUM_V_2020(grp_ent.rum2[i], "020") == 0)
        {
            lgdiag_2020 = 6;
            lgacte_2020 = 8;
        }
        else
            lgacte_2020 = 0;

        lgr = strlen(grp_ent.rum2[i]);

        if (lgdiag_2020 == 0 || lgacte_2020 == 0)
        {
            ret = 59;
            AddErreur_2020(CONTROLE, 59, i);
            return FreeErr_2020(rum, 59, CONTROLE, grp_res, ghs_val, rss_inf);
        }
        else
        {
            if (GetTaille_2020(grp_ent.rum2[i], -1) != lgr)
            {
                ret = 59;
                AddErreur_2020(CONTROLE, 59, i);
                return FreeErr_2020(rum, 59, CONTROLE, grp_res, ghs_val, rss_inf);
            }
            else
            {
                if ((rum[i] = RumToInterne_2020(grp_ent.rum2[i], -1)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
            }
        }
    } /* for */

    if (fptrum_2020 == NULL)
    {
        if ((ret = bChargeTableUM_2020(grp_ent.ficum)) != 0)
        {
            AddErreur_2020(IMPLEMENTATION, ret, 0);
            if (fptrum_2020 != NULL)
            {
                fclose(fptrum_2020);
                fptrum_2020 = NULL;
            }
            return FreeErr_2020(rum, ret, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
        }
    }

    if ((contfmt_2020(((RUM_2020*)(rum[nbsej_2020 - 1]))->datsor, "Z9Z99999", 1, 7) == 0) && (contdate_2020(((RUM_2020*)(rum[nbsej_2020 - 1]))->datsor) == 0))
    {
        if ((ret = ChoixTables_2020(((RUM_2020*)(rum[nbsej_2020 - 1]))->datsor, grp_res->version)) != 0)
        {
            AddErreur_2020(IMPLEMENTATION, ret, 0);
            return FreeErr_2020(rum, ret, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
        }

        /*mise à jour des variables globales pour la lecture des sous tables binaires*/
        opghm_2020();
        opghs_2020();
        opacccam_2020();
        opsrcdgac_2020();
        opdiag10_2020();
        opaccam_gc_2020(multitables_2020[TABLE_GESTECOMP_2020].fileptr, multitables_2020[TABLE_GESTECOMP_2020].offset_table[multitables_2020[TABLE_GESTECOMP_2020].periodecour]);
        opinnov_2020();
        optabcombi_2020();
        opccamdesc_2020();
        opghsminor_2020();
    }

    nbdiags_2020 = nbactes_2020 = 0;

    for (i = 0; i < nbsej_2020; i++)
    {
        strncpy(rumin_2020[i].finess, rum[i], sizeof(RUM_2020));
        intd = ToInt_2020(rumin_2020[i].nbds, 2);
        inddiag_2020[i] = nbdiags_2020;
        nbdiags_2020 += intd + 1 + 1;
        if (i == 0)
        {
            if ((ptr_diags_2020 = (char *)malloc(nbdiags_2020*lgdiag_2020)) == NULL)
            {
                AddErreur_2020(IMPLEMENTATION, 3, 0);
                return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
            }
        }
        else
        {
            if ((ptr_diags_2020 = (char *)realloc(ptr_diags_2020, nbdiags_2020*lgdiag_2020)) == NULL)
            {
                AddErreur_2020(IMPLEMENTATION, 3, 0);
                return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
            }
        }
        strncpy(ptr_diags_2020 + inddiag_2020[i] * lgdiag_2020, rumin_2020[i].diagp, lgdiag_2020);
        strncpy(ptr_diags_2020 + (inddiag_2020[i] + 1)*lgdiag_2020, rumin_2020[i].diagr, lgdiag_2020);
        if (intd != 0)
            strncpy(ptr_diags_2020 + (inddiag_2020[i] + 2)*lgdiag_2020, rum[i] + sizeof(RUM_2020), intd*lgdiag_2020);

        /* chargement des actes*/
        inta = ToInt_2020(rumin_2020[i].nba, 3);
        indacte_2020[i] = nbactes_2020;
        nbactes_2020 += inta;
        if (inta != 0)
        {
            if (nbactes_2020 == inta)
            {
                if ((ptr_actes_2020 = (char *)malloc(nbactes_2020*lgacte_2020)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);

                }
                /*CD FG13
                if((ptr_nbactes_2020=(char *)malloc(nbactes_2020*2))==NULL)*/
                if ((ptr_nbactes_2020 = (char *)malloc(nbactes_2020 * 4)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
                /*CD MAJ FG10*/
                if ((ptr_typact_2020 = (char *)malloc(nbactes_2020)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
            }
            else
            {
                if ((ptr_actes_2020 = (char *)realloc(ptr_actes_2020, nbactes_2020*lgacte_2020)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
                /*CD FG13
                if((ptr_nbactes_2020=(char *)realloc(ptr_nbactes_2020,nbactes_2020*2))==NULL)*/
                if ((ptr_nbactes_2020 = (char *)realloc(ptr_nbactes_2020, nbactes_2020 * 4)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
                /*CD MAJ FG10*/
                if ((ptr_typact_2020 = (char *)realloc(ptr_typact_2020, nbactes_2020)) == NULL)
                {
                    AddErreur_2020(IMPLEMENTATION, 3, 0);
                    return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
                }
            }
            for (j = 0; j < inta; j++)
            {
                if (RUM_V_2020(rum[i], "020") == 0)
                {

                    strncpy(ptr_actes_2020 + (indacte_2020[i] + j)*lgacte_2020, rum[i] + sizeof(RUM_2020)+intd*lgdiag_2020 + j*(lgacte_2020 + 2), lgacte_2020);
                    strncpy(ptr_nbactes_2020 + (indacte_2020[i] + j) * 4, rum[i] + sizeof(RUM_2020)+intd*lgdiag_2020 + j*(lgacte_2020 + 2) + lgacte_2020, 2);
                    strncpy(ptr_nbactes_2020 + (indacte_2020[i] + j) * 4 + 2, "  ", 2);

                    /*chargement des types d'activité*/
                    strncpy(zone, grp_ent.rum2[i] + 120, 2); zone[2] = 0;
                    nbdad_rum = atoi(zone);
                    offset_actes = 177 + (intd + nbdad_rum) * 8;
                    strncpy(ptr_typact_2020 + (indacte_2020[i] + j), grp_ent.rum2[i] + offset_actes + j * 29 + 19, 1);
                }
                else
                {
                    strncpy(ptr_actes_2020 + (indacte_2020[i] + j)*lgacte_2020, rum[i] + sizeof(RUM_2020)+intd*lgdiag_2020 + j*lgacte_2020, lgacte_2020);
                    strncpy(ptr_nbactes_2020 + (indacte_2020[i] + j) * 4, "    ", 4);
                }
            }
        }
    }  /* fin du for */

    if (nbactes_2020 == 0)
    {
        ptr_actes_2020 = NULL;
        ptr_nbactes_2020 = NULL;
        ptr_typact_2020 = NULL;
    }

    inddiag_2020[i] = nbdiags_2020;
    indacte_2020[i] = nbactes_2020;

    if ((ptr_dmask_2020 = (char *)calloc(nbdiags_2020, _dxmsize_2020)) == NULL)
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);

    }
    if ((ptr_omask_2020 = (char *)calloc(nbdiags_2020, _prfsize_2020)) == NULL)
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);

    }

    if ((ptr_emask_2020 = (char *)calloc(nbdiags_2020, 4)) == NULL)
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
    }

    if (nbactes_2020 != 0)
    {
        if ((ptr_amask_2020 = (char *)calloc(nbactes_2020, _prmsize_2020)) == NULL)
        {
            AddErreur_2020(IMPLEMENTATION, 3, 0);
            return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
        }
        if ((ptr_datdeb_2020 = (int *)calloc(nbactes_2020, sizeof(int))) == NULL)
        {
            AddErreur_2020(IMPLEMENTATION, 3, 0);
            return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);

        }
        if ((ptr_datfin_2020 = (int *)calloc(nbactes_2020, sizeof(int))) == NULL)
        {
            AddErreur_2020(IMPLEMENTATION, 3, 0);
            return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
        }
    }
    else
    {
        ptr_amask_2020 = NULL;
        ptr_datdeb_2020 = NULL;
        ptr_datfin_2020 = NULL;
    }

    for (j = 0; j < nbdiags_2020; j++)
    {
        dxmap10_2020(j, rumin_2020[0].sexe);
    }

    for (j = 0; j < nbactes_2020; j++)
    {
        if (lgacte_2020 == 8)
            prmap_ccam_2020(j);
    }

    /*chargement des indices d'UM pour chaque RUM du RSS*/
    for (i = 0; i < nbsej_2020; i++)
    {
        indice_ums_2020[i] = trouveindiceUM_2020(rumin_2020[i].unmed);
    }
	/*sauvegarde du type d'hospitalisation du 1er RUM*/
	if (indice_ums_2020[0] != -1)
	{
		for (i = 0; i < uminf_2020[indice_ums_2020[0]].nbautorisations; i++)
		{
			if
				((!date_strict_sup_2020(uminf_2020[indice_ums_2020[0]].autorisations[i].datdebeffet, rumin_2020[0].datent)) &&
				(!date_strict_sup_2020(rumin_2020[0].datent, uminf_2020[indice_ums_2020[0]].autorisations[i].datfineffet)))
			{
				typehospit1erRUM_exp_2020 = uminf_2020[indice_ums_2020[0]].autorisations[i].typhospit;
				break;
			}
		}
	}

    /***************CONTROLE DES RUM***************/
    ok_co = controle_2020(grp_ent.rum2);
    /**********************************************/

    ret = 9999;
    for (i = 0; i < MAXERRIMPL_2020; i++)
    if (ErrImpl_2020[i] != 0)
    if (ErrImpl_2020[i] < ret)
        ret = ErrImpl_2020[i];
    if (ret != 9999)
    {
        return FreeErr_2020(rum, ret, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
    }

    for (i = ErrCtrl_2020[0] + 1; i < MAXERRCTRL_2020; i++)
    if (ErrCtrl_2020[i] != 0)
    if (ErrCtrl_2020[i] < ret && ErreurCtrlBloquante_2020(ErrCtrl_2020[i]))
        ret = ErrCtrl_2020[i];

    if (ret != 9999)
    {
        nbdc_2020 = nbdiags_2020;
        nbac_2020 = nbactes_2020;
        return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
    }

    /*choix du DP*/
    grp_res->sejp = findmax_2020();
    sejourprinc_2020 = grp_res->sejp;

    if (grp_res->sejp < 0)
    {
        AddErreur_2020(IMPLEMENTATION, -1, 0);
        return FreeErr_2020(rum, -1, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
    }

    if ((ptr_inddiag_2020 = (int *)malloc(nbdiags_2020*sizeof(int))) == NULL)
    {
        AddErreur_2020(IMPLEMENTATION, 3, 0);
        return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
    }
    charged_2020(grp_res->sejp, &nbdc_2020);
    if (nbactes_2020 != 0)
    {
        if ((ptr_indacte_2020 = (int *)malloc(nbactes_2020*sizeof(int))) == NULL)
        {
            AddErreur_2020(IMPLEMENTATION, 3, 0);
            return FreeErr_2020(rum, 3, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
        }
        chargep_2020(&nbac_2020);
    }
    else
        ptr_indacte_2020 = NULL;

    /*CD controle cohérence nombre de séance / DP motif de séances*/
    if ((*(ptr_dmask_2020 + *(ptr_inddiag_2020)*_dxmsize_2020 + 8)) & puisi2_2020[6])/*DP motif de séance ?*/
    {
        strncpy(zone, rumin_2020[sejourprinc_2020].nbseance, 2);
        zone[2] = 0;
        lnbseance = atoi(zone);
        if ((lnbseance == 0) && (_dsj_2020 == 0))
        {
            for (i = 0; i<nbac_2020; i++)
            {
                masque = ptr_amask_2020 + (*(ptr_indacte_2020 + i))*_prmsize_2020;
                if ((*(masque + 44) & puisi2_2020[1]))
                    break;
            }
			if (i == nbac_2020)
			{
				ret = ERR_MOTIFSEANCE_SANSSEANCE_INCOHERENT;
				AddErreur_2020(CONTROLE, ret, sejourprinc_2020);
				return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
			}
        }
        if (lnbseance>0)
        {
            if (lnbseance > _dsj_2020 + 1)
                AddErreur_2020(CONTROLE, ERR_NSEANCE_DUREESEJ_INCOHERENT, nbsej_2020 - 1);
        }
    }

    /*initialisation pour UHCD*/
    uhcd_2020 = 0;

    ok_gr = group_2020(grp_res->cret, grp_res->cmd, grp_res->ghm);
    grp_res->ns_sor = nbsej_2020;

    if ((rumin_2020[0].modent == '8') && (rumin_2020[nbsej_2020 - 1].modsor == '8') && (_dsj_2020 > 0))
    {
        uhcd_2020 = 1;
        for (i = 0; i < nbsej_2020; i++)
        {
            if ((autorisation_valide_2020(TYPUM_UHCD)) && (sejourAutoriseEnTypAutor_2020(i, TYPUM_UHCD, rumin_2020[i].datent, rumin_2020[i].datsor, &nbjoursuhcd)))
            {
                if (nbjoursuhcd != (callos_2020(rumin_2020[i].datent, rumin_2020[i].datsor) + 1))
                {
                    uhcd_2020 = 0;
                    break;
                }
            }
            else
            {
                uhcd_2020 = 0;
                break;
            }
        }
    }

    rempli_info_2020(rss_inf, &grp_ent);

    /*vérification sur DP de séance*/
    if ((nbsej_2020 > 1) && (strncmp(grp_res->cmd, "28", 2) == 0))
    {
        ret = ERR_DPSEANCE_MULTIRUM;
        AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);
        return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
    }

    /*FG 2016 radiations partielles*/
    if (strncmp(grp_res->cmd, "28", 2) == 0)
    {
        if((strncmp(ptr_diags_2020 + *ptr_inddiag_2020*lgdiag_2020, "Z511", 4) == 0) || (strncmp(ptr_diags_2020 + *ptr_inddiag_2020*lgdiag_2020, "z511", 4) == 0))
        {
            if (strncmp(ptr_diags_2020 + *(ptr_inddiag_2020 + 1)*lgdiag_2020, "      ", lgdiag_2020) == 0)
            {
                ret = ERR_DR_ATTENDU;
                AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);
                return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
            }
        }
    }

	presence_exclusion_radiation_avastin = 0;
	presence_radiation_avastin = 0;
	if (age_an_2020 > 17)
	{
		for (i = 0; i < nbdc_2020; i++)
		{
			/*exclusion de radiation*/
			if ((*(ptr_dmask_2020 + *(ptr_inddiag_2020 + i)*_dxmsize_2020 + 19)) & puisi2_2020[0])
			{
				presence_exclusion_radiation_avastin = 1;
				break;
			}
			/*radiation*/
			if (!presence_radiation_avastin)
			{
				if ((*(ptr_dmask_2020 + *(ptr_inddiag_2020 + i)*_dxmsize_2020 + 18)) & puisi2_2020[0])
					presence_radiation_avastin = 1;
			}
		}
		if((presence_radiation_avastin) && (!presence_exclusion_radiation_avastin))
			ghs_val->flagAvastin = 1;
	}

    /*FG 2016*/
    if ((strncmp(grp_res->cmd, "14", 2) == 0) && (strncmp(grp_res->ghm, "Z08", 3) == 0))
    {
        acteEvacCurMed = 0;
        for (i = 0; i < nbac_2020; i++)
        {
            tmpacte = ptr_actes_2020 + (*(ptr_indacte_2020 + i))*lgacte_2020;
            if ((strncmp(tmpacte, "JNJD002", 7) == 0)
                || (strncmp(tmpacte, "JNJP001", 7) == 0)
                )
            {
                acteEvacCurMed = 1;
                break;
            }
        }
        if (!acteEvacCurMed)
        {
            AddErreur_2020(CONTROLE, ERR_IVG_SANS_ACTE, nbsej_2020 - 1);
        }
    }


    /*CD FG13 DDR obligatoire sur GHM accouchement*/
    if (datedernregles_renseignee_2020 < 0)
    {
        if ((strncmp(grp_res->cmd, "14", 2) == 0) &&
            (strncmp(grp_res->ghm, "C04", 3) != 0) && /*sauf post partum*/
            (strncmp(grp_res->ghm, "M02", 3) != 0))
        {
            ret = ERR_DATEDERNREGLES_ABSENTE;
            AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);
            return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
        }
    }

    confcod = 0;
    flagconfcod = 0;

	laracineinfo = getRacineInfo_2020(_tmdc_2020, _tdrg_2020);
	strncpy(ghmdughs, grp_res->ghm, 5);

	/*indicateur RAAC*/
	for (s = 0; s < nbsej_2020; s++)
		if (rumin_2020[s].priseenchargeRAAC == '1')
		{
			indicateurRAAC_2020 = 1;
			break;
		}

	if (laracineinfo == NULL)
	{
		if (grp_res->ghm[3] == ' ')
		{
			AddErreur_2020(IMPLEMENTATION, 4, 0);
			return FreeErr_2020(rum, 4, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
		}
	}
	else
	{
		confcod = laracineinfo[10];
		/*RAAC*/
		critereRAAC_2020 = !laracineinfo[12] && indicateurRAAC_2020;
		calcule_niveau_2020(_tmdc_2020, _tdrg_2020, grp_res->cmd, grp_res->ghm, laracineinfo, 1); 
		if (critereRAAC_2020)
		{
			calcule_niveau_2020(_tmdc_2020, _tdrg_2020, grp_res->cmd, ghmdughs, laracineinfo, 0);
		}
	}

    if ((confcod != 0) && (_dsj_2020 < confcod) && (_ms_2020 != 9) && (_ms_2020 != 0) && ((_ms_2020 != 7) || (_ds_2020 != 1)) && !critereRAAC_2020)
    {
        if (rumin_2020[nbsej_2020 - 1].ccodage != '1')
        {
            ret = 120;
            AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);
            return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
        }
        else
        {
            /*pour générer un code retour pour montrer que la confirmation de codage était nécessaire*/
            AddErreur_2020(CONTROLE, 223, nbsej_2020 - 1);
            flagconfcod = 1;/*il faut confirmer et c'est fait*/
        }
    }

    /*FG13 confirmation codage hors CMD 14*/
    if (strncmp(grp_res->cmd, "14", 2) != 0)
    {
        if (
            (!(strncmp(grp_res->cmd, "27", 2) == 0)) &&
            (!(strncmp(grp_res->cmd, "26", 2) == 0)) &&
            (!(strncmp(grp_res->cmd, "25", 2) == 0)) &&
            (!(strncmp(grp_res->cmd, "12", 2) == 0)) &&
            (!(strncmp(grp_res->cmd, "22", 2) == 0)) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C09", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C11", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C12", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C03", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C04", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C10", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C05", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "C06", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M24", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M25", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M13", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M18", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M19", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "K07", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M30", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "01", 2) == 0) && (strncmp(grp_res->ghm, "M31", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C09", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C10", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C11", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C12", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C13", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "07", 2) == 0) && (strncmp(grp_res->ghm, "C14", 3) == 0))) &&
            (!((strncmp(grp_res->cmd, "23", 2) == 0) && (strncmp(grp_res->ghm, "Z02", 3) == 0)))
            )
        {
            ret = 0;
            if ((daZ37_2020) || (daaccouch_2020))
            {
                if (rumin_2020[nbsej_2020 - 1].ccodage != '1')
                    ret = 120;
                else
                    ret = 223;
            }
            else
            {
                /*actes d'accouchement*/
                if (presenceActeAccouchement_2020)
                {
                    if (rumin_2020[nbsej_2020 - 1].ccodage != '1')
                        ret = 120;
                    else
                        ret = 223;
                }
            }
            if (ret > 0)
            {
                AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);

                if (ret == 120)
                    return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
                else
                    flagconfcod = 1;/*il faut confirmer et c'est fait*/
            }
        }
    }
    /*FG 13.11e*/
    if (_dsj_2020 >= 365)
    {
        if (rumin_2020[nbsej_2020 - 1].ccodage != '1')
        {
            AddErreur_2020(CONTROLE, 120, nbsej_2020 - 1);
            return FreeErr_2020(rum, 120, CONTROLE, grp_res, ghs_val, rss_inf);
        }
        else
        {
            AddErreur_2020(CONTROLE, 223, nbsej_2020 - 1);
            flagconfcod = 1;
        }
    }

    /*pas a confirmer*/
    if ((flagconfcod == 0) && (_dsj_2020 >= confcod))
    {
        if (rumin_2020[nbsej_2020 - 1].ccodage == '1')
        {
            ret = ERR_CONFIRMATIONNONNECESSAIRE_PRESENTE;
            AddErreur_2020(CONTROLE, ret, nbsej_2020 - 1);
            return FreeErr_2020(rum, ret, CONTROLE, grp_res, ghs_val, rss_inf);
        }
    }

    /*Zone de surveillance de très courte durée : on relance le groupage avec durée de séjour à 0*/
    if (uhcd_2020 == 1)
    {
        dsejavantuhcd_2020 = 0;
        ok_gr = group_2020(cretuhcd, cmduhcd, ghmuhcd);
		laracineinfo = getRacineInfo_2020(_tmdc_2020, _tdrg_2020);
		if (laracineinfo == NULL)
		{
			if (ghmuhcd[3] == ' ')
			{
				AddErreur_2020(IMPLEMENTATION, 4, 0);
				return FreeErr_2020(rum, 4, IMPLEMENTATION, grp_res, ghs_val, rss_inf);
			}
		}
		else
		{
			if (!laracineinfo[12] && indicateurRAAC_2020)
				calcule_niveau_2020(_tmdc_2020, _tdrg_2020, cmduhcd, ghmuhcd, laracineinfo, 0);
			else
				calcule_niveau_2020(_tmdc_2020, _tdrg_2020, cmduhcd, ghmuhcd, laracineinfo, 1);
		}
		ghs_val->buhcd = 1;
		recode_ghm_2020(ghmuhcd, &_tdrg_2020);
    }
    else
    {
        if(critereRAAC_2020)
			recode_ghm_2020(ghmdughs, &_tdrg_2020);
		else
		    recode_ghm_2020(grp_res->ghm, &_tdrg_2020);
    }
    
	/*Gradation hdj 0 nuit*/
	if (_dsj_2020 == 0)
	{
		gradationHDJ_2020 = gradationHDJzero(&grp_ent, rss_inf, grp_res, laracineinfo==NULL?0:laracineinfo[13]);
	}
	/**/
	
	/*recode_ghm_2020(grp_res->ghm,&_tdrg_2020);*/
    rempli_valo_2020(grp_ent.type_etab, ghs_val, rss_inf, grp_res);

    memcpy(grp_res->ErrCtrl, ErrCtrl_2020, MAXERRCTRL_2020*sizeof(int));
    memcpy(grp_res->ErrArb, ErrArb_2020, MAXERRARB_2020*sizeof(int));
    memcpy(grp_res->ErrImpl, ErrImpl_2020, MAXERRIMPL_2020*sizeof(int));

    /*calcul de la signature RSS+GHS+FG*/
    calcule_signature_GHS_2020(rss_inf, ghs_val);

    for (i = 0; i < nbsejR_2020; i++)
    if (rum[i] != NULL)
    {
        free(rum[i]);
        rum[i] = NULL;
    }

    ok_gr = 9999;

    for (i = 0; i < MAXERRIMPL_2020; i++)
    if (ErrImpl_2020[i] != 0)
    if (ErrImpl_2020[i] < ok_gr)
        ok_gr = ErrImpl_2020[i];
    if (ok_gr != 9999)
    {
        sprintf(grp_res->cret, "%03d", ok_gr);
        return(ok_gr);
    }

    for (i = ErrCtrl_2020[0] + 1; i < MAXERRCTRL_2020; i++)
    if (ErrCtrl_2020[i] != 0)
    if ((ErrCtrl_2020[i] < ok_gr) && ErreurCtrlBloquante_2020(ErrCtrl_2020[i]))
        ok_gr = ErrCtrl_2020[i];

    if (ok_gr != 9999)
    {
        sprintf(grp_res->cret, "%03d", ok_gr);
        return(ok_gr);
    }

    for (i = 0; i < MAXERRARB_2020; i++)
    if (ErrArb_2020[i] != 0)
    if (ErrArb_2020[i] < ok_gr)
        ok_gr = ErrArb_2020[i];
    if (ok_gr != 9999)
    {
        sprintf(grp_res->cret, "%03d", ok_gr);
        return(ok_gr);
    }

    /* CD 08/12/03 gestion des erreurs non bloquantes*/
    errcat_okgr = 9999;
    for (i = ErrCtrl_2020[0] + 1; i < MAXERRCTRL_2020; i++)
    if (ErrCtrl_2020[i] != 0)
    {
        errcat = getCategorie_2020(ErrCtrl_2020[i]);
        if ((errcat < errcat_okgr) || ((errcat == errcat_okgr) && (ErrCtrl_2020[i] < ok_gr)))
        {
            if (errcat != 7)
            {
                ok_gr = ErrCtrl_2020[i];
                errcat_okgr = errcat;
            }
        }
    }

    if (ok_gr != 9999)
    {
        sprintf(grp_res->cret, "%03d", ok_gr);
        return(ok_gr);
    }
    return(0);
}

void  rempli_info_2020(RSS_INFO_2020 *rss_inf, GRP_ENTREE_2020 *pgrp_ent)
{
    char zone[10];
    int i;
    int nbacte_rum, nbdas_rum, nbdad_rum;

    rss_inf->agea = age_an_2020;
    rss_inf->agej = age_jour_2020;

    rss_inf->dstot = _dsj_2020;

    rempli_info_rum_2020(rss_inf, pgrp_ent->rum2);

    rss_inf->actlist.listzac = (char *)malloc(nbactes_2020 * 29 + 1);
    for (i = 0; i < nbsej_2020; i++)
    {
        nbacte_rum = GetNombreActesNum_2020(pgrp_ent->rum2[i], -1);
        nbdas_rum = GetNombreDiagnosticsNum_2020(pgrp_ent->rum2[i], -1);
        strncpy(zone, pgrp_ent->rum2[i] + 120, 2); zone[2] = 0;
        nbdad_rum = atoi(zone);
        strncpy(rss_inf->actlist.listzac + indacte_2020[i] * 29, pgrp_ent->rum2[i] + 177 + (nbdas_rum + nbdad_rum) * 8, nbacte_rum * 29);
    }
    *(rss_inf->actlist.listzac + (nbactes_2020 * 29)) = 0;
    rss_inf->actlist.nbac = nbactes_2020;

    rss_inf->diaglist.listdiag = (char *)malloc(nbdiags_2020*(lgdiag_2020)+3);
    /*FG11 inversion DP/DR*/
    /*if(!binversionDPDR_11b)*/
    {
        for (i = 0; i < nbdc_2020; i++)
            strncpy(rss_inf->diaglist.listdiag + i*lgdiag_2020, ptr_diags_2020 + *(ptr_inddiag_2020 + i)*lgdiag_2020, lgdiag_2020);
    }

    *(rss_inf->diaglist.listdiag + (nbdc_2020*lgdiag_2020)) = 0;

    /* MB 13/12/03*/
    rss_inf->diaglist.nbdiag = nbdc_2020;
}

/*Cette fonction remplit le tableau tabval de ghs_val, afin de donner les
éléments RUM par RUM permettant de calculer la valorisation des suppléments
pour la réa, la surveillance continue et les soins intensifs*/
/*CD MAJ FG 10
void rempli_valo_rum(GHS_VALORISATION_11b *ghs_val,char *rum[])*/
void rempli_info_rum_2020(RSS_INFO_2020 *rssinf, char *rum[])
{
    int igsrum;
    int get_igs_rum_2020(int i);
    int i, j, k, a, m;
    int dernierTypeUM;
    char zone[3];
    unsigned char article[12];
    char trans[4];
    long hdg10;
    int hdgf10;
    int lbsc;
    unsigned char ptr_src_DIAG_actes_associes[4 * 50];
    int nbactesassocies, nbactesassociesenfants;
    long haccam;
    int hafccam;
    char *adrcodiag;
    char *adrcodacte;
    unsigned char *acteass;
    int deduction_igs_age;
    int src_critereacte[tab_max];/*0 :pas traité;1:critère vrai;2:critère faux*/

    /*calcul de la déduction des points générés par l'age*/
    if (age_an_2020 < 40) deduction_igs_age = 0;
    else
    if (age_an_2020 <= 59) deduction_igs_age = 7;
    else
    if (age_an_2020 <= 69) deduction_igs_age = 12;
    else
    if (age_an_2020 <= 74) deduction_igs_age = 15;
    else
    if (age_an_2020 <= 79) deduction_igs_age = 16;
    else deduction_igs_age = 18;

    zone[2] = 0;
    memset(src_critereacte, 0, sizeof(src_critereacte));
    for (i = 0; i < nbsejR_2020; i++)
    {
        igsrum = get_igs_rum_2020(i);
        if (sej_rea_2020(i, rum[i], igsrum) != 0)
        {
            rssinf->tabval[i].brea = 1;
        }
        else
        {
            rssinf->tabval[i].brea = 0;
        }

        dernierTypeUM = sejouravdernAut_2020(i);
        if (dernierTypeUM == TYPUM_BLANC)
            strncpy(rssinf->tabval[i].typum, "  ", 2);
        else
        {
            sprintf(zone, "%02d", dernierTypeUM);
            strncpy(rssinf->tabval[i].typum, zone, 2);
        }
        rssinf->tabval[i].dsp = los_2020[i];

        /*éligibilité du RUM au supplément SRC*/
        lbsc = 0;
        if ((age_an_2020 >= 18) && ((igsrum - deduction_igs_age) >= 15))
            lbsc = 1;
        else
        if ((age_an_2020 < 18) || ((igsrum - deduction_igs_age) >= 7))
        {
            /*DP,DR DA seul ?*/
            /*FG 11bMaj : on ne teste plus seulement le DP mais aussi le DR et les DA
            for(j=inddiag_11b[i];j<=inddiag_11b[i];j++)*/
            for (j = inddiag_2020[i]; j < inddiag_2020[i + 1]; j++)
            {
                /*if(j==(inddiag_11b[i]+1))*/ /*ne s'applique pas au DR*/
                /*    continue;*/
                /*FG 11bMaj : liste de DIAG pour les enfants seulement*/
                if (age_an_2020 < 18)/*enfants seulement*/
                {
                    if ((*(ptr_dmask_2020 + j*_dxmsize_2020 + 22)) & puisi2_2020[0])
                    {
                        lbsc = 1;
                        break;
                    }
                }
                /*adultes + enfants*/
                if ((*(ptr_dmask_2020 + j*_dxmsize_2020 + 21)) & puisi2_2020[3])
                {
                    lbsc = 1;
                    break;
                }
            }
        }
        if (!lbsc)
        {
            /*acte seul déjà traité sur le RUM précédent?*/
            if ((i > 0) && (src_critereacte[i - 1] == 1))
                lbsc = 1;
            else
            {
                /*acte seul sur le RUM courant?*/
                for (j = indacte_2020[i]; j < indacte_2020[i + 1]; j++)
                {
                    if (*(ptr_amask_2020 + (j*_prmsize_2020) + 38) & puisi2_2020[7])
                    {
                        lbsc = 1;
                        src_critereacte[i] = 1;
                        break;
                    }
                }
                if (!lbsc)
                    src_critereacte[i] = 2;

                /*acte seul sur le RUM précédent?*/
                if ((i > 0) && (!lbsc))
                {
                    if (!(src_critereacte[i - 1]))
                    {
                        for (j = indacte_2020[i - 1]; j < indacte_2020[i]; j++)
                        {
                            if (*(ptr_amask_2020 + (j*_prmsize_2020) + 38) & puisi2_2020[7])
                            {
                                lbsc = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        /*if(!lbsc && ((igsrum-deduction_igs_age)>=7))*/
        if (!lbsc && ((age_an_2020 < 18) || ((igsrum - deduction_igs_age) >= 7)))
        {
            /*DP,DR ou DAS seul ?*/
            /*=>déjà fait avant*/

            /*DP,DR ou DA associé à un acte ?*/
            /*FG 11bMaj : on ne teste plus seulement le DP mais aussi le DR et les DA
            for(m=inddiag_11b[i];m<=inddiag_11b[i];m++)*/
            for (m = inddiag_2020[i]; ((m < inddiag_2020[i + 1]) && (!lbsc)); m++)
            {
                /*if(m==(inddiag_11b[i]+1))*/ /*ne s'applique pas au DR*/
                /*    continue;*/
                /*FG 11bMaj : liste de combinaison DIAG-ACTE pour les enfants seulement
                if((*(ptr_dmask_11b+m*_dxmsize_11b+21)) & puisi2_11b[4])*/
                nbactesassocies = 0;
                adrcodiag = ptr_diags_2020 + m*lgdiag_2020;
                strncpy(trans, adrcodiag, 3); trans[3] = '\0';
                hdg10 = (long)(toupper(trans[0]) - 'A') * 100;
                hdg10 += atoi(trans + 1);
                hashcode_2020(adrcodiag + 3, &hdgf10);
                /*Adultes+Enfants*/
                if ((*(ptr_dmask_2020 + m*_dxmsize_2020 + 21)) & puisi2_2020[4])
                {
                    /*FG 11e:TEM
                    fseek(srcdgac_2020,_offstSRCDGACTADENF_2020,SEEK_SET);*/

                    /*on mémorise la liste des actes associés au diag m courant du RUM i*/
                    for (j = 0; j < _NbArticlesSRCDGACTADENF_2020; j++)
                    {
                        /*FG 11e:TEM
                        fread(article,sizeof(unsigned char),_TailleArticleSRCDGACTADENF_2020,srcdgac_2020);*/
                        memcpy(article, h_srcdgac_2020 + _offstSRCDGACTADENF_2020 + j*sizeof(unsigned char)*_TailleArticleSRCDGACTADENF_2020, sizeof(unsigned char)*_TailleArticleSRCDGACTADENF_2020);

                        if (((article[0] * 256 + article[1]) == hdg10) && ((article[2] * 256 + article[3]) == hdgf10))
                        {
                            memcpy(ptr_src_DIAG_actes_associes + nbactesassocies * 4, &article[4], 4);
                            nbactesassocies++;
                        }
                        else
                        {
                            if (nbactesassocies > 0)
                                break;
                        }
                    }
                }
                /*Enfants seulement*/
                nbactesassociesenfants = 0;
                if ((age_an_2020 < 18) && ((*(ptr_dmask_2020 + m*_dxmsize_2020 + 22)) & puisi2_2020[1]))
                {
                    /*FG 11e:TEM
                    fseek(srcdgac_2020,_offstSRCDGACTENFANT_2020,SEEK_SET);*/
                    /*on mémorise la liste des actes associés au diag m courant du RUM i*/
                    for (j = 0; j < _NbArticlesSRCDGACTENFANT_2020; j++)
                    {
                        /*FG 11e:TEM
                        fread(article,sizeof(unsigned char),_TailleArticleSRCDGACTENFANT_2020,srcdgac_2020);*/
                        memcpy(article, h_srcdgac_2020 + _offstSRCDGACTENFANT_2020 + j*sizeof(unsigned char)*_TailleArticleSRCDGACTENFANT_2020, sizeof(unsigned char)*_TailleArticleSRCDGACTENFANT_2020);
                        if (((article[0] * 256 + article[1]) == hdg10) && ((article[2] * 256 + article[3]) == hdgf10))
                        {
                            memcpy(ptr_src_DIAG_actes_associes + (nbactesassociesenfants + nbactesassocies) * 4, &article[4], 4);
                            nbactesassociesenfants++;
                        }
                        else
                        {
                            if (nbactesassociesenfants > 0)
                                break;
                        }
                    }
                }
                nbactesassocies = nbactesassocies + nbactesassociesenfants;
                if (nbactesassocies > 0)
                {
                    /*on parcours chaque acte du RUM i pour voir s'il appartient à la liste des actes associés*/
                    for (j = 0; ((j < ToInt_2020(rumin_2020[i].nba, 3)) && (!lbsc)); j++)
                    {
                        haccam = 0;
                        adrcodacte = ptr_actes_2020 + (indacte_2020[i] + j)*lgacte_2020;
                        for (k = 0; k < 3; k++)
                        {
                            haccam += (long)((toupper(adrcodacte[k]) - 'A')*puis26_2020[k]);
                        }
                        if (haccam < 0 || haccam >= 17576) continue;
                        hafccam = (adrcodacte[3] - 'A') * 1000 + ToInt_2020(&adrcodacte[4], 3);

                        for (a = 0; a < nbactesassocies; a++)
                        {
                            acteass = ptr_src_DIAG_actes_associes + a * 4;
                            if (((acteass[0] * 256 + acteass[1]) == haccam) && ((acteass[2] * 256 + acteass[3]) == hafccam))
                            {
                                lbsc = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        rssinf->tabval[i].bsc = lbsc;
    }
}

void calcule_ghs_2020(int typetab, int codedcmd, int codedghm, int *ighs, int *bXsup, int *bXinf, int *ptypeexb)
{
    int lnbart;
    unsigned char article[22];
    int i;
    int offset;
    int numtest, test, param, fintest, reponse;
    int chercheReponseFausse; /*mode ou l'on cherche le premier
                              enregistrement dans la table où le champ "réponse"
                              est égal à zéro*/
    int fin;
    unsigned char *article_ptr;/*FG 11e:TEM*/

    /*strcpy(ghs, "9999");*/
    *ighs = 9999;
    *bXsup = 999;
    *bXinf = 0;
    *ptypeexb = 0;
    autorisationUMGHS_2020 = TYPUM_BLANC;

    if (typetab == 1) /*DGF*/
        offset = 0;
    else
        offset = 6;

    article[21] = 0;
    article_ptr = 0;
    if (_lTailleArticleGHS_2020 == 0) /*precaution*/
    {
        /*strcpy(ghs, "9999");*/
        *ighs = 9999;
        *bXsup = 999;
        *bXinf = 0;
        AddErreur_2020(IMPLEMENTATION, 4, 0);
        return;
    }
    lnbart = _lTailleTotaleTableGHS_2020 / _lTailleArticleGHS_2020;
    /*FG 11e:TEM
    fseek(ghsinf_2020,_offstghs_2020,SEEK_SET);*/

    /*recherche de la premiere ligne avec le GHM concerné*/
    for (i = 0; i < lnbart; i++)
    {
        /*FG 11e:TEM
        fread(article,sizeof(char),21,ghsinf_2020);*/
        article_ptr = h_ghsinf_2020 + _offstghs_2020 + i*_lTailleArticleGHS_2020;
        /*FG 11e:TEM if(article[0]==codedcmd)*/
        if (article_ptr[0] == codedcmd)
        {
            /*FG 11e:TEM
            if((article[1]*256+article[2])==codedghm)*/
            if ((article_ptr[1] * 256 + article_ptr[2]) == codedghm)
                break;
        }
    }
    if (i == lnbart) /*on n'a pas trouvé le GHM*/
    {
        /*strcpy(ghs, "9999");*/
        *ighs = 9999;
        *bXsup = 999;
        *bXinf = 0;
        return;
    }
    else
    {
        test = 0;
        fintest = 0;
        chercheReponseFausse = 0;
        fin = 0;
        while (!fin)
        {
            /*FG 11e:TEM
            numtest=article[4];
            param=article[5]*256+article[6];
            reponse=article[7];
            fintest=article[8];*/
            numtest = article_ptr[4];
            param = article_ptr[5] * 256 + article_ptr[6];
            reponse = article_ptr[7];
            fintest = article_ptr[8];

            if ((chercheReponseFausse == 0) || (reponse == 0))
            {
                /*appel de la fonction de test*/
                test = (*funcbufghs_2020[numtest])(param);
                if (reponse == 0)
                    chercheReponseFausse = 0;
            }
            else
            {
                test = 0;
            }
            if ((test == 1) && (fintest == 1))
            {
                *ighs = article_ptr[9 + offset] * 256 + article_ptr[10 + offset];

                *bXsup = article_ptr[11 + offset] * 256 + article_ptr[12 + offset];
                *bXinf = article_ptr[13 + offset] * 256 + article_ptr[14 + offset];
                *ptypeexb = article_ptr[3];
                return;
            }
            if ((test == 0) && (fintest == 0))
                chercheReponseFausse = 1;

            /*FG 11e:TEM
            fread(article,sizeof(char),21,ghsinf_2020);*/
            if (article_ptr != (h_ghsinf_2020 + _offstghs_2020 + lnbart*_lTailleArticleGHS_2020))
                article_ptr += _lTailleArticleGHS_2020;
            /*FG 11e:TEM
            if ((feof(ghsinf_2020)) || (article[0]!=codedcmd) || ((article[1]*256+article[2])!=codedghm))*/
            if (((article_ptr == (h_ghsinf_2020 + _offstghs_2020 + lnbart*_lTailleArticleGHS_2020))) || (article_ptr[0] != codedcmd) || ((article_ptr[1] * 256 + article_ptr[2]) != codedghm))
            {
                /*problème de cohérence dans la table binaire des GHS*/
                *ighs = 9999;
                *bXsup = 999;
                *bXinf = 0;
                AddErreur_2020(IMPLEMENTATION, 4, 0);
                fin = 1;
            }
        }
    }
}


void ghsminore_2020(int ighs, int *flagghsminor)
{
    int lnbart,j;
    unsigned char *article;
    int ghslu;
    *flagghsminor = 0; 
    if (_lTailleArticleGHSMINOR_2020 == 0) /*precaution*/
    {
        AddErreur_2020(IMPLEMENTATION, 4, 0);
        return;
    }

    lnbart = _lTailleTotaleTableGHSMINOR_2020 / _lTailleArticleGHSMINOR_2020;
    /*FG 11e:TEM
    fseek(innovref_2020,_offstinnovref_2020,SEEK_SET);*/

    /*recherche de la premiere ligne avec le code concerné*/
    for (j = 0; j < lnbart; j++)
    {
        article = h_ptghsminor_2020 + _offstghsminor_2020 + j*_lTailleArticleGHSMINOR_2020;
        ghslu = article[0] * 256 + article[1];
        if (ghslu == ighs)
        {
            *flagghsminor = 1;
            break;
        }
        else
        if (ghslu>ighs)
            break;
    }
}

/* FONCTIONS UTILISEES POUR LE CALCUL DES SUPPLEMENTS */
/*fonction suppléments n°0*/
void _supnull_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*ne fait rien*/
    if (ptypsup)
        *ptypsup = typsupaucun;
    return;
}
/*fonction supplément n°1*/
void _supnn1_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*FG13 SUPRUM psupp->durnna+=nbjours;*/
    psupp->durnna[pnsej] += nbjours;
    if (ptypsup)
        *ptypsup = typsupnna;
    /*psupp->flagnna=1;*/
}
/*fonction supplément n°2*/
void _supnn2_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*FG13 SUPRUM psupp->durnnb+=nbjours;*/
    psupp->durnnb[pnsej] += nbjours;
    if (ptypsup)
        *ptypsup = typsupnnb;
    /*psupp->flagnnb=1;*/
}
/*fonction supplément n°3*/
void _supnn3_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    if (prssinf->tabval[pnsej].brea == 1)
    {
        /*FG13 SUPRUM psupp->durnnc+=nbjours;*/
        psupp->durnnc[pnsej] += nbjours;
        /*psupp->flagnnc=1;*/
        if (ptypsup)
            *ptypsup = typsupnnc;
    }
    else
    {
        /*FG13 SUPRUM psupp->durnnb+=nbjours;*/
        psupp->durnnb[pnsej] += nbjours;
        /*psupp->flagnnb=1;*/
        if (ptypsup)
            *ptypsup = typsupnnb;
    }
}
/*fonction supplément n°4*/
void _suprea_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    if ((prssinf->tabval[pnsej].brea == 1) && (bActesMarqueursUMREA_2020[pnsej] == 1))
    {
        /*FG13 SUPRUM psupp->nbjours_rea+=nbjours;*/
        psupp->nbjours_rea[pnsej] += nbjours;
        /*psupp->flagrea=1;*/
        if (ptypsup)
            *ptypsup = typsuprea;
    }
    else
    {
        /*FG13 SUPRUM psupp->nbjours_rea_si+=nbjours;*/
        psupp->nbjours_rea_si[pnsej] += nbjours;
        /*psupp->flagreasi=1;*/
        if (ptypsup)
            *ptypsup = typsupreasi;
    }
}

/*fonction supplément n°5*/
void _supsra_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*FG13 SUPRUM psupp->nbjours_sra+=nbjours;*/
    psupp->nbjours_sra[pnsej] += nbjours;
    /*psupp->flagsra=1;*/
    if (ptypsup)
        *ptypsup = typsupsra;

}
/*fonction supplément n°6*/
void _supsc_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    if (prssinf->tabval[pnsej].bsc)
    {
        /*FG13 SUPRUM psupp->nbjours_sc+=nbjours;*/
        psupp->nbjours_sc[pnsej] += nbjours;
        /*psupp->flagsc=1;*/
        if (ptypsup)
            *ptypsup = typsupsc;
    }
    else
        *ptypsup = typsupaucun;
}
/*fonction supplément n°7*/
void _supssc_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*FG13 SUPRUM psupp->nbjours_ssc+=nbjours;*/
    psupp->nbjours_ssc[pnsej] += nbjours;
    /*psupp->flagssc=1;*/
    if (ptypsup)
        *ptypsup = typsupssc;
}

/*fonction supplément n°8*/
void _supsi_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    /*FG13 SUPRUM psupp->nbjours_si+=nbjours;*/
    psupp->nbjours_si[pnsej] += nbjours;
    /*psupp->flagsi=1;*/
    if (ptypsup)
        *ptypsup = typsupsi;
}
/*fonction supplément n°9*/
void _suprep_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    if (age_an_2020 < 18)
    {
        if (prssinf->tabval[pnsej].brea == 1)
        {
            /*FG13 SUPRUM psupp->nbjours_reaped+=nbjours;*/
            psupp->nbjours_reaped[pnsej] += nbjours;
            /*psupp->flagreaped=1;*/
            if (ptypsup)
                *ptypsup = typsupreaped;
        }
        else
        {
            /*FG13 SUPRUM psupp->nbjours_rea_si+=nbjours;*/
            psupp->nbjours_rea_si[pnsej] += nbjours;
            /*psupp->flagreasi=1;*/
            if (ptypsup)
                *ptypsup = typsupreasi;
        }
    }
    else
        /*on tente de le rebasculer en REA, sinon en STF*/
    {
        if ((prssinf->tabval[pnsej].brea == 1) && (bActesMarqueursUMREA_2020[pnsej] == 1))
        {
            psupp->nbjours_rea[pnsej] += nbjours;
            if (ptypsup)
                *ptypsup = typsuprea;
        }
        else
        {
            psupp->nbjours_rea_si[pnsej] += nbjours;
            if (ptypsup)
                *ptypsup = typsupreasi;
        }
    }
}
/*fonction supplément n°10*/
void _suprepanc_2020(int pnsej, RSS_INFO_2020 *prssinf, supplements_2020* psupp, int nbjours, int *ptypsup)
{
    if (prssinf->tabval[pnsej].brea == 1)
    {
        /*FG13 SUPRUM psupp->durnnc+=nbjours;*/
        psupp->durnnc[pnsej] += nbjours;
        /*psupp->flagnnc=1;*/
        if (ptypsup)
            *ptypsup = typsupnnc;
    }
    else
    {
        /*FG13 SUPRUM psupp->durnnb+=nbjours;*/
        psupp->durnnb[pnsej] += nbjours;
        /*psupp->flagnnb=1;*/
        if (ptypsup)
            *ptypsup = typsupnnb;
    }
}
/* FIN DES FONCTIONS UTILISEES POUR LE CALCUL DES SUPPLEMENTS */

void AjoutePlageValo_2020(int numRUM, char *datedeb, char *datefin, int nbjours, short typautor, int typsup, int flagpremjour, char finess[], char troisemecartypum, char typhospit)
{
    int i, nbp;
    i = 0;
    nbp = plagesAutor_2020[numRUM].nbintervalles;
    while ((i<nbp) && (date_strict_sup_2020(datedeb, plagesAutor_2020[numRUM].intervalles[i].date_deb)))
        i++;
    if (nbp>i)
        memcpy(&(plagesAutor_2020[numRUM].intervalles[i + 1]), &(plagesAutor_2020[numRUM].intervalles[i]), (nbp - i)*sizeof(struct _intervalles_2020));
    strncpy(plagesAutor_2020[numRUM].intervalles[i].date_deb, datedeb, 8);
    strncpy(plagesAutor_2020[numRUM].intervalles[i].date_fin, datefin, 8);
    plagesAutor_2020[numRUM].intervalles[i].nbjours = nbjours;
    /*pour la SRC, on garde le nb de jours sans suppléments (car critères non réunis)*/
    /*plagesAutor_2020[numRUM].intervalles[i].nbjours=(typsup!=typsupaucun)?nbjours:0;*/
    plagesAutor_2020[numRUM].intervalles[i].typeautor = typautor;
    plagesAutor_2020[numRUM].intervalles[i].typesupplement = typsup;
    plagesAutor_2020[numRUM].intervalles[i].flagpremierjour = flagpremjour;
    strncpy(plagesAutor_2020[numRUM].intervalles[i].finessgeo, finess, 9);
    plagesAutor_2020[numRUM].intervalles[i].troisemecartypum = troisemecartypum;
    plagesAutor_2020[numRUM].intervalles[i].typhospit = typhospit;
    plagesAutor_2020[numRUM].nbintervalles++;
}

void transcod_ghs_2020(char *pghsc, int pghss)
{
    if (pghss > 30000) /*convention : forfait en I*/
    {
        if ((pghss - 30000) > 9)
            sprintf(pghsc, "I%03d", pghss - 30000);
        else
            sprintf(pghsc, " I%02d", pghss - 30000);
    }
    else
    if (pghss > 20000) /*convention : forfait dialyse après SROS pour ex-OQN*/
    {
        if ((pghss - 20000) > 99)
            sprintf(pghsc, "D%03d", pghss - 20000);
        else
            sprintf(pghsc, " D%02d", pghss - 20000);
    }
    else
        sprintf(pghsc, "%04d", pghss);
}

/*CD MAJ FG10B*/
/*modification de la fonction de calcul des suppléments pour tenir compte
du fichier de référence REF_AUTO, et également des autorisations données par le fichier
d'autorisation d'UM FICUM_NAME*/
/*CD FG10c : ajout de la gestion des priorités de suppléments pour les journées communes
à plusieurs UM*/
void rempli_valo_2020(int typetabliss, GHS_VALORISATION_2020 *pghs_val, RSS_INFO_2020 *rss_inf, GRP_RESULTAT_2020 *grp_res)
{
    int bXsup, bXinf;
    int ighs, ighsinnov;

    char datetemp[9];
    int dernierjourj, dernierjourm, dernierjoura;
    int i, j, s;
    int nfonc;
    int testprim;
    int typsup;
    struct infosupRUM
    {
        int typsupplement[typsupMAX];
        int numRUM[typsupMAX];/*numéro du RUM pour lequel le supplément est compté*/
    };
    struct infosupRUM tabsup[MAX_NBJ_RUM_2020];

    int dursej_partielle_cumulee;
    int premjaut;
    int sejrea[tab_max];

    /*FG 11 : type exb : 1 forfaitaire, 2 à la journée*/
    int typeexb;

    supplements_2020 supplems;
    UMINFO_2020 luminf;
    char *date_deb_intersection;
    char *date_fin_intersection;
    int nbjinterv, bsup, typeautor;
    int _tmdc_2020_innov, _tdrg_2020_innov, bXsup_innov, bXinf_innov, typeexb_innov;

    /*calcule du GHS*/
    calcule_ghs_2020(typetabliss, _tmdc_2020, _tdrg_2020, &ighs, &bXsup, &bXinf, &typeexb);

    if (ighs == 9999)
        rempli_valo_vide_2020(pghs_val);
    else
        transcod_ghs_2020(pghs_val->ghs, ighs);
    
    /*on recopie le GHS dans ghsavantInnov*/
    strncpy(pghs_val->ghsavantInnov, pghs_val->ghs, 4);
    if (ghm_Innovation_2020(&_tmdc_2020_innov, &_tdrg_2020_innov))
    {
        calcule_ghs_2020(typetabliss, _tmdc_2020_innov, _tdrg_2020_innov, &ighsinnov, &bXsup_innov, &bXinf_innov, &typeexb_innov);
        if (ighsinnov != 9999)
            transcod_ghs_2020(pghs_val->ghs, ighsinnov);
    }

    /*calcul du nombre de seances*/
    calculeseances_2020(grp_res, pghs_val);

    /*regle generale : si GHS = 9999 alors toutes les valeurs sont nulles*/
    if (ighs==9999)
        return;

    /*FG 11g : indicateur de GHS minoré*/
    ghsminore_2020(ighs, &pghs_val->ghsminor);

    /*type de séjour*/
    if (_ms_2020 == 0 && _me_2020 == 0)
        pghs_val->pih_b = 1;
    else pghs_val->pih_b = 0;
    if (my_index_2020(rss_inf->diaglist.listdiag, "Z7580", 1) >= 0)
        pghs_val->pih_a = 1;
    else
        pghs_val->pih_a = 0;

    /*EXB*/
    /*FG 11b if (rss_inf->dstot<bXinf)*/
    if (_dsj_2020 < bXinf)
    {
        /* ET : 30/06/2004*/
        if (_ms_2020 == 9)/* || critereRAAC_2020)*/
        {
            pghs_val->sejxinf = 0;
            pghs_val->nbjxinf = 0.0f;
        }
        /*CD FG11
        else
        pghs_val->sejxinf=1;*/
        else
        {
            /*pghs_val->sejxinf=typeexb;*/
            if (typetabliss == 1)
            {
                switch (typeexb)
                {
                case 0:pghs_val->sejxinf = 0; break;
                case 1:pghs_val->sejxinf = 1; break;
                case 2:pghs_val->sejxinf = 2; break;
                case 3:pghs_val->sejxinf = 1; break;
                case 4:pghs_val->sejxinf = 2; break;
                case 5:pghs_val->sejxinf = 0; break;
                case 6:pghs_val->sejxinf = 0; break;
                default:pghs_val->sejxinf = 0;
                }
            }
            else
            {
                switch (typeexb)
                {
                case 0:pghs_val->sejxinf = 0; break;
                case 1:pghs_val->sejxinf = 1; break;
                case 2:pghs_val->sejxinf = 2; break;
                case 3:pghs_val->sejxinf = 0; break;
                case 4:pghs_val->sejxinf = 0; break;
                case 5:pghs_val->sejxinf = 1; break;
                case 6:pghs_val->sejxinf = 2; break;
                default:pghs_val->sejxinf = 0;
                }
            }
            /*FG 11b if(rss_inf->dstot==0)*/
            /*FG 11bMaj : on ne fait plus -0.5*/
            /*if(_dsj_11b==0)
                pghs_val->nbjxinf=bXinf-0.5f;
                else*/
            /*FG 11b pghs_val->nbjxinf=bXinf-rss_inf->dstot;*/
            pghs_val->nbjxinf = (float)(bXinf - _dsj_2020);
        }
    }
    else
    {
        pghs_val->sejxinf = 0;
        pghs_val->nbjxinf = 0.0f;
    }

    /*EXH*/
    if (_ms_2020 != 9)
    {
        /*FG 11b if (rss_inf->dstot>bXsup)*/
        if (_dsj_2020 > bXsup)
            /*FG 11b pghs_val->nbjxsup=rss_inf->dstot-bXsup;*/
            pghs_val->nbjxsup = _dsj_2020 - bXsup;
        else
            pghs_val->nbjxsup = 0;
    }
    else
    {
        /*FG 11b if (rss_inf->dstot+1>bXsup)*/
        if (_dsj_2020 + 1 > bXsup)
            /*FG 11b pghs_val->nbjxsup=rss_inf->dstot+1-bXsup;*/
            pghs_val->nbjxsup = _dsj_2020 + 1 - bXsup;
        else
            pghs_val->nbjxsup = 0;
    }
    memset(&supplems, 0, sizeof(supplements_2020));
    dursej_partielle_cumulee = 0;
    for (i = 0; i < nbsejR_2020; i++)
    {
        if (dursej_partielle_cumulee < MAX_NBJ_RUM_2020)
            /*FG13 SUPRUM memset(tabsup[dursej_partielle_cumulee],0,(typsupMAX)*sizeof(int));*/
            memset(&tabsup[dursej_partielle_cumulee], 0, sizeof(struct infosupRUM));
        dursej_partielle_cumulee += los_2020[i];
    }
    dursej_partielle_cumulee = 0;
    memset(sejrea, 0, sizeof(sejrea));
    for (i = 0; i<nbsejR_2020; i++)
    {
        if ((los_2020[i]>0) && (rumin_2020[i].modsor != '9'))
        {
            Ajout_jour_date_2020(rumin_2020[i].datent, los_2020[i] - 1, &dernierjourj, &dernierjourm, &dernierjoura);
            sprintf(datetemp, "%02d%02d%04d", dernierjourj, dernierjourm, dernierjoura);
        }
        else
        {
            strncpy(datetemp, rumin_2020[i].datsor, 8);
        }

        if (rss_inf->tabval[i].bsc == 0)/* && (umrefs_11b.typum[u]==TYPUM_SC))*/
        {
            if (i > 0)
            {
                if (sejrea[i - 1] == 1)
                {
                    rss_inf->tabval[i].bsc = 1;
                }
            }
        }
        /*FG 1311d*/
        if (((rumin_2020[i].prov == 'R') || (rumin_2020[i].prov == 'r')) &&
            ((rumin_2020[i].modent == '0') || (rumin_2020[i].modent == '7'))
            )
            rss_inf->tabval[i].bsc = 1;

        /*FG13 SUPRUM for(u=0;u<umrefs_11b.nbaut;u++)*/
        if (indice_ums_2020[i] == -1)
        {
            dursej_partielle_cumulee += los_2020[i];
            continue;
        }
        luminf = uminf_2020[indice_ums_2020[i]];
        for (s = 0; s < luminf.nbautorisations; s++)
        {
            nbjinterv = 0;
            if (interval_autorise_hist_2020(luminf.autorisations[s].datdebeffet, luminf.autorisations[s].datfineffet, rumin_2020[i].datent, datetemp, &date_deb_intersection, &date_fin_intersection, &nbjinterv))
            {
                nfonc = getnumfoncsupplement_2020(luminf.autorisations[s].typum);
                testprim = 0;
                if (nfonc == 0)/*pas de suppléments pour ces autorisations*/
                {
                    nbjinterv = 0;
                    typsup = typsupaucun;

                    AjoutePlageValo_2020(i, date_deb_intersection, date_fin_intersection, nbjinterv, luminf.autorisations[s].typum, typsup, 0, luminf.autorisations[s].finessgeo, luminf.autorisations[s].troisemecartypum, luminf.autorisations[s].typhospit);
                }
                else
                {
                    /*test des conditions nécessaires d'attribution de suppléments*/
                    switch (luminf.autorisations[s].typum)
                    {
                    case TYPUM_SRA:
                    case TYPUM_SSC:testprim = (typetabliss != 1); break;
                    case TYPUM_NEONAT2A:
                    case TYPUM_NEONAT2B:
                    case TYPUM_NEONAT3:testprim = (age_an_2020 < 2) && (!((strncmp(grp_res->cmd, "15", 2) == 0) && (strncmp(grp_res->ghm, "M05A", 4) == 0))); break;
                        /*case TYPUM_REAPEDIA:testprim=(age_an_2020<18) && (!((strncmp(grp_res->cmd,"15",2)==0) && (strncmp(grp_res->ghm,"M05A",4)==0)));break;*/
                    case TYPUM_REAPEDIA:testprim = !((strncmp(grp_res->cmd, "15", 2) == 0) && (strncmp(grp_res->ghm, "M05A", 4) == 0)); break;
                    default:testprim = 1;
                    }
                    if (testprim)
                    {
                        /*au moins un suppléments REA va être compté pour ce RUM, c'est une info pour SRC*/
                        if (luminf.autorisations[s].typum == TYPUM_REAPEDIA)
                        {
                            if (age_an_2020 < 18)
                            {
                                if (rss_inf->tabval[i].brea == 1)
                                    sejrea[i] = 1;
                            }
                            else
                            {
                                if ((rss_inf->tabval[i].brea == 1) && (bActesMarqueursUMREA_2020[i] == 1))
                                    sejrea[i] = 1;
                            }
                        }
                        else
                        if (luminf.autorisations[s].typum == TYPUM_NEONAT3)
                        {
                            if (rss_inf->tabval[i].brea == 1)
                                sejrea[i] = 1;
                        }
                        else
                        if (luminf.autorisations[s].typum == TYPUM_REA)
                        {
                            if ((rss_inf->tabval[i].brea == 1) && (bActesMarqueursUMREA_2020[i] == 1))
                                sejrea[i] = 1;
                        }
                        /*premier jour autorisé ?*/
                        premjaut = ((!date_strict_sup_2020(luminf.autorisations[s].datdebeffet, rumin_2020[i].datent)) && (!date_strict_sup_2020(rumin_2020[i].datent, luminf.autorisations[s].datfineffet)));
                        if (premjaut) /*le jour d'entrée dans l'UM est autorisé*/
                        {
                            typsup = typsupaucun;
                            nbjinterv--;
                            (*foncsupp_2020[nfonc])(i, rss_inf, &supplems, nbjinterv, &typsup);
                            if ((typsup != typsupaucun) && (dursej_partielle_cumulee < MAX_NBJ_RUM_2020))
                            {
                                {
                                    /*tabsup[dursej_partielle_cumulee][typsup]=1;
                                    tabsup[dursej_partielle_cumulee][0]=1;*/
                                    tabsup[dursej_partielle_cumulee].typsupplement[typsup] = 1;
                                    tabsup[dursej_partielle_cumulee].typsupplement[0] = 1;

                                    if (tabsup[dursej_partielle_cumulee].numRUM[typsup] == 0)
                                        tabsup[dursej_partielle_cumulee].numRUM[typsup] = i + 1;
                                }
                            }
                            if (typsup == typsupaucun)
                                nbjinterv++;
                            AjoutePlageValo_2020(i, date_deb_intersection, date_fin_intersection, nbjinterv, luminf.autorisations[s].typum, typsup, 1, luminf.autorisations[s].finessgeo, luminf.autorisations[s].troisemecartypum, luminf.autorisations[s].typhospit);
                        }
                        else
                        {
                            typsup = typsupaucun;
                            (*foncsupp_2020[nfonc])(i, rss_inf, &supplems, nbjinterv, &typsup);
                            AjoutePlageValo_2020(i, date_deb_intersection, date_fin_intersection, nbjinterv, luminf.autorisations[s].typum, typsup, 0, luminf.autorisations[s].finessgeo, luminf.autorisations[s].troisemecartypum, luminf.autorisations[s].typhospit);
                        }
                    }
                    else
                    {
                        nbjinterv = 0;
                        typsup = typsupaucun;
                        AjoutePlageValo_2020(i, date_deb_intersection, date_fin_intersection, nbjinterv, luminf.autorisations[s].typum, typsup, 0, luminf.autorisations[s].finessgeo, luminf.autorisations[s].troisemecartypum, luminf.autorisations[s].typhospit);
                    }
                }
            }
        }/*fin for(int s=0;s<luminf.nbautorisations;s++)*/
        dursej_partielle_cumulee += los_2020[i];
    }/*fin boucle RUM*/

    /*réajustement des suppléments pour les journées en commun des RUMs*/
    dursej_partielle_cumulee = 0;
    for (i = 0; i < nbsejR_2020; i++)
    {
        if ((dursej_partielle_cumulee < MAX_NBJ_RUM_2020) && (tabsup[dursej_partielle_cumulee].typsupplement[0] == 1))
        {
            tabsup[dursej_partielle_cumulee].typsupplement[0] = 0;
            for (j = 1; j < typsupMAX - 1; j++)
            {
                if (tabsup[dursej_partielle_cumulee].typsupplement[j] == 1)
                {
                    bsup = 1;
                    switch (j)
                    {
                    case typsupreaped:supplems.nbjours_reaped[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsuprea:supplems.nbjours_rea[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupnnc:supplems.durnnc[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupreasi:supplems.nbjours_rea_si[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupsra:supplems.nbjours_sra[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupsi:supplems.nbjours_si[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupnnb:supplems.durnnb[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupsc:supplems.nbjours_sc[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupssc:supplems.nbjours_ssc[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    case typsupnna:supplems.durnna[tabsup[dursej_partielle_cumulee].numRUM[j] - 1]++; break;
                    default:bsup = 0; break;
                    }
                    if (bsup)
                    {
                        plagesAutor_2020[tabsup[dursej_partielle_cumulee].numRUM[j] - 1].intervalles[0].nbjours++;
                        plagesAutor_2020[tabsup[dursej_partielle_cumulee].numRUM[j] - 1].intervalles[0].flagpremierjour = 0;
                    }
                    break;
                }
            }
        }
        dursej_partielle_cumulee += los_2020[i];
    }

    for (i = 0; i < nbsejR_2020; i++)
    {
        pghs_val->nbjreaR[i] = supplems.nbjours_rea[i];
        pghs_val->nbrepR[i] = supplems.nbjours_reaped[i];
        pghs_val->nbjsrcR[i] = supplems.nbjours_sc[i];
        pghs_val->nbjstf_issudereaR[i] = supplems.nbjours_rea_si[i];
        pghs_val->nbjstf_totR[i] = supplems.nbjours_rea_si[i] + supplems.nbjours_si[i];
        pghs_val->nbjnnaR[i] = supplems.durnna[i];
        pghs_val->nbjnnbR[i] = supplems.durnnb[i];
        pghs_val->nbjnncR[i] = supplems.durnnc[i];

        pghs_val->nbjrea += pghs_val->nbjreaR[i];
        pghs_val->nbrep += pghs_val->nbrepR[i];
        pghs_val->nbjsrc += pghs_val->nbjsrcR[i];
        pghs_val->nbjstf_issuderea += pghs_val->nbjstf_issudereaR[i];
        pghs_val->nbjstf_tot += pghs_val->nbjstf_totR[i];
        pghs_val->nbjnna += pghs_val->nbjnnaR[i];
        pghs_val->nbjnnb += pghs_val->nbjnnbR[i];
        pghs_val->nbjnnc += pghs_val->nbjnncR[i];

    }
    for (i = 0; i < nbsejR_2020; i++)
    {
        for (s = 0; s < plagesAutor_2020[i].nbintervalles; s++)
        {
            switch (plagesAutor_2020[i].intervalles[s].typesupplement)
            {
            case typsupaucun:typeautor = TYPUM_BLANC; break;
            case typsupreaped:typeautor = TYPUM_REAPEDIA; break;
            case typsuprea:typeautor = TYPUM_REA; break;
            case typsupnnc:typeautor = TYPUM_NEONAT3; break;
            case typsupreasi:typeautor = TYPUM_SI; break;
            case typsupsra:typeautor = TYPUM_SRA; break;
            case typsupsi:typeautor = TYPUM_SI; break;
            case typsupnnb:typeautor = TYPUM_NEONAT2B; break;
            case typsupsc:typeautor = TYPUM_SC; break;
            case typsupssc:typeautor = TYPUM_SSC; break;
            case typsupnna:typeautor = TYPUM_NEONAT2A; break;
            default:typeautor = TYPUM_BLANC; break;
            }
            plagesAutor_2020[i].intervalles[s].typesupplement = typeautor;
        }
    }

    analyse_actes_2020(rss_inf, pghs_val, grp_res->cmd, grp_res->ghm);
    /*supplement antepartum*/
    if (nbjdateaccouch_2020 > 0)
        CalculeSupplementAntepartum_2020(pghs_val);
}
/*Permet d'avoir le ou les types d'autorisations par RUM en cas de groupage en erreur*/
void infoUM_2020()
{
    char datetemp[9];
    int dernierjourj, dernierjourm, dernierjoura;
    int i, s;
    int typsup;
    UMINFO_2020 luminf;
    char *date_deb_intersection;
    char *date_fin_intersection;
    int nbjinterv;
    for (i = 0; i<nbsejR_2020; i++)
    {
        if ((los_2020[i]>0) && (rumin_2020[i].modsor != '9'))
        {
            Ajout_jour_date_2020(rumin_2020[i].datent, los_2020[i] - 1, &dernierjourj, &dernierjourm, &dernierjoura);
            sprintf(datetemp, "%02d%02d%04d", dernierjourj, dernierjourm, dernierjoura);
        }
        else
        {
            strncpy(datetemp, rumin_2020[i].datsor, 8);
        }

        if (indice_ums_2020[i] == -1)
            continue;
        luminf = uminf_2020[indice_ums_2020[i]];
        for (s = 0; s < luminf.nbautorisations; s++)
        {
            nbjinterv = 0;
            if (interval_autorise_hist_2020(luminf.autorisations[s].datdebeffet, luminf.autorisations[s].datfineffet, rumin_2020[i].datent, datetemp, &date_deb_intersection, &date_fin_intersection, &nbjinterv))
            {
                nbjinterv = 0;
                typsup = TYPUM_BLANC;

                /*mémorisation des autorisations en cas de changement durant le séjour dans l'UM*/
                AjoutePlageValo_2020(i, date_deb_intersection, date_fin_intersection, nbjinterv, luminf.autorisations[s].typum, typsup, 0, luminf.autorisations[s].finessgeo, luminf.autorisations[s].troisemecartypum, luminf.autorisations[s].typhospit);
            }
        }/*fin for*/
    }/*fin boucle RUM*/
}

void rempli_valo_vide_2020(GHS_VALORISATION_2020 *ghs_val)
{
    strcpy(ghs_val->ghs, "9999");
    ghs_val->nbjxsup = 0;
    ghs_val->sejxinf = 0;
    ghs_val->nbjxinf = 0.0f;

    /*CD MAJ FG10*/
    ghs_val->nbjrea = 0;
    ghs_val->nbjsrc = 0;
    ghs_val->nbjstf_issuderea = 0;
    ghs_val->nbjstf_tot = 0;

    /*CD FG 12 SUPPLPARRUM*/
    memset(ghs_val->nbjreaR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbrepR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjsrcR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjstf_issudereaR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjstf_totR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjnnaR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjnnbR, 0, tab_max*sizeof(int));
    memset(ghs_val->nbjnncR, 0, tab_max*sizeof(int));

    ghs_val->nbac687 = 0;
    ghs_val->nbac688 = 0;
    ghs_val->nbac689 = 0;

    ghs_val->nbacRX1 = 0;
    ghs_val->nbacRX2 = 0;
    ghs_val->nbacRX3 = 0;
    ghs_val->nbRCMI = 0;

    ghs_val->nbactsupp9623 = 0;
    /*ghs_val->nbactsupp9624=0;*/
    ghs_val->nbactsupp9625 = 0;
    ghs_val->nbactsupp9631 = 0;
    ghs_val->nbactsupp9632 = 0;
    ghs_val->nbactsupp9633 = 0;

    ghs_val->acprlvtorg = 0;
    ghs_val->pih_a = 0;
    ghs_val->pih_b = 0;

    ghs_val->nbjnna = 0;
    ghs_val->nbjnnb = 0;
    ghs_val->nbjnnc = 0;

    ghs_val->nbhd = 0;
    ghs_val->nbent1 = 0;
    ghs_val->nbent2 = 0;
    ghs_val->nbent3 = 0;
    ghs_val->nbchypb = 0;

    /*CD MAJ FG10B*/
    ghs_val->nbrep = 0;

    /*CD FG10c protonthérapie et ICT*/
    ghs_val->nbprot = 0;
    ghs_val->nbict = 0;
    /*FG1311d*/
    ghs_val->nbantepartum = 0;
    ghs_val->nbradiopedia = 0;

    /*FG1311g*/
    ghs_val->ghsminor = 0;
    ghs_val->supplSDC = 0;
    ghs_val->nbsupp_2= 0;

    /*FG 2016*/
	ghs_val->flagAvastin = 0;
	
}

/*CD 04/05/06*/
void init_valo_vide_2020(GHS_VALORISATION_2020 *ghs_val)
{
    memset(ghs_val, 0, sizeof(GHS_VALORISATION_2020));
    strcpy(ghs_val->ghs, "9999");
    strcpy(ghs_val->ghsavantInnov, "9999");
    memset(plagesAutor_2020, 0, tab_max*sizeof(intervallesAutor_2020));
}

void init_info_vide_2020(RSS_INFO_2020 *rss_inf)
{
    rss_inf->agea = 0;
    rss_inf->agej = 0;
    rss_inf->dstot = 0;

    rss_inf->actlist.nbac = 0;
    rss_inf->actlist.listzac = 0;
    rss_inf->diaglist.nbdiag = 0;
    rss_inf->diaglist.listdiag = 0;

    /*CD MAJ FG10*/
    memset(rss_inf->tabval, 0, tab_max*sizeof(INFO_RUM_2020));
    /*CD FG11*/
    memset(rss_inf->sigrssghsfg, 0, sizeof(rss_inf->sigrssghsfg));
}

int FreeErr_2020(char *rum[], int noerr, int type, GRP_RESULTAT_2020 *grp_res, GHS_VALORISATION_2020 *ghs_val, RSS_INFO_2020 *rss_inf)
{
    int j, i;

    for (j = 0; j < nbsejR_2020; j++)
    if (rum[j] != NULL)
    {
        free(rum[j]);
        (rum)[j] = NULL;
    }

    strcpy(grp_res->cmd, "90");
    sprintf(grp_res->cret, "%03d", noerr);
    grp_res->ns_sor = nbsej_2020;
    switch (type)
    {
        /*CD	   case IMPLEMENTATION : strcpy(grp_res->ghm,"910");*/
    case IMPLEMENTATION: strcpy(grp_res->ghm, "Z03Z");
        break;
    case GROUPAGE: strcpy(grp_res->ghm, "903");
        break;
        /*CD	   case CONTROLE : strcpy(grp_res->ghm,"900");*/
    case CONTROLE: strcpy(grp_res->ghm, "Z00Z");
        break;
    }

    rss_inf->agea = age_an_2020;
    rss_inf->agej = age_jour_2020;

    /*FG11b
    if (_dsj_11b>999)
    rss_inf->dstot=999;
    else
    rss_inf->dstot=_dsj_11b;*/
    rss_inf->dstot = _dsj_2020;

    /*calcul de la signature RSS+GHS+FG*/
    calcule_signature_GHS_2020(rss_inf, ghs_val);

    /*FG13 free_ptr_2020();*/
    for (i = 0; i < MAXERRCTRL_2020; i++)grp_res->ErrCtrl[i] = ErrCtrl_2020[i];
    for (i = 0; i < MAXERRARB_2020; i++) grp_res->ErrArb[i] = ErrArb_2020[i];
    for (i = 0; i < MAXERRIMPL_2020; i++)grp_res->ErrImpl[i] = ErrImpl_2020[i];

    return(noerr);
};

int dxmap10_2020(int indice, char sexe)
{
    long pos;
    unsigned char prof[10];

    /*CD*/
    memset(prof, 0, sizeof(prof));

    if (!convdiag10_2020(ptr_diags_2020 + indice*lgdiag_2020, prof))
        return 0;
    memcpy(ptr_emask_2020 + indice * 4, prof + 3, 4);
    pos = ((prof[0] * 256) + prof[1])*_dxmsize_2020 * 2 + _doffpro10_2020;
    if (sexe == '2')
        pos += _dxmsize_2020;

    /*FG 11e:TEM
    fseek(ptmdiag10_2020,pos,0);
    fread(ptr_dmask_2020+indice*_dxmsize_2020,_dxmsize_2020,1,ptmdiag10_2020);*/
    memcpy(ptr_dmask_2020 + indice*_dxmsize_2020, h_ptmdiag10_2020 + pos, _dxmsize_2020);

    pos = _doffoms10_2020 + (prof[2] * _prfsize_2020);
    /*FG 11e:TEM
    fseek(ptmdiag10_2020,pos,0);
    fread(ptr_omask_2020+indice*_prfsize_2020,_prfsize_2020,1,ptmdiag10_2020);*/
    memcpy(ptr_omask_2020 + indice*_prfsize_2020, h_ptmdiag10_2020 + pos, _prfsize_2020);
    return (1);
}

int trmap_2020(int nn)
{
    long pos;
    unsigned char buflec[7];

    pos = nn*_trsize_2020 + _toffset_2020;

    /*FG 11e:TEM
    fseek(pttree_2020,pos,0);
    fread(buflec,_trsize_2020,1,pttree_2020);*/
    memcpy(buflec, h_pttree_2020 + pos, _trsize_2020);

    treetab_2020.subclus = buflec[0];
    treetab_2020.param1 = buflec[1];
    treetab_2020.param2 = buflec[2];
    treetab_2020.nbranch = buflec[3];
    treetab_2020.pfils = (buflec[4] * 256) + buflec[5];
    _stpos_2020 = treetab_2020.pfils;
    return(0);
} /* int trmap_2020 */



int prmap_ccam_2020(int indice)
{
    long pos;
    unsigned char prof[10];

    /*Si l'acte est trouvé dans le fichier ccamcara, prof contient le profil
    Si l'acte n'est pas trouvé, prof est nul (tout à zéro)*/
    if (!convacte_ccam_2020(ptr_actes_2020 + indice*lgacte_2020, prof))
        return 0;

    pos = ((prof[0] * 256) + prof[1])*_prmsize_2020 + _poffpro_ccam_2020;

    *(ptr_datdeb_2020 + indice) = (int)((prof[2] * 256) + prof[3]);
    *(ptr_datfin_2020 + indice) = (int)((prof[4] * 256) + prof[5]);

    /*
    *(ptr_datdeb_2020+indice)=(int)0;
    *(ptr_datfin_2020+indice)=(int)32500;
    */
    /*FG 11e:TEM
    fseek(ptaccam_2020,pos,0);
    fread(ptr_amask_2020+indice*_prmsize_2020,_prmsize_2020,1,ptaccam_2020);*/
    memcpy(ptr_amask_2020 + indice*_prmsize_2020, h_ptaccam_2020 + pos, _prmsize_2020);

    return (1);
}


int group_2020(char cret[], char cmd[], char ghm[])
{
    int i, rval, err;
    char zone[5];
    void decode_ghm_2020(char[], int);
    int itemp;

    /*CD FG11*/
    binversionDPDR_2020 = 0;

    /*CD FG13.11d*/
    groupeRacine = -1;
    annuleActeOperGHMMed = 0;

    _sex_2020 = rumin_2020[0].sexe;
    zone[0] = rumin_2020[nbsej_2020 - 1].modsor;
    zone[1] = 0;
    _ms_2020 = atoi(zone);
    zone[0] = rumin_2020[nbsej_2020 - 1].dest;
    zone[1] = 0;
    _ds_2020 = atoi(zone);

    /*zone[0]=rumin_2020[0].dest;*/
    zone[0] = rumin_2020[0].prov;
    zone[1] = 0;
    _pr_2020 = atoi(zone);
    if ((rumin_2020[0].prov == 'R') || (rumin_2020[0].prov == 'r'))
        _pr_2020 = rumin_2020[0].prov - '0';

    zone[0] = rumin_2020[0].modent;
    zone[1] = 0;
    _me_2020 = atoi(zone);

    _hp30_2020 = rumin_2020[0].hosp30;
    if (strncmp(rumin_2020[0].pdsnaiss, "    ", 4) != 0)
    {
        strncpy(zone, rumin_2020[0].pdsnaiss, 4);
        zone[4] = 0;
        _pdsn_2020 = atoi(zone);
    }
    else
        _pdsn_2020 = 0;
    strncpy(zone, rumin_2020[0].nbseance, 2);
    zone[2] = 0;
    _seance_2020 = atoi(zone);

    /*CD MAJ FG10*/
    if (uhcd_2020 == 1)
    {
        dsejavantuhcd_2020 = _dsj_2020;
        _dsj_2020 = 0;
    }

    if (strncmp(rumin_2020[0].datent, rumin_2020[nbsej_2020 - 1].datsor, 8) == 0)
    {
        if (_seance_2020 == 0)
            _los_2020 = 4;
        else
            _los_2020 = 8;
    }
    else
    {
        if (_seance_2020 == 0)
            _los_2020 = 1;
        else
            _los_2020 = 2;
    }
    err = 0;
    i = 0;
    trmap_2020(i);
    /*fprintf(ptfTrace,"[%d]--%d.%d-",nbenr,i,treetab_2020.subclus);*/
    while (treetab_2020.subclus != 12)
    {
        rval = (*funcbuf_2020[treetab_2020.subclus])();
        /*fprintf(ptfTrace,"%d",rval);*/
        i = _stpos_2020 + rval;
        if (treetab_2020.subclus != 20)
        if (rval > treetab_2020.nbranch)
        {
            err = 1;
            break;
        }
        trmap_2020(i);
        /*fprintf(ptfTrace,"--%d.%d-",i,treetab_2020.subclus);*/
    }
    /*fprintf(ptfTrace,"\n");*/
    _tdrg_2020 = _stpos_2020;
    _trtc_2020 = treetab_2020.param1;
    _tmdc_2020 = treetab_2020.param2;
    sprintf(cmd, "%02d", _tmdc_2020);
    decode_ghm_2020(ghm, _tdrg_2020);

    /*CD FG13.11d*/
    if ((annuleActeOperGHMMed) && ((_trtc_2020 == 80) || (_trtc_2020 == 222)))
        _trtc_2020 = 0;

    sprintf(cret, "%03d", _trtc_2020);
    if ((_trtc_2020 != 0) && (uhcd_2020 == 0))
        AddErreur_2020(GROUPAGE, _trtc_2020, 0);
    if (err == 1)
    {
        _trtc_2020 = 4;
        /*CD FG11*/
        _tmdc_2020 = 90;
        _tdrg_2020 = 5037;
        strcpy(cmd, "90");
        strcpy(ghm, "Z03Z");
        strcpy(cret, "004");
        AddErreur_2020(IMPLEMENTATION, 4, 0);
    }
    if (binversionDPDR_2020)
    {
        /*on remet DP et DR dans le bon ordre*/
        itemp = *ptr_inddiag_2020;
        *ptr_inddiag_2020 = *(ptr_inddiag_2020 + 1);
        *(ptr_inddiag_2020 + 1) = itemp;
        binversionDPDR_2020 = 0;
    }
    return(_trtc_2020);
}  /* int group() */

void decode_ghm_2020(char ghm[], int _ghm)
{
    char zone[6];

    strcpy(ghm, "    ");
    sprintf(zone, "%04d", _ghm);
    switch (zone[3] - '0')
    {
    case 0: ghm[3] = ' '; break;
    case 1: ghm[3] = 'A'; break;
    case 2: ghm[3] = 'B'; break;
    case 3: ghm[3] = 'C'; break;
    case 4: ghm[3] = 'D'; break;
    case 5: ghm[3] = 'E'; break;
    case 6: ghm[3] = 'J'; break;
    case 7: ghm[3] = 'Z'; break;
    }
    switch (zone[0] - '0')
    {
    case 1: ghm[0] = 'C'; break;
    case 2: ghm[0] = 'H'; break;
    case 3: ghm[0] = 'K'; break;
    case 4: ghm[0] = 'M'; break;
    case 5: ghm[0] = 'Z'; break;

    }
    strncpy(ghm + 1, zone + 1, 2);
}
void recode_ghm_2020(char ghm[], int *_ghm)
{
    char zone[6];

    *_ghm = 0;
    switch (ghm[3])
    {
    case 'A': *_ghm += 1; break;
    case 'B': *_ghm += 2; break;
    case 'C': *_ghm += 3; break;
    case 'D': *_ghm += 4; break;
    case 'E': *_ghm += 5; break;
    case 'J': *_ghm += 6; break;
    case 'Z': *_ghm += 7; break;
    case 'T': *_ghm += 8; break;
    case '1': *_ghm += 9; break;
    case '2': *_ghm += 10; break;
    case '3': *_ghm += 11; break;
    case '4': *_ghm += 12; break;
    }
    switch (ghm[0])
    {
    case 'C': *_ghm += 10000 * 1; break;
    case 'H': *_ghm += 10000 * 2; break;
    case 'K': *_ghm += 10000 * 3; break;
    case 'M': *_ghm += 10000 * 4; break;
    case 'Z': *_ghm += 10000 * 5; break;
    }
    zone[2] = 0;
    strncpy(zone, ghm + 1, 2);
    *_ghm += 100 * atoi(zone);
}

/*-----
Fonction 0
Recuperation du numero de cmd
Premier octet du masque du diagnostic principal
-------*/
int _dxsex_2020()
{
    int dcmd;
    dcmd = *(ptr_dmask_2020 + treetab_2020.param1 + (*ptr_inddiag_2020)*_dxmsize_2020);
    return (dcmd);
}


/*-----
Fonction 1
Recuperation de la categorie de diagnostics ( _dxcatv )
Position treetab_2020.param1 du masque du diagnostic principal
-------*/
int _dxcat_2020()
{
    int _dxcatv;
    _dxcatv = *(ptr_dmask_2020 + treetab_2020.param1 + (*ptr_inddiag_2020)*_dxmsize_2020);
    return (_dxcatv);
}

/*-----
Fonction 2
Test d'une caracteristique des masques de bits de tous les actes
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 pour un des actes est-il positionne
-------*/
int _prbit_2020()
{
    int i;
    unsigned int a, b;

    for (i = 0; i < nbac_2020; i++)
    {
        a = *(ptr_amask_2020 + treetab_2020.param1 + *(ptr_indacte_2020 + i)*_prmsize_2020);
        b = treetab_2020.param2;
        if (a & b)
            return(1);
    }
    return(0);
}

/*------
Fonction 3
Test de l'age
Si treetab_2020.param2 = 0 on teste l'age en annees
Si treetab_2020.param2 = 1 on teste l'age en jours
Test : est-ce que l'age est superieur a treetab_2020.param1
-------*/
int _agegt_2020()
{
    if (treetab_2020.param2 == 0)
    {
        if (age_an_2020 > treetab_2020.param1)
            return(1);
        else
            return(0);
    }
    else
    if (treetab_2020.param2 == 1)
    {
        if (age_an_2020 > 0)
            return(1);
        if (age_jour_2020 > treetab_2020.param1)
            return(1);
        else
            return(0);
    }
    return 9999;
}

/*-------
Fonction interne permettant de tester la presence de CMA dans les diagnostics
secondaires en tenant compte des listes d'exclusion
--------*/
int _dexbit_2020()
{
    int i;
    char *exc;
    long pos;
    unsigned char a, b, c, d;
    int pr1, pr2;
    int pr1b;

    a = *(ptr_emask_2020 + *(ptr_inddiag_2020)* 4);
    b = *(ptr_emask_2020 + *(ptr_inddiag_2020)* 4 + 1);
    pr1 = 256 * a + b;
    /* Doit-on tester uniquement si le DR est non vide ?? */
    pr1b = 0;
    if (strncmp(ptr_diags_2020 + *(ptr_inddiag_2020 + 1)*lgdiag_2020, "      ", lgdiag_2020) != 0)
    {
        a = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4);
        b = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4 + 1);
        pr1b = 256 * a + b;
    }
    /* Modification MB 10/10/03 : recherche d'exclusion ne concerne pas le DR */
    for (i = 2; i < nbdc_2020; i++)
    {
        a = *(ptr_emask_2020 + *(ptr_inddiag_2020 + i) * 4 + 2);
        b = *(ptr_emask_2020 + *(ptr_inddiag_2020 + i) * 4 + 3);
        pr2 = 256 * a + b;
        a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        b = (unsigned char)treetab_2020.param2;
        if (a & b && pr2 == 0)
            return(1);
        if (!(a & b))
            continue;
        pos = _doffexc10_2020 + _excsize10_2020*(long)pr2;
        exc = (char *)malloc(_excsize10_2020);
        /*FG 11e:TEM
        fseek(ptmdiag10_2020,pos,0);
        fread(exc,_excsize10_2020,1,ptmdiag10_2020);*/
        memcpy(exc, h_ptmdiag10_2020 + pos, _excsize10_2020);

        a = exc[(int)(pr1 / 8)];
        b = (unsigned char)puisi2_2020[pr1 % 8];
        c = exc[(int)(pr1b / 8)];
        d = (unsigned char)puisi2_2020[pr1b % 8];
        free(exc);
        if (!(a & b) && !(c & d))
            return(1);
    }
    return(0);
}

/*-------
Fonction 4
Test des CMA ou age superieur a 69 ans
La presence de CMA est teste par la fonction _dexbit
--------*/
int _agecc_2020()
{

    if (age_an_2020 > 69 || _sdxbit_2020())
        return(1);
    else
        return(0);
}

/*------
Fonction 5
Elle permet de tester la presence d'une caracteristique dans le masque de bits
du diagnostic principal
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic principal
est-il positionne
-------*/
int _pdxbit_2020()
{
    unsigned int a, b;

    a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020)*_dxmsize_2020);
    b = treetab_2020.param2;
    if (a & b)
        return(1);
    else
        return(0);
}

/*--------
Fonction 6
Test de la presence d'une caracteristique dans les masques de bits de tous les
diagnostics secondaires
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic secondaire i
est-il positionne
----------*/
int _sdxbit_2020()
{
    int i, pr1, pr2;
    unsigned char a, b, c, d;
    if (treetab_2020.param1 > 128)
    {
        treetab_2020.param1 -= 128;
        a = *(ptr_emask_2020 + *(ptr_inddiag_2020)* 4);
        b = *(ptr_emask_2020 + *(ptr_inddiag_2020)* 4 + 1);
        pr1 = 256 * a + b;
        /* E.T. : 2005/10/14 -> le DR n'était pas pris en compte*/
        c = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4);
        d = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4 + 1);
        pr2 = 256 * c + d;
        if (pr1 != 0 || pr2 != 0)
            return(_dexbit_2020());
    }
    /* le test ne concerne que les DAS  */
    for (i = 2; i < nbdc_2020; i++)
    {
        a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        b = treetab_2020.param2;
        if (a & b)
            return(1);
    }
    return(0);
}

/*--------
Fonction 7
Test de la presence d'une caracteristique dans les masques de bits de tous les diagnostics (principal et secondaires)
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic i
est-il positionne
----------*/
int _adxbit_2020()
{
    int i;
    unsigned int a, b;

    for (i = 0; i < nbdc_2020; i++)
    {
        a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        if (a)
        {
            b = treetab_2020.param2;
            if (a & b)
                return(1);
        };
    }
    return(0);
}
/*--------
Fonction 8
Test de l'absence d'une caracteristique dans le masque de bits de l'un des
diagnostics (principal et secondaires)
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic i
n'est pas positionne
---------*/
int _odxbit_2020()
{
    int i;
    unsigned int a, b;
    char* tmp;

    tmp = malloc(_dxmsize_2020 + 1);
    for (i = 0; i <= _dxmsize_2020; i++) tmp[i] = 0;

    for (i = 0; i < nbdc_2020; i++)
    {
        /* si le masque de diag n'est pas nul on fait le test */
        if (memcmp((ptr_dmask_2020 + *(ptr_inddiag_2020 + i)*_dxmsize_2020), tmp, _dxmsize_2020) != 0)
        {
            a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
            b = treetab_2020.param2;
            if ((a & b) == 0)
            {
                free(tmp);
                return(0);
            };
        };
    }
    free(tmp);
    return(1);
}

/*--------
Fonction 9
Test de l'absence d'une caracteristique dans le masque de bits de l'un des
actes opératoires
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 de l'acte i
n'est pas positionne
----------*/
int _oprbit_2020()
{
    int i;
    unsigned int a, b;
    int rep;

    rep = 0;
    for (i = 0; i < nbac_2020; i++)
    {
        if (*(ptr_amask_2020 + *(ptr_indacte_2020 + i)*_prmsize_2020) & 128)
        {
            rep = 1;
            a = *(ptr_amask_2020 + treetab_2020.param1 + *(ptr_indacte_2020 + i)*_prmsize_2020);
            b = treetab_2020.param2;
            if ((a & b) == 0)
                return(0);
        }
    }
    return(rep);

}  /* int _oprbit */

/*-------
Fonction 10
Est-ce que au moins deux actes ont la caracteristique bit treetab_2020.param2 de
l'octet treetab_2020.param1 positionnee
--------*/
int _sprbit_2020()
{
    int i, j;
    unsigned int a, b;

    j = 0;
    for (i = 0; i < nbac_2020; i++)
    {
        a = *(ptr_amask_2020 + treetab_2020.param1 + *(ptr_indacte_2020 + i)*_prmsize_2020);
        b = treetab_2020.param2;
        if (a & b)
        {
            if (j == 1)
                return(1);
            else
                j++;
        }
    }
    return(0);
}

/*------
Fonction 11
Test du mode de sortie et destination
Les tests sont codes en binaire
--------*/
int _dspst_2020()
{

    if (_modsor_2020[0] & treetab_2020.param1 || _modsor_2020[1] & treetab_2020.param2)
        return(1);
    else
        return(0);

}

/*------
Fonction 12
Sert uniquement a representer les feuilles terminales de l'arbre
avec les numeros de GHM
-------*/
int _xfrdd_2020()
{
    return(0);
}

/*------
Fonction 13
Elle permet de tester la valeur de la categorie de diagnostic du diagnostic
principal place dans l'octet treetab_2020.param1 du masque de bit
Est-elle egale a treetab_2020.param2
*/
int _dxceq_2020()
{
    int _dxcatv;

    _dxcatv = *(ptr_dmask_2020 + treetab_2020.param1 + (*ptr_inddiag_2020)*_dxmsize_2020);
    if (_dxcatv == treetab_2020.param2)
        return(1);
    else
        return(0);
}
/*------
Fonction 14
Elle permet de tester le sexe
Le sexe est-il egal a treetab_2020.param1
*/
int _sexeq_2020()
{
    if (_sex_2020 == treetab_2020.param1)
        return(1);
    else
        return(0);
}

/*-------
Fonction 15
Cette fonction permet d'orienter vers la CMD 24 si il y a lieu
Seances(presence de seances) ou sejours ambulatoires ( duree de sejour < 24h)
Les tests sont codes en binaire ( voir mise a jour de _los)
--------*/
int _cksejr_2020()
{
    if (treetab_2020.param1 & _los_2020)
        return(1);
    else
        return(0);
}

/*-------
Fonction 16
Test du poids de naissance
Le poids de naissance est-il superieur au poids code en ECC2 dans treetab_2020.param1 et treetab_2020.param2
--------*/
int _pdsngt_2020()
{
    /*   if(_pdsn_2020==0)return(1);  */
    if (_pdsn_2020 > ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2))
        return(1);
    else
        return(0);
}

/*-----
Fonction 17
Test de l'indicateur Hospitalisation lors des 30 derniers jours
------*/
int _hosp30_2020()
{
    if (_hp30_2020 == treetab_2020.param1)
        return(1);
    else
        return(0);
}

/*-------
Fonction 18
Est-ce que au moins deux diagmostics ont la caracteristique bit treetab_2020.param2
de l'octet treetab_2020.param1 positionnee
--------*/
int _tdxbit_2020()
{
    int i, j;
    unsigned int a, b;

    j = 0;

    for (i = 0; i < nbdc_2020; i++)
    {
        a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        b = treetab_2020.param2;
        if (a & b)
        {
            j++;
            if (j == 2 && i == 1) j--;
            if (j == 2) return(1);
        }
    }
    return(0);
}

/*------
Fonction 19
Test du mode de sortie ou de la destination
Si treetab_2020.param2 = 0 on teste le mode de sortie par rapport a treetab_2020.param1
Si treetab_2020.param2 = 1 on teste la destination par rapport a treetab_2020.param1
Si treetab_2020.param2 = 2 on teste le mode d'entrée par rapport a treetab_2020.param1
Si treetab_2020.param2 = 3 on teste la provenance par rapport a treetab_2020.param1
--------*/
int _msoreq_2020()
{
    switch (treetab_2020.param2)
    {
    case 0:
        if (_ms_2020 == treetab_2020.param1)
            return(1);
        break;
    case 1:
        if (_ds_2020 == treetab_2020.param1)
            return(1);
        break;
    case 2:
        if (_me_2020 == treetab_2020.param1)
            return(1);
        break;
    case 3:
        if (_pr_2020 == treetab_2020.param1)
            return(1);
        break;
    };
    return(0);
}

/*-------
Fonction 20
Fonction qui permet de faire une redirection vers un noeud code en ECC2
dans treetab_2020.param1 et treetab_param2;
--------*/
int _goto_2020()
{
    return((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2);
}

/*-------
Fonction 21
Test de l'igs
L'igs est-il superieur a la valeur code en ECC2 dans treetab_2020.param1 et treetab_2020.param2
--------*/
int _igsgt_2020()
{
    if (_igs_2020 == 0)
        return(0);
    if (_igs_2020 > ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2))
        return(1);
    else
        return(0);
}

/*------
Fonction 22
Test de la duree de sejour
La duree de sejour est strictement inferieure a la valeur code en ECC2 dans treetab_2020.param1 et treetab_2020.param2
-------*/
int _dsjlt_2020()
{
    if (_dsj_2020 < ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2))
        return(1);
    else
        return(0);
}

/*------
Fonction 23
DR intégré dans les DA, et effectue un goto
Arguments:
_stpos = numéro du noeud suivant
Fonctions supprimées en V7
-------*/
int _DR_DA_2020()
{

    _OffsetDR_2020 = 0;
    return 0;
};

/*------
Fonction 24
Le DR redevient lui même, et effectue un goto
Arguments:
_stpos = numéro du noeud suivant
Fonctions supprimées en V7
-------*/
int _DA_DR_2020()
{
    _OffsetDR_2020 = _dxmsize_2020;
    return 0;
};

/*-----
Fonction 25 => 0+1 pour le DR
Recuperation du numero de cmd
Premier octet du masque du diagnostic relie
-------*/
int _dxcatvDR_2020()
{
    int dcmd;
    /*CD FG11*/
    /*_dxcatv = *(ptr_dmask_2020+treetab_2020.param1+((*ptr_inddiag_2020)+1)*_dxmsize_2020);*/
    dcmd = *(ptr_dmask_2020 + treetab_2020.param1 + (*(ptr_inddiag_2020 + 1))*_dxmsize_2020);
    return (dcmd);
};

/*------
Fonction 26 => 5 pour le DR
Elle permet de tester la presence d'une caracteristique dans le masque de bits
du diagnostic relié
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic principal
est-il positionne
-------*/
int _pdxbitDR_2020()
{
    unsigned int a, b;

    /*CD FG11*/
    /*a = *(ptr_dmask_2020 + treetab_2020.param1 + ((*ptr_inddiag_2020)+1)*_dxmsize_2020) ;*/
    if (!binversionDPDR_2020)
        a = *(ptr_dmask_2020 + treetab_2020.param1 + (*(ptr_inddiag_2020 + 1))*_dxmsize_2020);
    else
        a = *(ptr_dmask_2020 + treetab_2020.param1 + (*(ptr_inddiag_2020))*_dxmsize_2020);

    b = treetab_2020.param2;
    if (a & b)
        return(1); /* Si le DR est un DA, olars on retourne 0*/
    else
        return(0);
}

/*------
Fonction 27 => 13 pour le DR
Elle permet de tester la valeur de la categorie de diagnostic du diagnostic
relie place dans l'octet treetab_2020.param1 du masque de bit
Est-elle egale a treetab_2020.param2
*/
int _dxceqDR_2020()
{
    int _dxcatv;

    /*CD FG11*/
    /*_dxcatv = *(ptr_dmask_2020+treetab_2020.param1+((*ptr_inddiag_2020)+1)*_dxmsize_2020);*/
    _dxcatv = *(ptr_dmask_2020 + treetab_2020.param1 + (*(ptr_inddiag_2020 + 1))*_dxmsize_2020);
    if (_dxcatv == treetab_2020.param2)
        return(_dxcatv); /* Si le DR est un DA, alors on retourne 0*/
    else
        return(0);
}

/*------
Fonction 28
ajoute une erreur dans le vecteur de groupage, et effectue un goto
Arguments:
_stpos = numéro du noeud suivant
treetab_2020.param1=noméro de l'erreur à insérer
-------*/

int _AddErr_2020()
{
    AddErreur_2020(GROUPAGE, treetab_2020.param1, 0);
    return 0;
};

/*------
Fonction 29
Test de la duree de sejour
La duree de sejour est strictement inferieure a la valeur code en ECC2 dans treetab_2020.param1 et treetab_2020.param2
-------*/
int _dsjeq_2020()
{
    if (_dsj_2020 == ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2))
        return(1);
    else
        return(0);
}
/*-------
Fonction 30
Test sur le nombre de séances
---------*/
int _nbsea_2020()
{
    if (_seance_2020 == ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2)) return(1);
    else return(0);
}
/*-----
Fonction 31
Test inverse du test 7
-----*/
int _nadxbit_2020()
{
    int i;
    unsigned int a, b;
    char * tmp;

    tmp = malloc(_dxmsize_2020 + 1);
    for (i = 0; i <= _dxmsize_2020; i++) *(tmp + i) = 0;
    /* on test ds le DP,DR,DAs */
    for (i = 0; i < nbdc_2020; i++)
    {
        if (memcmp((ptr_dmask_2020 + *(ptr_inddiag_2020 + i)*_dxmsize_2020), tmp, _dxmsize_2020) != 0)
        {
            a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
            b = treetab_2020.param2;
            if (!(a & b)){
                free(tmp);
                return(1);
            }
        }
    }
    free(tmp);
    return(0);
}

/*----
Fonction 32
Test inverse du test 2
-----*/
int _nprbit_2020()
{
    int i;
    unsigned int a, b;

    for (i = 0; i < nbac_2020; i++)
    {
        a = *(ptr_amask_2020 + treetab_2020.param1 + *(ptr_indacte_2020 + i)*_prmsize_2020);
        b = treetab_2020.param2;
        if (!(a & b))
            return(1);
    }
    return(0);
}


/*CD MAJ FG10
----
Fonction 33
Test de l'existence d'au moins un acte dont le type d'activité
est égal à param1
-----*/
int _prtypact_2020()
{
    int i;
    for (i = 0; i < nbactes_2020; i++)
    {
        if ((*(ptr_typact_2020 + i) - '0') == treetab_2020.param1)
            return 1;
    }
    return(0);
}

/*CD MAJ FG11
----
Fonction 34
Inversion DP / DR
-----*/
int _invDPDR_2020()
{
    int dcmddr;
    int itemp;
    int dxcatDR;
    binversionDPDR_2020 = 0;
    if (strncmp(ptr_diags_2020 + *(ptr_inddiag_2020 + 1)*lgdiag_2020, "          ", lgdiag_2020) == 0)
        return 0;
    dcmddr = *(ptr_dmask_2020 + *(ptr_inddiag_2020 + 1)*_dxmsize_2020);
    dxcatDR = *(ptr_dmask_2020 + *(ptr_inddiag_2020 + 1)*_dxmsize_2020 + 1);
    /*FG11e
    if(dcmddr==90)
    return 0;*/
    if ((dcmddr == 0) && (dxcatDR == 3))
        return 0;

    /*inversion des indices du DP et du DR*/
    itemp = *ptr_inddiag_2020;
    *ptr_inddiag_2020 = *(ptr_inddiag_2020 + 1);
    *(ptr_inddiag_2020 + 1) = itemp;
    binversionDPDR_2020 = 1;
    return 0;
}

/*CD MAJ FG11
----
Fonction 35
Inversion DP / DR réalisée ?
-----*/
int _invDPDRreal_2020()
{
    if (binversionDPDR_2020)
        return 1;
    return 0;
}
/*CD MAJ FG11
----
Fonction 36
Test de la presence d'une caracteristique dans les masques de bits de tous les diagnostics (principal et secondaires sauf DR)
Le bit treetab_2020.param2 de l'octet treetab_2020.param1 du diagnostic i
est-il positionne
-----*/
int _saufdrdxbit_2020()
{
    unsigned int a, b;
    int i;
    for (i = 0; i < nbdc_2020; i++)
    {
        if (i != 1)
        {
            a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
            b = treetab_2020.param2;
            if (a & b)
                return(1);
        }
    }
    return(0);
}

/*CD MAJ FG1311d*/
/*----
Fonction 38
est-ce que le groupe de la racine est compris entre p1 et p2
-----*/
int _grracinterv_2020()
{
    if ((groupeRacine >= treetab_2020.param1) && (groupeRacine <= treetab_2020.param2))
        return 1;
    return 0;
}
/*----
Fonction 39
détermination du groupe racine en fonction du poids et de l'AG
-----*/
int _grracdeterm_2020()
{
    if (groupeRacine < 0)
        groupeRacine = DeterminerGroupeRacine_Poids_AG_2020();
    return 0;
}
/*----
Fonction 40
annulation de l'erreur due à un acte opératoire reclassant dans un GHM médical
-----*/
int _annulacreclass_2020()
{
    annuleActeOperGHMMed = 1;
    return 0;
}
/*--------
Fonction 41
Test de la presence d'un DIAG (principal et secondaires) ayant la dxcat passée en paramètre
----------*/
int _dxctdeq_2020()
{
    int i, dxcat, cmd;
    for (i = 0; i < nbdc_2020; i++)
    {
        cmd = *(ptr_dmask_2020 + 0 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        dxcat = *(ptr_dmask_2020 + 1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
        if ((dxcat == treetab_2020.param2) && (cmd == treetab_2020.param1))
            return 1;
    }
    return(0);
}

/*-------
Fonction 42
Test du poids de naissance
Le poids de naissance est-il strictement > 0 et strictement < au poids code en ECC2 dans treetab_2020.param1 et treetab_2020.param2
--------*/
int _pdsngp_2020()
{
    if ((_pdsn_2020 > 0) && (_pdsn_2020 < ((unsigned char)treetab_2020.param1 * 256 + (unsigned char)treetab_2020.param2)))
        return(1);
    else
        return(0);


}

/*--------
Fonction 43
Test de la presence d'un DIAG (principal et secondaires SAUF DR) ayant la dxcat passée en paramètre
----------*/
int _dxctdeqsaufDR_2020()
{
    int i, dxcat, cmd;
    for (i = 0; i < nbdc_2020; i++)
    {
        if (i != 1)
        {
            cmd = *(ptr_dmask_2020 + 0 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
            dxcat = *(ptr_dmask_2020 + 1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
            if ((dxcat == treetab_2020.param2) && (cmd == treetab_2020.param1))
                return 1;
        }
    }
    return(0);
}

void analyse_actes_2020(RSS_INFO_2020 *rss_inf, GHS_VALORISATION_2020 *ghs_val, char *cmd, char *ghm)
{
    static char ghm_dial_excluded[80] = "28Z01Z*28Z02Z*28Z03Z*28Z04Z*11K02J";
	static char ghm_radioth_excluded[80] = "28Z19Z*28Z20Z*28Z21Z*28Z22Z*28Z10Z*28Z11Z*28Z23Z*28Z24Z*28Z25Z*28Z18Z";
	static char racineghm_posedef[6] = "05C19";

    char zone[5];
    short i;
    char cmdghm[7];

    /*variables pour prélèvement d'organes*/
    int prvlt;
    int prvlt_foie, prvlt_rein, prvlt_coeur, prvlt_poumon, prvlt_intestin, prvlt_pancreas;

    char *masque;
    int bCalcRadioth, bCalcDial,bCalcSDC;
    int ghm28Z15Z, ghm28Z16Z;

    ghm28Z15Z = ((strncmp(cmd, "28", 2) == 0) && (strncmp(ghm, "Z15Z", 4) == 0));
    ghm28Z16Z = ((strncmp(cmd, "28", 2) == 0) && (strncmp(ghm, "Z16Z", 4) == 0));


    if (ghs_val == NULL)
        return;
    ghs_val->nbac687 = 0;
    ghs_val->nbac688 = 0;
    ghs_val->nbac689 = 0;

    ghs_val->nbacRX1 = 0;
    ghs_val->nbacRX2 = 0;
    ghs_val->nbacRX3 = 0;
    ghs_val->nbRCMI = 0;

    ghs_val->nbactsupp9623 = 0;
    /*ghs_val->nbactsupp9624=0;*/
    ghs_val->nbactsupp9625 = 0;
    ghs_val->nbactsupp9631 = 0;
    ghs_val->nbactsupp9632 = 0;
    ghs_val->nbactsupp9633 = 0;

    ghs_val->nbhd = 0;
    ghs_val->nbent1 = 0;
    ghs_val->nbent2 = 0;
    ghs_val->nbent3 = 0;
    ghs_val->nbchypb = 0;
    ghs_val->nbprot = 0;
    ghs_val->nbict = 0;
    ghs_val->acprlvtorg = 0;
    ghs_val->nbradiopedia = 0;
	ghs_val->supplSDC = 0;


    cmdghm[6] = 0;
    strcpy(cmdghm, cmd);
    strcat(cmdghm, ghm);

    bCalcRadioth = bCalcDial = bCalcSDC = 1;
    if (my_index_2020(ghm_dial_excluded, cmdghm, 1) >= 0)
        bCalcDial = 0;
    if (my_index_2020(ghm_radioth_excluded, cmdghm, 1) >= 0)
        bCalcRadioth = 0;
	/*2017*/
	if (strncmp(cmdghm, racineghm_posedef,5)==0)
		bCalcSDC = 0;

    prvlt = prvlt_foie = prvlt_rein = prvlt_coeur = prvlt_poumon = prvlt_intestin = prvlt_pancreas = 0;

    for (i = 0; i < nbac_2020; i++)
    {
        masque = ptr_amask_2020 + (*(ptr_indacte_2020 + i))*_prmsize_2020;

        if (bCalcDial)
        {
            /*HD*/
            if ((*(masque + 32) & puisi2_2020[6]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbhd += atoi(zone);
            }
            /*ent1*/
            if ((*(masque + 23) & puisi2_2020[7]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbent1 += atoi(zone);
            }
            /*ent2*/
            if ((*(masque + 24) & puisi2_2020[0]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbent2 += atoi(zone);
            }
            /*ent3*/
            if ((*(masque + 30) & puisi2_2020[5]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbent3 += atoi(zone);
            }
        }
        if (bCalcRadioth)
        {
            if ((*(masque + 42) & puisi2_2020[0])) /*687*/
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                /**nba687+=atoi(zone);*/
                ghs_val->nbac687 += atoi(zone);
            }

            /*GHS 9611 et 9612 : supprimés => on laisse à 0
            if((*(masque+22) & puisi2_2020[6]))
            {
            strncpy(zone,ptr_nbactes_2020+(*(ptr_indacte_2020+i))*4,4);
            zone[4]=0;
            ghs_val->nbac688+=atoi(zone);
            }
            if((*(masque+24) & puisi2_2020[1]))
            {
            strncpy(zone,ptr_nbactes_2020+(*(ptr_indacte_2020+i))*4,4);
            zone[4]=0;
            ghs_val->nbac689+=atoi(zone);
            }
            */

            /*protonthérapie*/
            if ((*(masque + 31) & puisi2_2020[1]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbprot += atoi(zone);
            }
            /*ICT*/
            if (_prmsize_2020 > 34)
            {
                if ((*(masque + 34) & puisi2_2020[1]))
                {
                    strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                    zone[4] = 0;
                    ghs_val->nbict += atoi(zone);
                }
            }
            /*RCMI GHS 9622*/
            if ((*(masque + 41) & puisi2_2020[7]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbRCMI += atoi(zone);
            }
            /*Gamaknife : pas compté en sus du GHS => on laisse à 0*/

            /*Cyberknife dédié*/
            if ((*(masque + 38) & puisi2_2020[3]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbacRX2 += atoi(zone);
            }

            /*GHS 9623 Cyberknife non dédié*/
            if ((*(masque + 42) & puisi2_2020[4]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbactsupp9623 += atoi(zone);
            }
            /*GHS 9624*/
            /*if((*(masque+42) & puisi2_2020[5]))
            {
            strncpy(zone,ptr_nbactes_2020+(*(ptr_indacte_2020+i))*4,4);
            zone[4]=0;
            ghs_val->nbactsupp9624+=atoi(zone);
            }*/
            /*GHS 9625*/
            if ((*(masque + 42) & puisi2_2020[2]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbactsupp9625 += atoi(zone);
            }
            /*GHS 9631*/
            if ((*(masque + 41) & puisi2_2020[1]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbactsupp9631 += atoi(zone);
            }
            /*GHS 9632*/
            if ((*(masque + 41) & puisi2_2020[2]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbactsupp9632 += atoi(zone);
            }
            /*GHS 9633*/
            if ((*(masque + 41) & puisi2_2020[3]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbactsupp9633 += atoi(zone);
            }
        }
		if (bCalcSDC)
		{
			if ((!ghs_val->supplSDC) &&
				((*(masque + 24) & puisi2_2020[6]))) /*SDC*/
			{
				ghs_val->supplSDC = 1;
			}
		}
        /*CD FG11 aphérèse*/
        if ((!ghm28Z16Z) && (_prmsize_2020 > 38))
        {
            if ((*(masque + 38) & puisi2_2020[4]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbacRX3 += atoi(zone);
            }
        }
        /*caissons hyperbare*/
        if (!ghm28Z15Z)
        {
            if ((*(masque + 31) & puisi2_2020[2]))
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbchypb += atoi(zone);
            }
        }
        /*prélèvement d'organes*/
        if ((*(masque + 27) & puisi2_2020[5]))
        {
            /*CD FG10B*/
            prvlt = 1;
            if (*(masque + 32) & puisi2_2020[7])
            {
                prvlt_foie = 1;
                /*prvlt=1;*/
            }
            if (*(masque + 33) & puisi2_2020[0])
            {
                prvlt_rein = 1;
                /*prvlt=1;*/
            }
            if (*(masque + 33) & puisi2_2020[1])
            {
                prvlt_coeur = 1;
                /*prvlt=1;*/
            }
            if (*(masque + 33) & puisi2_2020[2])
            {
                prvlt_poumon = 1;
                /*prvlt=1;*/
            }
            if (*(masque + 33) & puisi2_2020[3])
            {
                prvlt_intestin = 1;
                /*prvlt=1;*/
            }
            if (*(masque + 33) & puisi2_2020[4])
            {
                prvlt_pancreas = 1;
                /*prvlt=1;*/
            }
        }
        /*FG13.11d supplément radiothérapie pédiatrique*/
        if (age_an_2020 < 18)
        {
            if (
                (*(masque + 27) & puisi2_2020[0]) ||
                (*(masque + 22) & puisi2_2020[5]) ||
                (*(masque + 39) & puisi2_2020[3]) ||
                (*(masque + 41) & puisi2_2020[1]) ||
                (*(masque + 41) & puisi2_2020[2]) ||
                (*(masque + 41) & puisi2_2020[3])
				/*
				|| (*(masque + 40) & puisi2_2020[5]) ||
                (*(masque + 40) & puisi2_2020[6]) ||
                (*(masque + 40) & puisi2_2020[7]) ||
				(*(masque + 41) & puisi2_2020[0])
				*/
                )
            {
                strncpy(zone, ptr_nbactes_2020 + (*(ptr_indacte_2020 + i)) * 4, 4);
                zone[4] = 0;
                ghs_val->nbradiopedia += atoi(zone);
            }
        }
    }
    /*CD FG13*/
    /*if(bCalcRadioth)
    {
    if((ghs_val->nbprot>0) || (ghs_val->nbict>0))
    ghs_val->nbac687=0;
    }*/
    if (prvlt == 1)
    {
        if ((prvlt_foie == 1) && (prvlt_rein == 1) && (prvlt_coeur == 1) && (prvlt_pancreas == 1) && ((prvlt_intestin == 1) || (prvlt_poumon == 1)))
            ghs_val->acprlvtorg = 2;
        else
        if (((prvlt_foie == 1) || (prvlt_rein == 1)) && ((prvlt_coeur + prvlt_poumon + prvlt_intestin + prvlt_pancreas) == 0))
            ghs_val->acprlvtorg = 1;
        else
            ghs_val->acprlvtorg = 3;
    }
    else
        ghs_val->acprlvtorg = 0;
}

int gradationHDJzero(GRP_ENTREE_2020 * grp_ent, RSS_INFO_2020 * rss_inf, GRP_RESULTAT_2020 * grp_res, int racine_exclue)
{
	int i;
	int nbjoursuhcd;
	unsigned char a, b;
	char *masque;
	/*séances*/
	if (strncmp(grp_res->cmd, "28", 2) == 0)
		return 0;
	/*nouveaux nés*/
	if (strncmp(grp_res->cmd, "15", 2) == 0)
		return 0;
	
	/***************exclusion de racines***************/
	if (racine_exclue == 1)
		return 0;
	/**************************************************/
	
	/***************dc et transfert***************/
	if ((_ms_2020 == 9) || (_ms_2020 == 7))
		return 0;
	/*********************************************/
	
	/***************actes excluant********************/
	for (i = 0; i < nbac_2020; i++)
	{
		masque = ptr_amask_2020 + (*(ptr_indacte_2020 + i))*_prmsize_2020;
		if( 
			((*(masque + 51) & puisi2_2020[0])) /*actes classants en C,K ou Z*/
			|| ((*(masque + 22) & puisi2_2020[2])) /*actes autorisant l'activité 4*/
			|| ((*(masque + 31) & puisi2_2020[2])) /*caissons hyperbares*/
			|| ((*(masque + 38) & puisi2_2020[4])) /*aphérèses sanguine */
			|| ((*(masque + 51) & puisi2_2020[1])) /*transfusions*/
			|| ((*(masque + 44) & puisi2_2020[1])) /*Actes d'irradiation en dose unique*/
			|| ((*(masque + 51) & puisi2_2020[2])) /*Actes excluants la gradation*/
		  )
			return 0;
	}
	/*************************************************/

	/************exclusion d'UM***********************/
	for (i = 0; i < nbsej_2020; i++)
	{
		if (autorisation_valide_2020(TYPUM_UHCD) && (sejourAutoriseEnTypAutor_2020(i, TYPUM_UHCD, rumin_2020[i].datent, rumin_2020[i].datsor, &nbjoursuhcd)))
			return 0;
	}
	/*************************************************/

	/****************codes CIM excluant***************/
	/* le test ne concerne que les DAS  */
	for (i = 2; i < nbdc_2020; i++)
	{
		a = *(ptr_dmask_2020 + treetab_2020.param1 + *(ptr_inddiag_2020 + i)*_dxmsize_2020);
		b = treetab_2020.param2;
		if ((*(ptr_dmask_2020 + 32 + *(ptr_inddiag_2020 + i) * _dxmsize_2020)) & puisi2_2020[4])
			return 0;
	}
	/*************************************************/
	return 1;
}

int get_igs_rum_2020(int i)
{
    char zone[4];
    strncpy(zone, rumin_2020[i].igs, 3);
    zone[3] = 0;
    return atoi(zone);
}

int sej_rea_2020(int i, char *rum, int igsrum)
{
    int j;
    char *masque;
    char zone[3];
    int acte_marqueur_1;
    int acte_marqueur_2;
    int ver, nbd;

    acte_marqueur_1 = 0;
    acte_marqueur_2 = 0;
    zone[2] = 0;

    ver = GetVersion_2020(rum, -1);
    nbd = GetNombreDiagnosticsNum_2020(rum, ver) + GetNombreDADNum_2020(rum, ver);

    for (j = indacte_2020[i]; j < indacte_2020[i + 1]; j++)
    {
        masque = ptr_amask_2020 + j*_prmsize_2020;
        if ((*(masque + 27) & puisi2_2020[3]))
        {
            acte_marqueur_1 = 1;
        }
        if ((*(masque + 27) & puisi2_2020[4]))
        {
            switch (ver)
            {
            case 12:strncpy(zone, rum + 177 + nbd * 8 + (j - indacte_2020[i]) * 26 + 24, 2); zone[2] = 0; break;
            case 13:
			case 14:
			case 15:
				strncpy(zone, rum + 177 + nbd * 8 + (j - indacte_2020[i]) * 29 + 27, 2); zone[2] = 0; break;
            }
            acte_marqueur_2 += atoi(zone);
        }
    }
    bActesMarqueursUMREA_2020[i] = 0;
    /*CD MAJ FG11b
    if (age_an_11b<16)*/
    if (age_an_2020<18)
    {
        if (acte_marqueur_1)
        {
            bActesMarqueursUMREA_2020[i] = 1;
            return 1;
        }
        /*FG 11b if (acte_marqueur_2>2)
            return 1;*/
        /*if (acte_marqueur_2>(nboccrequis-1))*/
        if (acte_marqueur_2>2)
            bActesMarqueursUMREA_2020[i] = 1;
        if (acte_marqueur_2 > 0)
            return 1;
    }
    else
    {
        if (igsrum >= BORNEIGS_2020)
        {
            if (acte_marqueur_1)
            {
                bActesMarqueursUMREA_2020[i] = 1;
                return 1;
            }
            /*FG 11b if (acte_marqueur_2>2)
                return 1;*/
            /*if (acte_marqueur_2>(nboccrequis-1))*/
            if (acte_marqueur_2 > 2)
                bActesMarqueursUMREA_2020[i] = 1;
            if (acte_marqueur_2 > 0)
                return 1;
        }
    }
    return 0;
}

unsigned char ToAscii_2020(unsigned char orig_char)
{
#ifdef ASCII_MOD
    return orig_char;
#else
    /* implémenter la fonction de conversion en ASCII dans le cas d'un codage ebcdic*/
    /* ce tableau est un exemple*/
    return ebcdic_to_ascii_2020[orig_char];
#endif
}

/*ouverture d'une table binaire*/
int OuvrirTable_2020(int indice_table, char *pnomtable, char *pdirtab)
{
    char nomtableComplet[512];
    long loffst;
    int lTailleArticle;
    long lTailleTotaleTable;
    int nbplages, i;
    unsigned char article[50];
    FILE **fptr;

    char **htab;/*FG 11e:TEM*/
    int taillef;/*FG 11e:TEM*/

    fptr = &(multitables_2020[indice_table].fileptr);
    if (*fptr != NULL)
        return 0;
    sprintf(nomtableComplet, "%s%s", pdirtab, pnomtable);
    *fptr = fopen(nomtableComplet, "rb");
    if (*fptr == NULL)
        return(9);

    /*FG 11e:TEM*/
    htab = &(multitables_2020[indice_table].htable);
    fseek(*fptr, 0, SEEK_END);
    taillef = ftell(*fptr);
    fseek(*fptr, 0, SEEK_SET);
    if ((*htab = malloc(taillef)) == NULL)
    {
        printf("echec malloc pour table\n");
        return(9);
    }

    /*remplissage du tableau de période*/
    fread(entete_table_2020.sign, sizeof(struct ENTETE_2020), 1, *fptr);

    /*initialisation de la version de la classification*/
    if (indice_table == TABLE_ARBRE_2020)
        strncpy(versi_2020, entete_table_2020.verc, 2);

    accstb_2020(*fptr, &loffst, &lTailleArticle, &lTailleTotaleTable);

    nbplages = entete_stab_2020.nastn[0] * 256 + entete_stab_2020.nastn[1];
    multitables_2020[indice_table].nbplages = nbplages;
    for (i = 0; i < nbplages; i++)
    {
        fread(article, sizeof(char), lTailleArticle, *fptr);

        multitables_2020[indice_table].datedeb[i] = article[0] * 256 + article[1];
        multitables_2020[indice_table].datefin[i] = article[2] * 256 + article[3];
        multitables_2020[indice_table].offset_table[i] = ((article[6] * 256 + article[7]) * 256 + article[8]) * 256 + article[9];
        /*CD 02/04/07 mémorisation du n° de tarif*/
        multitables_2020[indice_table].numerotarif_public[i] = article[4];
        multitables_2020[indice_table].numerotarif_prive[i] = article[5];
    }
    /*FG 11e:TEM*/
    fseek(*fptr, 0, SEEK_SET);
    fread(*htab, taillef, 1, *fptr);
    if (ferror(*fptr))
    {
        printf("echec lecture table de la table\n");
        return(9);
    }
    return 0;
}
/*Ouverture des fichiers binaires des tables
  pdirtab : chemin des tables binaires*/
int OuvrirTables_2020(char *pdirtab, char *versioncl)
{
    int ret;
    if ((ret = OuvrirTable_2020(TABLE_ARBRE_2020, TR_NAME, pdirtab)) != 0)
        return ret;
    strncpy(versioncl, versi_2020, 2);
    versioncl[2] = 0;
    if ((ret = OuvrirTable_2020(TABLE_GHS_2020, GHS_INFO, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_ACTES_2020, CCAM_NAME, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_DIAG_2020, DX_NAME10, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_GESTECOMP_2020, TAB_GESTCOMP, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_REFAUTO_2020, REF_AUTO, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_GHMINFO_2020, GHM_INFO, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_SRCDGACT_2020, SRCDGACT_NAME, pdirtab)) != 0)
        return ret;

    /*FG13*/
    if ((ret = OuvrirTable_2020(TABLE_INNOVREFER_2020, INNOV_REFER, pdirtab)) != 0)
        return ret;

    /*FG1311d*/
    if ((ret = OuvrirTable_2020(TABLE_COMBI_2020, TABCOMBI_NAME, pdirtab)) != 0)
        return ret;
    /*FG11g*/
    if ((ret = OuvrirTable_2020(TABLE_CCAMDESC_2020, CCAMDESC_NAME, pdirtab)) != 0)
        return ret;
    if ((ret = OuvrirTable_2020(TABLE_GHSMINOR_2020, GHSMINOR_NAME, pdirtab)) != 0)
        return ret;
    return 0;

}
int ChoixSousTable_2020(int indicetable, char *datesortie)
{
    int i;
    int ret;
    int lnbj;
    ret = 502;/*erreur : aucune période ne correspond au RSS dans cette table*/

    lnbj = nombre_jour_1980_2020(datesortie);
    multitables_2020[indicetable].periodecour = multitables_2020[indicetable].nbplages - 1;
    for (i = 0; i < multitables_2020[indicetable].nbplages; i++)
    {
        if ((lnbj >= multitables_2020[indicetable].datedeb[i]) && (lnbj < multitables_2020[indicetable].datefin[i]))
        {
            multitables_2020[indicetable].periodecour = i;
            ret = 0;
            break;
        }
    }
    return ret;
}
/*Sélection des tables en fonction de la date de sortie du dernier RUM du RSS*/
int ChoixTables_2020(char *datesortie, char *versioncl)
{
    int ret;
    if ((ret = ChoixSousTable_2020(TABLE_ARBRE_2020, datesortie)) != 0)
        return ret;
    else
    {
        optree_2020();
        strncpy(versioncl, entete_table_2020.verc, 2);
        versioncl[2] = 0;
    }
    if ((ret = ChoixSousTable_2020(TABLE_DIAG_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_ACTES_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_GHS_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_GESTECOMP_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_REFAUTO_2020, datesortie)) != 0)
        return ret;
    if ((ret = chargeRefAutorisationsCourantes_2020()) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_GHMINFO_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_SRCDGACT_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_INNOVREFER_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_COMBI_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_CCAMDESC_2020, datesortie)) != 0)
        return ret;
    if ((ret = ChoixSousTable_2020(TABLE_GHSMINOR_2020, datesortie)) != 0)
        return ret;
    return 0;
}

/*Fonction qui renvoie le type d'autorisation de l'avant dernier jour du séjour n° numsej du RSS*/
int sejouravdernAut_2020(int numsej)
{
    int avdernierTypeUM;
    char datetemp[9];
    int dernierjourj, dernierjourm, dernierjoura;

    if (numsej<0)
        return TYPUM_BLANC;
    avdernierTypeUM = TYPUM_BLANC;

    if (los_2020[numsej]>0)
    {
        Ajout_jour_date_2020(rumin_2020[numsej].datent, los_2020[numsej] - 1, &dernierjourj, &dernierjourm, &dernierjoura);
        sprintf(datetemp, "%02d%02d%04d", dernierjourj, dernierjourm, dernierjoura);
    }
    else
    {
        strncpy(datetemp, rumin_2020[numsej].datsor, 8);
    }

    trouveTypeUM_2020(datetemp, indice_ums_2020[numsej], &avdernierTypeUM);

    if (autorisation_valide_2020(avdernierTypeUM))
        return avdernierTypeUM;
    else
        return TYPUM_BLANC;
}

/*fonction renvoie 1 si le RUM numsej est autorisé en type typum pour au moins 1 jour de la période
définie par datedeb,datefin, et dans ce cas le nombre de jours autorisés est renvoyé dans nbjours
renvoie zéro sinon
*/
int sejourAutoriseEnTypAutor_2020(int numsej, int typum, char datedeb[], char datefin[], int *nbjours)
{
    int premj;
    return est_autorise_2020(indice_ums_2020[numsej], typum, datedeb, datefin, nbjours, &premj);
}

/*calcul des séances*/
void calculeseances_2020(GRP_RESULTAT_2020 *grp_res, GHS_VALORISATION_2020 *ghs_val)
{
    char   zone[8];
    strncpy(zone, rumin_2020[sejourprinc_2020].nbseance, 2);
    zone[2] = 0;
    ghs_val->nbseance = atoi(zone);
    /*  FG 1311d
        if((cmd28) && (ghs_val->nbseance==0))
        ghs_val->nbseance=1;*/
}


void AjouteOctetsSign_2020(unsigned int *pCrc, unsigned char *pbuf, long pnboctets)
{
    unsigned int Polynome = 0xA001;
    long CptOctet = 0;
    unsigned char CptBit = 0;
    unsigned char Parity = 0;
    Polynome = 0xA001; /* Polynôme = 2^15 + 2^13 + 2^0 = 0xA001.*/

    if ((pbuf == NULL) || (pnboctets == 0))
        return;
    for (CptOctet = 0; CptOctet < pnboctets; CptOctet++)
    {
        (*pCrc) ^= *(pbuf + CptOctet);  /*Ou exculsif entre octet message et CRC*/

        for (CptBit = 0; CptBit <= 7; CptBit++)  /* Mise a 0 du compteur nombre de bits */
        {
            Parity = *pCrc;
            (*pCrc) >>= 1;          /* Décalage a droite du crc*/
            if (!(Parity & 1))
                (*pCrc) ^= Polynome; /* Test si nombre impair -> Apres decalage à droite il y aura une retenue     */
            /* "ou exclusif" entre le CRC et le polynome generateur.*/
        }
    }
}

unsigned int CalculerVersionFGTableBinaires_2020(GRP_ENTREE_2020 *pgrp_ent)
{
    FILE *fptr;
    int i;
    long nboctets;
    char nomtableComplet[512];
    unsigned char * buf;
    unsigned int Crc = 0xFFFF;
    char * nomst[NB_TABLES_BINAIRES_2020] = { TR_NAME, DX_NAME10, CCAM_NAME, GHS_INFO, TAB_GESTCOMP, REF_AUTO, GHM_INFO, SRCDGACT_NAME, INNOV_REFER, TABCOMBI_NAME, CCAMDESC_NAME, GHSMINOR_NAME };

    /*calcul du CRC des tables binaires*/
    for (i = 0; i < NB_TABLES_BINAIRES_2020; i++)
    {
        sprintf(nomtableComplet, "%s%s", pgrp_ent->dirtab, nomst[i]);
        fptr = fopen(nomtableComplet, "rb");
        if (fptr == NULL)
            continue;
        fseek(fptr, 0, SEEK_END);
        nboctets = ftell(fptr);
        if (nboctets == 0)
        {
            fclose(fptr);
            continue;
        }
        if (nboctets > MAX_BUFFER_FIC)
            buf = (unsigned char*)malloc(nboctets);
        else
            buf = buffic_2020;
        if (buf == NULL)
        {
            fclose(fptr);
            continue;
        }
        fseek(fptr, 0, SEEK_SET);
        fread(buf, 1, nboctets, fptr);

        AjouteOctetsSign_2020(&Crc, buf, nboctets);
        if (nboctets > MAX_BUFFER_FIC)
        {
            free(buf);
            buf = NULL;
        }
        fclose(fptr);
    }
    /*calcul du CRC du CLASS_ID*/
    AjouteOctetsSign_2020(&Crc, CLS_ID_2020, strlen(CLS_ID_2020));
    return Crc;
}
void signer_RSS_2020(unsigned char *rum[], int nbRUM, unsigned char *res)
{
    int i, j, k, low, high;
    unsigned char *p;
    unsigned long long crc = INITIALCRC, part;
    static int initcrcrss = 0;
    static unsigned long long CRCTable[256];

    if (!initcrcrss)
    {
        initcrcrss = 1;
        for (i = 0; i < 256; i++)
        {
            part = i;
            for (j = 0; j < 8; j++)
            {
                if (part & 1)
                    part = (part >> 1) ^ POLY64REV;
                else
                    part >>= 1;
            }
            CRCTable[i] = part;
        }
    }
    for (k = 0; k < nbRUM; k++)
    {
        p = rum[k];
        while (ToAscii_2020(*p))
            crc = CRCTable[(crc ^ ToAscii_2020(*p++)) & 0xff] ^ (crc >> 8);
    }
    low = crc & 0xffffffff;
    high = (crc >> 32) & 0xffffffff;
    sprintf(res, "%010u%010u", high, low);
    return;
}


int CalculeCle8_2020(char *pchaine)
{
    int i, res1, restemp;
    res1 = 0;
    for (i = 8; i > 0; i--)
    {
        if ((i % 2) == 0)
        {
            restemp = *(pchaine + 8 - i) * 2;
            res1 += restemp / 10 + restemp % 10;
        }
        else
            res1 += *(pchaine + 8 - i);
    }
    res1 = 10 - (res1 % 10);
    if (res1 == 10)
        return 0;
    return res1;
}
unsigned long long puissance10LL_2020(int p)
{
    int i;
    unsigned long long resp;
    resp = 1ULL;
    for (i = 0; i < p; i++)
    {
        resp = resp * 10ULL;
    }
    return resp;
}

int CalculeCle13_2020(char *pchaine)
{
    int i;
    unsigned long long nb1, nb2;
    int reste;
    nb1 = 0;

    for (i = 0; i < 13; i++)
    {
        nb1 = nb1 + ((*(pchaine + 13 - i - 1)) - '0')*puissance10LL_2020(i);
    }
    nb2 = nb1 / 97ULL;
    reste = (int)(nb1 - nb2 * 97ULL);

    return 97 - reste;
}

void calcule_signature_GHS_2020(RSS_INFO_2020 *prssinf, GHS_VALORISATION_2020 *pghs_val)
{
    int res;
    char chainet[30];
    char chtmp[5];
    memset(chainet, 0, sizeof(chainet));
    memset(chtmp, 0, sizeof(chtmp));
    /*signature du RSS, GHS et version FG*/
    strncpy(chainet, prssinf->sigfg, 5);
    strncpy(chainet + 5, prssinf->sigrss, 20);
    strncpy(chainet + 25, pghs_val->ghs, 4);

    strncpy(prssinf->sigrssghsfg, "0000", 4);

    res = CalculeCle8_2020(chainet);
    sprintf(chtmp, "%1d", res);
    strncpy(prssinf->sigrssghsfg + 4, chtmp, 1);

    res = CalculeCle13_2020(chainet + 8);
    sprintf(chtmp, "%02d", res);
    strncpy(prssinf->sigrssghsfg + 5, chtmp, 2);

    res = CalculeCle8_2020(chainet + 21);
    sprintf(chtmp, "%1d", res);
    strncpy(prssinf->sigrssghsfg + 7, chtmp, 1);
}

unsigned char* getRacineInfo_2020(int codedcmd, int codedghm)
{
	int lnbart,i;
	unsigned char *article;
	if (_lTailleTotaleTableGHM_2020 == 0)
		return NULL;
	lnbart = _lTailleTotaleTableGHM_2020 / _lTailleArticleGHM_2020;
	article = NULL;
	/*recherche de la premiere ligne avec la racine concernée*/
	for (i = 0; i < lnbart; i++)
	{
		article = h_ghminf_2020 + _offstghm_2020 + i * sizeof(unsigned char)* _lTailleArticleGHM_2020;
		if (article[0] == codedcmd)
		{
			if ((article[1] * 256 + article[2]) == (codedghm / 10))
				break;
		}
	}
	if (i == lnbart) /*on n'a pas trouvé la racine*/
		return NULL;
	return article;
}

int calcule_niveau_2020(int codedcmd, int codedghm, char *pcmd, char *pghm, unsigned char *pracineinfo, int bseuilduree)
{
	unsigned char *article;
	int i;
	char niveau, cl;
	int GC;
	int MOINS_2, PLUS_69;
	unsigned char a, b, c, d;
	int pr1, pr1b, pr2;
	long pos;
	char *exc;
	int bexclu;

	article = pracineinfo;
	niveau = pghm[3];
	if (article == 0)
	{
		if (niveau == ' ')
		{
			_tmdc_2020 = 90;
			_tdrg_2020 = 5037;
			strcpy(pcmd, "90");
			strcpy(pghm, "Z03Z");
			AddErreur_2020(IMPLEMENTATION, 4, 0);
		}
		return 0;
	}

	/*GHM >=28*/
	if (codedcmd >= 28)
	{
		if (niveau == ' ')
		{
			_tmdc_2020 = 90;
			_tdrg_2020 = 5037;
			strcpy(pcmd, "90");
			strcpy(pghm, "Z03Z");
			AddErreur_2020(IMPLEMENTATION, 4, 0);
		}
		return 0;
	}

	/*sejours courts*/
	if (_dsj_2020 < 3)
	{
		GC = article[5];
		switch (GC)
		{
		case 0:break;
		case 1:if (_dsj_2020 == 0) niveau = 'J'; break;
		case 2:if (_dsj_2020 == 0) niveau = 'T'; break;
		case 3:if (_dsj_2020 < 2) niveau = 'T'; break;
		case 4:if (_dsj_2020 < 3) niveau = 'T'; break;
		}
		if ((niveau == ' ') && bseuilduree)
			niveau = '1';
		pghm[3] = niveau;
	}

	/*tabcombi*/
	/*niveau CMD 14*/
	/*if(pnivCMD14)
	*pnivCMD14=article[11];*/
	modifier_niveauCMD14_2020(&niveau, article[11]);
	pghm[3] = niveau;

	if (bseuilduree)
	{
		/*nivellement par les bornes basses des durées de séjours*/
		if (niveau == 'B' || niveau == 'C' || niveau == 'D')
		{
			if (niveau == 'D' && _dsj_2020 > 4)
				niveau = 'D';
			else
				if (niveau >= 'C' && _dsj_2020 > 3)
					niveau = 'C';
				else
					if (niveau >= 'B' && _dsj_2020 > 2)
						niveau = 'B';
					else
						niveau = 'A';
			pghm[3] = niveau;
		}
	}
	if (pghm[3] != ' ')
		return 0;

	/*niveller*/
	niveau = '1';

	a = *(ptr_emask_2020 + *(ptr_inddiag_2020) * 4);
	b = *(ptr_emask_2020 + *(ptr_inddiag_2020) * 4 + 1);
	pr1 = 256 * a + b;
	pr1b = 0;
	if (strncmp(ptr_diags_2020 + *(ptr_inddiag_2020 + 1)*lgdiag_2020, "        ", lgdiag_2020) != 0)
	{
		a = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4);
		b = *(ptr_emask_2020 + *(ptr_inddiag_2020 + 1) * 4 + 1);
		pr1b = 256 * a + b;
	}
	for (i = 2; i<nbdc_2020; i++)
	{
		bexclu = 0;
		cl = getNiveauCMADiag_2020(*(ptr_inddiag_2020 + i), article);
		if (cl>niveau)
		{
			/*test si exclu par DP ou DR*/
			if ((pr1 != 0) || (pr1b != 0))
			{
				a = *(ptr_emask_2020 + *(ptr_inddiag_2020 + i) * 4 + 2);
				b = *(ptr_emask_2020 + *(ptr_inddiag_2020 + i) * 4 + 3);
				pr2 = 256 * a + b;
				if (pr2 != 0)
				{
					pos = _doffexc10_2020 + _excsize10_2020 * (long)pr2;
					exc = (char *)malloc(_excsize10_2020);
					/*FG 11e:TEM
					fseek(ptmdiag10_2020,pos,0);
					fread(exc,_excsize10_2020,1,ptmdiag10_2020);*/
					memcpy(exc, h_ptmdiag10_2020 + pos, _excsize10_2020);

					a = exc[(int)(pr1 / 8)];
					b = (unsigned char)puisi2_2020[pr1 % 8];
					c = exc[(int)(pr1b / 8)];
					d = (unsigned char)puisi2_2020[pr1b % 8];
					free(exc);
					if ((a & b) || (c & d))
						bexclu = 1;
				}
			}
			if (!bexclu)
				niveau = cl;
		}
	}

	MOINS_2 = article[3];
	PLUS_69 = article[4];

	if (age_an_2020<2)
	{
		if ((niveau == '1') && (MOINS_2 == 1))
			niveau = '2';
	}
	else
		if (age_an_2020>69)
		{
			switch (PLUS_69)
			{
			case 0:break;
			case 1:if (niveau == '1') niveau++; break;
			case 2:if ((age_an_2020 > 79) && (niveau == '1')) niveau++; break;
			case 3:if (niveau<'3') niveau++; break;
			case 4:if ((age_an_2020>79) && (niveau < '3')) niveau++; break;
			case 5:if (niveau<'4') niveau++; break;
			case 6:if ((age_an_2020>79) && (niveau<'4')) niveau++; break;
			}
		}

	/*DC*/
	if ((_ms_2020 == 9) && (niveau == '1'))
		niveau = '2';

	if (bseuilduree)
	{
		/*durée*/
		if ((niveau >= '4') && (_dsj_2020 > 4)) niveau = '4';
		else
			if ((niveau >= '3') && (_dsj_2020 > 3)) niveau = '3';
			else
				if ((niveau >= '2') && (_dsj_2020 > 2)) niveau = '2';
				else
					niveau = '1';
	}
	pghm[3] = niveau;
	return 0;
}

char getNiveauCMADiag_2020(int pindice, unsigned char particle[])
{
    char nivres;
    char *masque;

    masque = ptr_dmask_2020 + pindice*_dxmsize_2020;
    if ((*(masque + 21)) & puisi2_2020[1]) /*niveau 4*/
        nivres = '4';
    else
    if ((*(masque + 21)) & puisi2_2020[0])/*niveau 3*/
        nivres = '3';
    else
    if ((*(masque + 20)) & puisi2_2020[7])/*niveau 2*/
        nivres = '2';
    else
        nivres = '1';

    if (nivres != '1')
    {
        /*DIAGs en O*/
        if ((age_an_2020<14) && ((*(masque + 19)) & puisi2_2020[3]))
            nivres = '1';
        else
            /*DIAGs en P*/
        if (
            (age_an_2020>1) &&
            ((*(masque + 19) & puisi2_2020[4]) ||
            ((*(ptr_diags_2020 + pindice*lgdiag_2020) == 'P') || (*(ptr_diags_2020 + pindice*lgdiag_2020) == 'p')))
            )
            nivres = '1';
        /*exclusion par GHM*/
        else
        {
            if ((*(masque + particle[8])) & particle[9])
                nivres = '1';
        }
    }
    return nivres;
}

/*Code Innovation*/
int ghm_Innovation_2020(int *codedcmd, int *codedghm)
{
    int lnbart;
    int i, j;
    unsigned char article[19];
    int lg, plg, deb, fin;
    char *strtemp;
    char *pchaine;
    char codeinnov[TAILLE_CODE_INNOV_2020];

    /*innovation
    lire la zone filler des RUMs*/
    for (i = 0; i < nbsej_2020; i++)
    {
        pchaine = rumin_2020[i].numeroinno;
        lg = sizeof(rumin_2020[i].numeroinno);
        if (strncmp(pchaine, "                                       ", lg) == 0)
            continue;/*zone filler non renseignée*/

        if (numero_innovation_2020 == -1)
            numero_innovation_2020 = i;

        deb = 0;
        strtemp = pchaine;
        while (*strtemp == ' ')
        {
            strtemp++;
            deb++;
        }
        fin = lg - 1;
        strtemp = pchaine + lg - 1;
        while (*strtemp == ' ')
        {
            strtemp--;
            fin--;
        }
        plg = fin - deb + 1;
        if (plg > TAILLE_CODE_INNOV_2020)
            continue;
        strncpy(codeinnov, pchaine + deb, plg);
        for (j = plg; j < TAILLE_CODE_INNOV_2020; j++)
            codeinnov[j] = ' ';

        article[18] = 0;
        if (_TailleArticleInnovref_2020 == 0) /*precaution*/
        {
            AddErreur_2020(IMPLEMENTATION, 4, 0);
            return 0;
        }
        lnbart = _TailleTotaleTableInnovref_2020 / _TailleArticleInnovref_2020;
        /*FG 11e:TEM
        fseek(innovref_2020,_offstinnovref_2020,SEEK_SET);*/

        /*recherche de la premiere ligne avec le code concerné*/
        for (j = 0; j < lnbart; j++)
        {
            /*FG 11e:TEM
            fread(article,sizeof(char),18,innovref_2020);*/
            memcpy(article, h_innovref_2020 + _offstinnovref_2020 + j*sizeof(char)* 18, sizeof(char)* 18);

            if (strncmp(article, codeinnov, 15) == 0)
            {
                *codedcmd = article[15];
                *codedghm = article[16] * 256 + article[17];
                numero_innovation_2020 = i;
                return 1;
            }
        }
    }
    return 0;
}

int DeterminerGroupeRacine_Poids_AG_2020()
{
    unsigned char article[11];
    int i, valp1, valp2, valag1, valag2;
    int agegesta;
    char zone[3];

    int lnbart;
    if (_TailleArticleCombiPoidsAG_2020 == 0) /*precaution*/
        return 0;
    lnbart = _TailleTotaleTableCombiPoidsAG_2020 / _TailleArticleCombiPoidsAG_2020;
    /*FG 11e:TEM
    fseek(tabcombi_2020,_offstcombiPoidsAG_2020,SEEK_SET);*/


    if (agegest_renseigne_2020 < 0)
        agegesta = 99;
    else
    {
        strncpy(zone, rumin_2020[agegest_renseigne_2020].agegest, 2);
        zone[2] = 0;
        agegesta = atoi(zone);
    }
    /*recherche de la ligne correspondant au poids et AG*/
    for (i = 0; i < lnbart; i++)
    {
        /*FG 11e:TEM
        fread(article,sizeof(char),10,tabcombi_2020);*/
        memcpy(article, h_tabcombi_2020 + _offstcombiPoidsAG_2020 + i*sizeof(char)* 10, sizeof(char)* 10);

        valp1 = article[0] * 256 + article[1];
        valp2 = article[2] * 256 + article[3];
        valag1 = article[4] * 256 + article[5];
        valag2 = article[6] * 256 + article[7];
        if ((_pdsn_2020 >= valp1) && (_pdsn_2020 <= valp2) &&
            (agegesta >= valag1) && (agegesta <= valag2))
        {
            return (article[8] * 256 + article[9]);
        }
    }
    return 0;
}

/*modification du niveau de sévérité pour la CMD14*/
void modifier_niveauCMD14_2020(char *niveau, int numtable)
{
    unsigned char article[11];
    int i, valag1, valag2, valniv1, valniv2;
    char zone[3];
    int tailleArticle, lnbart, agegesta;
    long tailleTotaleTable, offsettable;

    if ((*niveau != 'A') && (*niveau != 'B') && (*niveau != 'C'))
        return;

    /*numtable représente le numéro de la sous-table à utiliser pour le calcul du niveau*/
    switch (numtable)
    {
    case 2:
        tailleArticle = _TailleArticleCombiAGNivA_2020;
        tailleTotaleTable = _TailleTotaleTableCombiAGNivA_2020;
        offsettable = _offstcombiAGNivA_2020;
        break;
    case 3:
        tailleArticle = _TailleArticleCombiAGNivB_2020;
        tailleTotaleTable = _TailleTotaleTableCombiAGNivB_2020;
        offsettable = _offstcombiAGNivB_2020;
        break;
    case 4:
        tailleArticle = _TailleArticleCombiAGNivC_2020;
        tailleTotaleTable = _TailleTotaleTableCombiAGNivC_2020;
        offsettable = _offstcombiAGNivC_2020;
        break;
    default:
        return;
    }

    if (tailleArticle == 0) /*precaution*/
        return;
    lnbart = tailleTotaleTable / tailleArticle;
    /*FG 11e:TEM
    fseek(tabcombi_2020,offsettable,SEEK_SET);*/

    if (agegest_renseigne_2020 < 0)
        agegesta = 99;
    else
    {
        strncpy(zone, rumin_2020[agegest_renseigne_2020].agegest, 2);
        zone[2] = 0;
        agegesta = atoi(zone);
    }
    /*recherche de la ligne correspondant à l'AG et au niveau courant*/
    for (i = 0; i < lnbart; i++)
    {
        /*FG 11e:TEM
        fread(article,sizeof(char),10,tabcombi_2020);*/
        memcpy(article, h_tabcombi_2020 + offsettable + i*sizeof(char)* 10, sizeof(char)* 10);

        valag1 = article[0] * 256 + article[1];
        valag2 = article[2] * 256 + article[3];
        valniv1 = article[4] * 256 + article[5];
        valniv2 = article[6] * 256 + article[7];
        if ((agegesta >= valag1) && (agegesta <= valag2) &&
            (*niveau >= valniv1 + 65) && (*niveau <= valniv2 + 65))
        {
            *niveau = article[8] * 256 + article[9] + 65;
            return;
        }
    }
    return;
}

void CalculeSupplementAntepartum_2020(GHS_VALORISATION_2020 *pghs_val)
{
    int i = 0;
    char *masque;
    for (i = 0; i < nbdc_2020; i++)
    {
        masque = ptr_dmask_2020 + (*(ptr_inddiag_2020 + i))*_dxmsize_2020;
        if ((*(masque + 25)) & puisi2_2020[1])
        {
            pghs_val->nbantepartum = nbjdateaccouch_2020;
            break;
        }
    }
}

void getRUMs_2020(RUM_2020 **lesRums)
{
    fprintf(stdout, "getRums : finess = %9.9s\n", rumin_2020[0].finess);
    *lesRums = rumin_2020;
}

