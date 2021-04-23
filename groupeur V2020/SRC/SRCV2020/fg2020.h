/*identifiant de la fonction groupage*/
#define CLS_ID_2020 "7db2ec11-8f8e-479b-a1e8-c8b2d573f154"

#define tab_max 99 /* Nombre maximum de RUM */

#define MAX_FUNC_2020 44     /* Nombre de fonctions max appelées par l'arbre de décision */

#define MAX_FUNCGHS_2020 12  /* Nombre de fonctions max pour le calcul des GHS */

#define caras '+'

#define MAX_NBJ_RUM_2020 2000

/*FG13*/
#define TAILLE_CODE_INNOV_2020 15

/* definitions des tailles des tables */

long _doffpref_2020;       /* Offset des prefixes de diag*/
int  _dsizepref_2020;         /* longueur d'un prefixe de diagnostic */

long _doffnop10_2020;      /* Offset des numeros de masques */
int  _dsize_2020;             /* longueur d'un no de masque de diagnostic */
long _doffpro10_2020;      /* Offset des masques caracteristiques*/
int _dxmsize_2020;         /* Longueur des masques de cara. de diag */
long _doffoms10_2020;      /* Offset des masques OMS */
long _doffexc10_2020;      /* Offset des liste d'exclusion */
int _excsize10_2020;       /* Taille des masques de liste d'exclusion */
#define _tpref_2020 2         /* Taille des prefixes */
#define _tsuf_2020  9         /* Taille des suffixes */
#define _nbpref_2020 2600     /* Nombre de prefixes */
int _prfsize_2020;         /* longueur d'un profil OMS */
int lgdiag_2020;           /* Longueur d'un diagnostic */
int lgacte_2020;           /* Longueur d'un acte */

long  _poffnop_2020;            /* Offset des numeros de masques actes*/
int _psize_2020;                /* longueur no de masque  */
long  _poffpro_2020;              /* Offset des masques caracteristiques actes*/
int _prmsize_2020;              /* longueur d'un masque d'acte */


long  _poffpref_ccam_2020;      /* Offset des prefixes d'actes*/
int   _psizepref_ccam_2020;     /* longueur d'un prefixe d'acte*/

long    _poffnop_ccam_2020;     /* Offset des numeros de masques actes*/
int     _psize_ccam_2020;         /* longueur no de masque  */
long    _poffpro_ccam_2020;      /* Offset des masques caracteristiques actes*/
#define _tpref_ccam_2020 2         /* Taille des prefixes */
#define _tsuf_ccam_2020  9         /* Taille des suffixes */


int    _trsize_2020   ;        /* Taille des informations d'un noeud */
long     _toffset_2020;        /* Taille de l'offset des numeros de noeud */

#include "tables2020.h"      /* Noms des tables en clair */
#include "erreurs2020.h"

/* PROTOTYPE DES FONCTIONS UTILISEES */
/*****************************************************************************
* Fonction FreeErr :                                                         *
* Description :                                                              *
* Fonction utilitaire permettant de sortir proprement de la fonction grp     *
******************************************************************************/
int FreeErr_2020 (char *rum[], int noerr,int type,GRP_RESULTAT_2020 *grp_res,GHS_VALORISATION_2020 *ghs_val,RSS_INFO_2020 *rss_inf);
/*****************************************************************************
* Fonction callage :                                                         *
* Description :                                                              *
* Calcul l'age anniversaire entre la date de naissance et la date  d'entrée. *
* En jour et en année                                                        *
******************************************************************************/
void callage_2020 (char *datnais,char *datsor);


/****************************************************************************
Fonction controle
Description:
Fonction contôlant la réalisation du controle d’un RSS :
    - Appel de la date système (fonction calldate)
    - Lancement de la fonction de contrôle des champs des RUMs (ctlsej)
    - Lancement de la fonction de contôle OMS (fonction foms)
******************************************************************************/
int controle_2020();

/*****************************************************************************
Fonction optree
Paramètres
Description:
Ouverture de la table de l’arbre de décision
******************************************************************************/
/*CD 22/01/07 int optree_11b(char *dirtab);
*/
int optree_2020();

/*****************************************************************************
Fonction opdiag10
Paramètres
Description:
Ouverture de la table des diagnostics
*******************************************************************************/
/*CD 22/01/07 int opdiag10_2020(char *dirtab);
*/
int opdiag10_2020();

/*****************************************************************************
Fonction opacte
Paramètres
Description:
Ouverture de la table des actes
******************************************************************************/
int opacte_2020(char *dirtab);

/*****************************************************************************
Fonction prmap
Description:
Récupération des informations nécessaire au groupage concernant un acte donné.
Après avoir calculé l’offset pour cet acte (fonction convacte), lecture dans la
tables des actess des informations concernant le groupage.
******************************************************************************/
int prmap_ccam_2020(int indice);


/*****************************************************************************
Fonction dxmap10_2020
Description:
Récupération des informations nécessaire au groupage concernant un diagnostic
donné. Après avoir calculé l’offset pour ce diagnostic (fonction convdiag10_2020),
lecture dans la tables des diagnostics des informations concernant le groupage,
l’appartenance aux listes d’exclusion et les contrôles de cohérence OMS.
*****************************************************************************/
int dxmap10_2020(int indice,char sexe);
int convdiag10_2020 (char *adrcod,unsigned char prof[]);


/*****************************************************************************
Fonction trmap
Description
Lit dans la table de l’arbre les informations concernant la marche à suivre :
n° de la fonction à appeler, n° de base du prochain noeud à atteindre,
paramètres éventuels de la fonction.
La description des fonctions de tests du groupage sont décrit dans les sources.
******************************************************************************/
int trmap_2020(int nn);

/*******************************************************************************
Fonction charged
Description:
Détermine  les diagnostics du RSS qui seront utilisés lors du groupage, après
élimination des doublons (fonction doublon) et des diagnostics vides.
********************************************************************************/
void charged_2020(int sejp,int *nbdc);
int doublon_2020(int indcour,int *ptr_indice,int nb,char *ptr_diact,int lg);

/*******************************************************************************
Fonction chargep
Paramètres
Description:
Détermine les actes du RSS qui seront utilisés lors du groupage apès élimination
 des doublons (fonction doublon_acte) et des actes vides.
********************************************************************************/
void chargep_2020(int *nbac);
int doublon_acte_2020(int indcour, int *ptr_indice, int nb, char *ptr_actes_2020,int lg);

/****************************************************************************
Fonction findmax
Description:
Détermination du diagnostic principal dans le cas d’un séjour multi-unité.
****************************************************************************/
int findmax_2020();

/***RUM***/
typedef  struct
{
  #include "rum2020.h"
} RUM_2020 ;

/******************************************************************************
Fonction group
Description:
Initialise les variables nécessaires au groupage et réalise le groupage,
c.a.d. réalise le parcours de l’arbre en fonction des données du RSS.
********************************************************************************/
int group_2020 (char cret[],char cmd[],char ghm[]);

/*****************************************************************************
Fonctions utilisées pour le parcours de l'arbre de décision
******************************************************************************/

/*FG13 : suppression fonction _nbfaisceaux*/
int _dxsex_2020(), _dxcat_2020(),  _prbit_2020(),  _agegt_2020(),  _agecc_2020(),
      _pdxbit_2020(), _sdxbit_2020(), _adxbit_2020(), _odxbit_2020(), _oprbit_2020(),
      _sprbit_2020(), _dspst_2020(),  _xfrdd_2020(),   _dxceq_2020(),  _sexeq_2020(),
      _cksejr_2020(),_pdsngt_2020(),_hosp30_2020(),_dexbit_2020(),_tdxbit_2020(),_msoreq_2020(),
      _goto_2020(),_igsgt_2020(),_dsjlt_2020(),_DR_DA_2020(),_DA_DR_2020(),_dxcatvDR_2020(),
      _pdxbitDR_2020(),_dxceqDR_2020(),_AddErr_2020(),_dsjeq_2020(),_nbsea_2020(),_nadxbit_2020(),_nprbit_2020(),_prtypact_2020(),_invDPDR_2020(),_invDPDRreal_2020(),_saufdrdxbit_2020(),
      _grracinterv_2020(), _grracdeterm_2020(), _annulacreclass_2020(), _dxctdeq_2020(), _pdsngp_2020(), _dxctdeqsaufDR_2020();

/****************************************************************************
Fonctions utilitaires
convdate : transforme une date jjmmaaaa en 3 variables jj mm aa
callos   : calcule la duree en jour entre 2 date
hashcode : applique le hashcoding au diagnostic
free_ptr : libere les chaines allouées dynamiquement
carlong  : transforme une chaine de caracteres en  entier long non signe
nombre_jour_1980 : Rend le nombre de jour entre la date et le 01/01/1980
errfic   : met à jour les erreurs dues à des pb dans les tables
grpclose : Fermeture des Tables
accstb   : Récupère des informations dans un entete de table ou de sous-table
*****************************************************************************/
void convdate_2020 (char *adat,int *p_jj,int *p_mm,int *p_aa);
int callos_2020 (char *date,char *dats);
unsigned long carlong_2020 (unsigned char c[]);
int hashcode_2020(char *acode,int *hcod);
void free_ptr_2020();
int nombre_jour_1980_2020(char *date);
int errfic_2020 (char *rum[], int noerr,int type,GRP_RESULTAT_2020 *grp_res,GHS_VALORISATION_2020 *ghs_val,RSS_INFO_2020 *rss_inf);
void grpclose_2020();
void accstb_2020 (FILE *fich,long *_offset,int *_size,long *_taille);

int sej_rea_2020(int i, char *rum, int igsrum);

void analyse_actes_2020(RSS_INFO_2020 *rss_inf,GHS_VALORISATION_2020 *ghs_val, char *cmd, char *ghm );

int gradationHDJzero(GRP_ENTREE_2020 * grp_ent, RSS_INFO_2020 * rss_inf, GRP_RESULTAT_2020 * grp_res, int racine_exclue);


void init_valo_vide_2020(GHS_VALORISATION_2020 *ghs_val);
void rempli_valo_vide_2020(GHS_VALORISATION_2020 *ghs_val);
void rempli_valo_2020(int typetabliss,GHS_VALORISATION_2020 *ghs_val,RSS_INFO_2020 *rss_inf,GRP_RESULTAT_2020 *grp_res);

void init_info_vide_2020(RSS_INFO_2020 *rss_inf);
void  rempli_info_2020(RSS_INFO_2020 *rss_inf,GRP_ENTREE_2020*);
void rempli_info_rum_2020(RSS_INFO_2020 *rssinf,char *rum[]);

char getNiveauCMADiag_2020(int pindice,unsigned char[]);
unsigned char* getRacineInfo_2020(int codedcmd, int codedghm);
int calcule_niveau_2020(int codedcmd, int codedghm, char *pcmd, char *pghm, unsigned char *pracineinfo, int bseuilduree);

void recode_ghm_2020(char ghm[],int *_ghm);
void calcule_ghs_2020(int typetab, int codedcmd, int codedghm, int *ighs, int *bXsup, int *bXinf, int *ptypeexb);
void ghsminore_2020(int ighs, int *flagghsminor);

int DeterminerGroupeRacine_Poids_AG_2020();
void modifier_niveauCMD14_2020(char *niveau,int numtable);
void CalculeSupplementAntepartum_2020(GHS_VALORISATION_2020 *pghs_val);

/*FG13 Innovation*/
int ghm_Innovation_2020(int *codedcmd, int *codedghm);

/* VARIABLES GLOBALES */
RUM_2020 rumin_2020[tab_max];
int nbsej_2020;
int binversionDPDR_2020;

void getRUMs_2020(RUM_2020 **lesRums);

/* Variables globales de foncgrp */
char  *ptr_diags_2020;         /* Ptr sur zone contenant les diags P+S                  */
char  *ptr_actes_2020;         /* Ptr sur zone contenant les actes                      */
char  *ptr_nbactes_2020;       /* Ptr sur zone contenant les actes                      */
int inddiag_2020[tab_max+1];   /* Tab. contenant les positions des Diag P.              */
int indacte_2020[tab_max+1];   /* Tab. contenant la position des premiers actes         */
int *ptr_inddiag_2020;         /* Ptr sur zone contenant les indices des diags retenus  */
int *ptr_indacte_2020;         /* Ptr sur zone contenant les incdices des actes retenus */
int nbac_2020,nbdc_2020;
char  *ptr_dmask_2020;         /* Ptr sur la zone des masques de diags                  */
char  *ptr_omask_2020;         /* Ptr sir la zone des profils oms des diags             */
char  *ptr_emask_2020;
char  *ptr_amask_2020;         /* Ptr sur la zone des masques d'actes                   */

/*CD MAJ FG10*/
char  *ptr_typact_2020;        /* Ptr sur les types d'activité des actes                */

int(*funcbuf_2020 [MAX_FUNC_2020])()=
                { _dxsex_2020, _dxcat_2020, _prbit_2020, _agegt_2020, _agecc_2020, _pdxbit_2020,
                  _sdxbit_2020, _adxbit_2020, _odxbit_2020, _oprbit_2020, _sprbit_2020,
                  _dspst_2020, _xfrdd_2020, _dxceq_2020, _sexeq_2020, _cksejr_2020, _pdsngt_2020,
                  _hosp30_2020,_tdxbit_2020,_msoreq_2020,_goto_2020,_igsgt_2020,_dsjlt_2020,
                  _DR_DA_2020,_DA_DR_2020,_dxcatvDR_2020,_pdxbitDR_2020,_dxceqDR_2020,_AddErr_2020,
                  _dsjeq_2020,_nbsea_2020,_nadxbit_2020,_nprbit_2020,_prtypact_2020,_invDPDR_2020,_invDPDRreal_2020,_saufdrdxbit_2020,
                  0,/*fonction n°37 : nb faisceaux n'existe plus*/
                  _grracinterv_2020,/*n°38*/
                  _grracdeterm_2020,/*n°39*/
                  _annulacreclass_2020,/*n°40*/
                  _dxctdeq_2020,/*n°41*/
                  _pdsngp_2020,/*n°42*/
                  _dxctdeqsaufDR_2020/*n°43*/
} ;


int _Vrai_2020(int), _acteCara_2020(int), _typeUM_2020(int), _typeLitdedie_2020(int), _dureeTotSejStrictSup_2020(int), _dpcarac_2020(int), _dureeSejPartielleStrictSup_2020(int), _dascarac_2020(int), _agesup_2020(int param), _criteres_forfdiabete_2020(int param), _criteres_GHSgradation_2020(int param);
int(*funcbufghs_2020[MAX_FUNCGHS_2020])(int) = { _Vrai_2020, _acteCara_2020, _typeUM_2020, _typeLitdedie_2020, _dureeTotSejStrictSup_2020, _dpcarac_2020, _dureeSejPartielleStrictSup_2020, _dascarac_2020, _agesup_2020,_criteres_forfdiabete_2020,_criteres_GHSgradation_2020 };

int     _tdrg_2020;               /* contient le numéro de G.H.M.               */
int     _tmdc_2020;               /* contient le numéro de C.M.D.               */
int     _trtc_2020;               /* contient le code retour                    */
char    _modsor_2020[2];          /* adresse du code mode de sortie             */
int     age_an_2020,age_jour_2020;  /*âge en années et âge en jours               */
char    _sex_2020;                /* adresse du code sexe (1=homme, 2=femme)    */
char    _los_2020;                /* adresse de la durée de séjour              */
char    _hp30_2020;               /* adresse hosp ds les 30 jours               */
int     _pdsn_2020;               /* Poids de naissance                         */
int     _ms_2020;                 /* mode de sortie                             */
int     _ds_2020;                 /* Destination                                */
int     _igs_2020;                /* IGS                                        */
int     _dsj_2020;                /* Durée de séjour                            */
int     _OffsetDR_2020;           /*@@ Offset pour prendre en compte les DR   @@*/
int     _me_2020;                 /*@@ Mode d'entrée                          @@*/
int     _pr_2020;                 /*@@ Provenance                             @@*/
int     _seance_2020 ;            /* Nb de séances                              */

/*CD FG11 durée de séjour avant règle UHCD*/
int dsejavantuhcd_2020;

typedef unsigned char uchar;
/* Definition de la structure d'entete de table */
struct ENTETE_2020 {
    char sign[8];
    char verc[2];
    char revc[2];
    char datc[6];
    char nomt[8];
    uchar ffic;
    uchar nstb;
    uchar kgen[4];
} entete_table_2020;
/* Definition de la structure d'entete de sous-table */
struct entete_2020 {
    char sgstn[8];
    char vcstn[2];
    char rcstn[2];
    char dgstn[6];
    uchar nastn[2];
    /*CD FG11*/
    /*uchar lastn;*/
    uchar lastn[2];/*taille article de la table*/
    uchar lgstn[4];/*taille totale de la sous table*/
    uchar ofstn[4];
    uchar dpstn;
    uchar lpstn;
    char restn;
}entete_stab_2020;

/* Definition de la structure d'un noeud */
struct tree_2020  {
    int  subclus;       /* fonction / numéro de G.H.M. */
    int  param1;
    int  param2;
    int  nbranch;       /* nombre de sous-branches / 0 si G.H.M. atteint */
    int  pfils;         /* numero de noeud du premier fils */
}   treetab_2020;         /* définition de la structure de l'arbre */
int _stpos_2020;          /* position de départ du niveau suivant, par noeud */

FILE    *ptacte_2020;     /* pointeur du fichier tables d'actes */
FILE    *ptaccam_2020;    /* pointeur du fichier tables d'actes */

/* tableau de travail masques des diagnostics du RMG */
FILE    *ptmdiag10_2020;  /* pointeur du fichier tables de diagnostics CIM 10*/

FILE    *pttree_2020;     /* pointeur du fichier table arbre de groupage*/

int los_2020[tab_max];    /* durees de sejour par sejour service */
int or_2020[tab_max];     /* indicateur d'actes classants */

FILE *ghsinf_2020;
long _offstghs_2020;
int _lTailleArticleGHS_2020;
long _lTailleTotaleTableGHS_2020;

FILE *ghminf_2020;
long _offstghm_2020;
int _lTailleArticleGHM_2020;
long _lTailleTotaleTableGHM_2020;

FILE *srcdgac_2020;
long _offstSRCDGACTADENF_2020;
int _TailleArticleSRCDGACTADENF_2020;
long _TailleTotaleTableSRCDGACTADENF_2020;
int _NbArticlesSRCDGACTADENF_2020;

/*FG 12Maj : actes pour SRC pour les enfants seulement*/
long _offstSRCDGACTENFANT_2020;
int _TailleArticleSRCDGACTENFANT_2020;
long _TailleTotaleTableSRCDGACTENFANT_2020;
int _NbArticlesSRCDGACTENFANT_2020;

/*MAJ FG10 descripteur de fichier de la table gestes complémentaires*/
FILE    *ptfidx_gc_2020;     /* Pointeur de la table des gestes complémentaires */

/*FG13 : table de reference pour l'innovation*/
FILE *innovref_2020;
long _offstinnovref_2020;
int _TailleArticleInnovref_2020;
long _TailleTotaleTableInnovref_2020;
int _NbArticlesInnovref_2020;

/*FG1311d*/
FILE *tabcombi_2020;

long _offstcombiPoidsAG_2020;
int _TailleArticleCombiPoidsAG_2020;
long _TailleTotaleTableCombiPoidsAG_2020;
int _NbArticlesCombiPoidsAG_2020;

long _offstcombiAGNivA_2020;
int _TailleArticleCombiAGNivA_2020;
long _TailleTotaleTableCombiAGNivA_2020;
int _NbArticlesCombiAGNivA_2020;

long _offstcombiAGNivB_2020;
int _TailleArticleCombiAGNivB_2020;
long _TailleTotaleTableCombiAGNivB_2020;
int _NbArticlesCombiAGNivB_2020;

long _offstcombiAGNivC_2020;
int _TailleArticleCombiAGNivC_2020;
long _TailleTotaleTableCombiAGNivC_2020;
int _NbArticlesCombiAGNivC_2020;


/*FG11g*/
FILE *ptccamdesc_2020;              /*pointeur vers le fichier des actes CCAM deszcriptive*/
long  _poffpref_ccamdesc_2020;      /* Offset des prefixes d'actes CCAM desc*/
int   _psizepref_ccamdesc_2020;     /* longueur d'un prefixe d'acte CCAM desc*/

long    _poffnop_ccamdesc_2020;     /* Offset des numeros de masques actes CCAM desc*/
int     _psize_ccamdesc_2020;       /* longueur d'un article dans la 2eme sous table de CCAM desc*/

FILE *ptghsminor_2020;              /*pointeur vers le fichier des GHS minorés*/
long _offstghsminor_2020;
int _lTailleArticleGHSMINOR_2020;
long _lTailleTotaleTableGHSMINOR_2020;


/*MAJ FG 10*/
#define MAXERRBLOQ_2020 200

/*tableau trié des erreurs bloquantes, terminé par -1*/
/*FG13*/
int erreursbloq_2020[MAXERRBLOQ_2020]={10,11,13,14,15,16,17,19,20,21,23,24,25,26,27,28,29,30,32,33,34,35,36,37,39,40,41,42,43,45,46,49,50,51,52,53,54,55,56,57,58,59,62,67,68,70,71,73,78,79,82,94,95,103,110,113,114,115,116,117,118,119,120,
124,125,126,127,128,129,130,131,132,133,134,135,142,143,144,145,147,149,150,151,160,161,162,163,165,167,168,169,173,174,175,176,177,178,180,181,182,185,186,187,188,189,190,191,193,194,195,196,197,198,-1};

/*catégorie d'erreurs facultatives*/
int errCatGragradation_2020[2] = { ERR_INDICCATEGNBINTERV_ATTENDU, ERR_SEJOURNONJUSTIFIE_INCORRECT };
int errcatac_2020[4]={111,112,170,192};

/*FG11b
int errcatda_11b[2]={70,71};*/

int errcatum_2020[2]={100,101};

int errcatautres_2020[4]={152,153,154,223};

int ErrCtrl_2020[MAXERRCTRL_2020+1];   /* Vecteur d'erreur                                           */
int ErrArb_2020[MAXERRARB_2020+1];     /* Vecteur d'erreur                                           */
int ErrImpl_2020[MAXERRIMPL_2020+1];   /* Vecteur d'erreur                                           */


char versi_2020[3]; /* Pour conserver la valeur de version sans aller lire l'entête syst. */

int   *ptr_datdeb_2020;
int   *ptr_datfin_2020;

#define BORNEIGS_2020 15

/*MAJ FG9*/
/*zone de surveillance de très courte durée*/
int uhcd_2020;

/*FG11b*/
extern int agegest_renseigne_2020;/*sert aussi pour le groupage*/
/*FG13*/
extern int datedernregles_renseignee_2020;
extern int numero_innovation_2020;

extern int daZ37_2020;
extern int daaccouch_2020;

/*FG1311d*/
extern int nbjdateaccouch_2020;/*sert aussi pour la valorisation*/
extern int presenceActeAccouchement_2020;

/*23/06/05 conservation du pointeur sur RSS_INFO_11b*/
RSS_INFO_2020 *lrss_inf_2020;

/*conservation du pointeur sur GHS_VALORISATION_2020*/
GHS_VALORISATION_2020 *lghs_val_2020;

/*tableau d'exemple de transcodage de ascii vers ebcdic*/
const unsigned char ebcdic_to_ascii_2020 [256] =
{
    0x00,0x01,0x02,0x03,0x00,0x09,0x00,0x7f,0x00,0x00,0x00,0x0b,0x0c,0x0d,0x0e,0x0f
        ,0x10,0x11,0x12,0x13,0x00,0x00,0x08,0x00,0x18,0x19,0x00,0x00,0x1c,0x1d,0x1e,0x1f
        ,0x00,0x00,0x00,0x00,0x00,0x0a,0x17,0x1b,0x00,0x00,0x00,0x00,0x00,0x05,0x06,0x07
        ,0x00,0x00,0x16,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x14,0x15,0x00,0x1a
        ,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5b,0x2e,0x3c,0x28,0x2b,0x21
        ,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x24,0x2a,0x29,0x3b,0x5e
        ,0x2d,0x2f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x2c,0x25,0x5f,0x3e,0x3f
        ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x3a,0x23,0x40,0x27,0x3d,0x22
        ,0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x00,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x00,0x7e,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x7b,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x7d,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x00,0x00,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x00,0x00,0x00,0x00,0x00,0x00
        ,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x00,0x00,0x00,0x00,0x00,0xff
} ;

/*Identificateurs de table binaire*/
#define TABLE_ARBRE_2020 0
#define TABLE_DIAG_2020  1
#define TABLE_ACTES_2020 2
#define TABLE_GHS_2020 3
#define TABLE_GESTECOMP_2020 4
#define TABLE_REFAUTO_2020 5
#define TABLE_GHMINFO_2020 6
#define TABLE_SRCDGACT_2020 7
#define TABLE_INNOVREFER_2020 8
#define TABLE_COMBI_2020 9
#define TABLE_CCAMDESC_2020 10
#define TABLE_GHSMINOR_2020 11

#define NB_TABLES_BINAIRES_2020 12

/*nombre maximum de périodes pour la sélection des sous tables*/
#define MAX_PERIODE_2020 50

/*tableau donnant pour chaque table binaire les informations concernant
la sous table courante à utiliser pour le groupage du RSS. Cette sous-table est obtenue
grâce à l'offset n°periodecour dans le tableau offset_table*/
struct multiTable_2020
{
    FILE *fileptr; /*descripteur du fichier de la table binaire*/
    int datedeb[MAX_PERIODE_2020];/*date de début de période en nombre de jours depuis le 01/01/1980*/
    int datefin[MAX_PERIODE_2020];/*date de fin de période en nombre de jours depuis le 01/01/1980*/
    long offset_table[MAX_PERIODE_2020];/*offset de chaque sous table dans le fichier binaire*/
    int nbplages;/*nombre de période(s)*/
    int periodecour;/*période sélectionnée pour le RSS.*/
    /*CD 02/04/07*/
    int numerotarif_public[MAX_PERIODE_2020]; /*numéro du tarif public*/
    int numerotarif_prive[MAX_PERIODE_2020];  /*numéro du tarif prive*/

    /*FG 11e:TEM*/
    char *htable;
};
struct multiTable_2020 multitables_2020[NB_TABLES_BINAIRES_2020];

/*fonctions de gestion des tables multipériodes*/
int OuvrirTables_2020(char *pdirtab, char *versioncl);
int ChoixTables_2020(char *datesortie,char *versioncl);

/*CD 25/01/07 sejour principal du RSS*/
int sejourprinc_2020;

/*25/01/07 Fonctions d'informations sur les autorisations d'UM*/
int sejourdjAutorise_2020(int numsej,int ptypum);
int sejourAutoriseEnTypAutor_2020(int numsej,int typum,char datedeb[],char datefin[],int *nbjours);
extern int autorisation_valide_2020(int ptypum);
extern int sejouravdernAut_2020(int numsej);

struct _supplements_2020
{
    /*FG13 SUPRUM int nbjours_rea,nbjours_sc,nbjours_si,durnna,durnnb,durnnc,nbjours_rea_si,nbjours_sra,nbjours_reaped,nbjours_ssc;*/
    int nbjours_rea[tab_max],nbjours_sc[tab_max],nbjours_si[tab_max],durnna[tab_max],durnnb[tab_max],durnnc[tab_max],nbjours_rea_si[tab_max],nbjours_sra[tab_max],nbjours_reaped[tab_max],nbjours_ssc[tab_max];
};
typedef struct _supplements_2020 supplements_2020;

enum typesuppl_2020 /*conserver l'ordre*/
{
    typsupreaped=1,
    typsuprea=2,
    typsupnnc=3,
    typsupreasi=4,
    typsupsra=5,
    typsupsi=6,
    typsupnnb=7,
    typsupsc=8,
    typsupssc=9,
    typsupnna=10,
    typsupaucun=11,
    typsupMAX=12
};


#define MAX_FONCSUPP_2020 50
void _supnull_2020(int,RSS_INFO_2020*,supplements_2020*,int, int*), _supnn1_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*), _supnn2_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*), _supnn3_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),
     _suprep_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),_suprepanc_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),_suprea_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),_supsra_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),
     _supsi_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),_supsc_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*),_supssc_2020(int,RSS_INFO_2020*,supplements_2020*,int,int*);
void(*foncsupp_2020 [MAX_FONCSUPP_2020])(int,RSS_INFO_2020*,supplements_2020*,int,int*)={ _supnull_2020,_supnn1_2020,_supnn2_2020,_supnn3_2020,_suprea_2020,_supsra_2020,_supsc_2020,_supssc_2020,_supsi_2020,_suprep_2020,_suprepanc_2020};


/*CD 26/01/07*/
/*Fonction de calcul des séances*/
void calculeseances_2020(GRP_RESULTAT_2020 *grp_res,GHS_VALORISATION_2020 *ghs_val);

/*calcul des signatures*/
void signer_RSS_2020(unsigned char *rum[], int nbRUM,unsigned char *res);
unsigned int CalculerVersionFGTableBinaires_2020(GRP_ENTREE_2020 *pgrp_ent);
void calcule_signature_GHS_2020(RSS_INFO_2020 *prssinf, GHS_VALORISATION_2020 *pghs_val);


/*************************************************************************************************
Fonction grp_2020
  Fonction groupage version 13
  Cette fonction groupe un fichier de RSS au format 016 en format 116.
Paramètres :
*Paramètre 1 en entrée : Structure GRP_ENTREE contenant :
    -> rum2[tab_max] : Tableau de pointeurs sur des chaînes de caractères.
        Ces chaînes contiennent les différents RUM d’un RSS. Chaque chaîne (correspondant à un RUM) doit se terminer
        par un caractère nul (‘\0’). Ce tableau comporte au plus tab_max pointeurs, soit 99 éléments. Seul le format 016 est accepté.

    -> dirtab[BUFSIZ]: Chaîne de caractères contenant le chemin d’accès absolu, complet, pour atteindre le répertoire
        contenant les tables. Il doit être écrit de telle sorte que l’accès à une table donnée consiste à concaténer cette chaîne
        avec le nom de la table recherchée (terminé par '/').

    -> int ns : Entier contenant le nombre de séjours élémentaires du malade au cours de son séjour dans l’établissement hospitalier.
        Un appel à la fonction grp_2020() avec ce paramètre à 0 entraîne la fermeture des tables de groupage.

    -> int type_etab : Entier indiquant si l’établissement est ex-DGF (=1), ou ex-OQN (différent de 1).

*Paramètre 2 en sortie: Structure GRP_RESULTAT_2020 contenant :
    -> int ns_sor : Entier contenant le nombre de séjours après le traitement dans la fonction groupage. Ce nombre est désormais
        égal au nombre de séjours en entrée.

    -> int sejp : Entier destiné à recevoir le numéro du RUM désigné par le groupage (ou plus précisément par l'algorithme de choix du DP) comme contenant le
                  diagnostic principal et le diagnostic relié, en débutant la numérotation des RUM à 0.

    -> char version[3] : Chaîne de 3 caractères destinée à recevoir le numéro de version de la classification lu dans les tables (terminée par le caractère NULL).
    -> char cmd[3] : Chaîne de 3 caractères de long, destinée à recevoir le numéro de la CMD déterminée par le groupage (terminée par le caractère NULL).
    -> char ghm[5] : Chaîne de 5 caractères de long, destinée à recevoir le numéro du GHM déterminé par le groupage (terminée par le caractère NULL).
    -> char cret[4]: Chaîne de 4 caractères de long, destinée à recevoir le code retour déterminé par le groupage. Il s’agit de la
        valeur en caractère du retour de la fonction grp_2020().Cette chaîne est terminée par le caractère NULL.

    -> int ErrCtrl[MAXERRCTRL_2020] : Tableau de MAXERRCTRL entiers courts correspondant au vecteur des erreurs contenues dans chacun des RUM du RSS.
        Il est construit de la façon suivante :
            entier n°1    : (nr) nombre de RUM détectés dans le RSS
            entier n°2    : (n1) nombre d’erreurs détectées dans le RUM n°1…
            entier n°(nr+1)    : nombre d’erreurs détectées dans le RUM n°nr
            entiers n°(nr+2) à (nr+1+n1)    : codes des erreurs du RUM n°1
            entiers n°(nr+n1+2) à (nr+1+n1+n2)    : codes des erreurs du RUM n°2…

    -> int ErrArb[MAXERRARB_2020] : Tableau de MAXERRARB entiers courts correspondant au vecteur des erreurs détectéespar l’arbre de décision
        de la classification. Les erreurs sont stockées dans ce vecteur de manière séquentielle. Ce vecteur n’a pas de structure particulière.

    -> int ErrImpl[MAXERRIMPL_2020] : Tableau de MAXERRIMPL entiers courts correspondant au vecteur des erreurs dues à l’implémentation.
        Les erreurs sont stockées dans ce vecteur de manière séquentielle. Ce vecteur n’a pas de structure particulière.

*Paramètre 3 en sortie: Structure GHS_VALORISATION_2020 contenant :

    -> char ghs[5] : Chaîne de 5 caractères de long, destinée à recevoir le numéro du GHS déterminé à partir du GHM obtenu par le groupage,
        et des informations contenues dans le fichier ghsinfo.tab. Dans le cas de la dialyse pour les établissements ex-OQN,
        cette variable contient le libellé du forfait dialyse après SROS (commençant par D).
        Pour les établissements ex-DGF dans le cas de la dialyse, elle contient le numéro du GHS après SROS.

    -> int nbjxsup : Entier contenant le nombre de journées au-delà de la borne extrême haute pour le GHS considéré.
    -> int sejxinf : Entier contenant 0 si la durée du séjour est supérieure à la borne extrême basse du GHS. Sinon, égal à 1 si exb forfaitaire, 2 si exb à la journée, 0 si pas de minoration
    -> float nbjxinf : Décimal égal à borneinf-DS si borneinf>DS,0 sinon
    -> int buhcd : Entier utilisé comme booléen, égal à 1 si le GHS applique la règle spécifique aux UHCD, 0 sinon.


    -> int nbjreaR[tab_max];Entier contenant le nombre de suppléments de réanimation pour le RUM numéro i
    -> int nbrepR[tab_max];Entier contenant le nombre de suppléments de réanimation pédiatrique pour le RUM numéro i
    -> int nbjsrcR[tab_max];Entier contenant le nombre de suppléments de surveillance continue pour le RUM numéro i
    -> int nbjstf_issudereaR[tab_max];Entier contenant le nombre de suppléments soins intensifs provenant de la réanimation pour le RUM numéro i
    -> int nbjstf_totR[tab_max];Entier contenant le nombre total de suppléments soins intensifs, y compris les suppléments provenant de la réanimation, pour le RUM numéro i
    -> int nbjnnaR[tab_max];Entier contenant le nombre de journées supplémentaires pour la néonatologie pour le RUM numéro i
    -> int nbjnnbR[tab_max];Entier contenant le nombre de journées supplémentaires pour la néonatologie avec soins intensifs pour le RUM numéro i
    -> int nbjnncR[tab_max];Entier contenant le nombre de journées supplémentaires pour la réanimation néonatale pour le RUM numéro i

    -> int nbjrea : Entier contenant le nombre suppléments de réanimation.
    -> int nbrep : Entier contenant le nombre de suppléments de réanimation pédiatrique.
    -> int nbjsrc : Entier contenant le nombre de suppléments de surveillance continue.
    -> int nbjstf_issuderea : Entier contenant le nombre suppléments de soins intensifs provenant de la réanimation.
    -> int nbjstf_tot : Entier contenant le nombre total de suppléments de soins intensifs, y compris les suppléments provenant de la réanimation.

    -> int nbjnna: Entier contenant le nombre de suppléments pour la néonatologie (type 2A).
    -> int nbjnnb: Entier contenant le nombre de suppléments pour la néonatologie avec soins intensifs (type 2B).
    -> int nbjnnc: Entier contenant le nombre de suppléments pour la réanimation néonatale (type 3).

    -> int nbac687: Entier contenant le nombre d’actes supplémentaires (hors séances) de radiothérapie menant au GHS 9610.
    -> int nbac688: Entier contenant le nombre d’actes supplémentaires (hors séances) de radiothérapie menant au GHS 9611.
    -> int nbac689: Entier contenant le nombre d’actes supplémentaires (hors séances) de radiothérapie menant au GHS 9612.

    -> int nbacRX1; Entier contenant le nombre de suppléments "Gama Knife" (GHS 6523)
    -> int nbacRX2; Entier contenant le nombre de suppléments "Cyber Knife" (GHS 9621)
    -> int nbacRX3; Entier contenant le nombre de suppléments "aphérèse" (GHS 9615)

    -> int nbRCMI; Entier contenant le nombre d'actes supplémentaires menant au GHS 9622 (RCMI)

    -> int nbactsupp9623; Entier contenant le nombre d'actes supplémentaires menant au GHS 9623
    -> int nbactsupp9624; Entier contenant le nombre d'actes supplémentaires menant au GHS 9624
    -> int nbactsupp9625; Entier contenant le nombre d'actes supplémentaires menant au GHS 9625
    -> int nbactsupp9631; Entier contenant le nombre d'actes supplémentaires menant au GHS 9631
    -> int nbactsupp9632; Entier contenant le nombre d'actes supplémentaires menant au GHS 9632
    -> int nbactsupp9633; Entier contenant le nombre d'actes supplémentaires menant au GHS 9633

    -> int acprlvtorg: Entier contenant la catégorie de prélèvement d’organes du séjour considéré. (0, 1, 2 ou 3).

    -> int pih_a : Entier utilisé comme booléen, contenant 0 en cas d’absence de code diagnostic associé Z75.80, 1 sinon.
    -> int pih_b : Entier utilisé comme booléen, contenant 0 si le mode d’entrée et le mode de sortie est égal à 0, 1 sinon.

    -> char prestdial[5] : chaine de 5 caractères utilisée pour les établissements ex-OQN uniquement: contient la prestation dialyse associée au GHM obtenu.

    -> int nbhd : Entier contenant le nombre d'actes supplémentaires hors séances pour l'hémodialyse.
    -> int ent1 : Entier contenant le nombre d'actes supplémentaires hors séances pour les entraînements à la dialyse péritonéale automatisée.
    -> int ent2 : Entier contenant le nombre d'actes supplémentaires hors séances pour les entraînements à la dialyse péritonéale continue ambulatoire.
    -> int ent3 : Entier contenant le nombre d'actes supplémentaires hors séances pour les entraînements à l'hémodialyse.
    -> int nbchhypb : Entier contenant le nombre de caissons hyperbare.

    -> int nbseance : Entier contenant le nombre total de séances
    -> int nbprot : Entier contenant le nombre d'actes supplémentaires menant dans le GHS 9619 (protonthérapie)
    -> int nbict : Entier contenant le nombre d'actes supplémentaires menant dans le GHS 9620 (ICT)


*Paramètre 4 en sortie: Structure RSS_INFO_2020 contenant :
    -> int agea : Entier contenant l’âge en années calculée par la fonction groupage
    -> int agej : Entier contenant l’âge en jours calculée par la fonction groupage
    -> int dstot: Entier contenant la durée totale de séjour calculée par la fonction groupage

    -> DIAGLIST_2020 diaglist    est une structure contenant :
        -    int nbdiag : entier contenant le nombre de diagnostics différents.
        -    char *listdiag : chaine de caractères contenant la liste de ces diagnostics avec en première position le diagnostic principal,
            puis en deuxième position le diagnostic relié, puis l’ensemble des diagnostics associés retenus pour le RSS.

    -> ACTLIST_2020 actlist est une structure contenant :
        -    int nbac : entier contenant le nombre de zones d’actes du RSS.
        -    char *listzac : chaine de caractères contenant la liste des zones d’actes .
        La présentation de ces zones d’actes est identique à celle du RUM.

    -> INFO_RUM_2020 tabval[tab_max] est un tableau de tab_max éléments contenant pour chaque RUM :
        - int brea : entier égal à 1 si le RUM contient des actes marqueurs de réanimation, 0 sinon.
        - int bsc : entier égal à 1 si le RUM possède les caractéristiques pour obtenir des suppléments de SC sans considérer le type d'autorisation de l'UM, 0 sinon.
        - char typum[2] : chaine de 2 caractères représentant le type d'autorisation du dernier jour passé dans l'Unité Médicale.
        - int dsp : entier contenant la durée de séjour partielle du RUM.

    -> char sigfg[5] : chaine de 5 caractères contenant la signature de la FG : tables binaires + CLS_ID
    -> char sigrss[20] : chaine de 20 caractères contenant la signature du RSS
    -> char sigrssghsfg[8];chaine de 8 caractères contenant la signature de sigrss et du GHS et de sigfg

Valeur en retour : entier
        la fonction grp_2020() renvoie un entier égal à zéro si les contrôles n’ont rien détecté d’anormal, et si le
        groupage n’a pas retrouvé d’anomalies. Dans le cas contraire, la valeur retourné est le numéro de
        l’erreur détectée (sauf pour certaines erreurs de controles non bloquantes non retournées par la fonction groupage).

        Toute erreur de contrôle dont la valeur est contenue dans la liste erreursbloq_2020 du fichier fg13.h
        est irrémédiable, et ne permet pas d’obtenir le groupage du R.S.S. Les erreurs de contrôle non contenues dans cette liste
        ne sont pas graves (autrement dit non bloquantes) en terme de groupage (par exemple : date système improbable).De telles erreurs ne sont détectées que par
        l’analyse du vecteur décrit plus haut (ErrCtrl).
        Les erreurs d'implémentation (tableau ErrImpl) sont aussi irrémédiables
        Les erreurs de parcours de l'arbre de décision (tableau ErrArb) peuvent être bloquantes ou non bloquantes.
***********************************************************************************************************/

int grp_2020 (GRP_ENTREE_2020 grp_ent,GRP_RESULTAT_2020 *grp_res,GHS_VALORISATION_2020 *ghs_val,RSS_INFO_2020 *rss_inf);

/*FG 11e:TEM*/
/*pointeurs sur buffers contenant les tables binaires*/
char *h_ptaccam_2020;
char *h_ptmdiag10_2020;
char *h_pttree_2020;
char *h_ghsinf_2020;
char *h_ptfidx_gc_2020;
char *h_ghminf_2020;
char *h_srcdgac_2020;
char *h_innovref_2020;
char *h_tabcombi_2020;
char *h_refauto_2020;
char *h_ptccamdesc_2020;
char *h_ptghsminor_2020;

/*FG 2019 RAAC*/
int critereRAAC_2020;
int indicateurRAAC_2020;

/*type hospitalisation 1er RUM*/
char typehospit1erRUM_exp_2020;
/*séjour éligible au forfait diabète*/
int sejEliForfDiabete_2020;


int gradationHDJ_2020; /* =1 si Hdj 0j et RSS non exclus de la gradation HDJ (avant tests des variables du RUM concernant la gradation. -1 si séjour de plus de 0j*/
int sejourHdjexterne_2020; /*le séjour d'Hdj relève de l'externe*/



