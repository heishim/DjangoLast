#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "fgmco.h"
#include "ctrlgenficum.h"

long  nbenr;
enum versionFG
{
	V2019,
	V2020
};

int my_index1(char s1[],char s2[])
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

void trim(char s[],char s2[])
{
   int i,j;

   j=0;
   for(i=0;(unsigned int)i<strlen(s);i++)
   {
      if(s[i]!=' ')
	  {
		  s2[j]=s[i];
		  j++;
	  }
    }
   s2[j]=0;
}

void decoupe(char s1[],char s2[],char s3[])
{
   int pos;char r[130],l[130];

    pos=my_index1(s1,"=");
    if (pos<0)
	{
		s2=0;
		s3=0;
		return;
	}
    strncpy(l,s1,pos);
	l[pos]=0;
	trim(l,s2);
    strncpy(r,s1+pos+1,strlen(s1)-pos-1);
	r[strlen(s1)-pos-1]=0;
	trim(r,s3);
}

int Test_Ver(char bufin[],char VERS[])
{
/* Permet de tester la version de RUM */
 if (strncmp(bufin+9,VERS,3)==0) return(0);
 else return(1);
 }

int trouveSlash(char *pch)
{
    int i;
    int taillech=strlen(pch);
    for(i=taillech;i>=0;i--)
    {
        if((pch[i]=='/') || (pch[i]=='\\') || (pch[i]==':'))
            break;
    }
    return i;
}

int GenereFichierUM(char *pdirtables,char *lficumin,char *lficumout,FILE *lflog, int bGestionPortee,int bgenerefinessgeo)
{
    char* msgerr_ium;
    int ret;

    if((ret=CtrlFicUM(lficumin,lficumout,pdirtables,1,bGestionPortee,bgenerefinessgeo))!=0)
    {
        switch(ret)
        {
        case 1 : msgerr_ium="impossible d'ouvrir le fichier";break;
        case 2 : msgerr_ium="impossible de créer le fichier de travail";break;
        case 3 : msgerr_ium="impossible de créer le fichier de rapport";break;
        case 4 : msgerr_ium="erreurs de format ou dates incohérentes détectées";break;
        case 5 : msgerr_ium="au moins deux dates de début d'autorisation identiques";break;
        case 6 : msgerr_ium="erreur système";break;
        case 8 : msgerr_ium="type d'autorisation non acceptee avec l'UM reservee $$$$";break;
		case 9: msgerr_ium = "fichier de références des autorisations d'UM corrompu"; break;
		case 10: msgerr_ium = "au moins deux dates de début d'autorisation identiques pour une autorisation globale au finess géographique"; break;
		case 11: msgerr_ium = "Le type d'autorisation 72 IVG n'est plus accepté au 1er mars 2019."; break;
		default: msgerr_ium = "erreur inconnue.";
        }
        fprintf(lflog,"Le fichier ficum.txt n'a pas été généré car :  %s\n",msgerr_ium);
        if(ret!=3)
            fprintf(lflog,"Voir le fichier de log : %s.log\n",lficumin);
        return 1;
    }
    return 0;
}

void genereRDS_V2019(FILE *frds, char *rum[])
{
	int lg;
	char zone[20];
	lg = strlen(rum[0]);
	/* ----------------------- Génération du RDS ----------------------- */
	/* de n°RSS à sexe */

	if (datedernregles_renseignee_2019<0)
		strncpy(zone, rum[0] + 108, 8);
	else
		strncpy(zone, rum[datedernregles_renseignee_2019] + 108, 8);
	zone[8] = 0;

	if (agegest_renseigne_2019<0)
		fprintf(frds, "%20.20s%9.9s%3.3s%3.3s%2.2s%2.2s%4.4s%3.3s%02d%03d%03d%2.2s%8.8s%c", rum[0] + 12, rum[0], "R05", "119", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, grp_res_2020.cret, grp_ent_2019.ns, rss_inf_2019.agea, rss_inf_2019.agej, rum[0] + 106, zone, *(rum[0] + 70));
	else
		fprintf(frds, "%20.20s%9.9s%3.3s%3.3s%2.2s%2.2s%4.4s%3.3s%02d%03d%03d%2.2s%8.8s%c", rum[0] + 12, rum[0], "R05", "119", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, grp_res_2020.cret, grp_ent_2019.ns, rss_inf_2019.agea, rss_inf_2019.agej, rum[agegest_renseigne_2019] + 106, zone, *(rum[0] + 70));
	/* de mode entrée à destination */
	fprintf(frds, "%c%c%2.2s%4.4s%c%c", *(rum[0] + 85), *(rum[0] + 86), rum[grp_ent_2019.ns - 1] + 87 + 2, rum[grp_ent_2019.ns - 1] + 87 + 4, *(rum[grp_ent_2019.ns - 1] + 95), *(rum[grp_ent_2019.ns - 1] + 96));
	/* durée de séjour totale et code postal */
	if (rss_inf_2019.dstot>9999)
		fprintf(frds, "9999%5.5s", rum[0] + 97);
	else
		fprintf(frds, "%4.4d%5.5s", rss_inf_2019.dstot, rum[0] + 97);
	/* poids d'entrée */
	if (lg<106)
		fprintf(frds, "%4.4s", "    ");
	else
		fprintf(frds, "%4.4s", rum[0] + 102);

	/* nbseances et nnbseances avant SROS*/
	fprintf(frds, "%02d", ghs_val_2019.nbseance);

	/* igs */
	/*sprintf(zone,"%03d",_igs_2019);*/
	fprintf(frds, "%03d", _igs_2019);
	/* de GHS à buhcd */
	fprintf(frds, "%4.4s%04d%1d%05d%1d", ghs_val_2019.ghs, ghs_val_2019.nbjxsup, ghs_val_2019.sejxinf, (int)(ghs_val_2019.nbjxinf), ghs_val_2019.buhcd);
	/* nbsuppléments réa à nbjssc */
	fprintf(frds, "%03d%03d%03d%03d%03d", ghs_val_2019.nbjrea, ghs_val_2019.nbrep, ghs_val_2019.nbjsrc, ghs_val_2019.nbjstf_issuderea, ghs_val_2019.nbjstf_tot);

	/*nb suppléments menant aux GHS 9610, 9611, 9612*/
	fprintf(frds, "%03d%03d%03d", ghs_val_2019.nbac687, ghs_val_2019.nbac688, ghs_val_2019.nbac689);

	/*nb suppléments menant aux GHS 9619 et 9620*/
	fprintf(frds, "%03d%03d", ghs_val_2019.nbprot, ghs_val_2019.nbict);

	/*nb suppléments RX1,RX2, aphérèse et RCMI*/
	fprintf(frds, "%03d%03d%03d%03d", ghs_val_2019.nbacRX1, ghs_val_2019.nbacRX2, ghs_val_2019.nbacRX3, ghs_val_2019.nbRCMI);

	/*nb suppléments GHS 9623,9625,9631,9632,9633*/
	fprintf(frds, "%03d%03d%03d%03d%03d", ghs_val_2019.nbactsupp9623, ghs_val_2019.nbactsupp9625, ghs_val_2019.nbactsupp9631, ghs_val_2019.nbactsupp9632, ghs_val_2019.nbactsupp9633);

	/*présence d'actes de prélèvement d'organes*/
	fprintf(frds, "%1d", ghs_val_2019.acprlvtorg);
	/*neonat*/
	fprintf(frds, "%03d%03d%03d", ghs_val_2019.nbjnna, ghs_val_2019.nbjnnb, ghs_val_2019.nbjnnc);
	/*GHS avant innovation*/
	fprintf(frds, "%4.4s", ghs_val_2019.ghsavantInnov);

	/*acte dialyse supplémentaires*/
	fprintf(frds, "%03d%03d%03d%03d", ghs_val_2019.nbhd, ghs_val_2019.nbent1, ghs_val_2019.nbent2, ghs_val_2019.nbent3);
	/*caissons hyperbare*/
	fprintf(frds, "%03d", ghs_val_2019.nbchypb);

	/*suppléments antepartum*/
	fprintf(frds, "%03d", ghs_val_2019.nbantepartum);
	/* supplement radiotherapie*/
	fprintf(frds, "%03d", ghs_val_2019.nbradiopedia);

	/*Indicateur de GHS minoré*/
	fprintf(frds, "%1d", ghs_val_2019.ghsminor);

	/*supplément SDC*/
	fprintf(frds, "%1d", ghs_val_2019.supplSDC);

	/*filler sur 2*/
	fprintf(frds, "  ");

	/*supplément supp2*/
	fprintf(frds, "%03d", ghs_val_2019.nbsupp_2);

	/*flag Avastin*/
	fprintf(frds, "%1d", ghs_val_2019.flagAvastin);
	/*filler*/
	fprintf(frds, " ");

	if (strncmp(grp_res_2020.cmd, "90", 2) == 0)
	{
		/* DP et DR */
		fprintf(frds, "%6.6s%6.6s", "      ", "      ");
		/* nbDAS et nb Zones d'actes */
		fprintf(frds, "0000000");
	}
	else
	{
		/* DP et DR */
		fprintf(frds, "%6.6s%6.6s", rss_inf_2019.diaglist.listdiag, rss_inf_2019.diaglist.listdiag + lgdiag_2019);
		/* nbDAS et nb Zones d'actes */
		fprintf(frds, "%02d%05d", rss_inf_2019.diaglist.nbdiag - 2, rss_inf_2019.actlist.nbac);
		/*DAS*/
		fwrite(rss_inf_2019.diaglist.listdiag + lgdiag_2019 * 2, sizeof(char), lgdiag_2019*(rss_inf_2019.diaglist.nbdiag - 2), frds);
		/*actes*/
		fwrite(rss_inf_2019.actlist.listzac, sizeof(char), 29 * rss_inf_2019.actlist.nbac, frds);
	}
	fprintf(frds, "\n");
	/* --------------------- Fin génération du RDS --------------------- */

}
void genereRDS_V2020(FILE *frds, char *rum[])
{
	int lg;
	char zone[20];
	lg = strlen(rum[0]);
	/* ----------------------- Génération du RDS ----------------------- */
	/* de n°RSS à sexe */

	if (datedernregles_renseignee_2020<0)
		strncpy(zone, rum[0] + 108, 8);
	else
		strncpy(zone, rum[datedernregles_renseignee_2020] + 108, 8);
	zone[8] = 0;

	if (agegest_renseigne_2020<0)
		fprintf(frds, "%20.20s%9.9s%3.3s%3.3s%2.2s%2.2s%4.4s%3.3s%02d%03d%03d%2.2s%8.8s%c", rum[0] + 12, rum[0], "R06", "120", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, grp_res_2020.cret, grp_ent_2020.ns, rss_inf_2020.agea, rss_inf_2020.agej, rum[0] + 106, zone, *(rum[0] + 70));
	else
		fprintf(frds, "%20.20s%9.9s%3.3s%3.3s%2.2s%2.2s%4.4s%3.3s%02d%03d%03d%2.2s%8.8s%c", rum[0] + 12, rum[0], "R06", "120", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, grp_res_2020.cret, grp_ent_2020.ns, rss_inf_2020.agea, rss_inf_2020.agej, rum[agegest_renseigne_2020] + 106, zone, *(rum[0] + 70));
	/* de mode entrée à destination */
	fprintf(frds, "%c%c%2.2s%4.4s%c%c", *(rum[0] + 85), *(rum[0] + 86), rum[grp_ent_2020.ns - 1] + 87 + 2, rum[grp_ent_2020.ns - 1] + 87 + 4, *(rum[grp_ent_2020.ns - 1] + 95), *(rum[grp_ent_2020.ns - 1] + 96));
	/* durée de séjour totale et code postal */
	if (rss_inf_2020.dstot>9999)
		fprintf(frds, "9999%5.5s", rum[0] + 97);
	else
		fprintf(frds, "%4.4d%5.5s", rss_inf_2020.dstot, rum[0] + 97);
	/* poids d'entrée */
	if (lg<106)
		fprintf(frds, "%4.4s", "    ");
	else
		fprintf(frds, "%4.4s", rum[0] + 102);

	/* nbseances et nnbseances avant SROS*/
	fprintf(frds, "%02d", ghs_val_2020.nbseance);

	/* igs */
	/*sprintf(zone,"%03d",_igs_2020);*/
	fprintf(frds, "%03d", _igs_2020);
	/* de GHS à buhcd */
	fprintf(frds, "%4.4s%04d%1d%05d%1d", ghs_val_2020.ghs, ghs_val_2020.nbjxsup, ghs_val_2020.sejxinf, (int)(ghs_val_2020.nbjxinf), ghs_val_2020.buhcd);
	/* nbsuppléments réa à nbjssc */
	fprintf(frds, "%03d%03d%03d%03d%03d", ghs_val_2020.nbjrea, ghs_val_2020.nbrep, ghs_val_2020.nbjsrc, ghs_val_2020.nbjstf_issuderea, ghs_val_2020.nbjstf_tot);

	/*nb suppléments menant aux GHS 9610, 9611, 9612*/
	fprintf(frds, "%03d%03d%03d", ghs_val_2020.nbac687, ghs_val_2020.nbac688, ghs_val_2020.nbac689);

	/*nb suppléments menant aux GHS 9619 et 9620*/
	fprintf(frds, "%03d%03d", ghs_val_2020.nbprot, ghs_val_2020.nbict);

	/*nb suppléments RX1,RX2, aphérèse et RCMI*/
	fprintf(frds, "%03d%03d%03d%03d", ghs_val_2020.nbacRX1, ghs_val_2020.nbacRX2, ghs_val_2020.nbacRX3, ghs_val_2020.nbRCMI);

	/*nb suppléments GHS 9623,9625,9631,9632,9633*/
	fprintf(frds, "%03d%03d%03d%03d%03d", ghs_val_2020.nbactsupp9623, ghs_val_2020.nbactsupp9625, ghs_val_2020.nbactsupp9631, ghs_val_2020.nbactsupp9632, ghs_val_2020.nbactsupp9633);

	/*présence d'actes de prélèvement d'organes*/
	fprintf(frds, "%1d", ghs_val_2020.acprlvtorg);
	/*neonat*/
	fprintf(frds, "%03d%03d%03d", ghs_val_2020.nbjnna, ghs_val_2020.nbjnnb, ghs_val_2020.nbjnnc);
	/*GHS avant innovation*/
	fprintf(frds, "%4.4s", ghs_val_2020.ghsavantInnov);

	/*acte dialyse supplémentaires*/
	fprintf(frds, "%03d%03d%03d%03d", ghs_val_2020.nbhd, ghs_val_2020.nbent1, ghs_val_2020.nbent2, ghs_val_2020.nbent3);
	/*caissons hyperbare*/
	fprintf(frds, "%03d", ghs_val_2020.nbchypb);

	/*suppléments antepartum*/
	fprintf(frds, "%03d", ghs_val_2020.nbantepartum);
	/* supplement radiotherapie*/
	fprintf(frds, "%03d", ghs_val_2020.nbradiopedia);

	/*Indicateur de GHS minoré*/
	fprintf(frds, "%1d", ghs_val_2020.ghsminor);

	/*supplément SDC*/
	fprintf(frds, "%1d", ghs_val_2020.supplSDC);

	/*top eligibilité gradation pour les 0j*/
	if(gradationHDJ_2020<0)
		fprintf(frds, " ");/*non concernés*/
	else
		fprintf(frds, "%1d", gradationHDJ_2020);

	/*filler sur 1*/
	fprintf(frds, " ");

	/*supplément supp2*/
	fprintf(frds, "%03d", ghs_val_2020.nbsupp_2);

	/*flag Avastin*/
	fprintf(frds, "%1d", ghs_val_2020.flagAvastin);
	/*filler*/
	fprintf(frds, " ");

	if (strncmp(grp_res_2020.cmd, "90", 2) == 0)
	{
		/* DP et DR */
		fprintf(frds, "%6.6s%6.6s", "      ", "      ");
		/* nbDAS et nb Zones d'actes */
		fprintf(frds, "0000000");
	}
	else
	{
		/* DP et DR */
		fprintf(frds, "%6.6s%6.6s", rss_inf_2020.diaglist.listdiag, rss_inf_2020.diaglist.listdiag + lgdiag_2020);
		/* nbDAS et nb Zones d'actes */
		fprintf(frds, "%02d%05d", rss_inf_2020.diaglist.nbdiag - 2, rss_inf_2020.actlist.nbac);
		/*DAS*/
		fwrite(rss_inf_2020.diaglist.listdiag + lgdiag_2020 * 2, sizeof(char), lgdiag_2020*(rss_inf_2020.diaglist.nbdiag - 2), frds);
		/*actes*/
		fwrite(rss_inf_2020.actlist.listzac, sizeof(char), 29 * rss_inf_2020.actlist.nbac, frds);
	}
	fprintf(frds, "\n");
	/* --------------------- Fin génération du RDS --------------------- */

}

/*
Argv[1]=nom du fichier de RSS à traiter
Argv[2]=répertoire des tables
Argv[3]=1 si établissement ex DGF, 2 si ex OQN
Argv[4]= nom du fichier d'autorisations d'Unités Médicales
sortie=même nom avec des suffixes sup,err,log, grp
*/
int main(int argc, char * argv[])
{
	FILE *ent, *sor, *ferr, *fsup, *flog, *frds, *fstp;

    char *rum[tab_max];
	char  bufin[31001],bufav[31001];
	int pm,dm,eof,i;
    int ret,nbsej,deb,j;
    long  nbr_mf,nbrss,nbrss_err,nbrum,nbrum_err;
    char  csj;
    char  defsuf[516],nf[520];
    int fgutilisee,lgchaine,posslash;

	/*CD modif du 12/03/04 :le main est appelé avec 3 arguments:
							1) nom du fichier RSS
							2) répertoire des tables (finissant par /)
							3) type d'établissement : 1:DGF ; 2:OQN
                            4) nom du fichier d'autorisation d'UM 
                            5) répertoire de travail(avec droits d'écriture)*/

	if(argc<6)
    {
        printf("syntaxe : %s {nom du fichier RSS} {repertoire des tables finissant par '/'} {type d'etablissement 1:DGF 2:OQN} {nom complet du fichier d'autorisation d'UM} {repertoire de travail (avec droits d'ecriture)}\n",argv[0]);
        return(1);
    }
    if (atoi(argv[3])==1) /*etablissements ex-DGF*/
    {
		grp_ent_2020.type_etab = 1;
		grp_ent_2019.type_etab = 1;
	}
    else
    if (atoi(argv[3])==2) /*etablissement ex-OQN*/
    {
		grp_ent_2020.type_etab = 2;
		grp_ent_2019.type_etab = 2;
	}
    else
    {
        /*printf("syntaxe : %s {nom du fichier RSS} {repertoire des tables finissant par '/'} {type d'etablissement 1:DGF 2:OQN} {nom complet du fichier d'autorisation d'UM}\n",argv[0]);*/
        printf("syntaxe : %s {nom du fichier RSS} {repertoire des tables finissant par '/'} {type d'etablissement 1:DGF 2:OQN} {nom complet du fichier d'autorisation d'UM} {repertoire de travail (avec droits d'ecriture)}\n",argv[0]);
        return(1);
    }

    for(i=0;i<tab_max;i++)  {  rum[i]=NULL; };

    ent=sor=ferr=fsup=flog=frds=fstp=NULL;

    nbenr=nbr_mf=nbrss=nbrss_err=nbrum=nbrum_err=0;

    strcpy(defsuf,argv[5]);
    lgchaine = strlen(defsuf);
    if((defsuf[lgchaine-1]!='/') && (defsuf[lgchaine-1]!='\\'))
        strcat(defsuf,"/");
    posslash=trouveSlash(argv[1]);
    lgchaine = strlen(argv[1]);
    if(posslash!=-1)
    {
        if(posslash!=lgchaine-1)
        {
            strcat(defsuf,argv[1]+posslash+1);
        }
    }
    else
    {
        strcat(defsuf,argv[1]);
    }

    sprintf(nf,"%s.sup",defsuf);
    if ((fsup=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }


    sprintf(nf,"%s.err",defsuf);
    if ((ferr=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }

    sprintf(nf,"%s.log",defsuf);
    if ((flog=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }

    sprintf(nf,"%s.grp",defsuf);
    if ((sor=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }

    sprintf(nf,"%s.rds",defsuf);
    if ((frds=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }

    sprintf(nf,"%s.stp.txt",defsuf);
    if ((fstp=fopen(nf,"w"))==NULL)
    {
        /*_fcloseall();*/
        return(1);
    }

    /*Initialisation des répertoires des tables binaires des fonctions groupages*/
	sprintf(grp_ent_2020.dirtab, "%s%s", argv[2], "V2020/");
	sprintf(grp_ent_2019.dirtab, "%s%s", argv[2], "V2019/");

	/*construction des fichiers d'autorisations d'UM reconnues par les FG*/
	strcpy(grp_ent_2019.ficum, argv[5]);
	lgchaine = strlen(grp_ent_2019.ficum);
	if ((grp_ent_2019.ficum[lgchaine - 1] != '/') && (grp_ent_2019.ficum[lgchaine - 1] != '\\'))
		strcat(grp_ent_2019.ficum, "/");
	strcat(grp_ent_2019.ficum, "ficum.fg2019$.txt");
	if (GenereFichierUM(grp_ent_2019.dirtab, argv[4], grp_ent_2019.ficum, flog, 1, 1))
	{
		/*_fcloseall();*/
		return 1;
	}

	strcpy(grp_ent_2020.ficum, argv[5]);
	lgchaine = strlen(grp_ent_2020.ficum);
	if ((grp_ent_2020.ficum[lgchaine - 1] != '/') && (grp_ent_2020.ficum[lgchaine - 1] != '\\'))
		strcat(grp_ent_2020.ficum, "/");
	strcat(grp_ent_2020.ficum, "ficum.fg2020$.txt");
	if (GenereFichierUM(grp_ent_2020.dirtab, argv[4], grp_ent_2020.ficum, flog, 1, 1))
	{
		/*_fcloseall();*/
		return 1;
	}


	if ((ent = fopen(argv[1], "rb")) == 0)
    {
        fprintf(flog,"Probleme ouverture %s\n",argv[1]);
        /*_fcloseall();*/
        return(1);
    }

    dm=1;
    pm=0;
    eof=0;
    nbsej=0;
    mfgets_2020(bufav,31000,ent);

    while(!eof) /*pour chaque RUM lu (les rum peuvent faire parti de RSS différents)*/
    {
        strcpy(bufin,bufav);
        nbenr++;
        if (mfgets_2020(bufav,31000,ent)==0)
        {
            eof=1;
            strcpy(bufav,"ZZZZZZZZZZZZZZZZZZZZZZZZZ");
        }
        pm=dm;
        dm=0;
        
		if (((Test_Ver(bufin, "019") != 0) && (Test_Ver(bufin, "020") != 0)) || (strlen(bufin)<130))
        {
            for(i=0;i<nbsej-1;i++)
            {
                if(rum[i]!=NULL)
                {
                    free(rum[i]);
                    rum[i]=NULL;
                }
            }
            fprintf(ferr,"N° d'enregistrement : %ld -  N° de RSS : %20.20s -> Version %3.3s  - Longueur : %d\n",nbenr,bufin+12,bufin+9,strlen(bufin));
            nbsej=0;
            nbr_mf++;
            continue;
        }
        /*Comparaison des identifiants de RSS, de manière à savoir si l'on est dans un autre RSS
        dm signale par conséquant le fait de finir le RSS  précédent */
        if(strncmp(bufin+12,bufav+12,20)!=0)
            dm=1;
        if (pm==1) /**/
        {
            nbsej=0;
        }
        /*On alloue le rum*/
        rum[nbsej]=(char *)malloc(strlen(bufin)+1);

        /*Si il n'y a pas de pbs d'allocation, on le stock*/
        if (rum[nbsej]==NULL)
        {
            fprintf(ferr,"N° de RSS : %20.20s -> Erreur FGM001 : Pb d'allocation mémoire \n",bufin+12);
            for(i=0;i<nbsej-1;i++)
            {
                free(rum[i]);
                rum[i]=NULL;
            }
            /*_fcloseall();*/
            return(1);
        }
        strcpy(rum[nbsej],bufin);

        /*On passe au séjour suivant*/
        nbsej++;

        if (dm==1) /*Avant de passer au RSS suivant, on effectue le groupage du RSS en cours*/
        {

            if((contfmt_2020(rum[nbsej-1]+87,"Z9Z99999",1,7)!=0) ||
                (contdate_2020(rum[nbsej-1]+87)!=0) || (!date_strict_sup_2020("01032020",rum[nbsej-1]+87)))
            {
                /*après 1er mars 2020 : groupage V2020*/
                for(i=0;i<nbsej;i++)
                {
                    if(Test_Ver(rum[i],"020")!=0)
                        nbr_mf++;
                    grp_ent_2020.rum2[i]=rum[i];
                }
                grp_ent_2020.ns=nbsej;
                fgutilisee=V2020;
                ret=grp_2020(grp_ent_2020,&grp_res_2020,&ghs_val_2020,&rss_inf_2020);
            }
            else
            {
                /*avant 1er mars 2020 : groupage V2019*/
                for(i=0;i<nbsej;i++)
                {
                    if(Test_Ver(rum[i],"019")!=0)
                        nbr_mf++;
                    grp_ent_2019.rum2[i]=rum[i];
                }
                grp_ent_2019.ns=nbsej;
                fgutilisee=V2019;
                ret=grp_2019(grp_ent_2019,(GRP_RESULTAT_2019*)(&grp_res_2020),&ghs_val_2019,&rss_inf_2019);
            }

            if(strncmp(grp_res_2020.cmd,"90",2)==0)
                nbrss_err++;
            else
                nbrss++;

            if(fgutilisee==V2020)
            /*V2020*/
            {
                genereRDS_V2020(frds, rum);
                /*génération du fichier .stp.txt*/
                fprintf(fstp,"%20.20s%5.5s%20.20s%8.8s\n",rum[0]+12,rss_inf_2020.sigfg,rss_inf_2020.sigrss,rss_inf_2020.sigrssghsfg);
            }
            else
            /*V2019*/
            {
                genereRDS_V2019(frds,rum);
                /*génération du fichier .stp.txt*/
                fprintf(fstp,"%20.20s%5.5s%20.20s%8.8s\n",rum[0]+12,rss_inf_2019.sigfg,rss_inf_2019.sigrss,rss_inf_2019.sigrssghsfg);
            }

            for(i=0;i<nbsej;i++)
            {
                if (strncmp(grp_res_2020.cmd,"90",2)==0 )
                {
                    nbrum_err++;
                    fprintf(ferr,"%20.20s,%4.4s,%2.2s,%4.4s,%3.3s,",rum[i]+12,rum[i]+71,grp_res_2020.cmd,grp_res_2020.ghm,grp_res_2020.cret);
                    deb=0;
                    if (i>0)
                        for(j=1;j<i+1;j++)
                            deb += grp_res_2020.ErrCtrl[j];
                    for(j=0;j<grp_res_2020.ErrCtrl[i+1];j++)
                    {
                        fprintf(ferr,"%d,",grp_res_2020.ErrCtrl[nbsej+deb+j+1]);
                    }
                    fprintf(ferr,"\n");
                    
					fprintf(sor, "%2.2s%2.2s%4.4s%2.2s%2.2s%3.3s%s\n", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, " 1", rum[i] + 10, grp_res_2020.cret, rum[i]);
                }
                else
                {
                    nbrum++;
					fprintf(sor, "%2.2s%2.2s%4.4s%2.2s%2.2s%3.3s%s\n", grp_res_2020.version, grp_res_2020.cmd, grp_res_2020.ghm, " 1", rum[i] + 10, grp_res_2020.cret, rum[i]);

                    if(fsup!=NULL)
                    {
                        csj=' ';
                        if (grp_res_2020.sejp==i)  csj='X';
                        fprintf(fsup,"%20.20s,%4.4s,%2.2s,%4.4s,%c,%d,",rum[i]+12,rum[i]+71,grp_res_2020.cmd,grp_res_2020.ghm,csj,nbsej);

                        deb=0;
                        if (i>0)
                            for(j=1;j<i+1;j++)
                                deb += grp_res_2020.ErrCtrl[j];

                        fprintf(fsup,"%d,",grp_res_2020.ErrCtrl[i+1]);
                        for(j=0;j<grp_res_2020.ErrCtrl[i+1];j++)
                            fprintf(fsup,"%d,",grp_res_2020.ErrCtrl[nbsej+deb+j+1]);
                        fprintf(fsup,"\n");
                    }
                }
            }
            if(fgutilisee==V2019)
                free_ptr_2019();
            else
                free_ptr_2020();
            for(i=0;i<nbsej;i++)
            {
                free(rum[i]);
                rum[i]=NULL;
            }
        }
    } /* fermeture du while */

    grpclose_2019();
    grpclose_2020();

    fprintf(flog,"Nombre d'enregistrements dans le fichier d'entrée   : %ld \n",nbenr);
    fprintf(flog,"Nombre d'enregistrements ayant un format non traité : %ld \n",nbr_mf);
    fprintf(flog,"Nombre de RUM traités                               : %ld \n",nbrum);
    fprintf(flog,"Nombre de RUM en erreur (CM 90)                     : %ld \n",nbrum_err);
    fprintf(flog,"Nombre de RSS traités                               : %ld \n",nbrss);
    fprintf(flog,"Nombre de RSS en erreur                             : %ld \n",nbrss_err);
    fclose(flog);
    if(fsup!=NULL)
        fclose(fsup);
    fclose(ferr);
    fclose(ent);
    fclose(frds);
    fclose(fstp);

    /*_fcloseall();*/
    return(0);
} /*main*/

