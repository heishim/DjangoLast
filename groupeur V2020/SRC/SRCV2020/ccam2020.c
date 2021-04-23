/******************************************************************************/
/* Agence Technique de l'Information sur l'Hospitalisation (A.T.I.H.)         */
/* Date de création : 04 janvier 2006                                         */
/* Fonctions                                                                  */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "ccam2020.h"
#include "tables2020.h"

/*CD 22/01/07
short opaccam_gc(char dxtab[],FILE **ptfidx_lp)*/
short opaccam_gc_2020(FILE *ptfidx_lp, long offsettable)
{
	int i;
    /*ENTETE_TABLE_2020 entete_table;*/
    char *ptoffft;/*FG 11e:TEM*/

    /*FG 11e:TEM
    fseek(ptfidx_lp,offsettable,SEEK_SET);
    fread(entete_table.nomt,sizeof(ENTETE_TABLE_2020),1,ptfidx_lp);*/

	/*FG 11e:TEM
    for(i=0;i<entete_table.nstb;i++)
		accstb_ccam_gc_2020(ptfidx_lp,&sstab_info_2020[i],offsettable);
    */
    ptoffft=h_ptfidx_gc_2020+offsettable+sizeof(struct ET_2020);
    for(i=0;i<*(h_ptfidx_gc_2020+offsettable+27);i++)
    {
        sstab_info_2020[i].taille=*(ptoffft+20)*256+*(ptoffft+21);
        sstab_info_2020[i].offset=carlong_2020(ptoffft+26)+offsettable;
        sstab_info_2020[i].lgtot=carlong_2020(ptoffft+22);
        ptoffft+=sizeof(ENTETE_SSTAB_2020);
    }

	return(0);
}

/* Indique si un acte est un GC*/
int est_gc_2020(char code[])
{
	int fpos;
	unsigned char code_binaire[7];
	char zone[6];
	long hc,hc_aux;            /* Hachage des radicaux */
	static int puis26[3]={676,26,1};
	int i;
	unsigned char *buf;

	hc = 0;
	strncpy(zone,code+4,5);
	zone[5]=0;

	for(i=0;i<3;i++)
	{
		hc_aux = toupper(code[i])-'A';
		hc+=(long)(hc_aux*puis26[i]);
	}

	code_binaire[0]=(unsigned char)(hc/256);
	code_binaire[1]=(unsigned char)(hc%256);

	code_binaire[2]=code[3]-'A';
	code_binaire[3]=(atoi(zone))/(256*256*256);
	code_binaire[4]=((atoi(zone))%(256*256*256))/(256*256);
	code_binaire[5]=(((atoi(zone))%(256*256*256))%(256*256))/256;
	code_binaire[6]=(((atoi(zone))%(256*256*256))%(256*256))%256;

	if((buf = (unsigned char *)malloc(sstab_info_2020[3].lgtot))==0)
	{
		return(-1);
	}

	fpos = sstab_info_2020[3].offset;
	/*FG 11e:TEM
    fseek(ptfidx_gc_2020,fpos,0);
	fread(buf,sstab_info_2020[3].lgtot,1,ptfidx_gc_2020);*/
    memcpy(buf,h_ptfidx_gc_2020+fpos,sstab_info_2020[3].lgtot);

	i=0;
    while((i< sstab_info_2020[3].lgtot) && (memcmp(buf+i,code_binaire,7)!=0))
		i+=sstab_info_2020[3].taille;

	free(buf);
	if(i< sstab_info_2020[3].lgtot)
		return 0;
	else
		return 1;

}

/* Ne vérifie pas si l'acte est bien formé */
int liste_compl_2020(char code_in[],char phase,char activite,char **res)
{
	int i;                     /* Variable de boucle */
	static int puis26[3]={676,26,1};
	char code[9];              /* Le code entier : L1L2L3L4C1C2C3C4C5 */
					   	       /* Code : L1L2L3L4C1C2C3 */
						       /* Activité : C4    Phase : C5 */

	long hc,hc_aux;            /* Hachage des radicaux */

	unsigned char sufx[4];     /* Info sur les suffixes récupéré dans ST1 donnant les 2 infos suivantes : */
	int nb_sufx;               /* Nb de suffixes */
	int offset_sufx;           /* Offset du premier suffixe dans ST2 */

	unsigned char hc_term[5];  /* Hachage des suffixes (terminaisons) */

	unsigned char *buf;        /* Buffer */
	unsigned char buf2[7];     /* Buffer */
	long fpos;                 /* Permet de calculer les déplacements à effectuer dans la table */

	unsigned char info_st2[10];/* Infos récupérées dans ST2 donnant : */
	int nb_gest;               /* Nombre de gestes complémentaires */
	int offset_ind_gc;         /* Offset de l'index des GC dans ST3 */

	unsigned char ind_lst_gc[2];  /* Info récupérée dans ST3 */
	int offset_lst_gc;            /* L'offset dans ST4 */

	char zone[6];                /* Suffixe */

	strncpy(code,code_in,7);
	code[7]=phase;
	code[8]=activite;
	hc=0;
	for(i=0;i<3;i++)
	{
		hc_aux = toupper(code[i])-'A';
		if ( hc_aux<0 || hc_aux>26) return(-1);
		hc+=(long)(hc_aux*puis26[i]);
	}

	if(hc<0 || hc>=_nbpref_ccam_2020)
	{
		return(-1);
	}

	hc *= sstab_info_2020[0].taille;
	/*FG 11e:TEM
    fseek(ptfidx_gc_2020,sstab_info_2020[0].offset+hc,0);
	fread(sufx,sstab_info_2020[0].taille,1,ptfidx_gc_2020);*/
    memcpy(sufx,h_ptfidx_gc_2020+sstab_info_2020[0].offset+hc,sstab_info_2020[0].taille);

	nb_sufx=sufx[0]*256+sufx[1];
	if(nb_sufx==0)
	{
		return(1);
	}

	offset_sufx=sufx[2]*256+sufx[3];

	strncpy(zone,code+4,5);zone[5]=0;

	if((buf = (unsigned char *)malloc(nb_sufx*sstab_info_2020[1].taille))==0)
	{
		return(-1);
	}

	fpos=sstab_info_2020[1].offset+offset_sufx*sstab_info_2020[1].taille;
	/*FG 11e:TEM
    fseek(ptfidx_gc_2020,fpos,0);
	fread(buf,nb_sufx*sstab_info_2020[1].taille,1,ptfidx_gc_2020);*/
    memcpy(buf,h_ptfidx_gc_2020+fpos,nb_sufx*sstab_info_2020[1].taille);

	hc_term[0]=code[3]-'A';
	hc_term[1]=(atoi(zone))/(256*256*256);
	hc_term[2]=((atoi(zone))%(256*256*256))/(256*256);
	hc_term[3]=(((atoi(zone))%(256*256*256))%(256*256))/256;
	hc_term[4]=(((atoi(zone))%(256*256*256))%(256*256))%256;

	i=0;
	while((i< nb_sufx * sstab_info_2020[1].taille) && (memcmp(buf+i,hc_term,5)!=0))
		i+=sstab_info_2020[1].taille;

	if(i<nb_sufx*sstab_info_2020[1].taille)
	{
		memcpy(info_st2,buf+i+5,sstab_info_2020[1].taille - 5);
	}
    else
    {
		free(buf);
        return(1);
	}

	nb_gest = info_st2[0];
	offset_ind_gc = info_st2[1]*256+info_st2[2];
	if(nb_gest==0)
	{
		free(buf);
		return(1);
	}

	if ((*res = (char *)malloc(TAILLE_ACTE_C_2020*nb_gest+3))==NULL)
	{
		free(buf);
		return(-1);
	}

	*(*res)=nb_gest;
	for(i=0;i<nb_gest;i++)
	{
		fpos=sstab_info_2020[2].offset+i*sstab_info_2020[2].taille+offset_ind_gc*sstab_info_2020[2].taille;
		/*FG 11e:TEM
        fseek(ptfidx_gc_2020,fpos,0);
		fread(ind_lst_gc,2,1,ptfidx_gc_2020);*/
        memcpy(ind_lst_gc,h_ptfidx_gc_2020+fpos,2);

		offset_lst_gc = ind_lst_gc[0]*256+ind_lst_gc[1];

		fpos=sstab_info_2020[3].offset+offset_lst_gc*sstab_info_2020[3].taille;
		/*FG 11e:TEM
        fseek(ptfidx_gc_2020,fpos,0);
		fread(buf2,7,1,ptfidx_gc_2020);*/
        memcpy(buf2,h_ptfidx_gc_2020+fpos,7);

		*(*res+1+i*TAILLE_ACTE_C_2020) = (char)((buf2[0]*256+buf2[1])/676+'A');
		*(*res+2+i*TAILLE_ACTE_C_2020) = (char)(((buf2[0]*256+buf2[1])%676)/26+'A');
		*(*res+3+i*TAILLE_ACTE_C_2020) = (char)(((buf2[0]*256+buf2[1])%676)%26+'A');

		*(*res+4+i*TAILLE_ACTE_C_2020) = (char)(buf2[2]+'A');

		sprintf(*res+5+i*TAILLE_ACTE_C_2020,"%5.5i",buf2[3]*256*256*256+buf2[4]*256*256+buf2[5]*256+buf2[6]);
	}

	free(buf);
	return(0);
}

/* Indique si un GC est dans une liste donnée */
int actelisteGC_2020(char code[],char activite,char *res)
{
	int i;
	char code_aux[10];
	code_aux[9]='\0';
	sprintf(code_aux,"%8.8s%c",code,activite);
	for (i=0;i<res[0];i++)
		if (memcmp(code_aux,res+1+i*9,9)==0)
			return 0;
	return -1;
}


/* Indique si un tableau ne contient que des -1 */
int tableauvide_2020(int tableau[],int *nbelements)
{
	int i=0;

	while ((i<*nbelements) && (tableau[i]==-1))
		i++;
	if (tableau[i-1]==-1 && i==*nbelements)
		return 0;
	else
		return -1;
}
