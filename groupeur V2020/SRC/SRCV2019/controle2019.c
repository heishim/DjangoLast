#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>



#include "controle2019.h"
#include "erreurs2019.h"
#include "rumfmt2019.h"
#include "ccam2019.h"

#include "gestionUM2019.h"

int offset_actes; /*pointeur sur les actes du RUM*/
int datenaissok;
char typehosp1erRUM;
int indicRAAC;
                  
/******************************************************************************
Fonction controle
Description:
Fonction contôlant la réalisation du controle d’un RSS :
	- Appel de la date système (fonction calldate)
	- Lancement de la fonction de contrôle des champs des RUMs (ctlsej_2019)
	- Lancement de la fonction de contôle OMS (fonction foms)
	- Mise à jour de CMD - GHM - Code Retour (fonction majghm)
	Last change:  MB   28 Sep 99   10:35 pm
******************************************************************************/
/*CD MAJ FG9
int controle()*/
int controle_2019(char *rums[])
{
	int i;
	age_jour_2019=age_an_2019=0;
    calldate_2019(dateptr_2019);
    poidsok_2019=1;
    datentreesejok_2019=1;
	daZ37_2019=0;
    daaccouch_2019=0;
    presenceActeAccouchement_2019=0;
    poidsnull_2019=0;
	indicRAAC = 0;
    
    for(i=0;i<nbsej_2019;i++)
	{
	    ctlsej_2019(i,rums[i]);
	}
    return(0);
} /* FIN DE LA FONCTION CONTROLE */

void ctldaterea_2019(char *rum, int nosej, int bdate_entree, int bdate_sortie)
{
	int j,ret;
    /*int nbdas_rum,nbdad_rum;
	char zone[4];*/
    int lret1,lret2;
    char *masque;
    int bacteaccouch;
    char *datereal;
    int puisi2[8]={128,64,32,16,8,4,2,1};

    /*FG11b dates de réalisation obligatoire si acte d'accouchement*/
    lret1=0;
    lret2=0;
    bacteaccouch=0;
    for(j=indacte_2019[nosej];j<indacte_2019[nosej+1];j++)
	{
		/*on n'ajoute qu'une seule erreur*/
         if(lret1 && lret2)
            break;
        masque = ptr_amask_2019+j*_prmsize_2019;
        /*bacteaccouch = ((*(masque+39) & 1) || (*(masque+14) & 8));*/
        bacteaccouch = (*(masque+41) & puisi2[6]);
        if((!lret1) && (!bacteaccouch))
        {
            datereal=rum+offset_actes+(j-indacte_2019[nosej])*29;
            ret=fcdaterea_2019(datereal,nosej,bdate_entree,bdate_sortie);
            /*FG 2016 if(ret!=0)*/
            if (ret != 0)
            {
                AddErreur_2019(CONTROLE,ret,nosej);
                lret1=1;
            }
            else
            if (strncmp(datereal, "        ", 8) == 0)
            {
                AddErreur_2019(CONTROLE, 102, nosej);
                lret1 = 1;
            }
        }
        if((!lret2) && (bacteaccouch))
        {
            presenceActeAccouchement_2019=1;
            datereal=rum+offset_actes+(j-indacte_2019[nosej])*29;
            ret=fcdaterea_2019(datereal,nosej,bdate_entree,bdate_sortie);
            if((ret!=0) || (strncmp(datereal,"        ",8)==0))
            {
                AddErreur_2019(CONTROLE,ERR_ACTEACC_OBLIGATOIRE,nosej);
                lret2=1;
            }
            else
            {
                if(nbjdateaccouch_2019<0)
                {
                    nbjdateaccouch_2019=nombre_jour_1980_2019(datereal) - nombre_jour_1980_2019(rumin_2019[0].datent)-2;
                    if(nbjdateaccouch_2019<0)
                        nbjdateaccouch_2019=0;
                }
            }
        }
	}
}

int ctlsej_2019(int nosej, char *rum)
{
    int i, j, ret, ret1, ret2, ret3;

    int ret4,ret5,ret6,ret7;
    int indiced;

    char *masque;
    int jj,mm,aa;
    char datetemp[9];

    int actesAutorisantGC[NBMAX_ACTES_AUTORISANT_GC_2019]; /*actes autorisant des gestes complémentaires du RUM*/
    int nbactesAutorisantGC;/*nombre d'actes autorisant des gestes complémentaires du RUM*/
    int gestesCompl[NBMAX_GC_2019];/*gestes complémentaires à contrôler du RUM*/
    int nbGestesCompl; /*nombre gestes complémentaires à contrôler du RUM*/

    char tmpacte[5];
    int acteyyyy;
    int erractformatactiv;/*sert à n'ajouter qu'une seule erreur pour la valeur de l'activité de l'acte*/
    int erractextdocAnesth;/*sert à n'ajouter qu'une seule erreur d'absence d'ext documentaire pour les actes d'anesthésie*/
    char extdoc;


    /*CD FG13*/
    struct actesDedoublonnes
    {
        char acte[8];
        int cptactiv;
    };
    struct actesDedoublonnes tabActesRUMdedoublonnes[999];
    int nbActesRUMdedoublonnes,typactivite;
    int bcontroleActivite;
    int puisi2[8]={128,64,32,16,8,4,2,1};
    char zone[4];
    int nbdas_rum,nbdad_rum;

    strncpy(zone,rumin_2019[nosej].nbds,2);zone[2]=0;
    nbdas_rum=atoi(zone);
	nbdad_rum=GetNombreDADNum_2019(rum,-1);
	offset_actes = 177+(nbdas_rum+nbdad_rum)*8;
    
    ret=fcnf_2019(rumin_2019[nosej].finess); /* contrôle du n° FINESS */
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcnr_2019(rumin_2019[nosej].numrss ); /* Contrôle du n°de RSS */
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=ret2=fcde_2019(rumin_2019[nosej].datent); /* Contrôle de la date d'entrée */
    ret2=(ret!=0 && ret<60);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    if((nosej==0) && ret2)
        datentreesejok_2019=0;

    datenaissok = 0;
    ret = fcdn_2019(rumin_2019[nosej].naiss, ret2); /* fonction de contrôle de la date de naissance */
    ret1=(ret!=0 && ret<60);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcsx_2019(rumin_2019[nosej].sexe); /*fonction de contrôle du sexe */
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcus_2019(rumin_2019[nosej].unmed); /*@@ contrôle du code unité de soins @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
	ret7=ret=fcme_2019(rumin_2019[nosej].modent); /*@@ fonction de contrôle du mode d'entrée @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcpr_2019(rumin_2019[nosej].prov,rumin_2019[nosej].modent,nosej); /*@@ fonction de contrôle du mode de provenance @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcds_2019(rumin_2019[nosej].datsor,ret2,nosej); /*@@ fonction de contrôle de la date de sortie (vérification de cohérence uniquement) @@*/
    ret3=(ret!=0 && ret<60);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcms_2019(rumin_2019[nosej].modsor); /*@@ Contrôle du mode de sortie (contrôle de cohérence uniquement) @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fcdst_2019(rumin_2019[nosej].dest,rumin_2019[nosej].modsor,nosej); /*@@ Fonction de contrôle de a destination @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    if (!(ret2 || ret3))
        ret=fc24_2019(rumin_2019[nosej].sej_24,nosej); /*@@ Contrrôle de durée <= 24 H @@*/

    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    ret=fccp_2019(rumin_2019[nosej].codpost); /*@@ Fonction de contrôle du code postal @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    
    /* on a besoin de l'âge pour les tests sur les diagnostics et age gestationnel*/ 
    if(nosej==0 && !(ret1 || ret2))
    {
        callage_2019(rumin_2019[nosej].datent,rumin_2019[nosej].naiss);
        if (age_jour_2019<0)
            AddErreur_2019(CONTROLE,15,nosej);
		else
		if ((!ret7) && (rumin_2019[0].modent == 'N' || rumin_2019[0].modent == 'n'))
		{
			if(age_jour_2019 != 0 || age_an_2019 != 0)
				AddErreur_2019(CONTROLE, ERR_INCOMPATIB_AGEMODENTREEN, nosej);
		}
    }
    
    ret=fcpn_2019(rumin_2019[nosej].pdsnaiss,nosej); /*@@ fonction de contrôle du poids de naissance @@*/
    if(ret!=0)
    {
        AddErreur_2019(CONTROLE,ret,nosej);

    }
    ret=fcse_2019(rumin_2019[nosej].nbseance); /*@@ Fonction de controle du nombre de séances @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    

    ret=fcph_2019(rumin_2019[nosej].hosp30);   /*@@ Fonction de controle du format du champ hosp dans les 30 jours @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    ret=fcnbd_2019(rumin_2019[nosej].nbds);    /*@@ Fonction de contrôle du nombre de diagnostiques @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    ret=fcnbd_2019(rum+120);    /*@@ Fonction de contrôle du nombre de DAD @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);


    ret=fcnba_2019(rumin_2019[nosej].nba);     /*@@ Fonction de contrôle du nombre d'actes @@*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    /*CD FG11*/
    /*confirmation de codage*/
    ret=fcconfcodage_2019(rumin_2019[nosej].ccodage);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    /*type de machine en radiothérapie*/
    ret=fctmr_2019(rumin_2019[nosej].typemachine_radioth);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    /*type de dosimétrie*/
    ret=fctypedos_2019(rumin_2019[nosej].typedosimetrie);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);
    
    /*nombre de faisceaux*/
    /*supprimé en FG13
    ret=fcnbfaisc_2019(rumin_2019[nosej].nbfaisc);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);*/

    /*CD FG13 date des dernières règles*/
    ret=fcdatedernregles_2019(nosej,rumin_2019[nosej].datedernregles);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

	/*fg 2019 : conversion HP->HC*/
	fcconvHPHC_2019(nosej, rumin_2019[nosej].conversionHPHC);
	
	/*fg 2019 : indicateur de séjour RAAC*/
	fcindicRAAC_2019(nosej, rumin_2019[nosej].priseenchargeRAAC);

	for(i=inddiag_2019[nosej];i<inddiag_2019[nosej+1];i++) /*@@ Boucle de contrôle sur les diagnostiques @@*/
    {
        if(i==inddiag_2019[nosej])
            /*DP*/
            indiced=1;
        else
            if(i==inddiag_2019[nosej]+1)
                /*DR*/
                indiced=2;
            else
                /*DAS*/
                indiced=0;

        if((daZ37_2019==0) && (indiced==0))
        {
            if((strncmp(ptr_diags_2019+i*6,"Z37",3)==0) || (strncmp(ptr_diags_2019+i*6,"z37",3)==0))
                daZ37_2019=1;
        }
        if((daaccouch_2019==0) && (indiced==0))
        {
            if(((strncmp(ptr_diags_2019+i*6,"O80",3)==0) || (strncmp(ptr_diags_2019+i*6,"o80",3)==0)) ||
                ((strncmp(ptr_diags_2019+i*6,"O81",3)==0) || (strncmp(ptr_diags_2019+i*6,"o81",3)==0)) ||
                ((strncmp(ptr_diags_2019+i*6,"O82",3)==0) || (strncmp(ptr_diags_2019+i*6,"o82",3)==0)) ||
                ((strncmp(ptr_diags_2019+i*6,"O83",3)==0) || (strncmp(ptr_diags_2019+i*6,"o83",3)==0)) ||
                ((strncmp(ptr_diags_2019+i*6,"O84",3)==0) || (strncmp(ptr_diags_2019+i*6,"o84",3)==0)))
                daaccouch_2019=1;
        }
        
        ret=fcdg10_2019(ptr_diags_2019+i*6,ptr_dmask_2019+i*_dxmsize_2019,indiced,nosej);
        if(ret!=0)
            AddErreur_2019(CONTROLE,ret,nosej);

        if(strncmp(ptr_diags_2019+i*6,"      ",6)!=0)
            ControleOMS_2019(nosej,ptr_omask_2019+i*_prfsize_2019,rumin_2019[nosej].sexe,age_jour_2019,age_an_2019,indiced);
    }                


    nbactesAutorisantGC=0;
    nbGestesCompl=0;
    
    erractformatactiv=0;
    erractextdocAnesth=0;

    nbActesRUMdedoublonnes=0;
    bcontroleActivite=1;
    for(i=indacte_2019[nosej];i<indacte_2019[nosej+1];i++) /*@@ @@@@@@@@ Boucle de contrôle sur les actes @@@@@@@@ @@*/
    {
        masque = ptr_amask_2019+i*_prmsize_2019;
        ret4=fcaccam_2019(ptr_actes_2019+i*lgacte_2019,masque);
        if(ret4!=0)
        {
            AddErreur_2019(CONTROLE,ret4,nosej);
            bcontroleActivite=0;
        }
        
        ret=fcnbra_2019(ptr_nbactes_2019+i*4);
        if(ret!=0)
        {
            AddErreur_2019(CONTROLE,ret,nosej);
            /*bcontroleActivite=0;*/
        }
        /*FG 11e controle de l'extension documentaire*/
        extdoc=*(rum+offset_actes+(i-indacte_2019[nosej])*29+20);
        if(extdoc!=' ')
        {
            if(contfmt_2019((&extdoc),"A",0,1)!=0)
                AddErreur_2019(CONTROLE,ERR_ACTECCAM_EXTDOC_ERRONEE,nosej);
            else
                if((toupper(extdoc)=='O') || (toupper(extdoc)=='I'))
                    AddErreur_2019(CONTROLE,ERR_ACTECCAM_EXTDOC_ERRONEE,nosej);
        }

        /*FG 11g controle de l'extension ATIH (code descriptif)*/
        if (!ret4)
        {
            ret = fccodedesc_2019(nosej,rum + offset_actes + (i - indacte_2019[nosej]) * 29 + 8);
            if (ret != 0)
                AddErreur_2019(CONTROLE, ret, nosej);
        }
        /*FG 13.11d Incompatibilité acte-sexe*/
        if ((*(masque+43) & puisi2[1]) && (rumin_2019[nosej].sexe=='2'))
            AddErreur_2019(CONTROLE,ERR_INCOMPATIB_ACTE_SEXE,nosej);
        /*FG 13.11d Incompatibilité acte-age*/
        /*if (((age_an_2019!=0) || (age_jour_2019>=29)) && (*(masque+44) & puisi2[2]))*/
        if (((age_an_2019!=0) || (age_jour_2019>=29)) && (*(masque+44) & puisi2[2]) && (poidsnull_2019 || (_pdsn_2019>=3000)))
            AddErreur_2019(CONTROLE,ERR_INCOMPATIB_ACTE_AGE,nosej);
        
        tmpacte[4]=0;
        strncpy(tmpacte,ptr_actes_2019+i*lgacte_2019,4);
        /*CD MAJ FG10 pour les actes en YYYY, on controle uniquement le format*/
        acteyyyy = ((toupper(tmpacte[0])=='Y') && (toupper(tmpacte[1])=='Y') && (toupper(tmpacte[2])=='Y')
            && (toupper(tmpacte[3])=='Y'));

        if((!acteyyyy) && (!ret4))
        {
            ret5=fcdataccam_2019(*(ptr_datdeb_2019+i),*(ptr_datfin_2019+i),rumin_2019[nosej].datent,rumin_2019[nosej].datsor,ret2,ret3);
            if(ret5!=0)
                AddErreur_2019(CONTROLE,ret5,nosej);
        }
        /*CD MAJ FG10*/
        /*contrôles du type d'activité de l'acte*/
        /*CD on ne teste pas l'activité des actes en YYYY
        ret=fcactivite_2019(i,ret4);*/
        ret6=fcactivite_2019(i,ret4 || acteyyyy);
        if(ret6!=0)
        {
            /*si erreur 103 : on ne l'ajoute qu'une seule fois*/
            if(ret6==103)
            {
                bcontroleActivite=0;
                if(erractformatactiv==0)
                {
                    AddErreur_2019(CONTROLE,ret6,nosej);
                    erractformatactiv=1;
                }
            }
            else
                AddErreur_2019(CONTROLE,ret6,nosej);
        }
        else
        {
            /*remplissage des tableaux des actes autorisant les gestes complémentaires*/
            /*et des gestes complémentaires à contrôler*/
            if((!ret4) && (!acteyyyy))
                fprinccompl_2019(actesAutorisantGC,&nbactesAutorisantGC,gestesCompl,&nbGestesCompl,masque,i);
            
            /*FG 13.11e extension documentaire non renseignée pour un acte CCAM d'anesthesie*/
            if(erractextdocAnesth==0)
            {
                if((!ret4) && (!acteyyyy))
                {
                    if((*(ptr_typact_2019+i))=='4')
                    {
                        /*extension documentaire non renseignée*/
                        if(extdoc==' ')
                        {
                            AddErreur_2019(CONTROLE,ERR_ACTECCAM_ANESTH_EXTDOC_ABS,nosej);
                            erractextdocAnesth=1;
                        }
                    }
                }
            }
        }
        
        /*CD FG13*/
        if(bcontroleActivite)
        {
            for(j=0;j<nbActesRUMdedoublonnes;j++)
            {
                if(strncmp(ptr_actes_2019+i*lgacte_2019,tabActesRUMdedoublonnes[j].acte,8)==0)
                    break;
            }
            typactivite=*(ptr_typact_2019+i)-'0';
            if(j>=nbActesRUMdedoublonnes)
            {
                /*il faut qu'il y ait au moins une zone d'acte avec activité 1 pour un ensemble de même actes avec activités différentes de 1*/
                if ( (acteyyyy) || /*on exclut les actes YYYY du controle*/
                     (   /*(  ((typactivite==4) && (!ret6)) || ((typactivite==5) && (!ret6))
                         
                         )*/
                         (!ret6) /*activité autorisée pour cet acte*/
                         && (typactivite!=1)
                         && (*(masque+42) & puisi2[6]) /*si l'acte a une activite 1 "fictive"*/
                     )
                   )
                {
                    /*rien*/
                }
                else
                {
                    strncpy(tabActesRUMdedoublonnes[nbActesRUMdedoublonnes].acte,ptr_actes_2019+i*lgacte_2019,8);
                    if(typactivite==1)
                        tabActesRUMdedoublonnes[nbActesRUMdedoublonnes].cptactiv=1;
                    else
                        tabActesRUMdedoublonnes[nbActesRUMdedoublonnes].cptactiv=0;
                    nbActesRUMdedoublonnes++;
                }
            }
            else
            {
                /*l'acte est déjà mémorisé*/
                if((tabActesRUMdedoublonnes[j].cptactiv!=1) && (typactivite==1))
                    tabActesRUMdedoublonnes[j].cptactiv=1;
            }
        }
    }

    /*contrôle sur activité : un acte avec activité autre que 1 doit avoir le même acte avec activite 1*/
    if(bcontroleActivite)
    {
        for(i=0;i<nbActesRUMdedoublonnes;i++)
        {
            if(tabActesRUMdedoublonnes[i].cptactiv==0)
            {
                AddErreur_2019(CONTROLE,ERR_ACTIVITEPRINCACTE_ABSENT,nosej);
                break;
            }
        }
    }


    /*CD FG10c fichier d'UM obligatoire
    ret=fcum_11b(rumin_11b[nosej].typum);
    if(ret!=0)
    AddErreur_11b(CONTROLE,ret,nosej);*/

    /*CD FG13*/
    controle_refs_um_2019(nosej);

    ret=fclitded_2019(rumin_2019[nosej].litded);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    /*contrôle de la date de réalisation des actes*/
    ctldaterea_2019(rum,nosej,ret2,ret3);

    /*CD FG11b age gestationnel*/
    ret=fcagegest_2019(nosej,rumin_2019[nosej].agegest,rumin_2019[nbsej_2019-1].modsor);
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    if(nosej==nbsej_2019-1)
    {
        /*age gestationnel requis*/
        if(agegest_renseigne_2019<0)
        {
             /*pour les nouveaux nés*/
            if (strncmp(rumin_2019[0].datent,rumin_2019[0].naiss,8)==0)
                AddErreur_2019(CONTROLE,ERR_AG_ABSENT,nosej);
             /*à l'accouchement*/
             if((presenceActeAccouchement_2019) || daZ37_2019)
                 AddErreur_2019(CONTROLE,ERR_AG_ABSENT,nosej);
        }
        else
        {
            /*FG 4.6 : si acte d'accouchement et age gestationnel<22SA=>erreur bloquante*/
            if(presenceActeAccouchement_2019)
            {
                strncpy(zone,rumin_2019[agegest_renseigne_2019].agegest,2);
                zone[2]=0;
                if(atoi(zone)<22)
                    AddErreur_2019(CONTROLE,ERR_AG_FAIBLE_ACCOUCHEMENT,nosej);
            }
        }
        if (datedernregles_renseignee_2019<0)
        {
            if((presenceActeAccouchement_2019) || daZ37_2019)
                AddErreur_2019(CONTROLE,ERR_DATEDERNREGLES_ABSENTE,nosej);
        }
        else
        {
            if(strncmp(&rumin_2019[0].sexe,"1",1)==0)
                AddErreur_2019(CONTROLE,ERR_DATEDERNREGLES_SEXEINCOMPATIBLE,nosej);

            if(datentreesejok_2019)
            {
                if(date_strict_sup_2019(rumin_2019[datedernregles_renseignee_2019].datedernregles,rumin_2019[0].datent))
                    AddErreur_2019(CONTROLE,ERR_DATEDERNREGLES_SUPDATEENT,nosej);
                else
                {
                    Soustraire_jour_date_2019(rumin_2019[0].datent,310,&jj,&mm,&aa);
                    sprintf(datetemp,"%02d%02d%04d",jj,mm,aa);
                    if(date_strict_sup_2019(datetemp,rumin_2019[datedernregles_renseignee_2019].datedernregles))
                        AddErreur_2019(CONTROLE,ERR_DATEDERNREGLES_TROPANCIENNE,nosej);
                }
            }
        }
    }

    ret=fcigsrum_2019(rumin_2019[nosej].igs);/*controle de l'IGS du RUM*/
    if(ret!=0)
        AddErreur_2019(CONTROLE,ret,nosej);

    /*FG10*/
    /*contrôles des couples gestes complémentaires - actes principaux*/
    ret=fccouplesact_2019(actesAutorisantGC,&nbactesAutorisantGC,gestesCompl,&nbGestesCompl);
    if(ret>0)
        AddErreur_2019(CONTROLE,ret,nosej);
    if(ret==-1)
        AddErreur_2019(IMPLEMENTATION,3,nosej);
    
    return(0);
}; /* FIN DE LA FONCTION ctlsej_2019 */



/********************************************/
/* FONCTION DE CONTROLE DU NUMERO DE FINESS */
/********************************************/
int fcnf_2019 (char finess[])
{
	int ret;

	if((ret=contfmt_2019(finess,"9A9999999",0,9))!=0)
    {
        /*CD MAJ FG10 
        switch(ret)
        {
            case 1 : return(75);
            case 2 : return(75);
            case 3 : return(76);
        }
        */
        switch(ret)
        {
            case 1 :
            case 2 : 
            case 3 : return(76);
        }
    }

/* 	if (nbsej_2019>1)
		if(strncmp(finess,rumin_2019[0].finess,9)!=0)
			return(4);  */
return(0);
}
/*****************************************/
/* FONCTION DE CONTROLE DU NUMERO DE RSS */
/*****************************************/
int	fcnr_2019 (char numrss[])
{
	int ret;

	/*CD fg10c*/
    /*if((ret=contfmt_2019(numrss,"9999999",1,0))!=0)*/
    if((ret=contfmt_2019(numrss,"YYYYYYYYYYYYYYYYYYYY",1,0))!=0)

          switch(ret) {
             case 1 : return(11);
             case 2 : return(60);
             case 3 : return(61);
             }

	if (nbsej_2019>1)
	/*CD fg10c*/
    /*if(strncmp(numrss,rumin_2019[0].numrss,7)!=0)*/
	/*if(strncmp(numrss,rumin_2019[0].numrss,sizeof(rumin_2019[0].numrss))!=0)*/
    if(strncmp(numrss,rumin_2019[0].numrss,20)!=0)
		return(10);

return(0);
} /* fin de la fonction fcnr */

/************************************************/
/* FONCTION DE CONTROLE DE LA DATE DE NAISSANCE */
/************************************************/

int   fcdn_2019 (char zone[],int ret2)
{
	int annai,ancour,ret;
	char dz[5];

	if((ret=contfmt_2019(zone,"Z9Z99999",1,7))!=0)
	{
          switch(ret) {
             case 1 : return(13);
             case 2 : return(14);
             case 3 : return(14);
             }
	}

	if(contdate_2019(zone)!=0)
		return(39);
    datenaissok = 1;
    if (!ret2)
	{
      strncpy(dz,zone+4,4);
      dz[4]=0;
      annai = atoi(dz);
      strncpy(dz,rumin_2019[0].datent+4,4);
      dz[4]=0;
      ancour = atoi(dz);
      if(annai > ancour || annai < ancour-140)
		  return(15);
    }

    if (nbsej_2019>1)
    {
        if(strncmp(zone,"0101",4)!=0 && strncmp(rumin_2019[0].naiss,"0101",4)!=0)
        {
            if(strncmp(zone,rumin_2019[0].naiss,8)!=0)
                return(45);
        }
        else
        {
            if(strncmp(zone+4,rumin_2019[0].naiss+4,4)!=0) return(45);
        }
    }
   return(0);

} /* fin de la fonction fcdn */

/*************************************/
/* FONCTION DE CONTROLE DU CODE SEXE */
/*************************************/

int	fcsx_2019 (char zone)
{
        int ret;

	/* CODE SEXE ABSENT */
	if((ret=contfmt_2019(&zone,"9",0,1))!=0) return(16);

	/* VALEUR ERRONEE DU CODE SEXE */
	if(contval_2019(&zone,"12",1)==0) return(17);

	if (nbsej_2019>1)
		if(strncmp(&zone,&rumin_2019[0].sexe,1)!=0) return(46);

return(0);
} /* fin de la fonction fcsx */

/***********************************************/
/* FONCTION DE CONTROLE DU CODE UNITE DE SOINS */
/***********************************************/

int	fcus_2019 (char zone[])
{

    int ret;

	if((ret=contfmt_2019(zone,"ZZZZ",3,0))!=0)
          switch(ret) {
             case 1 : return(62);
             case 2 : return(62);
             /*CD 30/01/2006 
             case 2 : return(63);*/
             }

return(0);
} /* fin de la fonction fcus */

/********************************************/
/* FONCTION DE CONTROLE DE LA DATE D'ENTREE */
/********************************************/

int	fcde_2019 (char zone[])
{
    int ret;
    char datang[8];

	if((ret=contfmt_2019(zone,"Z9Z99999",1,7))!=0)
	{
          switch(ret) {
             case 1 : return(19);
             case 2 : return(19);
             case 3 : return(20);
             }
	}
	if(contdate_2019(zone)!=0) return(21);

        strncpy(datang,zone+4,4);
        strncpy(datang+4,zone+2,2);
        strncpy(datang+6,zone,2);
	if(strncmp(datang,dateptr_2019,8) > 0)
        return(64);
        if(strncmp(zone+4,"1984",4)<=0) return(77);
return(0);
} /* fin de la fonction fcde */

/*****************************************/
/* FONCTION DE CONTROLE DU MODE D'ENTREE */
/*****************************************/

int   fcme_2019 (char zone)
{ /*@@ zone est le mode d'entrée @@*/
    int ret;

	/* MODE D'ENTREE ABSENT */
	/* FG V2019 : mode d'entrée N */
	/* if((ret=contfmt_2019(&zone,"9",3,1))!=0)*/
	if ((ret = contfmt_2019(&zone, "A", 3, 1)) != 0)
	{
          switch(ret) {
             case 1 : return(24);
             case 2 : return(24);
             }
	}
return(0);
} /* fin de la fonction fcme */

/*********************************************/
/* FONCTION DE CONTROLE DE LA PROVENANCE     */
/*********************************************/
int fcpr_2019 (char prov,char modent,int nosej)
{
    int me,mp;

    if(modent=='6' || modent=='7' || modent=='0')
        if(prov==' ')
            return(53);

    me=modent-'0';
    mp=prov-'0';
    if (prov==' ') mp= -1;
    switch(me)
    {
    case 0 :
    case 6 :
    case 7 : 
        switch(mp)
        {
        case 1 :
        case 2 :
        case 3 :
        case 4 :  
        case 6 :
        case 34: /*'R'*/
        case 66: /*'r'*/
            if((me==0) && (mp==6))
                return 25;
            if((me==6) && ((mp==34) || (mp==66)) )
                return 25;
            break;
            /*if(!((me==0) && (mp==6)))
                break;*/
        default : return(25);
        }
        break;
    case 8 : 
        switch(mp)
        {
            case -1 :
                /*
                case 1 :
                case 2 :
                */
            case 5 :
                /*CD FG11 case 6 :*/
            case 7 :  break;
            default : return (25);
        }
             break;
	case ('N' - '0'): /*Naissance à l'hopital*/
	case ('n' - '0'):
		switch (mp)
		{
		case -1: break;
		default: return (25);
		}
		break;
	default: return(25);
    }

/* controle general mode entree et provenance */
/* Mode entree ou provenance incorrecte si premier RUM */
    if(nosej==0)
    {
        if ((me==6 && mp==1)) return(26);
        if (me==0)
        {
            if (rumin_2019[nbsej_2019-1].modsor!='0') return(26);
        }
        else if (rumin_2019[nbsej_2019-1].modsor=='0') return(26);
    }
    else
    {
        if (!((me==6 && mp==1) || (me==0))) return(27);
        if (me==0)
        {
            if (rumin_2019[nosej-1].modsor!='0') return(27);
            else if (nombre_jour_1980_2019(rumin_2019[nosej].datent) - nombre_jour_1980_2019(rumin_2019[nosej-1].datsor) > 1 )return(50);
        }
        else if (rumin_2019[nosej-1].modsor=='0') return(27);
    }
    return(0);
}

/*********************************************/
/* FONCTION DE CONTROLE DE LA DATE DE SORTIE */
/*********************************************/

int	fcds_2019 (char zone[],int ret2,int nosej)
{
    int ret;
    char  datang[8];
    if((ret=contfmt_2019(zone,"Z9Z99999",1,7))!=0)
    {
        switch(ret) {
                case 1 : return(28);
                case 2 : return(29);
                case 3 : return(29);
        }
    }
    if(contdate_2019(zone)!=0) return(30); /*@@ vérifie si le format de la date est cohérent @@*/

    if(!ret2)
    {
        if(nombre_jour_1980_2019(rumin_2019[nosej].datsor) -
            nombre_jour_1980_2019(rumin_2019[nosej].datent) < 0) return(32); /*@@ vérifie si la date de sortie est postérieur à la date d'entrée @@*/
        if(nosej>0)
        {
            if(rumin_2019[nosej-1].modsor!='0' || rumin_2019[nosej].modent!='0')
                if(strncmp(rumin_2019[nosej-1].datsor,rumin_2019[nosej].datent,8)!=0) return(23);
        }
    }
    strncpy(datang,zone+4,4); /*@@ écriture de dates au format annee mois jourspour une comparaison texte @@*/
    strncpy(datang+4,zone+2,2);
    strncpy(datang+6,zone,2);
    if(strncmp(datang,dateptr_2019,8) > 0) /*@@ comparaison de dates @@*/
        return(65);


    return(0);
} /* fin de la fonction fcds */


/******************************************/
/* FONCTION DE CONTROLE DU MODE DE SORTIE */
/******************************************/
int   fcms_2019 (char zone)
{
    int ret;
/*@@ Ici, ajouter le mode de sortie 0 @@*/
	/* MODE DE SORTIE ABSENT */
	if((ret=contfmt_2019(&zone,"9",3,1))!=0)
	{
          switch(ret) {
             case 1 : return(33);
             case 2 : return(33);
             }
	}
return(0);

} /* fin de la fonction fcms */

/*********************************************/
/* FONCTION DE CONTROLE DE LA DESTINATION     */
/*********************************************/

/*@@ à modifier éventuellement @@*/
int fcdst_2019 (char dest,char modsor,int nosej)
{
    int ms,md;

    if(modsor=='6' || modsor=='7' || modsor=='0')
        if(dest==' ') return(54);

    ms=modsor-'0';
    md=dest-'0';
    if (dest==' ') md= -1 ;
    switch(ms)
    {
    case 0 :
    case 6 :
    case 7 : switch(md)
             {
                case 1 :
                case 2 :
                case 3 :
                case 4 :
                /*CD FG11*/
                case 6 :
                /*CD FG11b*/
                    if(!((ms==0) && (md==6)))
                        break;
                default : return(34);
             }
             break;
    case 8 : switch(md)
             {
                case -1 :
                /*FG11b case 5 :*/
                /*CD FG11 case 6 :*/
                case 7 : break;
                default : return(34);
             }
             break;
    case 9 : if(dest!=' ')return(34);
        break;
    default: return(34);
}

/* controle general mode sortie et destination */
/* Mode sortie ou destination incorrecte si dernier RUM */

    if(nosej==nbsej_2019-1)
    {
        if (ms==6 && md==1) return(35);
        if (ms==0)
        {
            if (rumin_2019[0].modent!='0') return(35);
            else if (nombre_jour_1980_2019(rumin_2019[nbsej_2019-1].datsor) - nombre_jour_1980_2019(rumin_2019[0].datent) > 1 ) return(50);
        }
        else if (rumin_2019[0].modent=='0') return(35);
    }
    else
    {
        if (!((ms==6 && md==1) || (ms==0))) return(49);
        if (ms==0)
        {
            if (rumin_2019[nosej+1].modent!='0') return(49);
        }
        else
            if (rumin_2019[nosej+1].modent=='0')return(49);
    }

return(0);
}

/*********************************************/
/* FONCTION DE CONTROLE SEJOUR INF A 24 H    */
/*********************************************/
int fc24_2019 (char sej24,int nosej)
{
	/*@@ Sera utilisée pour calculer si la durée des actes fantômes @@*/

    if(nombre_jour_1980_2019(rumin_2019[nosej].datsor)
		- nombre_jour_1980_2019(rumin_2019[nosej].datent) > 1)
		return(0);
    if (nosej==nbsej_2019-1)
		if (contval_2019(&sej24," 01",1)==0)
			return(78);
	return(0);
}
/*********************************************/
/* FONCTION DE CONTROLE CODE POSTAL          */
/*********************************************/
int fccp_2019(char codep[])
{
    int ret;
    if ((ret = contfmt_2019(codep, "99999", 0, 5)) != 0)
    {
        switch (ret) {
        case 1: return(ERR_CODE_POSTAL_NONRENSEIGNE);
        case 2: return(ERR_CODE_POSTAL_NONRENSEIGNE);
        case 3: return(81);
        }
    }
    return(0);
}
/*********************************************/
/* FONCTION DE CONTROLE PDS DE NAISSANCE     */
/*********************************************/
int fcpn_2019 (char pdsn[],int nosej)
{
    int ret,j;
    char zone[5];
    int ipn;

    if(nosej==0)
    {
        if((ret=contfmt_2019(pdsn,"9999",1,3))!=0)
        {
            poidsok_2019=0;
            if (ret==3)return(82);
            /*CD FG11b*/
            if (ret==2)
            {
                for(j=0;j<4;j++)
                {
                    if((pdsn[j]!='0') && (pdsn[j]!=' '))
                        return(ERR_POIDS_NBCARAC);
                }
            }
        }
        strncpy(zone,pdsn,4);
        zone[4]=0;
        ipn=atoi(zone);
        if(ipn==0)
            poidsnull_2019=1;
        
        /*CD FG 13.11f*/
        if(strncmp(pdsn,"    ",4)!=0)
        {
            _pdsn_2019=ipn;
        }
        else
            _pdsn_2019=0;
        /**/
        
        if(poidsnull_2019 && (age_an_2019==0) && (age_jour_2019<29))
            return(ERR_POIDS_REQUIS);
        
        if((ipn>0) && (ipn<100))
            return(ERR_POIDS_NBCARAC);
    }
    return(0);
}
/*********************************************/
/* FONCTION DE CONTROLE DU NOMBRE DE SEANCES */
/*********************************************/
int	fcse_2019 (char seance[])
{
   int ret;
   char dz[3];
   /* VALEUR ERRONEE DU NOMBRE DE SEANCES */
   if((ret=contfmt_2019(seance,"99",3,0))!=0)
       if(ret==3)  return(36);

   /* SEANCES SUR UN RSS MULTI-SEJOUR */
   strncpy(dz,seance,2);dz[2]=0;
   if(nbsej_2019>1 && atoi(dz)!=0)
       return(37);
   if(atoi(dz)<0 || atoi(dz)>31)return(66);
   return(0);

} /* fin de la fonction fcse */


/*********************************************/
/* FONCTION DE CONTROLE DU NOMBRE DE DIAGS */
/*********************************************/

int fcnbd_2019(char nbd[])
{
  int ret,Resultat;
  char TPS[3];
  Resultat=0;

  if ((ret=contfmt_2019(nbd,"99",1,0))!=0)
  {
      switch(ret) {
             case 1 :
                 Resultat=55;
                 break;
             case 3 :
                 Resultat=56;
                 break;
      };
  }
  else
  {
      TPS[0]=nbd[0];
      TPS[1]=nbd[1];
      TPS[2]=0;
  };
 return Resultat;
}
/*********************************************/
/* FONCTION DE CONTROLE DU NOMBRE DE ZONES D'ACTES */
/*********************************************/

int fcnba_2019(char nba[])
{

  int ret;
	/*CD FG 11
    if ((ret=contfmt_2019(nba,"99",1,0))!=0)*/
    if ((ret=contfmt_2019(nba,"999",1,0))!=0)
	{
          switch(ret) {
             case 1 : return(57);
             case 3 : return(58);
             }
	}
return(0);
}
/***********************************************************/
/* FONCTION DE CONTROLE DU CODE DE CONFIRMATION DE CODAGE */
/***********************************************************/
int fcconfcodage_2019(char pccod)
{
    if(contval_2019(&pccod," 12",1)==0)
        return(121);
    return(0);
}
/*******************************************************************/
/* FONCTION DE CONTROLE DU CODE DE TYPE DE MACHINE EN RADIOTHERAPIE*/
/*******************************************************************/
int fctmr_2019(char pccod)
{
    if(contval_2019(&pccod," 1234",1)==0) return(122);
    return(0);
}
/*****************************************************/
/* FONCTION DE CONTROLE DU CODE DE TYPE DE DOSIMETRIE*/
/*****************************************************/
int fctypedos_2019(char pccod)
{
    if(contval_2019(&pccod," 1234",1)==0) return(123);
    return(0);
}

/************************************************************/
/* FONCTION DE CONTROLE DE DIAGNOSTIC CONTROLE CIM10           */
/************************************************************/
int fcdg10_2019 (char *diag ,char *masque,int fdp,int nosej)
{
	char TmpDiag,TmpChar; /*@@ pour optimiser un peu @@*/
    int dxc;
    int puisi2[8]={128,64,32,16,8,4,2,1};

    /*DP différent de blanc*/
    if((fdp==1) && (strncmp(diag,"      ",6)==0))
        return 40;
	/*DR différent de blanc*/
    if((fdp==2) && (strncmp(diag,"      ",6)==0))
		return(0);
	
    /*DP ou DR*/
    if((fdp==1) || (fdp==2))
	{
		if (contfmt_2019(diag,"X99++Z",0,3)!=0)
            return((fdp==1)?41:51); 
	    TmpChar=*(masque+5);
	    
        if (!(TmpChar & 1))
            return((fdp==1)?67:94);/*n'a jamais existé*/
	    
        if (TmpChar & 2)
            return((fdp==1)?68:95);/*n'existe plus*/

        /*diags en U à ne pas utiliser*/
        if (((int)(*masque))==23)/*CMD 23*/
        {
            if(*(masque+1)==14) /*dxcat 14*/
            {
                return((fdp==1)?ERR_DP_EN_U:ERR_DR_EN_U);
            }
        }
        
        if (((int)(*masque))==0)/*CMD 90*/
        {
            dxc = *(masque+1);
            if(dxc==0)
            {
                /*diag interdit en DP ou DR*/
                return ((fdp==1)?113:116);
            }
            if(dxc==1)
            {
                /*diag en V,W,X,Y cause externe de morbidité*/
                return ((fdp==1)?114:117);
            }
            if(dxc==2)
            {
                /*diag imprécis car catégorie non vide*/
                return ((fdp==1)?115:118);
            }
            /*FG 13.11e*/
            if(dxc==3)
            {
                /*diag interdit en DP (mais pas en DR)*/
                if(fdp==1)
                    return 113;
            }
        }
        /*CD MAJ FG1311d*/
        if((*(masque+19)) & puisi2[3])
        {
            if(age_an_2019<=8)
                return ((fdp==1)?ERR_DPO_AGE:ERR_DRO_AGE);
        }
        if((*(masque+19)) & puisi2[4])
        {
            if(age_an_2019>=2)
                return ((fdp==1)?ERR_DPP_AGE:ERR_DRP_AGE);
        }

        if(fdp==1) /*DP*/
        {
            /*DP=P95*/
            if( ((diag[0]=='P') || (diag[0]=='p')) && (strncmp(diag+1,"95",2)==0) )
            {
                if(rumin_2019[nbsej_2019-1].modsor!='9')
                    /*return (ERR_INCOMPATIB_DPMODSOR);*/
                    AddErreur_2019(CONTROLE,ERR_INCOMPATIB_DPMODSOR,nosej);
                if  (
                    (rumin_2019[0].modent!='N' && rumin_2019[0].modent != 'n' && rumin_2019[0].modent != '8') ||
                    (rumin_2019[nbsej_2019-1].modsor!='9') ||
                    (strncmp(rumin_2019[0].datent,rumin_2019[nbsej_2019-1].datsor,8)!=0) ||
                    (age_an_2019!=0) ||
                    (age_jour_2019!=0))
                    return (ERR_INCOMPATIB_DP95);
                if(poidsnull_2019)
                    return (ERR_INCOMPATIB_DP95);
            }
			
			if ((nosej==0) && (rumin_2019[0].modent == 'N' || rumin_2019[0].modent == 'n'))
			{
				if (((diag[0] == 'Z') || (diag[0] == 'z')) && (strncmp(diag + 1, "762", 3) == 0))
				{
					AddErreur_2019(CONTROLE, ERR_INCOMPATIB_DPMODENTREEN, nosej);
				}
			}
        }
	}
	if(fdp==0)/*DAS*/
	{
		/*FG 11b : diag a blanc => erreur*/
        if(strncmp(diag,"      ",6)==0)
            return(42);
		if(contfmt_2019(diag,"X99++Z",0,3)!=0)
            return(42);
		
        TmpChar=*(masque+5);
		TmpDiag=toupper(*diag); /*@@ première lettre du diagnostique @@*/
        
        if(!(TmpChar & 1) && (TmpDiag!='U'))
            return(70);
		if(TmpChar & 2)
            return(71);

        /*diags en U à ne pas utiliser*/
        if (((int)(*masque))==23)/*CMD 23*/
        {
            if(*(masque+1)==14) /*dxcat 14*/
            {
                return ERR_DA_EN_U;
            }
        }

        if (((int)(*masque))==0)/*CMD 90*/
        {
            dxc = *(masque+1);
            if(dxc==2)
            {
                /*DA imprécis car catégorie non vide*/
                return 119;
            }
        }
        /*CD MAJ FG11b*/
        if((*(masque+19)) & puisi2[3])
        {
            if(age_an_2019<=8)
                return ERR_DAO_AGE;
        }
        if((*(masque+19)) & puisi2[4])
        {
            if(age_an_2019>=2)
                return ERR_DAP_AGE;
        }
	}
return(0);
}

/**************************************************/
/* FONCTION DE CONTROLE DU NOMBRE DE REALISATION  */
/**************************************************/

int fcnbra_2019(char nba[])
{
    int ret;
    char zone[3];

    /*FG 11b nombre d'exécution obligatoire*/
    /*if (strncmp(nba,"  ",2)==0) return(0);*/
    
    if ((ret=contfmt_2019(nba,"99",1,0))!=0)
    {
        switch(ret)
        {
             case 1 : return(52);
             case 3 : return(52);
        }
    }
    strncpy(zone,nba,2);zone[2]=0;
    if (atoi(zone)==0) return(52);
    return(0);
}


/**********************************************************************/
/* FONCTION DE CONTROLE DU FORMAT et de l'EXISTENCE D'UN ACTE CCAM*****/
/**********************************************************************/
int fcaccam_2019(char *acte,char *masque)
{
   int ret;
   if((ret=contfmt_2019(acte,"XXXX9999",0,8))!=0)
	{
		/*FG 11b Acte CCAM à blanc => erreur*/
        /*if(ret==1)
            return(0);*/
        return(43);
	}
	/*CD MAJ FG10
    if (!(*(masque+16) & 1))
        return(73);
	
    if(*(masque+16) & 2)
		return(74);

	if (nombre_jour_1980_11b(datent)>pdf)
		return(78);
	if(strncmp(datsor,"        ",8)!=0)
		if (nombre_jour_1980_11b(datsor)<pdb )
			return(79);
    */
    if (!(*(masque+16) & 1))
    {
        return(73);
    }
/*	if (nombre_jour_1980_11b(datent)>pdf)
		return(47);
	if(strncmp(datsor,"        ",8)!=0)
		if (nombre_jour_1980_11b(datsor)<pdb )
			return(48);*/
    return(0);
}
/*********************************************************************************/
/* FONCTION DE CONTROLE DE l'EXTENSION ATIH (OU CODE DESCRIPTIF) D'UN ACTE CCAM***/
/*********************************************************************************/
/***le code acte est sur 10 catactère***/
int fccodedesc_2019(int nosej,char *acte)
{
	int extexist, trouveext;
	char zone[3];
	if ((strncmp(acte + 7, "   ", 3) != 0) && ((*(acte + 7) != '-') || (contfmt_2019(acte + 8, "99", 0, 0) != 0)))
    {
		return ERR_ACTECCAM_CODEDESC_FORMAT;
    }
	//else
	{
		trouveext = trouve_codedesc_2019(acte, &extexist);
		if (!trouveext)
		{
			strncpy(zone, acte + 8, 2);
			zone[2] = 0;
			if (atoi(zone) != 0)
			{
				return ERR_ACTECCAM_CODEDESC_ERRONEE;
			}
			else
			{
				if (extexist)
					AddErreur_2019(CONTROLE, ERR_ACTECCAM_CODEDESC_EXISTE_CODEEABLANC, nosej);
				else
					return ERR_ACTECCAM_CODEDESC_ERRONEE;/*n'arrive normalement jamais*/
			}
		}
	}
    return(0);
}

int fcdataccam_2019(int pdb,int pdf,char datent[],char datsor[],int ret2,int ret3)
{
	if(!ret2)
        if (nombre_jour_1980_2019(datent)>pdf)
	        return(78);
	
    if (!ret3)
        if(strncmp(datsor,"        ",8)!=0)
		    if (nombre_jour_1980_2019(datsor)<pdb )
	            return(79);
    return(0);
}
int fcfil_2019(char zone[])
{
 if(lgdiag_2019==6)
	 if(strncmp(zone,"    ",4)!=0)
		 return(83);
 return(0);
}

int fcph_2019(char hosp)
{
 if(contval_2019(&hosp," Xx",1)==0)return(79);
 return(0);
}

/************************************************/
/* FONCTION DE CONTROLE DU TYPE DE LIT DEDIE    */
/************************************************/
int fclitded_2019(char typlitded[])
{
	char zone[3];
    if(strncmp(typlitded,"  ",2)!=0)
	{
		if((contfmt_2019(typlitded,"99",0,2))!=0)
			return 101;
		
		/*CD FG10B*/
        /*if(contval_11b(typlitded,"08",2)==0)
			return 101;*/
        strncpy(zone,typlitded,2);
        zone[2]=0;
        if(autlitdedie_valide_2019(atoi(zone))==0)
            return 101;
	}
	return 0;
}

/***********************************************************/
/* FONCTION DE CONTROLE DE LA DATE DE REALISATION DE L ACTE*/
/***********************************************************/
int	fcdaterea_2019(char zone[],int nosej, int bdate_entree, int bdate_sortie)
{
	int ret;
	if(strncmp(zone,"        ",8)!=0)
	{
		if((ret=contfmt_2019(zone,"Z9Z99999",1,7))!=0)
		{
			switch(ret)
			{
			  case 1 :
			  case 2 :
			  case 3 : return(102);
			}
		}
		if(contdate_2019(zone)!=0)
			return(102); /*@@ vérifie si le format de la date de réalisation est cohérent @@*/
		
		if(!bdate_entree)
		{
			if(nombre_jour_1980_2019(zone) -
				nombre_jour_1980_2019(rumin_2019[nosej].datent) < 0)
				return(102); /*@@ vérifie si la date de réalisation est postérieure à la date d'entrée @@*/
		}
		if(!bdate_sortie)
		{
			if(nombre_jour_1980_2019(rumin_2019[nosej].datsor) -
				nombre_jour_1980_2019(zone) < 0)
				return(102); /*@@ vérifie si la date de réalisation est antérieure à la date de sortie @@*/
		}
	}
	return(0);
} /* fin de la fonction fcdaterea */

/***********************************************************/
/* FONCTION DE CONTROLE DU CODE ACTIVITE DE L ACTE**********/
/***********************************************************/
/*CD MAJ FG10
int	fcactivite(char zone)*/
int	fcactivite_2019(int indiceacte, int bacte)
{
	char activite;
    char *masque;
    int puisi2[8]={128,64,32,16,8,4,2,1};
    /*CD FG10b ne générer qu'une seule erreur 103*/
    activite = *(ptr_typact_2019+indiceacte);
    if ((activite==' ') || (contval_2019(&activite,"12345",1)==0))
        return 103;
    if(!bacte)
    {
        masque = ptr_amask_2019+indiceacte*_prmsize_2019;
        switch(activite)
        {
        case '1': if(!(*(masque+31) & puisi2[7]))
                    return 111;
                  break;
        case '2': if(!(*(masque+32) & puisi2[0]))
                    return 111;
                  break;
        case '3': if(!(*(masque+32) & puisi2[1]))
                    return 111;
                  break;
        case '4': if(!(*(masque+22) & puisi2[2]))
                    return 110;
                  break;
        case '5': if(!(*(masque+32) & puisi2[2]))
                    return 111;
                  break;
        }
    }
	return 0;
}

/************************************************************************************/
/* FONCTION déterminant les actes autorisant des gestes complémentaires**************/
/* et les gestes complémentaires à contrôler ****************************************/
/************************************************************************************/
void fprinccompl_2019(int actesAutorisantGC[], int *nbactesAutorisantGC, int gestescompl[], int *nbGestesCompl, char *masque,int i)
{
    char code_complet[10];
	int puisi2[8]={128,64,32,16,8,4,2,1};
    code_complet[9] = '\0';
	if(*(masque+32) & puisi2[5]) /*acte autorisant des gestes complémentaires*/
    {
        if(nbactesAutorisantGC)
        {
            actesAutorisantGC[*nbactesAutorisantGC]=i;
            (*nbactesAutorisantGC)++;
        }
    }
	if(*(masque+32) & puisi2[4]) /*geste complémentaire à contrôler*/
	{
		sprintf(code_complet,"%8.8s%c",ptr_actes_2019+i*lgacte_2019,ptr_typact_2019[i]);
		if (est_gc_2019(code_complet)==0)
			if(nbGestesCompl)
			{
				gestescompl[*nbGestesCompl]=i;
				(*nbGestesCompl)++;
			}
	}
}


/************************************************************************************/
/* FONCTION DE CONTROLE DES COUPLES GESTES COMPLEMENTAIRES-ACTES PRINCIPAUX**********/
/************************************************************************************/
int	fccouplesact_2019(int actesAutorisantGC[], int *nbactesAutorisantGC, int gestescompl[], int *nbGestesCompl)
{
	int cpt,i;
	int ret;
	char *res;
	int ret2;

	res = NULL;

	if((!nbactesAutorisantGC)||(!nbGestesCompl))
        return 0;

	if(*nbGestesCompl==0)
        return 0;
	
	if (*nbactesAutorisantGC==0 && *nbGestesCompl>0)
		return 112;

	for (cpt=0;cpt<*nbactesAutorisantGC;cpt++)
	{
		ret = liste_compl_2019(ptr_actes_2019+actesAutorisantGC[cpt]*lgacte_2019,ptr_actes_2019[actesAutorisantGC[cpt]*lgacte_2019+7],ptr_typact_2019[actesAutorisantGC[cpt]],&res);
		if (ret==0)
		{
			for (i=0;i<*nbGestesCompl;i++)
			{
				if (gestescompl[i]!=-1)
					if((ret2=actelisteGC_2019(ptr_actes_2019+gestescompl[i]*lgacte_2019,ptr_typact_2019[gestescompl[i]],res))==0)
						gestescompl[i]=-1;
			}
			if (tableauvide_2019(gestescompl,nbGestesCompl)==0)
            {
				if(res!=NULL)
                {
                    free(res);
                    res=NULL;
                }
                return 0;
            }
		}
        else
        if(ret==-1)
            return ret;
        if(res!=NULL)
        {
            free(res);
            res=NULL;
        }
	}
    return 112;
}



/*@@ contrôle de cohérence d'un char* avec un format:
zone= zone contenant la variable à tester
format= zone texte comportant des charactères corespondant au type corespondant de charactére attendu dans zone (ex: '9' => on attend un chiffre)
ft= cmment traiter les espaces dans zone (1=>ignorer les espaces avant,2=> ignorer les espaces aprés, 3=> ignorer les espaces avant et aprés)
@@
le format ne doit pas être plus grand strictement que la chaine a tester (zone)
taille : taille minimum de zone trimée en fonction de ft
*/
int contfmt_2019 (char *zone,char format[],int ft,int taille)
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
	
    /*t est la taille de zone sans les espaces a gauche et a droite*/
    t=pcnbd-pcnbg+1;

	/*cette fonction renvoie 1 si zone est a blanc ou si la zone trimée est + grande que le format*/
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
	t=fin-deb+1; /* t est la taille de zone trimée en fonction du paramètre ft*/

	if(t<taille) /*t = taille minimum de zone trimée en fonction de ft*/
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

int contval_2019(char *zone,char *valeur,int lgt)
{
	unsigned int i;

	/*CD MAJ FG9
	for(i=0;i<strlen(valeur) / lgt;)*/
    for(i=0;i<strlen(valeur);)
        {
		  if(strncmp(zone,valeur+i,lgt)==0)
			return(1);
          i+=lgt;
        }
	return(0);
}

/*@@ controle du format de la date (sans plus) @@*/
int contdate_2019 (char *adate)
{
	int bis; /* variables de travail */
	static  int tabj[2][13] = {
				{0,31,28,31,30,31,30,31,31,30,31,30,31},
				{0,31,29,31,30,31,30,31,31,30,31,30,31}	};
	int jj,mm,aa;

	convdate_2019(adate,&jj,&mm,&aa); /*lis le format "étrange" et le transforme en qq chose d'utilisable*/
	bis= aa%4 == 0 && aa%100 !=0 || aa%400==0;
	/* controle mois   */
	if (mm < 1 || mm > 12) return(1);
	/* controle brut du jour */
	if (jj < 1 || jj > tabj[bis][mm]) return(1);
	return(0);
}

int mask_null_2019 (char *masque, int lg)
{
  int i;
  for(i=0;i<lg;i++)
    if(*(masque+i)!= 0)return(0);
  return(1);
}


void calldate_2019 (char wdate[])
{
int i;
char ddate[33];
char mois[13][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
time_t    clock;

     strcpy(wdate,"");
     clock = time(0);
     strcpy(ddate,ctime(&clock));
     for(i=0;i<12;i++)
      {
       if(strncmp(ddate+4,mois[i],3)==0)
		   break;
      }
     strncpy(wdate,ddate+20,4);
     sprintf(wdate+4,"%02.2d",i+1);
     strncpy(wdate+6,ddate+8,2);
     if(wdate[6]==' ')
		 wdate[6]='0';
     wdate[8]='\0';
}
/* controle de type DATIM sur ptr_omask_2019 */
void ControleOMS_2019(int nosej,char *masque,char sexe,int agej,int agea,int fdp)
{
	int ret;
	
if ((ret=CtrlDiagZenDP_2019 (masque,fdp))!=0)AddErreur_2019(CONTROLE,ret,nosej);
/*FG11 if ((ret=CtrlDiagDague_2019 (masque,fdp))!=0)AddErreur_2019(CONTROLE,ret,nosej);*/
if ((ret=CtrlDiagRare_2019 (masque,fdp))!=0)	AddErreur_2019(CONTROLE,ret,nosej);
if ((ret=CtrlDiagImprecis_2019 (masque,fdp))!=0)AddErreur_2019(CONTROLE,ret,nosej);
/*CD modif du 01/03/04
  if ((ret=CtrlDiagAge (masque,fdp,agej,agea))!=0)AddErreur_2019(CONTROLE,ret,nosej);*/
if ((ret=CtrlDiagAge_2019 (masque,fdp,agea,agej))!=0)AddErreur_2019(CONTROLE,ret,nosej);
if ((ret=CtrlDiagSexe_2019 (masque,fdp,sexe))!=0)AddErreur_2019(CONTROLE,ret,nosej);
}

/* CD modif du 01/03/04
int CtrlDiagSexe (char *masque,int fdp,char sexe)
{
	if(fdp==1)
	{
     if (sexe=='1' && (*(masque+11)!=0))
		 return(86);
     if (sexe=='2' && (*(masque+12)!=0))
		 return(86); 
	}
	if(fdp==0)
	{
     if (sexe=='1' && (*(masque+11)!=0))
		 return(92);
     if (sexe=='2' && (*(masque+12)!=0))
		 return(92); 
	}
	if(fdp==2)
	{
     if (sexe=='1' && (*(masque+11)!=0))
		 return(98);
     if (sexe=='2' && (*(masque+12)!=0))
		 return(98); 
	}
return(0);
}
*/
int CtrlDiagSexe_2019 (char *masque,int fdp,char sexe)
{
	if(fdp==1)
	{
     if (sexe=='1' && (*(masque+12)!=0))
		 return(86);
     if (sexe=='2' && (*(masque+11)!=0))
		 return(86); 
	}
	if(fdp==0)
	{
     if (sexe=='1' && (*(masque+12)!=0))
		 return(92);
     if (sexe=='2' && (*(masque+11)!=0))
		 return(92); 
	}
/*CD DR 10/12/03*/
	if(fdp==2)
	{
     if (sexe=='1' && (*(masque+12)!=0))
		 return(98);
     if (sexe=='2' && (*(masque+11)!=0))
		 return(98); 
	}
return(0);
}


int CtrlDiagAge_2019 (char *masque,int fdp,int agea,int agej)
{

     if (agej<29 && agea==0)
	 {
		if(fdp==1)
		{
		if (*(masque+3)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+3)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+3)!=0)
			return(97);
		}
	 }
	 if (agej>=29 && agea==0)
	 {
		if(fdp==1)
		{
		if (*(masque+4)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+4)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+4)!=0)
			return(97);
		}
	 }
	 if (agea>0 && agea<10)
	 {
		if(fdp==1)
		{
		if (*(masque+5)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+5)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+5)!=0)
			return(97);
		}
	 }
	 if (agea>=10 && agea<20)
	 {
		if(fdp==1)
		{
		if (*(masque+6)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+6)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+6)!=0)
			return(97);
		}
	 }
	 if (agea>=20 && agea<65)
	 {
		if(fdp==1)
		{
		if (*(masque+7)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+7)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+7)!=0)
			return(97);
		}
	 }
	 if (agea>=65)
	 {
		if(fdp==1)
		{
		if (*(masque+8)!=0)
			return(85);
		}
		if(fdp==0)
		{
		if (*(masque+8)!=0)
			return(91);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+8)!=0)
			return(97);
		}
	 }

return(0);
}

int CtrlDiagImprecis_2019 (char *masque,int fdp)
{
		if(fdp==1)
		{
		if (*(masque+10)!=0)
			return(87);
		}
		if(fdp==0)
		{
		if (*(masque+10)!=0)
			return(93);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
		if (*(masque+10)!=0)
			return(99);
		}
		return(0);
}
/*int CtrlDiagDague_2019 (char *masque,int fdp)
{
		if(fdp==1)
		{
		if (*(masque+2)!=0)
			return(89);
		}
		return(0);
}*/
int CtrlDiagRare_2019 (char *masque,int fdp)
{
		if(fdp==1)
		{
		if (*(masque)!=0)
			return(84);
		}
		if(fdp==0)
		{
		if (*(masque)!=0)
			return(90);
		}
/*CD DR 10/12/03*/
		if(fdp==2)
		{
			if (*(masque)!=0)
				return(96);
		}

		return(0);
}

int CtrlDiagZenDP_2019 (char *masque,int fdp)
{
/* uniquement en DP */
		if(fdp==1)
		{
		if (*(masque+9)!=0)
			return(88);
		}
		return(0);
}

/*controle des autorisations utilisées*/
void controle_refs_um_2019(int pnosej)
{
    int j;
    UMINFO_2019 luminf;
    short autorum;

    if(indice_ums_2019[pnosej]<0)
        return;
    luminf = uminf_2019[indice_ums_2019[pnosej]];
    for(j=0;j<luminf.nbautorisations;j++)
    {
        autorum = luminf.autorisations[j].typum;
        if((autorum==88) || (autorisation_valide_2019(autorum)))
        {
            luminf.autorisations[j].invalidee=0;
        }
        else
        {
            luminf.autorisations[j].invalidee=1;
            /*Type d'autorisation non autorisée par la sous table de réference*/
            AddErreur_2019(CONTROLE,100,pnosej);
        }
    }
}

/*****************************************************/
/* FONCTION DE CONTROLE DE L AGE GESTATIONNEL*/
/*****************************************************/
int fcagegest_2019(int pnosej,char agegest[],char pmodsordernierum)
{
    int ret;
    char zone[5];
    int iagest;
    int bagegestvide;
    int ipoids;
    char *dp;
    
    bagegestvide=(strncmp(agegest,"  ",2)==0);
    if (!bagegestvide)
    {
        if ((ret=contfmt_2019(agegest,"99",1,0))!=0)
        {
            switch(ret)
            {
                case 3 : return(ERR_AG_NONNUMERIQUE);
            }
        }
        
        strncpy(zone,agegest,2);zone[2]=0;
        iagest=atoi(zone);
        
        if(iagest<1)
            return 0;
        if(agegest_renseigne_2019<0);
            agegest_renseigne_2019=pnosej;

        if (iagest>44)
            return(ERR_AG_INCOHERENT);
        if ((iagest<22) && (pmodsordernierum!='9') && (age_an_2019==0) )
            return(ERR_AG_INCOHERENT);

        /*CD FG13 combinaison age gestationnel et poids à l'entrée*/
        if(poidsok_2019)
        {
            strncpy(zone,rumin_2019[0].pdsnaiss,4);
            zone[4]=0;
            ipoids=atoi(zone);
            if(ipoids>0)
            {
                if((iagest>=37) && (ipoids<1000))
                {
                    dp = ptr_diags_2019+(inddiag_2019[pnosej])*6;
                    if(!(((dp[0]=='P') || (dp[0]=='p')) && (strncmp(dp+1,"95",2)==0)))
                        return ERR_AGPOIDS_INCOHERENT;
                }
                if((iagest<=32) && (ipoids>4000))
                    return ERR_AGPOIDS_INCOHERENT;
                if((iagest<=28) && (ipoids>2500))
                    return ERR_AGPOIDS_INCOHERENT;
            }
        }
    }
    return(0);
}

/*******************************************************/
/* FONCTION DE CONTROLE DE LA DATE DES DERNIERES REGLES*/
/*******************************************************/
int fcdatedernregles_2019(int pnosej,char pdatedernregles[])
{
    int ret;
    
    if(strncmp(pdatedernregles,"        ",8)==0)
        return 0;
    

    if((ret=contfmt_2019(pdatedernregles,"Z9Z99999",1,7))!=0)
    {
        switch(ret)
        {
             case 2 :
             case 3 : return(ERR_DATEDERNREGLES_NONNUMERIQUE);
        }
    }
    if(contdate_2019(pdatedernregles)!=0)
        return(ERR_DATEDERNREGLES_INCOHERENTE);

    if(datedernregles_renseignee_2019<0)
        datedernregles_renseignee_2019=pnosej;
    else
    {
        /*la DDR doit être constante*/
        if(strncmp(pdatedernregles,rumin_2019[datedernregles_renseignee_2019].datedernregles,8)!=0)
            return(ERR_DATEDERNREGLES_DIFFERENTES);
    }
    return(0);
}
/************************************************************/
/* FONCTION DE CONTROLE DE L'inDICATEUR DE CONVERSION HP->HC*/
/************************************************************/
void fcconvHPHC_2019(int nosej, char pconvhphc)
{
	UMINFO_2019 luminf;
	int s;
	char typhosp;
	if (contval_2019(&pconvhphc, " 12", 1) == 0)
		AddErreur_2019(CONTROLE, ERR_CONVHPHC_NONNUMERIQUE, nosej);
	typhosp = 'X';
	if (indice_ums_2019[nosej] != -1)
	{
		luminf = uminf_2019[indice_ums_2019[nosej]];
		for (s = 0; s < luminf.nbautorisations; s++)
		{
			if 
				((!date_strict_sup_2019(luminf.autorisations[s].datdebeffet, rumin_2019[nosej].datent)) &&
				(!date_strict_sup_2019(rumin_2019[nosej].datent,luminf.autorisations[s].datfineffet)))
			{
				typhosp = luminf.autorisations[s].typhospit;
				break;
			}
		}
	}
	if (nosej == 0)
		typehosp1erRUM = typhosp;

	if ((pconvhphc == '1') && (typhosp == 'P'))
	{
		AddErreur_2019(CONTROLE, ERR_CONVHPHC_CODEAOUIETUMHP, nosej);
	}

	if (nosej == 1)
	{
		if (callos_2019(rumin_2019[0].datent, rumin_2019[0].datsor) == 0)
		{
			if ((typehosp1erRUM == 'P') && (pconvhphc == '2'))
			{
				AddErreur_2019(CONTROLE, ERR_CONVHPHC_CONDITIONOKETANON, nosej);
			}
			else
				if ((typehosp1erRUM == 'P' || typehosp1erRUM == 'M') && (pconvhphc == ' '))
				{
					AddErreur_2019(CONTROLE, ERR_CONVHPHC_CONDITIONOKETAVIDE, nosej);
				}
		}
	}

}
/*************************************************************************/
/* FONCTION DE CONTROLE DE L'inDICATEUR DE PRISE EN CHARGE SEJOUR RAAC ***/
/*************************************************************************/
void fcindicRAAC_2019(int nosej, char priseenchargeRAAC)
{
	if (!indicRAAC && (rumin_2019[nosej].priseenchargeRAAC == '1'))
		indicRAAC = 1;

	if (contval_2019(&priseenchargeRAAC, " 12", 1) == 0)
		AddErreur_2019(CONTROLE, ERR_INDICRAAC_NONNUMERIQUE, nosej);
	else
	if (nosej == nbsej_2019 - 1)
	{
		if ((indicRAAC) &&
			(
			((rumin_2019[nbsej_2019 - 1].modsor == '7') && (rumin_2019[nbsej_2019 - 1].dest == '1'))
				||
				(rumin_2019[nbsej_2019 - 1].modsor == '9')
				)
			)
		{
			AddErreur_2019(CONTROLE, ERR_INDICRAAC_MODESOR_INCOMPATIBLE, nosej);
		}
	}
}

/*********************************************/
/* FONCTION DE CONTROLE DE L'IGS *************/
/*********************************************/
int	fcigsrum_2019 (char paramigs[])
{
   int ret;
   if(strncmp(paramigs,"   ",3)==0)
       return 0;
   /* VALEUR ERRONEE DE L'IGS */
   if ((ret=contfmt_2019(paramigs,"999",1,0))!=0)
       if(ret==3)  return(ERR_IGS_NONNUMERIQUE);
   return(0);
} /* fin de la fonction fcigsrum_2019 */

