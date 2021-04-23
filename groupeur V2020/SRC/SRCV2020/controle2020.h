
#define tab_max 99                /* Nombre maximum de RUM */
#define NBMAX_ACTES_AUTORISANT_GC_2020 999
#define NBMAX_GC_2020 999

extern struct RUM_2020 {
#include "rum2020.h"                /* definition de la structure d'un RUM */
}rumin_2020[tab_max];

extern char *ptr_diags_2020;       /* Ptr sur zone contenant les diags P+S */
extern char *ptr_actes_2020;       /* Ptr sur zone contenant les actes */
extern char *ptr_nbactes_2020;       /* Ptr sur zone contenant les actes */
extern int lgacte_2020;       /* Ptr sur zone contenant les actes */
extern int inddiag_2020[tab_max];/* Tab. contenant les positions des Diag P. */
extern int indacte_2020[tab_max];/* Tab. contenant la position des premiers actes */
extern int *ptr_inddiag_2020;    /* Ptr. sur zone contenant les indices des diags retenus */
extern int *ptr_indacte_2020;    /* Ptr. sur zone contenant les incdices des actes retenus */
extern int *ptr_datdeb_2020;
extern int *ptr_datfin_2020;

extern int nbdiags_2020;         /* Nb. de diags */
extern int nbactes_2020;         /* Nb. d'actes */
extern char *ptr_dmask_2020;       /* Ptr. sur la zone des masques de diags */
extern char *ptr_omask_2020;       /* Ptr. sur la zone des profils oms des diags */
extern char *ptr_amask_2020;       /* Ptr. sur la zone des masques d'actes */
extern int _dxmsize_2020;        /* Taille du profil de diags */
extern int _prmsize_2020;        /* Taille du profil des actes */
extern int _prfsize_2020;        /* Taille du profil OMS */
extern int nbsej_2020;           /* Nombre de séjours */
extern int age_jour_2020,age_an_2020;
extern char _modsor_2020[2];       /* Mode de sortie+destination */
extern int lgdiag_2020;          /* Taille des diagnostic */

/*CD MAJ FG10*/
extern char *ptr_typact_2020;       /* Ptr. sur la zone des types d'activité*/

extern int     _pdsn_2020;               /* Poids de naissance                         */

/*****************************************************************************
Fonction ctlsej
Description :
Lancement de la fonction de contrôle des champs des RUMs
******************************************************************************/
int ctlsej_2020(int nosej, char *rum);


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
int fcnf_2020(char finess[] );
int fcnr_2020(char numrss[] );
int fcdn_2020(char naiss[],int ret2);
int fcsx_2020(char sexe);
int fcus_2020(char unmed[]);
int fcde_2020(char datent[]);
int fcme_2020(char modent);
int fcpr_2020(char prov,char modent,int nosej);
int fcds_2020(char datsor[],int ret,int nosej);
int fcms_2020(char modsor);
int fcdst_2020(char dest,char modsor,int nosej);
int fc24_2020(char sej_24,int nosej);
int fccp_2020(char codpost[]);
int fcpn_2020(char pdsnaiss[],int nosej);
int fcse_2020(char nbseance[]);
int fcnbd_2020(char nbds[]);
int fcnba_2020(char nba[]);
int fcnbra_2020(char nba[]);
int fcph_2020(char hosp30);
int fcdg10_2020(char *ptr_diags,char *ptr_dmask,int fdp,int nosej);
int fcaccam_2020(char *,char *);
int fcdataccam_2020(int,int,char[],char[],int,int);
void fprinccompl_2020(int[], int *, int[], int *, char *,int);
int	fccouplesact_2020(int[], int *, int gestescompl[], int *);

/*FG11*/
int fcconfcodage_2020(char pccod);
int fctmr_2020(char pccod);
int fctypedos_2020(char pccod);
/*CD FG12*/
int fcnbfaisc_2020(char pccod);

void ControleOMS_2020(int nosej,char *masque,char sexe,int agej,int agea,int fdp);
int fclitded_2020(char typlitded[]);
int fcdaterea_2020(char date_rea[],int nosej,int bdate_entree,int bdate_sortie);
int fcactivite_2020(int indiceacte,int bacte);

/*FG11g*/
int fccodedesc_2020(int nosej,char *acte, int ret2, int ret3);

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
int contfmt_2020 (char *zone,char format[],int ft,int taille);
int contdate_2020 (char *adate);
void  calldate_2020 (char *wdate);
int contval_2020(char *zone,char *valeur,int lgt);
void convdate_2020 (char *adat,int *p_jj,int *p_mm,int *p_aa);
int nombre_jour_1980_2020(char *date);
void callage_2020 (char *datnais,char *datsor);

/* int seance */
char dateptr_2020[9];		/* contient la date systeme */
int CtrlDiagSexe_2020 (char *masque,int fdp,char sexe);
/*int CtrlDiagDague_2020 (char *masque,int fdp);*/
int CtrlDiagImprecis_2020 (char *masque,int fdp);
int CtrlDiagRare_2020 (char *masque,int fdp);
int CtrlDiagZenDP_2020 (char *masque,int fdp);
int CtrlDiagAge_2020 (char *masque,int fdp,int agea,int agej);
int contfmt_2020 (char *zone,char format[],int ft,int taille);


/*CD FG11b*/
int fcagegest_2020(int pnosej,char agegest[],char pmodsor);
int agegest_renseigne_2020;
int poidsok_2020;
int nbjdateaccouch_2020;

/*int diagantepartum;*/
int datentreesejok_2020;

/*IMG*/
/*int dpavor;*/
/*int diagZ640;*/

int daZ37_2020;
int daaccouch_2020;/*DIAG*/
int presenceActeAccouchement_2020;
int poidsnull_2020;

void controle_refs_um_2020(int numsej);

/*CD FG13*/
int datedernregles_renseignee_2020;
int numero_innovation_2020;

/*CD FG13*/
int fcdatedernregles_2020(int pnosej,char pdatedernregles[]);

/*FG v2019*/
void fcconvHPHC_2020(int nosej, char pconvhphc);

/*FG v2019*/
void fcindicRAAC_2020(int nosej, char priseenchargeRAAC);

/*FG v2020*/
void fcGradationHdjzero_2020(int nosej);

/*controle de l'IGS*/
int	fcigsrum_2020 (char paramigs[]);

extern int trouve_codedesc_2020(char *codeactedesc, int *extexist, int *datedeb, int *datefin);

