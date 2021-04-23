#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ctrlgenficum.h"

#define PrintLogUM(X,Y) if(ficlog) fprintf(ficlog,X,Y)
#define Fclose(X) if(X){ fclose(X);X=NULL; }
#define CRC32_POLY 0x04c11db7
#define MOD_ASCII

FILE *ficlog;
FILE *ficautoref;
unsigned long table_crc32[256];
int bGestionPorteeGlobale;

static int posUMcourante=0;
static char UMcourante[5];

/*****fonctions utilitaires*******/
/*@@ contrôle de cohérence d'un char* avec un format:
zone= zone contenant la variable à tester
format= zone texte comportant des charactères corespondant au type corespondant de charactére attendu dans zone (ex: '9' => on attend un chiffre)
ft= cmment traiter les espaces dans zone (1=>ignorer les espaces avant,2=> ignorer les espaces aprés, 3=> ignorer les espaces avant et aprés)
@@*/
int controle_format(char *zone,char format[],int ft,int taille)
{
	int i,j,lg,ret,t,deb,fin;
	int pcnbg,pcnbd;
	char zone2[24],fmt2[24];

	lg=strlen(format);
	pcnbg= -1;
	pcnbd= lg+1;
	for(i=0,j=lg-1;i<lg;i++,j--)
	{
		if(*(zone+i)!=' ' && pcnbg== -1)pcnbg=i;
		if(*(zone+j)!=' ' && pcnbd== lg+1)pcnbd=j;
	}
	t=pcnbd-pcnbg+1;

	if(t>lg) return(1);

	deb=0;
	fin=lg-1;
	if(ft==1)
		deb=pcnbg;
	if(ft==2)
		fin=pcnbd;
	if(ft==3)
	{
		deb=pcnbg;
		fin=pcnbd;
	}
	t=fin-deb+1;

	if(t<taille)
		return(2);

	strncpy(zone2,zone+deb,t);
	strncpy(fmt2,format+deb,t);

	ret=1;
	for (i=0;i<t;i++)
	{
		switch(*(fmt2+i)){
		case '9' :
			ret=isdigit(*(zone2+i));
			break;
		case 'Z' :
			ret=(isdigit(*(zone2+i)) || *(zone2+i)==' ');
			break;
		case 'Y' :
			ret=(isalnum(*(zone2+i)) || *(zone2+i)==' ');
			break;
		case 'A' :
			ret=isalnum(*(zone2+i));
			break;
		case 'X' :
			ret=isalpha(*(zone2+i));
			break;
        case '+' :
            ret=(isdigit(*(zone2+i)) || *(zone2+i)==' ' || *(zone2+i)=='+');
			break;
		}
		if (!ret) return(3);
	}
	return(0);
}

void	date_jma (char *adat,int *p_jj,int *p_mm,int *p_aa)
{
        char zw[5];

        sprintf(zw,"%2.2s",adat);
        *p_jj=atoi(zw);
        sprintf(zw,"%2.2s",adat+2);
        *p_mm=atoi(zw);
        sprintf(zw,"%4.4s",adat+4);
        *p_aa=atoi(zw);

} /* fin de la fonction convdat */

/*@@ controle du format de la date (sans plus) @@*/
int controle_coherence_date (char *adate)
{
	int bis; /* variables de travail */
	static  int tabj[2][13] = {
				{0,31,28,31,30,31,30,31,31,30,31,30,31},
				{0,31,29,31,30,31,30,31,31,30,31,30,31}	};
	int jj,mm,aa;

	date_jma(adate,&jj,&mm,&aa); /*lis le format "étrange" et le transforme en qq chose d'utilisable*/
	bis= aa%4 == 0 && aa%100 !=0 || aa%400==0;
	/* controle mois   */
	if (mm < 1 || mm > 12) return(1);
	/* controle brut du jour */
	if (jj < 1 || jj > tabj[bis][mm]) return(1);
	return(0);
}

int compare_dates(char *date1,char *date2)
{
	char  datang1[8];
	char  datang2[8];

	strncpy(datang1,date1+4,4);
	strncpy(datang1+4,date1+2,2);
	strncpy(datang1+6,date1,2);

	strncpy(datang2,date2+4,4);
	strncpy(datang2+4,date2+2,2);
	strncpy(datang2+6,date2,2);

	return (strncmp(datang1,datang2,8));
}


int nombre_jour_depuis_1980(char *date)
{
	static int tab[2][12] = {
              {0,31,59,90,120,151,181,212,243,273,304,334},
              {0,31,60,91,121,152,182,213,244,274,305,335} };
	int 	jj, mm, aa;	/* date entree en entier */
        int i,bis,nbj;

        date_jma (date,&jj,&mm,&aa);
        nbj = 0;
        for(i=1980;i<aa;i++)
            if (i%4 == 0 && i%100 !=0 || i%400==0)
				nbj += 366 ;
            else
				nbj += 365;
        bis=(aa%4 == 0 && aa%100 !=0 || aa%400==0) ;
        nbj += tab[bis][mm-1];
        nbj += jj;
        return(nbj);
}

int nbjoursdansmois(int mois, int annee)
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
void Ajout_njour_a_date(char date[],int nbj,int *jour,int *mois,int *annee)
{
	int jj,mm,aa;

	date_jma (date,&jj,&mm,&aa);
	while (nbj+jj>nbjoursdansmois(mm,aa))
	{
		nbj=nbj-(nbjoursdansmois(mm,aa)-jj+1);
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
int my_index(char s1[],char s2[])
{
 int i,j,k;
 for(i=0;s1[i]!='\0';i++)
  {
   for(j=i,k=0;s2[k]!='\0' && s1[j]==s2[k];j++,k++);
   if(s2[k]=='\0')
	   return(i);
  }
  return(-1);
}

unsigned char To_Ascii(unsigned char orig_char)
{
#ifdef MOD_ASCII
	return orig_char;
#else
	/* implémenter la fonction de conversion en ASCII*/
    return ebcdic_to_ascii[orig_char];
#endif
}

void init_crc_32()
{
	int i, j;
	unsigned long c;

	for (i = 0; i < 256; ++i) {
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		table_crc32[i] = c;
	}
}
void signer_ficIUM(unsigned char *buf[], int nbium,unsigned char *res)
{
	unsigned char *p;
	unsigned long  crc;
	int i,len;
    static int initialize_crc32 = 0;
	
	/*if (!table_crc32[1])*/
    if(!initialize_crc32)
    {
		init_crc_32();
        initialize_crc32=1;
    }
	crc = 0xffffffff;
	for(i=0;i<nbium;i++)
	{
        p=buf[i];
        if(p==NULL)
            continue;
		len=strlen(p);
		for (p; len > 0; ++p, --len)
			crc = (crc << 8) ^ table_crc32[(crc >> 24) ^ To_Ascii(*p)];
	}
	sprintf (res, "%010u",~crc);
}
/*Fonction mafgets : lit une ligne d'un fichier ouvert en mode binaire
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
int mafgets(char *buf, int taillemax, FILE *ptrfile)
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

void pad_droite(char *pchaine)
{
    char trim_um[5];
    int j;

    trim_um[4]=0;
	strncpy(trim_um,"    ",4);
	j=0;
	while((j<4) && (pchaine[j]==' '))
		j++;
	if(j<4)
		strncpy(trim_um,&(pchaine[j]),4-j);
    strncpy(pchaine,trim_um,4);
}

int AjoutLigneUM(struct stUM *_stUM)
{
    if(nbum_source==MAX_LIGNE_UM)
    {
        if(ficlog)
            PrintLogUM("Nombre maximum de lignes ( = %d ) dans le fichier d'UM dépassé\n",MAX_LIGNE_UM);
        return ERR_UM_SYSTEME;
    }
    if(bGestionPorteeGlobale)
        strncpy(_stUM->datefinautor,"????????",8);
    memcpy(&tabums_source[nbum_source],_stUM,sizeof(struct stUM));
    /*on padde le N°d'UM à droite*/
    pad_droite(tabums_source[nbum_source].um);
    nbum_source++;
    return 0;
}

int date_strict_sup(char *date1,char *date2)
{
	char  datang1[8];
	char  datang2[8];

	strncpy(datang1,date1+4,4);
	strncpy(datang1+4,date1+2,2);
	strncpy(datang1+6,date1,2);

	strncpy(datang2,date2+4,4);
	strncpy(datang2+4,date2+2,2);
	strncpy(datang2+6,date2,2);

	return (strncmp(datang1,datang2,8) > 0);
}

/*****FIN fonctions utilitaires*******/
/*
Vérifie si chaque ligne du fichier contient TAILLE_LIGNE_UM_INITIAL caractères strictement
Vérifie également la syntaxe et la cohérence des dates
*/
int verifFormat(FILE *ficum)
{
    int eof;
    char buf[100];
    int ret,ret2;
    int ligne;
    int lgr_chaine;
    int numformat;
    int pg;
    struct stUM stumsource;
    struct stUM stumsource2;
    
    nbum_source=0;
    eof=0;
    ret=0;
    ligne=0;
    numformat=0;
    memset(tabums_source,0,MAX_LIGNE_UM*sizeof(struct stUM));
    while(!eof)
    {
        ligne++;
        if(mafgets(buf,100,ficum)==0)
        {
            eof=1;
            continue;
        }
        lgr_chaine = strlen(buf);
        switch(lgr_chaine)
        {
            case TAILLE_LIGNE_UM_INITIAL_FORMAT3:
                strncpy(&stumsource.um[0],buf,TAILLE_LIGNE_UM_INITIAL_FORMAT3);
                break;
            default:
                ret=ERR_UM_FORMAT;
                PrintLogUM("Erreur de format dans le fichier d'UM ligne %d\n",ligne);
                continue;
        }
        if( (controle_format(stumsource.datedebautor,"Z9Z99999",1,7)!=0)|| (controle_coherence_date(stumsource.datedebautor)!=0) )
        {
            ret=ERR_UM_FORMAT;
            PrintLogUM("Date au format incorrect ou incohérente ligne %d\n",ligne);
            continue;
        }
        if ((strncmp(stumsource.autorisation,"  ",2)==0) || ((controle_format(stumsource.autorisation,"99",0,2))!=0))
        {
            ret=ERR_UM_FORMAT;
            PrintLogUM("Format incorrect de l'autorisation d'UM ligne %d : doit être égale à blanc ou a un nombre de 00 à 99 (si le numéro est <10, le zéro devant l'unité est obligatoire)\n",ligne);
            continue;
        }
        
        if ((strncmp(stumsource.autorisation,"  ",2)==0) && (strncmp(stumsource.autorisation+2," ",1)!=0))
        {
            ret=ERR_UM_FORMAT;
            PrintLogUM("Format incorrect de l'autorisation d'UM ligne %d : si les 2 premiers caractères de l'autorisation sont a blancs, alors le 3eme doit l'etre aussi\n",ligne);
            continue;
        }
        
        if(bGestionPorteeGlobale)
        {
            pg=porteeGlobale(stumsource.autorisation);
            if(strncmp(stumsource.um,"$$$$",4)==0)
            {
                /*si UM=$$$$ vérifier que le type d'autorisation est utilisable en portée globale*/
                if(!pg)
                {
                    /*on ignore la ligne*/
					/*ret=ERR_UM_TYPENONAUTORISE_EN_GLOBAL;*/
                    PrintLogUM("Type d'autorisation non acceptee avec l'UM reservee $$$$, ligne %d (ignorée): \n",ligne);
                    continue;
                }
            }
            else
            {
                if(pg)
                {
                    /*si le type d'autorisation a une portée globale (i.e. valable quelque soit l'UM), alors l'UM devient $$$$*/
                    memcpy(&stumsource2.um[0],&stumsource.um[0],sizeof(struct stUM));
                    strncpy(stumsource2.um,"$$$$",4);
                    ret2=AjoutLigneUM(&stumsource2);
                    if(ret2!=0)
                        return ret2;
                }
            }
        }
        if(!ret)
        {
            ret2=AjoutLigneUM(&stumsource);
            if(ret2!=0)
                return ret2;
        }
    }
    return ret;
}


int __cdecl compare1( const void *arg1, const void *arg2 )
{
    struct stUM *lum1;
    struct stUM *lum2;
    int res;
    lum1=(struct stUM*)arg1;
    lum2=(struct stUM*)arg2;
    res=strncmp(lum1->um,lum2->um,4);
    if(res!=0)
        return res;
    res=compare_dates(lum1->datedebautor,lum2->datedebautor);
    return res;
}

/*
Tri en fonction de l'UM et de la date de début d'autorisation
*/
int trificum_um_datedebautor()
{
    /*tri des UM suivant l'ordre ascendant UM,DATE DE DEBUT D'AUTORISATION*/
    qsort(tabums_source,nbum_source,sizeof(struct stUM),compare1);
    return 0;
}
int verifdoublons()
{
    int ret,i,first;
    char umcourante[4];
    char date_cour[8];

    ret=0;
    first=1;
    for(i=0;i<nbum_source;i++)
    {
        if((first) || (strncmp(umcourante,tabums_source[i].um,4)!=0))
        {
            strncpy(umcourante,tabums_source[i].um,4);
            first=0;
        }
        else
        {
            /*CD FG13 : ne pas vérifier pour l'UM spéciale $$$$*/
            if((!bGestionPorteeGlobale) || (strncmp(tabums_source[i].um,"$$$$",4)!=0))
            {
                if(strncmp(date_cour,tabums_source[i].datedebautor,8)==0)
                {
                    PrintLogUM("Erreur dans l'unité médicale %4.4s , présence de doublons sur les dates de début d'autorisation",tabums_source[i].um);
                    ret=1;
                }
            }
        }
        strncpy(date_cour,tabums_source[i].datedebautor,8);
    }
    return ret;
}
int verifdoublonsSurAutorisationGlobaleAuFinessGeo()
{
	int ret, i, first;
	char date_cour[8];
	char finessgeocourant[9];
	int indiceumspec;
	char typumcourant[3];
	int	nbjours;
	int jj, mm, aa;
	char datetemp[9];

	/*on recherche d'abord l'indice de la premiere ligne correspondant à l'UM spéciale $$$$*/
	for (i = 0; i<nbum_source; i++)
	{
		if (strncmp(tabums_source[i].um, "$$$$", 4) == 0)
		{
			indiceumspec = i;
			break;
		}
	}
	if (i == nbum_source)
		return 0;/*il n'y a pas d'autorisation valable pour l'ensemble de l'établissement(portée globale)*/

	ret = 0;
	first = 1;
	
	for (i = indiceumspec; i<nbum_source && (strncmp(tabums_source[i].um, "$$$$", 4) == 0); i++)
	{
		if ((first) ||
			(strncmp(typumcourant, tabums_source[i].autorisation, 2) != 0) /*pour les autorisations globales aux */
			||
			(strncmp(finessgeocourant, tabums_source[i].finessgeo, 9) != 0))
		{
			if (!first)
			{
				strncpy(tabums_source[i - 1].datefinautor, "01012045", 8);
			}
			strncpy(typumcourant, tabums_source[i].autorisation, 2);
			strncpy(finessgeocourant, tabums_source[i].finessgeo, 9);
			first = 0;
		}
		else
		{
			if (strncmp(date_cour, tabums_source[i].datedebautor, 8) == 0)
			{
				/*PrintLogUM("Erreur pour l'autorisation globale %3.3s et pour le Finess Géographique %9.9s, présence de plusieurs dates de début d'autorisation", tabums_source[i].autorisation, tabums_source[i].finessgeo);*/
				if (ficlog)
					fprintf(ficlog, "Erreur pour l'autorisation globale %3.3s et pour le Finess Géographique %9.9s, présence de plusieurs dates de début d'autorisation", tabums_source[i].autorisation, tabums_source[i].finessgeo);
				ret = 1;
				break;
			}
			else
			{
				nbjours = nombre_jour_depuis_1980(tabums_source[i].datedebautor) - nombre_jour_depuis_1980(tabums_source[i - 1].datedebautor);
				Ajout_njour_a_date(tabums_source[i - 1].datedebautor, nbjours - 1, &jj, &mm, &aa);
				sprintf(datetemp, "%02d%02d%04d", jj, mm, aa);
				strncpy(tabums_source[i - 1].datefinautor, datetemp, 8);
			}
		}
		strncpy(date_cour, tabums_source[i].datedebautor, 8);
	}
	if (i>0)
		strncpy(tabums_source[i - 1].datefinautor, "01012045", 8);
	return ret;
}

/*vérifier la cohérence période saisie avec la date d'effet de l'autorisatrion*/
/*pour l'instant test uniquement de l'autorisation 72 (IVG) en dur*/
int verifDateEffetAutorisations()
{
	int i;
	for (i = 0; i < nbum_source; i++)
	{
		if (!strncmp(tabums_source[i].autorisation, "72", 2))
		{
			if (strncmp(tabums_source[i].datefinautor, "????????", 8) == 0)
				continue;
			if (!date_strict_sup("01032019", tabums_source[i].datefinautor))
				return 1;
		}
	}
	return 0;
}
void calculdatesfin()
{
    int i,first;
    char umcourante[4];
    char date_cour[8];
    char datetemp[9];
    int	nbjours;
    int jj,mm,aa;
    
    first=1;
    for(i=0;i<nbum_source;i++)
    {
        if((first) || (strncmp(umcourante,tabums_source[i].um,4)!=0))
        {
            if(!first)
            {
                /*CD FG13 : ne pas calculer pour l'UM spéciale $$$$*/
                if((!bGestionPorteeGlobale) || (strncmp(tabums_source[i-1].um,"$$$$",4)!=0))
                    strncpy(tabums_source[i-1].datefinautor,"01012045",8);
            }
            strncpy(umcourante,tabums_source[i].um,4);
            first=0;
        }
        else
        {
            /*CD FG13 : ne pas vérifier ni calculer pour l'UM spéciale $$$$*/
            if((!bGestionPorteeGlobale) ||(strncmp(tabums_source[i-1].um,"$$$$",4)!=0))
            {
                nbjours=nombre_jour_depuis_1980(tabums_source[i].datedebautor)-nombre_jour_depuis_1980(tabums_source[i-1].datedebautor);
                Ajout_njour_a_date(tabums_source[i-1].datedebautor,nbjours-1,&jj,&mm,&aa);
                sprintf(datetemp,"%02d%02d%04d",jj,mm,aa);
                strncpy(tabums_source[i-1].datefinautor,datetemp,8);
            }
        }
        strncpy(date_cour,tabums_source[i].datedebautor,8);
    }
    if(i>0)
    {
        /*CD FG13 : ne pas calculer pour l'UM spéciale $$$$*/
        if((!bGestionPorteeGlobale) || (strncmp(tabums_source[i-1].um,"$$$$",4)!=0))
            strncpy(tabums_source[i-1].datefinautor,"01012045",8);
    }
}
int __cdecl compare2( const void *arg1, const void *arg2 )
{
    struct stUM *lum1;
    struct stUM *lum2;
    int res;
    lum1=(struct stUM*)arg1;
    lum2=(struct stUM*)arg2;
    res=strncmp(lum1->um,lum2->um,4);
    if(res!=0)
        return res;
    res=strncmp(lum1->autorisation,lum2->autorisation,3);
    if ( ((strncmp(lum1->autorisation,"   ",3)==0) && (strncmp(lum2->autorisation,"000",3)==0)) ||
        ((strncmp(lum2->autorisation,"   ",3)==0) && (strncmp(lum1->autorisation,"000",3)==0)) )
        res=0;

    if(res!=0)
        return res;
    res=compare_dates(lum1->datedebautor,lum2->datedebautor);

	if (res != 0)
		return res;
	res = strncmp(lum1->finessgeo, lum2->finessgeo,9);
	return res;
}
int __cdecl compare3(const void *arg1, const void *arg2)
{
	struct stUM *lum1;
	struct stUM *lum2;
	int res;
	lum1 = (struct stUM*)arg1;
	lum2 = (struct stUM*)arg2;
	res = strncmp(lum1->um, lum2->um, 4);
	if (res != 0)
		return res;
	res = strncmp(lum1->autorisation, lum2->autorisation, 3);
	if (((strncmp(lum1->autorisation, "   ", 3) == 0) && (strncmp(lum2->autorisation, "000", 3) == 0)) ||
		((strncmp(lum2->autorisation, "   ", 3) == 0) && (strncmp(lum1->autorisation, "000", 3) == 0)))
		res = 0;
	if (res != 0)
		return res;
	res = strncmp(lum1->finessgeo, lum2->finessgeo, 9);
	if (res != 0)
		return res;
	res = compare_dates(lum1->datedebautor, lum2->datedebautor);
	return res;
}

/*
Tri en fonction de l'UM , de l'autorisation et de la date de début d'autorisation
*/
int trificum_um_aut_datedebautor()
{
    /*tri des UM suivant l'ordre ascendant UM,TYPE AUTORISATION,DATE DE DEBUT D'AUTORISATION*/
    qsort(tabums_source,nbum_source,sizeof(struct stUM),compare2);
    return 0;
}
/*
Tri en fonction de l'UM , de l'autorisation, du Finess Geo et de la date de début d'autorisation
*/
int trificum_um_aut_finessgeo_datedebautor()
{
	/*tri des UM suivant l'ordre ascendant UM,TYPE AUTORISATION,DATE DE DEBUT D'AUTORISATION*/
	qsort(tabums_source, nbum_source, sizeof(struct stUM), compare3);
	return 0;
}
/*signature fichier d'iUM*/
int calcul_cle_ficum(int bgenerefinessgeo)
{
    int i;
    char *tabumsource[MAX_LIGNE_UM];
    for(i=0;i<nbum_source;i++)
    {
        tabumsource[i]=NULL;
        if((bGestionPorteeGlobale) && (strncmp(tabums_source[i].datefinautor,"????????",8)==0))
            continue;
        
        if(bgenerefinessgeo)
            tabumsource[i] = (char*)malloc(sizeof(struct stUM)+1);
        else
            tabumsource[i] = (char*)malloc(sizeof(struct stUM)+1-9);
        
        if(tabumsource[i]==NULL)
            return ERR_UM_SYSTEME;
        
        if(bgenerefinessgeo)
        {
            strncpy(tabumsource[i],tabums_source[i].um,sizeof(struct stUM));
            *(tabumsource[i]+sizeof(struct stUM))=0;
        }
        else
        {
            strncpy(tabumsource[i],tabums_source[i].um,4);
            strncpy(tabumsource[i]+4,tabums_source[i].autorisation,23);
            *(tabumsource[i]+sizeof(struct stUM)-9)=0;
        }
    }
    signer_ficIUM(tabumsource,nbum_source,crcum);
    for(i=0;i<nbum_source;i++)
    {
        if(tabumsource[i]!=NULL)
            free(tabumsource[i]);
    }
    return 0;
}
int creerFichierUM(FILE *ficumout,int bgenerefinessgeo)
{   
    int i;
    for(i=0;i<nbum_source;i++)
    {
        if((bGestionPorteeGlobale) && (strncmp(tabums_source[i].datefinautor,"????????",8)==0))
            continue;
        if(bgenerefinessgeo)
            fwrite(&tabums_source[i],sizeof(struct stUM),1,ficumout);
        else
        {
            fwrite(tabums_source[i].um,4,1,ficumout);
            fwrite(tabums_source[i].autorisation,23,1,ficumout);
        }
        fprintf(ficumout,"\n");
    }
    fprintf(ficumout,"%s\n",crcum);
    return 0;
}


/*Fonction CtrlFicUM
Prend en entrée un fichier d'UM et le transforme 
en un format reconnu par la fonction groupage
3 arguments :
1er :  nom et chemin complet du fichier d'UM source
2eme : nom et chemin complet du fichier d'UM à générer
3eme : répertoire des tables de la FG finissant par /
4eme : fichier log généré si différent de zéro, pas généré si égal à zéro.
5eme : bgestionPortee:gestion de la portée globale du type d'autorisation 0:non 1:oui
renvoie zéro si la génération a réussi
6eme : 1 si la fonction doit inclure le numéro Finess géographique en sortie, 0 sinon*/
int CtrlFicUM(char *nficumin, char *nficumout, char *nreptabfg, int bgenerelog, int bgestionPortee,int bgenerefinessgeo)
{
    FILE *ficumin;
    FILE *ficumout;
    
    char nf[512];
    int ret,i,first;
    char umcourante[4];
    
    ret=0;
	indice_tabumsource$$$$ = -1;
    ficumin=ficumout=ficautoref=ficlog=NULL;
    bGestionPorteeGlobale=bgestionPortee;

    if(strcmp(nficumin,"")==0)
    {
        return ERR_UM_OUVERTURE_FICHIER;
    }
    ficlog=NULL;
    if(bgenerelog)
    {
        sprintf(nf,"%s.log",nficumin);
	    if ((ficlog=fopen(nf,"w"))==NULL)
	    {
            fprintf(stderr,"impossible de créer le fichier %s\n",nf);
            return(ERR_UM_CREATION_FICHIERLOG);
	    }
    }
        
    if ((ficumin=fopen(nficumin,"rb"))==NULL)
    {
        PrintLogUM("Impossible d'ouvrir le fichier d'UM source : %s\n",nficumin);
        Fclose(ficlog);
        return ERR_UM_OUVERTURE_FICHIER;
    }

    if(bGestionPorteeGlobale)
    {
        /*fichiers de références d'autorisations*/
        sprintf(nf,"%s%s",nreptabfg,TABLE_AUTOREFS_BINAIRE);
        if ((ficautoref=fopen(nf,"rb"))==NULL)
        {
            PrintLogUM("Impossible d'ouvrir le fichier de references d'autorisations : %s\n",nf);
            Fclose(ficumin);
            Fclose(ficlog);
            return ERR_UM_OUVERTURE_FICHIER;
        }
        ret=chargeRefAutorisations();
    }
    if(!ret)
    {
        ret = verifFormat(ficumin);
    }

    if(!ret)
    {
        trificum_um_datedebautor();
        if(verifdoublons()!=0)
            ret=ERR_UM_DOUBLONS;
        /*2013 : pour le champ "Numéro FINESS géographique" du RSA, il faut conserver le n°finess géo le plus récent associé a l'UM*/
        nbums_declarees=0;
        first=1;
        for(i=0;i<nbum_source;i++)
        {
            if((first) || (strncmp(umcourante,tabums_source[i].um,4)!=0))
            {
                if(!first)
                {
                    strncpy(ums_finessgeo[nbums_declarees].um,umcourante,4);
                    strncpy(ums_finessgeo[nbums_declarees].finessgeo_leplusrecent,tabums_source[i-1].finessgeo,9);
                    nbums_declarees++;
                }
                strncpy(umcourante,tabums_source[i].um,4);
                first=0;
            }
        }
        if(!first)
        {
            strncpy(ums_finessgeo[nbums_declarees].um,umcourante,4);
            strncpy(ums_finessgeo[nbums_declarees].finessgeo_leplusrecent,tabums_source[i-1].finessgeo,9);
            nbums_declarees++;
        }
    }
    
    if(!ret)
    {
        calculdatesfin();
		if (bGestionPorteeGlobale)
		{
			trificum_um_aut_finessgeo_datedebautor();
			/*pour l'UM $$$$, pour une autorisation globale au Finess geo donnée, et un FinessGeo donnée, la date de début d'effet doit être unique*/
			if (verifdoublonsSurAutorisationGlobaleAuFinessGeo() != 0)
				ret = ERR_UM_DOUBLONS_AUTGLOBFGEO;
		}
		trificum_um_aut_datedebautor();
        /*if(bGestionPorteeGlobale) 
            calculdatesfinAutorEtab();*/
    }
	if (!ret)
	{
		if (verifDateEffetAutorisations() != 0)
			ret = ERR_UM_TYPEAUT_PLUSACCEPTEE;
	}
	
	if(!ret)
    {
        ret=calcul_cle_ficum(bgenerefinessgeo);
    }

    if(!ret)
    {
        if ((nficumout==0)|| (strcmp(nficumout,"")==0))
        {
            PrintLogUM("Le nom du fichier d'UM cible est vide%s\n","");
            Fclose(ficlog);
            Fclose(ficumin);
            Fclose(ficautoref);
            return ERR_UM_CREATION_FICHIERUM;
        }
        
        if ((ficumout=fopen(nficumout,"w"))==NULL)
        {
            PrintLogUM("Impossible de créer le fichier d'UM cible : %s\n",nficumout);
            Fclose(ficlog);
            Fclose(ficumin);
            Fclose(ficautoref);
            return ERR_UM_CREATION_FICHIERUM;
        }
        creerFichierUM(ficumout,bgenerefinessgeo);
    }
    Fclose(ficlog);
    Fclose(ficumin);
    Fclose(ficumout);
    Fclose(ficautoref);
    return ret;
}

unsigned long carlong_gen_13e (unsigned char c[])
{
  int i;
  unsigned long l;

  l=0;
  for(i=0;i<4;i++)
    l = (l*256)+c[i];
  return(l);
}
void accstb_gen_13e (FILE *fich,long *_offset,int *_size,long *_taille)
{
    fread(entete_gen_stab_13e.sgstn,1,sizeof(struct entete_gen_13e),fich);
    *_size=entete_gen_stab_13e.lastn[0]*256+entete_gen_stab_13e.lastn[1];
    *_offset=carlong_gen_13e(entete_gen_stab_13e.ofstn);
    *_taille=carlong_gen_13e(entete_gen_stab_13e.lgstn);
}

int chargeRefAutorisations()
{
    long loffst;
    int lTailleArticle;
    long lTailleTotaleTable;
    long offset_table;
    int i;
    unsigned char article[50];

    long loffs;
    int _lTailleArticleREFAUTO_LITDED;
    long _lTailleTotaleTableREFAUTO_LITDED;
    int _lTailleArticleREFAUTO_UM;
    long _lTailleTotaleTableREFAUTO_UM;
    long _offsttabref_um;
    int lnbart;
    int nbplages;


    fread(entete_gen_table_13e.sign,sizeof(struct ENTETE_gen_13e),1,ficautoref);
    accstb_gen_13e(ficautoref,&loffst,&lTailleArticle,&lTailleTotaleTable);
    nbplages=entete_gen_stab_13e.nastn[0]*256+entete_gen_stab_13e.nastn[1];

    for(i=0;i<nbplages;i++)
    {
        fread(article,sizeof(char),lTailleArticle,ficautoref);
        if(i==nbplages-1)
            offset_table=((article[6]*256+article[7])*256+article[8])*256+article[9];
    }
    fseek(ficautoref,offset_table,SEEK_SET);

    /*entete général*/
    fread(entete_gen_table_13e.sign,sizeof(struct ENTETE_gen_13e),1,ficautoref);
    /*lit dédiés*/
    accstb_gen_13e(ficautoref,&loffs,&_lTailleArticleREFAUTO_LITDED,&_lTailleTotaleTableREFAUTO_LITDED);
    /*UM*/
    accstb_gen_13e(ficautoref,&loffs,&_lTailleArticleREFAUTO_UM,&_lTailleTotaleTableREFAUTO_UM);
    _offsttabref_um=loffs+offset_table;

    fseek(ficautoref,_offsttabref_um,SEEK_SET);
    if(_lTailleArticleREFAUTO_UM==0) /*precaution*/
	{
		return ERR_UM_FICHIER_CORROMPU;
	}
    lnbart = _lTailleTotaleTableREFAUTO_UM/_lTailleArticleREFAUTO_UM;
	umrefs_gen_13e.nbaut=lnbart;
    for(i=0;i<lnbart;i++)
    {
        fread(article,sizeof(char),3,ficautoref);
        umrefs_gen_13e.typum[i]=article[0];
        /*pas besoin du 2 ème octet
        umrefs_gen_13e.numfoncval[i]=article[1];*/
        umrefs_gen_13e.typeportee[i]=article[2];
    }
    return 0;
}

/*est ce que le type d'autorisation a une portée globale à l'établissement?*/
int porteeGlobale(char ptypum[])
{
    int i;
    int lumaut;
    int iptypum;
    char lptypum[3];
    strncpy(lptypum,ptypum,2);
    lptypum[2]=0;
    iptypum=atoi(lptypum);

    if(iptypum==0)
        return 0;
    for(i=0;i<umrefs_gen_13e.nbaut;i++)
    {
        lumaut=umrefs_gen_13e.typum[i];
        if(lumaut==iptypum)
        {
            if(umrefs_gen_13e.typeportee[i]==1)
                return 1;
            else
                return 0;
        }
        if(lumaut>iptypum)
            break;
    }
    return 0;
}

/*est ce qu'il existe un jour au moins en commun entre les 2 intervalles*/ 
int interval_autorise(char datedebutdeffet[], char datefindeffet[], char date_entree[], char date_sortie[])
{
    if(date_strict_sup(datedebutdeffet,date_sortie))
        return 0;
    if(date_strict_sup(date_entree,datefindeffet))
        return 0;
    return 1;
}
int intersection_intervalles(char *datedeb1, char *datefin1, char *datedeb2, char *datefin2, char **intersect1, char **intersect2)
{
	if (intersect1 == NULL)
		return 0;
	if (intersect2 == NULL)
		return 0;

	if (date_strict_sup(datedeb1, datefin2))
		return 0;
	if (date_strict_sup(datedeb2, datefin1))
		return 0;

	if (date_strict_sup(datedeb2, datedeb1))
		*intersect1 = datedeb2;
	else
		*intersect1 = datedeb1;

	if (date_strict_sup(datefin2, datefin1))
		*intersect2 = datefin1;
	else
	{
		*intersect2 = datefin2;
	}
	return 1;
}

/*renvoie la liste des autorisations globales valide par rapport a un intervalle de temps donné*/
/*une autorisation globale (autorisations de l'UM $$$$) est retenue si le patient a passé au moins une journée dans une UM dont le finess géographique a été autorisé dans le fichier d'UM pour cette autorisation globale*/
/*pindiceum : indice de la premiere ligne dans tabums_source avec l'um traitée*/
void ListeAutorGlobaleValides(int pindiceum, char date_entree[], char date_sortie[], int autorV[], int *nbAutV)
{
	int indiceumspec;
	int i, j, k, id;
	char zone[3];
	char umcour[4];
	char *d1, *d2, *d3, *d4;;

	if (nbAutV == 0)
		return;

	if (pindiceum < 0)
		return;
	strncpy(umcour, tabums_source[pindiceum].um, 4);

	/*on recherche d'abord l'indice de la premiere ligne correspondant à l'UM spéciale $$$$*/
	if (indice_tabumsource$$$$ < 0)
	{
		for (i = 0; i < nbum_source; i++)
		{
			if (strncmp(tabums_source[i].um, "$$$$", 4) == 0)
			{
				indiceumspec = i;
				break;
			}
		}
		if (i == nbum_source)
			return;
		else
			indice_tabumsource$$$$ = indiceumspec;
	}
	else
		indiceumspec= indice_tabumsource$$$$;
	zone[2] = 0;
	for (i = indiceumspec; i<nbum_source; i++)
	{
		if (strncmp(tabums_source[i].um, "$$$$", 4) != 0)
			break;
		if (strncmp(tabums_source[i].datefinautor, "????????", 8) == 0)
			continue;
		strncpy(zone, tabums_source[i].autorisation, 2);
		id = atoi(zone);
		for (j = 0; j<(*nbAutV); j++)
			if (autorV[j] == id)
				break;
		if (j != (*nbAutV))
		{
			//déjà validé
			continue;
		}
		for (k = pindiceum; k<nbum_source; k++)
		{
			if (strncmp(tabums_source[k].um, umcour, 4) != 0)
				break;
			if (strncmp(tabums_source[k].datefinautor, "????????", 8) == 0)
				continue;
			if (strncmp(tabums_source[k].finessgeo, tabums_source[i].finessgeo, 9)!=0)
				continue;
			
			d1 = d2 = d3 = d4 =0;
			if (intersection_intervalles(tabums_source[k].datedebautor, tabums_source[k].datefinautor, date_entree, date_sortie, &d1, &d2))
			{
				if (intersection_intervalles(tabums_source[i].datedebautor, tabums_source[i].datefinautor, d1, d2, &d3, &d4))
				{
					autorV[*nbAutV] = id;
					(*nbAutV)++;
					if ((*nbAutV) >= 9)
						return;
					break;
				}
			}
		}
	}
	return;
}

int SejourTotalementAutorise(int pindicetabrum, char date_entreeRUM[], char date_sortieRUM[])
{
    int i,first;
    char unimed[5];
    char date_min[8];
    char premiere_autor[3];

	if (pindicetabrum < 0)
		return 0;
	strncpy(unimed, tabums_source[pindicetabrum].um, 4);
	first=1;
    for(i=pindicetabrum;i<nbum_source;i++)
    {
        if(strncmp(tabums_source[i].um,unimed,4)!=0)
            break;

        if(first ||
            date_strict_sup(date_min,tabums_source[i].datedebautor))
        {
            strncpy(date_min,tabums_source[i].datedebautor,8);
            strncpy(premiere_autor,tabums_source[i].autorisation,3);
            first=0;
        }
        if(strncmp(tabums_source[i].datefinautor,"????????",8)==0)
            continue;

        if(strncmp(tabums_source[i].autorisation,"88",2)==0)
        {
            if(interval_autorise(tabums_source[i].datedebautor,tabums_source[i].datefinautor,date_entreeRUM,date_sortieRUM))
                return 0;/*l'UM a au moins un jour dans une UM supprimée*/
        }
    }
    if((strncmp(premiere_autor,"   ",3)!=0) && (date_strict_sup(date_min,date_entreeRUM)))
        return 0;
    return 1;
}

