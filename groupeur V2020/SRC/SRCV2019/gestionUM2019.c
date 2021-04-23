#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gestionUM2019.h"

#define CRC32_POLY_2019 0x04c11db7
unsigned long crc32_table_2019[256];

int getindicetypUM_2019(char typum[])
{
    char ltypum[3];
    strncpy(ltypum,typum,2);
    ltypum[2]=0;
    return atoi(ltypum);
}

void init_crc32_2019()
{
	int i, j;
	unsigned long c;

	for (i = 0; i < 256; ++i) {
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY_2019 : (c << 1);
		crc32_table_2019[i] = c;
	}
}
void signer_IUM_2019(unsigned char *buf[], int nbium,unsigned char *res)
{
	unsigned char *p;
	unsigned long  crc;
	int i,len;
    static int initcrc32_2019 = 0;
	
    if(!initcrc32_2019)
    {
        init_crc32_2019();
        initcrc32_2019=1;
    }
	crc = 0xffffffff;
	for(i=0;i<nbium;i++)
	{
		p=buf[i];
		len=strlen(p);
		for (p; len > 0; ++p, --len)
			crc = (crc << 8) ^ crc32_table_2019[(crc >> 24) ^ ToAscii_2019(*p)];
	}
	sprintf (res, "%010u",~crc);
}
int verifFichierUMsigne_2019()
{
    int eof;
    char buf[101];
    char buf2[101];
    int i,t;
    unsigned char signature[TAILLE_SIGNATURE_2019+1];
    int lgligne,nbligne;
    char *uminfs[MAX_LIGNE_UM_2019+1];
    
    eof=0;
    t=TAILLE_LIGNE_UM_2019;
    memset(buf,0,sizeof(buf));
    memset(buf2,0,sizeof(buf2));
    memset(uminfs,0,MAX_LIGNE_UM_2019+1);
    fseek(fptrum_2019,0,SEEK_SET);
    nbligne=0;
    while(!eof)
    {
        if(mfgets_2019(buf,100,fptrum_2019)==0)
        {
            eof=1;
            continue;
        }
        lgligne = strlen(buf);
        if(lgligne!=t)
        {
            strncpy(buf2,buf,lgligne);
            if(mfgets_2019(buf,100,fptrum_2019)!=0)
            {
                for(i=0;i<nbligne;i++)
                {
                    if(uminfs[i]!=NULL)
                        free(uminfs[i]);
                }
                return 501;
            }
            /*on est sur la signature*/
            uminfs[nbligne]=(char*)malloc(lgligne+1);
            strncpy(uminfs[nbligne],buf2,lgligne);
            *(uminfs[nbligne]+lgligne)=0;
            nbligne++;
            eof=1;
        }
        else
        {
            uminfs[nbligne]=(char*)malloc(TAILLE_LIGNE_UM_2019+1);
            strncpy(uminfs[nbligne],buf,TAILLE_LIGNE_UM_2019);
            *(uminfs[nbligne]+TAILLE_LIGNE_UM_2019)=0;
            nbligne++;
        }
    }
    if(nbligne<1)
        return 501;
    /*verifier la taille de la signature*/
    if(strlen(uminfs[nbligne-1])!=TAILLE_SIGNATURE_2019)
        return 501;
    /*verifier que c'est la bonne signature*/
    signer_IUM_2019(uminfs,nbligne-1,signature);
    if(strncmp(uminfs[nbligne-1],signature,TAILLE_SIGNATURE_2019)!=0)
        return 501;
    
    for(i=0;i<nbligne;i++)
    {
        if(uminfs[i]!=NULL)
            free(uminfs[i]);
    }
    return 0;
}

/*Fonction mfgets : lit une ligne d'un fichier ouvert en mode binaire
Cette fonction lit caractère par caractère le fichier en entrée jusqu'à trouver le
premier caractère de saut de ligne ('\n') ou de fin de ligne ('\r'), et
place l'ensemble de ces caractères lus dans le buffer en sortie. Le curseur du fichier est ensuite positionné
sur le premier caractère non égal à '\n' ou à '\r'.
*paramètres:
    buf : le buffer en sortie contenant la ligne lue.
    taillemax : la taille maximum du nombre de caractères lus.
    ptrfile : pointeur sur le fichier ouvert en mode BINAIRE.
*valeur en retour : renvoie 0 si une erreur de lecture a eu lieu ou si la fin de fichier a
    été détectée. Renvoie 1 si tout s'est bien passé.
*/
int mfgets_2019(char *buf, int taillemax, FILE *ptrfile)
{
    int car,ret,crlf,fin,nbcar;
    ret=1;
    crlf=0;
    fin=0;
    nbcar=0;
    while(!fin)
    {
        car = fgetc(ptrfile);
        if(ferror(ptrfile))
        {
            ret = 0;
            break;
        }
        if(feof(ptrfile))
        {
            if(nbcar>0)
                ungetc(car,ptrfile);
            else
            {
                ret = 0;
            }
            break;
        }
        if(!crlf)
        {
            if((car == '\n') || (car == '\r'))
            {
                crlf=1;
            }
        }
        else
        {
            if((car != '\n') && (car != '\r'))
            {
                ungetc(car,ptrfile);
                break;
            }
        }
        if((car != '\n') && (car != '\r'))
        {
            if(nbcar>=taillemax)
            {
                ungetc(car,ptrfile);
                break;
            }
            buf[nbcar]=car;
            nbcar++;
        }
    }
    buf[nbcar] = 0;
    return ret;
}

int bChargeTableUM_2019(char sFic[])
{
	char buffer[51];
    int first;
    char umcourante[4];
    int indice;
    int i,j;
    int ret;

    indice_um$$$$_2019=-1;
	if(fptrum_2019!=NULL)
        return 0;
    if((fptrum_2019 = fopen(sFic,"r"))== NULL)
	{
		return(500);
	}
    memset(buffer,0,51);
	
    /*vérification du fichier d'UM*/
    if((ret=verifFichierUMsigne_2019())!=0)
    {
        fclose(fptrum_2019);
        fptrum_2019=NULL;
        return ret;
    }
    
    memset(uminf_2019,0,MAX_UM_2019*sizeof(UMINFO_2019));
	for(i=0;i<MAX_UM_2019;i++)
    {
        for(j=0;j<MAX_TYPUM_2019;j++)
            uminf_2019[i].umdeclarees[j] = -1;
    }
    nbuminf_2019=0;
    first=1;
    fseek(fptrum_2019,0,SEEK_SET);
	while(mfgets_2019(buffer,50,fptrum_2019)!=0)
	{
		if(strlen(buffer)!=TAILLE_LIGNE_UM_2019)
            continue;
        if(first || (strncmp(buffer,umcourante,4)!=0))
        {
            strncpy(umcourante,buffer,4);
            if(!first)
                nbuminf_2019++;
			first=0;
            if(nbuminf_2019==MAX_UM_2019)
            {
                fclose(fptrum_2019);
                fptrum_2019=NULL;
                return 501;
		    }
            strncpy(uminf_2019[nbuminf_2019].um,buffer,4);
            uminf_2019[nbuminf_2019].nbautorisations=1;
            
            indice = getindicetypUM_2019(buffer+FICUM_TYP_2019);
            uminf_2019[nbuminf_2019].umdeclarees[indice]=0;
            uminf_2019[nbuminf_2019].autorisations[0].typum = indice;
            uminf_2019[nbuminf_2019].autorisations[0].invalidee = 0;
            strncpy(uminf_2019[nbuminf_2019].autorisations[0].datdebeffet,buffer+FICUM_DATE_DEB_2019,8);
            strncpy(uminf_2019[nbuminf_2019].autorisations[0].datfineffet,buffer+FICUM_DATE_FIN_2019,8);
            strncpy(uminf_2019[nbuminf_2019].autorisations[0].finessgeo,buffer+FICUM_FINESSGEO_2019,9);

            uminf_2019[nbuminf_2019].autorisations[0].troisemecartypum=*(buffer+FICUM_TYP_2019+2);
            uminf_2019[nbuminf_2019].autorisations[0].typhospit=*(buffer+FICUM_TYPHOSPIT_2019);

            /*CD FG13*/
            if(strncmp(uminf_2019[nbuminf_2019].um,"$$$$",4)==0)
                indice_um$$$$_2019=nbuminf_2019;
        }
        else
        {
            if(uminf_2019[nbuminf_2019].nbautorisations==MAX_AUTORISATIONS_UM_2019)
            {
			    fclose(fptrum_2019);
                fptrum_2019=NULL;
                return 501;
		    }
            indice = getindicetypUM_2019(buffer+FICUM_TYP_2019);
            if(uminf_2019[nbuminf_2019].umdeclarees[indice]==-1)
                uminf_2019[nbuminf_2019].umdeclarees[indice]=uminf_2019[nbuminf_2019].nbautorisations;
            uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].typum = indice;
            uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].invalidee = 0;
            strncpy(uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].datdebeffet,buffer+FICUM_DATE_DEB_2019,8);
            strncpy(uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].datfineffet,buffer+FICUM_DATE_FIN_2019,8);
            strncpy(uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].finessgeo,buffer+FICUM_FINESSGEO_2019,9);
            
            uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].troisemecartypum=*(buffer+FICUM_TYP_2019+2);
            uminf_2019[nbuminf_2019].autorisations[uminf_2019[nbuminf_2019].nbautorisations].typhospit=*(buffer+FICUM_TYPHOSPIT_2019);

            uminf_2019[nbuminf_2019].nbautorisations++;
        }
	}
	if(!first)
        nbuminf_2019++;
	return 0;
}

int estdeclare_2019(int indum, int typdum)
{
    return (uminf_2019[indum].umdeclarees[typdum] > -1);
}

int premierindiceTypUM_2019(int indum, int typdum)
{
    return (uminf_2019[indum].umdeclarees[typdum]);
}

int intersection_intervalles_2019(char *datedeb1, char *datefin1, char *datedeb2, char *datefin2, char **intersect1, char **intersect2)
{
	if (intersect1 == NULL)
		return 0;
	if (intersect2 == NULL)
		return 0;

	if (date_strict_sup_2019(datedeb1, datefin2))
		return 0;
	if (date_strict_sup_2019(datedeb2, datefin1))
		return 0;

	if (date_strict_sup_2019(datedeb2, datedeb1))
		*intersect1 = datedeb2;
	else
		*intersect1 = datedeb1;

	if (date_strict_sup_2019(datefin2, datefin1))
		*intersect2 = datefin1;
	else
	{
		*intersect2 = datefin2;
	}
	return 1;
}


int interval_autorise_2019(char datedebutdeffet[], char datefindeffet[], char date_entree[], char date_sortie[], int *nbjours)
{
    char *dateinf;
    char *datesup;

    if(nbjours)
        *nbjours=0;
    
    if(date_strict_sup_2019(datedebutdeffet,date_sortie))
        return 0;
    if(date_strict_sup_2019(date_entree,datefindeffet))
        return 0;
    
    if(date_strict_sup_2019(date_entree,datedebutdeffet))
        dateinf = date_entree;
    else
        dateinf = datedebutdeffet;
    
    if(date_strict_sup_2019(date_sortie,datefindeffet))
        datesup = datefindeffet;
    else
    {
        datesup = date_sortie;
    }

    if(nbjours)
        *nbjours = callos_2019(dateinf,datesup)+1;
    return 1;
}

int interval_autorise_hist_2019(char datedebutdeffet[], char datefindeffet[], char date_entree[], char date_sortie[], char** date_debfinale,char** date_finfinale,int *nbjours)
{
    char *dateinf;
    char *datesup;

    if(nbjours)
        *nbjours=0;
    
    if(date_strict_sup_2019(datedebutdeffet,date_sortie))
        return 0;
    if(date_strict_sup_2019(date_entree,datefindeffet))
        return 0;
    
    if(date_strict_sup_2019(date_entree,datedebutdeffet))
        dateinf = date_entree;
    else
        dateinf = datedebutdeffet;
    
    if(date_strict_sup_2019(date_sortie,datefindeffet))
        datesup = datefindeffet;
    else
    {
        datesup = date_sortie;
    }

    if(nbjours)
        *nbjours = callos_2019(dateinf,datesup)+1;
    *date_debfinale = dateinf;
    *date_finfinale = datesup;
    return 1;
}


/*teste si l'autorisatoin au niveau geographique est autorisée au moins 1 jour dans l'intervalle de temps [d1,d2] pour le finess Geo */
int finessGeoAutorise_2019(char *pfinessgeo, int autorisationGeo, char *d1, char *d2)
{
	UMINFO_2019 luminfspec;
	int indumspec,i;
	char *d3, *d4;
	/*test si le type d'autorisation est global au finess geo*/
	if (umrefs_2019.typeportee[autorisationGeo] != 1)
		return 0;
	indumspec = indice_um$$$$_2019;
	if (indumspec<0)
		return 0;
	if (!estdeclare_2019(indumspec, autorisationGeo))
		return 0;
	luminfspec = uminf_2019[indumspec];
	i = premierindiceTypUM_2019(indumspec, autorisationGeo);
	d3 = d4 = 0;

	while ((i < luminfspec.nbautorisations) && (luminfspec.autorisations[i].typum == autorisationGeo))
	{
		if (strncmp(luminfspec.autorisations[i].finessgeo, pfinessgeo, 9) == 0)
		{
			if (intersection_intervalles_2019(luminfspec.autorisations[i].datdebeffet, luminfspec.autorisations[i].datfineffet, d1, d2, &d3, &d4))
				return 1;
		}
		i++;
	}
	return 0;
}

/*teste si une plage de temps [date_entree,date_sortie] est autorisée en au moins 1 jour en type "typdum"
Renvoie 1 si oui, 0 sinon
Si oui, renvoie également le nombre de jours autorisés, renvoie aussi un indicateur "bPremJourAutor"
pour savoir si le jour de la date d'entrée est autorisé (si oui=1, 0 sinon)
*/
int est_autorise_2019(int indum, int typdum, char date_entree[], char date_sortie[], int *nbjours, int *bPremJourAutor)
{
    UMINFO_2019 luminf;
    int nbjinterv,i;
    int ret;
	char *d1, *d2;

	if(nbjours)
        *nbjours=0;
    else
        return 0;
    
    if(bPremJourAutor)
        *bPremJourAutor=0;
    else
        return 0;

    if(indum<0)
        return 0;
	i = 0;
	if (umrefs_2019.typeportee[typdum] != 1)
	{
		if (!estdeclare_2019(indum, typdum))
			return 0;
		i = premierindiceTypUM_2019(indum, typdum);
	}

	luminf = uminf_2019[indum];
	ret=0;
	d1 = d2 = 0;
	/*while ((i<luminf.nbautorisations) && (luminf.autorisations[i].typum == typdum))*/
	while (i<luminf.nbautorisations)
    {
		if (umrefs_2019.typeportee[typdum] != 1)
		{

			if (interval_autorise_2019(luminf.autorisations[i].datdebeffet, luminf.autorisations[i].datfineffet, date_entree, date_sortie, &nbjinterv))
			{
				*nbjours = *nbjours + nbjinterv;
				ret = 1;

				/*date d'entrée autorisée ?*/
				if (bPremJourAutor)
				{
					if ((!date_strict_sup_2019(luminf.autorisations[i].datdebeffet, date_entree)) && (!date_strict_sup_2019(date_entree, luminf.autorisations[i].datfineffet)))
						*bPremJourAutor = 1;
				}
			}
			i++;
			if ((i >= luminf.nbautorisations) || (luminf.autorisations[i].typum != typdum))
				break;
		}
		else
		{
			/*2017 autorisation globale signifie globale à un finess geo donné*/
			d1 = d2 = 0;
			if (intersection_intervalles_2019(luminf.autorisations[i].datdebeffet, luminf.autorisations[i].datfineffet, date_entree, date_sortie,&d1,&d2))
			{
				/*tester si le finess geo est autorisé en ioa pendant l'intervalle de temps defini par l'intervalle de temps [d1,d2]*/
				if (finessGeoAutorise_2019(luminf.autorisations[i].finessgeo,typdum, d1, d2))
					return 1;
			}
			i++;
		}
    }
	return ret;
}


int nbjours_mois_2019(int mois, int annee)
{
  int nbj;
  switch (mois)
	{
		 case 1:
		 case 3:
		 case 5:
		 case 7:
		 case 8:
		 case 10:
		 case 12: nbj = 31; break;
		 case 4:
		 case 6:
		 case 9:
		 case 11: nbj = 30; break;
		 case 2: if (((annee%4 == 0) && (annee%100 !=0)) || (annee%400==0))
                     nbj=29;
				 else
					 nbj = 28;
				break;
		 default : nbj=0;
	}
  return nbj;
}

/*ajoute nbj nombre de jours à une date donnée*/
void Ajout_jour_date_2019(char date[],int nbj,int *jour,int *mois,int *annee)
{
	int jj,mm,aa;

	convdate_2019 (date,&jj,&mm,&aa);
	while (nbj+jj>nbjours_mois_2019(mm,aa))
	{
		nbj=nbj-(nbjours_mois_2019(mm,aa)-jj+1);
		jj=1;
		if (mm==12)
    	{
			mm=1;
			aa++;
		}
		else
		{
			mm++;
		}
    }
	jj = jj+nbj;
	*jour = jj; *mois = mm; *annee = aa;
}
void Soustraire_jour_date_2019(char *date,int nbj,int *jour,int *mois,int *annee)
{
	int jj,mm,aa;

	convdate_2019 (date,&jj,&mm,&aa);
    if(nbj<0)
        nbj=0;
	while (jj-nbj<1)
	{
        nbj=nbj-jj;
        if(mm==1)
        {
            mm=12;
            aa--;
        }
        else
        {
            mm--;
        }
        jj=nbjours_mois_2019(mm,aa);
    }
	jj = jj-nbj;
	*jour = jj; *mois = mm; *annee = aa;
}



/* Retourne le type d'UM à une date donnée*/
int trouveTypeUM_2019(char date[],int indum,int *typdum)
{
	int i;
	UMINFO_2019 luminf;
    if(indum<0)
        return 0;
    luminf = uminf_2019[indum];
	i=0;
	while ((i<luminf.nbautorisations) && (date_strict_sup_2019(date,luminf.autorisations[i].datfineffet) ||  date_strict_sup_2019(luminf.autorisations[i].datdebeffet,date)))
    {
		i++;
	}
	if (i>=luminf.nbautorisations)
		return 0;
	*typdum=luminf.autorisations[i].typum;
	return 1;
}

int trouveindiceUM_2019(char strUM[])
{
    int i,j;
	char trim_um[5];

	trim_um[4]=0;
	strncpy(trim_um,"    ",4);

	j=0;
	while((j<4) && (strUM[j]==' '))
		j++;
	if(j<4)
		strncpy(trim_um,&(strUM[j]),4-j);
	for(i=0;i<nbuminf_2019;i++)
	{
		if(strncmp(uminf_2019[i].um,trim_um,4)==0)
		{
			return i;
		}
	}
	return -1; /*l'UM n'existe pas dans le fichier d'UM*/
}

/*L'autorisation d'UM est elle dans REF_AUTO (dépendant de la date de sortie du séjour)*/
int autorisation_valide_2019(int ptypum)
{
    if(ptypum==TYPUM_BLANC)
        return 1;
    return umrefs_2019.bvalide[ptypum];
}
int autlitdedie_valide_2019(int ptyplitded)
{
    return litdedref_2019.bvalide[ptyplitded];
}
int getnumfoncsupplement_2019(int ptypum)
{
    return umrefs_2019.numfoncval[ptypum];
}
void getPlagesValo_2019(intervallesAutor_2019 **lesPlagesValo)
{
    *lesPlagesValo=plagesAutor_2019;
}

