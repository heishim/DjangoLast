
#define tab_max 99                /* Nombre maximum de RUM */
#define NBMAX_ACTES_AUTORISANT_GC_2019 999
#define NBMAX_GC_2019 999

extern struct RUM_2019 {
#include "rum2019.h"                /* definition de la structure d'un RUM */
}rumin_2019[tab_max];

extern char *ptr_diags_2019;       /* Ptr sur zone contenant les diags P+S */
extern char *ptr_actes_2019;       /* Ptr sur zone contenant les actes */
extern char *ptr_nbactes_2019;       /* Ptr sur zone contenant les actes */
extern int lgacte_2019;       /* Ptr sur zone contenant les actes */
extern int inddiag_2019[tab_max];/* Tab. contenant les positions des Diag P. */
extern int indacte_2019[tab_max];/* Tab. contenant la position des premiers actes */
extern int *ptr_inddiag_2019;    /* Ptr. sur zone contenant les indices des diags retenus */
extern int *ptr_indacte_2019;    /* Ptr. sur zone contenant les incdices des actes retenus */
extern int *ptr_datdeb_2019;
extern int *ptr_datfin_2019;

extern int nbdiags_2019;         /* Nb. de diags */
extern int nbactes_2019;         /* Nb. d'actes */
extern char *ptr_dmask_2019;       /* Ptr. sur la zone des masques de diags */
extern char *ptr_omask_2019;       /* Ptr. sur la zone des profils oms des diags */
extern char *ptr_amask_2019;       /* Ptr. sur la zone des masques d'actes */
extern int _dxmsize_2019;        /* Taille du profil de diags */
extern int _prmsize_2019;        /* Taille du profil des actes */
extern int _prfsize_2019;        /* Taille du profil OMS */
extern int nbsej_2019;           /* Nombre de séjours */
extern int age_jour_2019,age_an_2019;
extern char _modsor_2019[2];       /* Mode de sortie+destination */
extern int lgdiag_2019;          /* Taille des diagnostic */

/*CD MAJ FG10*/
extern char *ptr_typact_2019;       /* Ptr. sur la zone des types d'activité*/

extern int     _pdsn_2019;               /* Poids de naissance                         */

/*****************************************************************************
Fonction ctlsej
Description :
Lancement de la fonction de contrôle des champs des RUMs
******************************************************************************/
int ctlsej_2019(int nosej, char *rum);


/******************************************************************************
Fonctions de controle de chacun des champs du RUM

fcnf	: Fonction de controle du n° finess
fcnr	: Fonction de controle du n° de RSS
fcdn	: Fonction de controle de la date de naissance
fcsx	: Fonction de controle du sexe
fcus	: Fonction de controle de l'unite medicale
fcde	: Fonction de controle de la date d'entree
fcme	: Fonction de controle du mode d'entree
fcpr	: Fonction de controle de la provenance
fcds	: Fonction de controle de la date de sortie
fcms	: Fonction de controle du mode se sortie
fcdst	: Fonction de controle de la destination
fc24	: Fonction de controle de sejour <24H
fccp	: Fonction de controle du code postal
fcpn	: Fonction de controle du poids de naissance
fcse	: Fonction de controle du nombre de séances
fcnbd	: Fonction de controle du nbre de diags
fcnba	: Fonction de controle du nbre d'actes
fcfil	: Fonction de controle du filler (anciennement diagp CIM9)
fcph	: Fonction de controle  hosp dans les 30 jours
fcdg10	: Fonction de controle diagp
fcac	: Fonction de controle des actes
foms	: Fonction de controle OMS
******************************************************************************/
int fcnf_2019(char finess[] );
int fcnr_2019(char numrss[] );
int fcdn_2019(char naiss[],int ret2);
int fcsx_2019(char sexe);
int fcus_2019(char unmed[]);
int fcde_2019(char datent[]);
int fcme_2019(char modent);
int fcpr_2019(char prov,char modent,int nosej);
int fcds_2019(char datsor[],int ret,int nosej);
int fcms_2019(char modsor);
int fcdst_2019(char dest,char modsor,int nosej);
int fc24_2019(char sej_24,int nosej);
int fccp_2019(char codpost[]);
int fcpn_2019(char pdsnaiss[],int nosej);
int fcse_2019(char nbseance[]);
int fcnbd_2019(char nbds[]);
int fcnba_2019(char nba[]);
int fcnbra_2019(char nba[]);
int fcph_2019(char hosp30);
int fcdg10_2019(char *ptr_diags,char *ptr_dmask,int fdp,int nosej);
int fcaccam_2019(char *,char *);
int fcdataccam_2019(int,int,char[],char[],int,int);
void fprinccompl_2019(int[], int *, int[], int *, char *,int);
int	fccouplesact_2019(int[], int *, int gestescompl[], int *);

/*FG11*/
int fcconfcodage_2019(char pccod);
int fctmr_2019(char pccod);
int fctypedos_2019(char pccod);
/*CD FG12*/
int fcnbfaisc_2019(char pccod);

void ControleOMS_2019(int nosej,char *masque,char sexe,int agej,int agea,int fdp);
int fclitded_2019(char typlitded[]);
int fcdaterea_2019(char date_rea[],int nosej,int bdate_entree,int bdate_sortie);
int fcactivite_2019(int indiceacte,int bacte);

/*FG11g*/
int fccodedesc_2019(int nosej,char *acte);

/******************************************************************************
Fonctions utilitaires
majvecerr	: Met à jour le vecteur d'erreur
contfmt	    : Controle de format pour une chaîne de caractere
contdate	: Controle de date
calldate	: Appel de la date systeme
contval		: Verifie l'appartenance a une plage de valeur
convdate : transforme une date jjmmaaaa en 3 variables jj mm aa
nombre_jour_1980 : Rend le nombre de jour entre la date et le 01/01/1980
callage : Calcul l'age anniversaire entre la date de naissance et la date
          d'entrée En jour et en année
*******************************************************************************/
int contfmt_2019 (char *zone,char format[],int ft,int taille);
int contdate_2019 (char *adate);
void  calldate_2019 (char *wdate);
int contval_2019(char *zone,char *valeur,int lgt);
void convdate_2019 (char *adat,int *p_jj,int *p_mm,int *p_aa);
int nombre_jour_1980_2019(char *date);
void callage_2019 (char *datnais,char *datsor);

/* int seance */
char dateptr_2019[9];		/* contient la date systeme */
int CtrlDiagSexe_2019 (char *masque,int fdp,char sexe);
/*int CtrlDiagDague_2019 (char *masque,int fdp);*/
int CtrlDiagImprecis_2019 (char *masque,int fdp);
int CtrlDiagRare_2019 (char *masque,int fdp);
int CtrlDiagZenDP_2019 (char *masque,int fdp);
int CtrlDiagAge_2019 (char *masque,int fdp,int agea,int agej);
int contfmt_2019 (char *zone,char format[],int ft,int taille);


/*CD FG11b*/
int fcagegest_2019(int pnosej,char agegest[],char pmodsor);
int agegest_renseigne_2019;
int poidsok_2019;
int nbjdateaccouch_2019;

/*int diagantepartum;*/
int datentreesejok_2019;

/*IMG*/
/*int dpavor;*/
/*int diagZ640;*/

int daZ37_2019;
int daaccouch_2019;/*DIAG*/
int presenceActeAccouchement_2019;
int poidsnull_2019;

void controle_refs_um_2019(int numsej);

/*CD FG13*/
int datedernregles_renseignee_2019;
int numero_innovation_2019;

/*CD FG13*/
int fcdatedernregles_2019(int pnosej,char pdatedernregles[]);

/*FG v2019*/
void fcconvHPHC_2019(int nosej, char pconvhphc);

/*FG v2019*/
void fcindicRAAC_2019(int nosej, char priseenchargeRAAC);


/*controle de l'IGS*/
int	fcigsrum_2019 (char paramigs[]);

/*FG 11g*/
extern int trouve_codedesc_2019(char *codeactedesc, int *extexist);

