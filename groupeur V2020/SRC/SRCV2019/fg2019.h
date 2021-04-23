/*identifiant de la fonction groupage*/
#define CLS_ID_2019 "57b32971-c231-4baa-ac02-2d66cc83d347"

#define tab_max 99 /* Nombre maximum de RUM */

#define MAX_FUNC_2019 44     /* Nombre de fonctions max appel�es par l'arbre de d�cision */

#define MAX_FUNCGHS_2019 12  /* Nombre de fonctions max pour le calcul des GHS */

#define caras '+'

#define MAX_NBJ_RUM_2019 2000

/*FG13*/
#define TAILLE_CODE_INNOV_2019 15

/* definitions des tailles des tables */

long _doffpref_2019;       /* Offset des prefixes de diag*/
int  _dsizepref_2019;         /* longueur d'un prefixe de diagnostic */

long _doffnop10_2019;      /* Offset des numeros de masques */
int  _dsize_2019;             /* longueur d'un no de masque de diagnostic */
long _doffpro10_2019;      /* Offset des masques caracteristiques*/
int _dxmsize_2019;         /* Longueur des masques de cara. de diag */
long _doffoms10_2019;      /* Offset des masques OMS */
long _doffexc10_2019;      /* Offset des liste d'exclusion */
int _excsize10_2019;       /* Taille des masques de liste d'exclusion */
#define _tpref_2019 2         /* Taille des prefixes */
#define _tsuf_2019  9         /* Taille des suffixes */
#define _nbpref_2019 2600     /* Nombre de prefixes */
int _prfsize_2019;         /* longueur d'un profil OMS */
int lgdiag_2019;           /* Longueur d'un diagnostic */
int lgacte_2019;           /* Longueur d'un acte */

long  _poffnop_2019;            /* Offset des numeros de masques actes*/
int _psize_2019;                /* longueur no de masque  */
long  _poffpro_2019;              /* Offset des masques caracteristiques actes*/
int _prmsize_2019;              /* longueur d'un masque d'acte */


long  _poffpref_ccam_2019;      /* Offset des prefixes d'actes*/
int   _psizepref_ccam_2019;     /* longueur d'un prefixe d'acte*/

long    _poffnop_ccam_2019;     /* Offset des numeros de masques actes*/
int     _psize_ccam_2019;         /* longueur no de masque  */
long    _poffpro_ccam_2019;      /* Offset des masques caracteristiques actes*/
#define _tpref_ccam_2019 2         /* Taille des prefixes */
#define _tsuf_ccam_2019  9         /* Taille des suffixes */


int    _trsize_2019   ;        /* Taille des informations d'un noeud */
long     _toffset_2019;        /* Taille de l'offset des numeros de noeud */

#include "tables2019.h"      /* Noms des tables en clair */
#include "erreurs2019.h"

/* PROTOTYPE DES FONCTIONS UTILISEES */
/*****************************************************************************
* Fonction FreeErr :                                                         *
* Description :                                                              *
* Fonction utilitaire permettant de sortir proprement de la fonction grp     *
******************************************************************************/
int FreeErr_2019 (char *rum[], int noerr,int type,GRP_RESULTAT_2019 *grp_res,GHS_VALORISATION_2019 *ghs_val,RSS_INFO_2019 *rss_inf);
/*****************************************************************************
* Fonction callage :                                                         *
* Description :                                                              *
* Calcul l'age anniversaire entre la date de naissance et la date  d'entr�e. *
* En jour et en ann�e                                                        *
******************************************************************************/
void callage_2019 (char *datnais,char *datsor);


/****************************************************************************
Fonction controle
Description:
Fonction cont�lant la r�alisation du controle d�un RSS :
    - Appel de la date syst�me (fonction calldate)
    - Lancement de la fonction de contr�le des champs des RUMs (ctlsej)
    - Lancement de la fonction de cont�le OMS (fonction foms)
******************************************************************************/
int controle_2019();

/*****************************************************************************
Fonction optree
Param�tres
Description:
Ouverture de la table de l�arbre de d�cision
******************************************************************************/
/*CD 22/01/07 int optree_11b(char *dirtab);
*/
int optree_2019();

/*****************************************************************************
Fonction opdiag10
Param�tres
Description:
Ouverture de la table des diagnostics
*******************************************************************************/
/*CD 22/01/07 int opdiag10_2019(char *dirtab);
*/
int opdiag10_2019();

/*****************************************************************************
Fonction opacte
Param�tres
Description:
Ouverture de la table des actes
******************************************************************************/
int opacte_2019(char *dirtab);

/*****************************************************************************
Fonction prmap
Description:
R�cup�ration des informations n�cessaire au groupage concernant un acte donn�.
Apr�s avoir calcul� l�offset pour cet acte (fonction convacte), lecture dans la
tables des actess des informations concernant le groupage.
******************************************************************************/
int prmap_ccam_2019(int indice);


/*****************************************************************************
Fonction dxmap10_2019
Description:
R�cup�ration des informations n�cessaire au groupage concernant un diagnostic
donn�. Apr�s avoir calcul� l�offset pour ce diagnostic (fonction convdiag10_2019),
lecture dans la tables des diagnostics des informations concernant le groupage,
l�appartenance aux listes d�exclusion et les contr�les de coh�rence OMS.
*****************************************************************************/
int dxmap10_2019(int indice,char sexe);
int convdiag10_2019 (char *adrcod,unsigned char prof[]);


/*****************************************************************************
Fonction trmap
Description
Lit dans la table de l�arbre les informations concernant la marche � suivre :
n� de la fonction � appeler, n� de base du prochain noeud � atteindre,
param�tres �ventuels de la fonction.
La description des fonctions de tests du groupage sont d�crit dans les sources.
******************************************************************************/
int trmap_2019(int nn);

/*******************************************************************************
Fonction charged
Description:
D�termine  les diagnostics du RSS qui seront utilis�s lors du groupage, apr�s
�limination des doublons (fonction doublon) et des diagnostics vides.
********************************************************************************/
void charged_2019(int sejp,int *nbdc);
int doublon_2019(int indcour,int *ptr_indice,int nb,char *ptr_diact,int lg);

/*******************************************************************************
Fonction chargep
Param�tres
Description:
D�termine les actes du RSS qui seront utilis�s lors du groupage ap�s �limination
 des doublons (fonction doublon_acte) et des actes vides.
********************************************************************************/
void chargep_2019(int *nbac);
int doublon_acte_2019(int indcour, int *ptr_indice, int nb, char *ptr_actes_2019,int lg);

/****************************************************************************
Fonction findmax
Description:
D�termination du diagnostic principal dans le cas d�un s�jour multi-unit�.
****************************************************************************/
int findmax_2019();

/***RUM***/
typedef  struct
{
  #include "rum2019.h"
} RUM_2019 ;

/******************************************************************************
Fonction group
Description:
Initialise les variables n�cessaires au groupage et r�alise le groupage,
c.a.d. r�alise le parcours de l�arbre en fonction des donn�es du RSS.
********************************************************************************/
int group_2019 (char cret[],char cmd[],char ghm[]);

/*****************************************************************************
Fonctions utilis�es pour le parcours de l'arbre de d�cision
******************************************************************************/

/*FG13 : suppression fonction _nbfaisceaux*/
int _dxsex_2019(), _dxcat_2019(),  _prbit_2019(),  _agegt_2019(),  _agecc_2019(),
      _pdxbit_2019(), _sdxbit_2019(), _adxbit_2019(), _odxbit_2019(), _oprbit_2019(),
      _sprbit_2019(), _dspst_2019(),  _xfrdd_2019(),   _dxceq_2019(),  _sexeq_2019(),
      _cksejr_2019(),_pdsngt_2019(),_hosp30_2019(),_dexbit_2019(),_tdxbit_2019(),_msoreq_2019(),
      _goto_2019(),_igsgt_2019(),_dsjlt_2019(),_DR_DA_2019(),_DA_DR_2019(),_dxcatvDR_2019(),
      _pdxbitDR_2019(),_dxceqDR_2019(),_AddErr_2019(),_dsjeq_2019(),_nbsea_2019(),_nadxbit_2019(),_nprbit_2019(),_prtypact_2019(),_invDPDR_2019(),_invDPDRreal_2019(),_saufdrdxbit_2019(),
      _grracinterv_2019(), _grracdeterm_2019(), _annulacreclass_2019(), _dxctdeq_2019(), _pdsngp_2019(), _dxctdeqsaufDR_2019();

/****************************************************************************
Fonctions utilitaires
convdate : transforme une date jjmmaaaa en 3 variables jj mm aa
callos   : calcule la duree en jour entre 2 date
hashcode : applique le hashcoding au diagnostic
free_ptr : libere les chaines allou�es dynamiquement
carlong  : transforme une chaine de caracteres en  entier long non signe
nombre_jour_1980 : Rend le nombre de jour entre la date et le 01/01/1980
errfic   : met � jour les erreurs dues � des pb dans les tables
grpclose : Fermeture des Tables
accstb   : R�cup�re des informations dans un entete de table ou de sous-table
*****************************************************************************/
void convdate_2019 (char *adat,int *p_jj,int *p_mm,int *p_aa);
int callos_2019 (char *date,char *dats);
unsigned long carlong_2019 (unsigned char c[]);
int hashcode_2019(char *acode,int *hcod);
void free_ptr_2019();
int nombre_jour_1980_2019(char *date);
int errfic_2019 (char *rum[], int noerr,int type,GRP_RESULTAT_2019 *grp_res,GHS_VALORISATION_2019 *ghs_val,RSS_INFO_2019 *rss_inf);
void grpclose_2019();
void accstb_2019 (FILE *fich,long *_offset,int *_size,long *_taille);

int sej_rea_2019(int i, char *rum, int igsrum);

void analyse_actes_2019(RSS_INFO_2019 *rss_inf,GHS_VALORISATION_2019 *ghs_val, char *cmd, char *ghm );

void init_valo_vide_2019(GHS_VALORISATION_2019 *ghs_val);
void rempli_valo_vide_2019(GHS_VALORISATION_2019 *ghs_val);
void rempli_valo_2019(int typetabliss,GHS_VALORISATION_2019 *ghs_val,RSS_INFO_2019 *rss_inf,GRP_RESULTAT_2019 *grp_res);

void init_info_vide_2019(RSS_INFO_2019 *rss_inf);
void  rempli_info_2019(RSS_INFO_2019 *rss_inf,GRP_ENTREE_2019*);
void rempli_info_rum_2019(RSS_INFO_2019 *rssinf,char *rum[]);

char getNiveauCMADiag_2019(int pindice,unsigned char[]);
unsigned char* getRacineInfo(int codedcmd, int codedghm);
int calcule_niveau_2019(int codedcmd, int codedghm, char *pcmd, char *pghm, unsigned char *pracineinfo, int bseuilduree);

void recode_ghm_2019(char ghm[],int *_ghm);
void calcule_ghs_2019(int typetab, int codedcmd, int codedghm, int *ighs, int *bXsup, int *bXinf, int *ptypeexb);
void ghsminore_2019(int ighs, int *flagghsminor);

int DeterminerGroupeRacine_Poids_AG_2019();
void modifier_niveauCMD14_2019(char *niveau,int numtable);
void CalculeSupplementAntepartum_2019(GHS_VALORISATION_2019 *pghs_val);

/*FG13 Innovation*/
int ghm_Innovation_2019(int *codedcmd, int *codedghm);

/* VARIABLES GLOBALES */
RUM_2019 rumin_2019[tab_max];
int nbsej_2019;
int binversionDPDR_2019;

void getRUMs_2019(RUM_2019 **lesRums);

/* Variables globales de foncgrp */
char  *ptr_diags_2019;         /* Ptr sur zone contenant les diags P+S                  */
char  *ptr_actes_2019;         /* Ptr sur zone contenant les actes                      */
char  *ptr_nbactes_2019;       /* Ptr sur zone contenant les actes                      */
int inddiag_2019[tab_max+1];   /* Tab. contenant les positions des Diag P.              */
int indacte_2019[tab_max+1];   /* Tab. contenant la position des premiers actes         */
int *ptr_inddiag_2019;         /* Ptr sur zone contenant les indices des diags retenus  */
int *ptr_indacte_2019;         /* Ptr sur zone contenant les incdices des actes retenus */
int nbac_2019,nbdc_2019;
char  *ptr_dmask_2019;         /* Ptr sur la zone des masques de diags                  */
char  *ptr_omask_2019;         /* Ptr sir la zone des profils oms des diags             */
char  *ptr_emask_2019;
char  *ptr_amask_2019;         /* Ptr sur la zone des masques d'actes                   */

/*CD MAJ FG10*/
char  *ptr_typact_2019;        /* Ptr sur les types d'activit� des actes                */

int(*funcbuf_2019 [MAX_FUNC_2019])()=
                { _dxsex_2019, _dxcat_2019, _prbit_2019, _agegt_2019, _agecc_2019, _pdxbit_2019,
                  _sdxbit_2019, _adxbit_2019, _odxbit_2019, _oprbit_2019, _sprbit_2019,
                  _dspst_2019, _xfrdd_2019, _dxceq_2019, _sexeq_2019, _cksejr_2019, _pdsngt_2019,
                  _hosp30_2019,_tdxbit_2019,_msoreq_2019,_goto_2019,_igsgt_2019,_dsjlt_2019,
                  _DR_DA_2019,_DA_DR_2019,_dxcatvDR_2019,_pdxbitDR_2019,_dxceqDR_2019,_AddErr_2019,
                  _dsjeq_2019,_nbsea_2019,_nadxbit_2019,_nprbit_2019,_prtypact_2019,_invDPDR_2019,_invDPDRreal_2019,_saufdrdxbit_2019,
                  0,/*fonction n�37 : nb faisceaux n'existe plus*/
                  _grracinterv_2019,/*n�38*/
                  _grracdeterm_2019,/*n�39*/
                  _annulacreclass_2019,/*n�40*/
                  _dxctdeq_2019,/*n�41*/
                  _pdsngp_2019,/*n�42*/
                  _dxctdeqsaufDR_2019/*n�43*/
} ;


int _Vrai_2019(int), _acteCara_2019(int), _typeUM_2019(int), _typeLitdedie_2019(int), _dureeTotSejStrictSup_2019(int), _dpcarac_2019(int), _dureeSejPartielleStrictSup_2019(int), _dascarac_2019(int), _agesup_2019(int param), _criteres_forfdiabete_2019(int param);
int(*funcbufghs_2019[MAX_FUNCGHS_2019])(int) = { _Vrai_2019, _acteCara_2019, _typeUM_2019, _typeLitdedie_2019, _dureeTotSejStrictSup_2019, _dpcarac_2019, _dureeSejPartielleStrictSup_2019, _dascarac_2019, _agesup_2019,_criteres_forfdiabete_2019 };

int     _tdrg_2019;               /* contient le num�ro de G.H.M.               */
int     _tmdc_2019;               /* contient le num�ro de C.M.D.               */
int     _trtc_2019;               /* contient le code retour                    */
char    _modsor_2019[2];          /* adresse du code mode de sortie             */
int     age_an_2019,age_jour_2019;  /*�ge en ann�es et �ge en jours               */
char    _sex_2019;                /* adresse du code sexe (1=homme, 2=femme)    */
char    _los_2019;                /* adresse de la dur�e de s�jour              */
char    _hp30_2019;               /* adresse hosp ds les 30 jours               */
int     _pdsn_2019;               /* Poids de naissance                         */
int     _ms_2019;                 /* mode de sortie                             */
int     _ds_2019;                 /* Destination                                */
int     _igs_2019;                /* IGS                                        */
int     _dsj_2019;                /* Dur�e de s�jour                            */
int     _OffsetDR_2019;           /*@@ Offset pour prendre en compte les DR   @@*/
int     _me_2019;                 /*@@ Mode d'entr�e                          @@*/
int     _pr_2019;                 /*@@ Provenance                             @@*/
int     _seance_2019 ;            /* Nb de s�ances                              */

/*CD FG11 dur�e de s�jour avant r�gle UHCD*/
int dsejavantuhcd_2019;

typedef unsigned char uchar;
/* Definition de la structure d'entete de table */
struct ENTETE_2019 {
    char sign[8];
    char verc[2];
    char revc[2];
    char datc[6];
    char nomt[8];
    uchar ffic;
    uchar nstb;
    uchar kgen[4];
} entete_table_2019;
/* Definition de la structure d'entete de sous-table */
struct entete_2019 {
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
}entete_stab_2019;

/* Definition de la structure d'un noeud */
struct tree_2019  {
    int  subclus;       /* fonction / num�ro de G.H.M. */
    int  param1;
    int  param2;
    int  nbranch;       /* nombre de sous-branches / 0 si G.H.M. atteint */
    int  pfils;         /* numero de noeud du premier fils */
}   treetab_2019;         /* d�finition de la structure de l'arbre */
int _stpos_2019;          /* position de d�part du niveau suivant, par noeud */

FILE    *ptacte_2019;     /* pointeur du fichier tables d'actes */
FILE    *ptaccam_2019;    /* pointeur du fichier tables d'actes */

/* tableau de travail masques des diagnostics du RMG */
FILE    *ptmdiag10_2019;  /* pointeur du fichier tables de diagnostics CIM 10*/

FILE    *pttree_2019;     /* pointeur du fichier table arbre de groupage*/

int los_2019[tab_max];    /* durees de sejour par sejour service */
int or_2019[tab_max];     /* indicateur d'actes classants */

FILE *ghsinf_2019;
long _offstghs_2019;
int _lTailleArticleGHS_2019;
long _lTailleTotaleTableGHS_2019;

FILE *ghminf_2019;
long _offstghm_2019;
int _lTailleArticleGHM_2019;
long _lTailleTotaleTableGHM_2019;

FILE *srcdgac_2019;
long _offstSRCDGACTADENF_2019;
int _TailleArticleSRCDGACTADENF_2019;
long _TailleTotaleTableSRCDGACTADENF_2019;
int _NbArticlesSRCDGACTADENF_2019;

/*FG 12Maj : actes pour SRC pour les enfants seulement*/
long _offstSRCDGACTENFANT_2019;
int _TailleArticleSRCDGACTENFANT_2019;
long _TailleTotaleTableSRCDGACTENFANT_2019;
int _NbArticlesSRCDGACTENFANT_2019;

/*MAJ FG10 descripteur de fichier de la table gestes compl�mentaires*/
FILE    *ptfidx_gc_2019;     /* Pointeur de la table des gestes compl�mentaires */

/*FG13 : table de reference pour l'innovation*/
FILE *innovref_2019;
long _offstinnovref_2019;
int _TailleArticleInnovref_2019;
long _TailleTotaleTableInnovref_2019;
int _NbArticlesInnovref_2019;

/*FG1311d*/
FILE *tabcombi_2019;

long _offstcombiPoidsAG_2019;
int _TailleArticleCombiPoidsAG_2019;
long _TailleTotaleTableCombiPoidsAG_2019;
int _NbArticlesCombiPoidsAG_2019;

long _offstcombiAGNivA_2019;
int _TailleArticleCombiAGNivA_2019;
long _TailleTotaleTableCombiAGNivA_2019;
int _NbArticlesCombiAGNivA_2019;

long _offstcombiAGNivB_2019;
int _TailleArticleCombiAGNivB_2019;
long _TailleTotaleTableCombiAGNivB_2019;
int _NbArticlesCombiAGNivB_2019;

long _offstcombiAGNivC_2019;
int _TailleArticleCombiAGNivC_2019;
long _TailleTotaleTableCombiAGNivC_2019;
int _NbArticlesCombiAGNivC_2019;


/*FG11g*/
FILE *ptccamdesc_2019;              /*pointeur vers le fichier des actes CCAM deszcriptive*/
long  _poffpref_ccamdesc_2019;      /* Offset des prefixes d'actes CCAM desc*/
int   _psizepref_ccamdesc_2019;     /* longueur d'un prefixe d'acte CCAM desc*/

long    _poffnop_ccamdesc_2019;     /* Offset des numeros de masques actes CCAM desc*/
int     _psize_ccamdesc_2019;       /* longueur d'un article dans la 2eme sous table de CCAM desc*/

FILE *ptghsminor_2019;              /*pointeur vers le fichier des GHS minor�s*/
long _offstghsminor_2019;
int _lTailleArticleGHSMINOR_2019;
long _lTailleTotaleTableGHSMINOR_2019;


/*MAJ FG 10*/
#define MAXERRBLOQ_2019 140

/*tableau tri� des erreurs bloquantes, termin� par -1*/
/*FG13*/
int erreursbloq_2019[MAXERRBLOQ_2019]={10,11,13,14,15,16,17,19,20,21,23,24,25,26,27,28,29,30,32,33,34,35,36,37,39,40,41,42,43,45,46,49,50,51,52,53,54,55,56,57,58,59,62,67,68,70,71,73,78,79,82,94,95,103,110,113,114,115,116,117,118,119,120,
124,125,126,127,128,129,130,131,132,133,134,135,142,143,144,145,147,149,150,151,160,161,162,163,165,167,168,169,173,174,175,176,177,178,180,181,182,185,186,187,188,189,190,191,-1};

/*cat�gorie d'erreurs facultatives*/
int errcatac_2019[4]={111,112,170,192};

/*FG11b
int errcatda_11b[2]={70,71};*/

int errcatum_2019[2]={100,101};

int errcatautres_2019[4]={152,153,154,223};

int ErrCtrl_2019[MAXERRCTRL_2019+1];   /* Vecteur d'erreur                                           */
int ErrArb_2019[MAXERRARB_2019+1];     /* Vecteur d'erreur                                           */
int ErrImpl_2019[MAXERRIMPL_2019+1];   /* Vecteur d'erreur                                           */


char versi_2019[3]; /* Pour conserver la valeur de version sans aller lire l'ent�te syst. */

int   *ptr_datdeb_2019;
int   *ptr_datfin_2019;

#define BORNEIGS_2019 15

/*MAJ FG9*/
/*zone de surveillance de tr�s courte dur�e*/
int uhcd_2019;

/*FG11b*/
extern int agegest_renseigne_2019;/*sert aussi pour le groupage*/
/*FG13*/
extern int datedernregles_renseignee_2019;
extern int numero_innovation_2019;

extern int daZ37_2019;
extern int daaccouch_2019;

/*FG1311d*/
extern int nbjdateaccouch_2019;/*sert aussi pour la valorisation*/
extern int presenceActeAccouchement_2019;

/*23/06/05 conservation du pointeur sur RSS_INFO_11b*/
RSS_INFO_2019 *lrss_inf_2019;

/*conservation du pointeur sur GHS_VALORISATION_2019*/
GHS_VALORISATION_2019 *lghs_val_2019;

/*tableau d'exemple de transcodage de ascii vers ebcdic*/
const unsigned char ebcdic_to_ascii_2019 [256] =
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
#define TABLE_ARBRE_2019 0
#define TABLE_DIAG_2019  1
#define TABLE_ACTES_2019 2
#define TABLE_GHS_2019 3
#define TABLE_GESTECOMP_2019 4
#define TABLE_REFAUTO_2019 5
#define TABLE_GHMINFO_2019 6
#define TABLE_SRCDGACT_2019 7
#define TABLE_INNOVREFER_2019 8
#define TABLE_COMBI_2019 9
#define TABLE_CCAMDESC_2019 10
#define TABLE_GHSMINOR_2019 11

#define NB_TABLES_BINAIRES_2019 12

/*nombre maximum de p�riodes pour la s�lection des sous tables*/
#define MAX_PERIODE_2019 50

/*tableau donnant pour chaque table binaire les informations concernant
la sous table courante � utiliser pour le groupage du RSS. Cette sous-table est obtenue
gr�ce � l'offset n�periodecour dans le tableau offset_table*/
struct multiTable_2019
{
    FILE *fileptr; /*descripteur du fichier de la table binaire*/
    int datedeb[MAX_PERIODE_2019];/*date de d�but de p�riode en nombre de jours depuis le 01/01/1980*/
    int datefin[MAX_PERIODE_2019];/*date de fin de p�riode en nombre de jours depuis le 01/01/1980*/
    long offset_table[MAX_PERIODE_2019];/*offset de chaque sous table dans le fichier binaire*/
    int nbplages;/*nombre de p�riode(s)*/
    int periodecour;/*p�riode s�lectionn�e pour le RSS.*/
    /*CD 02/04/07*/
    int numerotarif_public[MAX_PERIODE_2019]; /*num�ro du tarif public*/
    int numerotarif_prive[MAX_PERIODE_2019];  /*num�ro du tarif prive*/

    /*FG 11e:TEM*/
    char *htable;
};
struct multiTable_2019 multitables_2019[NB_TABLES_BINAIRES_2019];

/*fonctions de gestion des tables multip�riodes*/
int OuvrirTables_2019(char *pdirtab, char *versioncl);
int ChoixTables_2019(char *datesortie,char *versioncl);

/*CD 25/01/07 sejour principal du RSS*/
int sejourprinc_2019;

/*25/01/07 Fonctions d'informations sur les autorisations d'UM*/
int sejourdjAutorise_2019(int numsej,int ptypum);
int sejourAutoriseEnTypAutor_2019(int numsej,int typum,char datedeb[],char datefin[],int *nbjours);
extern int autorisation_valide_2019(int ptypum);
extern int sejouravdernAut_2019(int numsej);

struct _supplements_2019
{
    /*FG13 SUPRUM int nbjours_rea,nbjours_sc,nbjours_si,durnna,durnnb,durnnc,nbjours_rea_si,nbjours_sra,nbjours_reaped,nbjours_ssc;*/
    int nbjours_rea[tab_max],nbjours_sc[tab_max],nbjours_si[tab_max],durnna[tab_max],durnnb[tab_max],durnnc[tab_max],nbjours_rea_si[tab_max],nbjours_sra[tab_max],nbjours_reaped[tab_max],nbjours_ssc[tab_max];
};
typedef struct _supplements_2019 supplements_2019;

enum typesuppl_2019 /*conserver l'ordre*/
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


#define MAX_FONCSUPP_2019 50
void _supnull_2019(int,RSS_INFO_2019*,supplements_2019*,int, int*), _supnn1_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*), _supnn2_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*), _supnn3_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),
     _suprep_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),_suprepanc_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),_suprea_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),_supsra_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),
     _supsi_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),_supsc_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*),_supssc_2019(int,RSS_INFO_2019*,supplements_2019*,int,int*);
void(*foncsupp_2019 [MAX_FONCSUPP_2019])(int,RSS_INFO_2019*,supplements_2019*,int,int*)={ _supnull_2019,_supnn1_2019,_supnn2_2019,_supnn3_2019,_suprea_2019,_supsra_2019,_supsc_2019,_supssc_2019,_supsi_2019,_suprep_2019,_suprepanc_2019};


/*CD 26/01/07*/
/*Fonction de calcul des s�ances*/
void calculeseances_2019(GRP_RESULTAT_2019 *grp_res,GHS_VALORISATION_2019 *ghs_val);

/*calcul des signatures*/
void signer_RSS_2019(unsigned char *rum[], int nbRUM,unsigned char *res);
unsigned int CalculerVersionFGTableBinaires_2019(GRP_ENTREE_2019 *pgrp_ent);
void calcule_signature_GHS_2019(RSS_INFO_2019 *prssinf, GHS_VALORISATION_2019 *pghs_val);


/*************************************************************************************************
Fonction grp_2019
  Fonction groupage version 13
  Cette fonction groupe un fichier de RSS au format 016 en format 116.
Param�tres :
*Param�tre 1 en entr�e : Structure GRP_ENTREE contenant :
    -> rum2[tab_max] : Tableau de pointeurs sur des cha�nes de caract�res.
        Ces cha�nes contiennent les diff�rents RUM d�un RSS. Chaque cha�ne (correspondant � un RUM) doit se terminer
        par un caract�re nul (�\0�). Ce tableau comporte au plus tab_max pointeurs, soit 99 �l�ments. Seul le format 016 est accept�.

    -> dirtab[BUFSIZ]: Cha�ne de caract�res contenant le chemin d�acc�s absolu, complet, pour atteindre le r�pertoire
        contenant les tables. Il doit �tre �crit de telle sorte que l�acc�s � une table donn�e consiste � concat�ner cette cha�ne
        avec le nom de la table recherch�e (termin� par '/').

    -> int ns : Entier contenant le nombre de s�jours �l�mentaires du malade au cours de son s�jour dans l��tablissement hospitalier.
        Un appel � la fonction grp_2019() avec ce param�tre � 0 entra�ne la fermeture des tables de groupage.

    -> int type_etab : Entier indiquant si l��tablissement est ex-DGF (=1), ou ex-OQN (diff�rent de 1).

*Param�tre 2 en sortie: Structure GRP_RESULTAT_2019 contenant :
    -> int ns_sor : Entier contenant le nombre de s�jours apr�s le traitement dans la fonction groupage. Ce nombre est d�sormais
        �gal au nombre de s�jours en entr�e.

    -> int sejp : Entier destin� � recevoir le num�ro du RUM d�sign� par le groupage (ou plus pr�cis�ment par l'algorithme de choix du DP) comme contenant le
                  diagnostic principal et le diagnostic reli�, en d�butant la num�rotation des RUM � 0.

    -> char version[3] : Cha�ne de 3 caract�res destin�e � recevoir le num�ro de version de la classification lu dans les tables (termin�e par le caract�re NULL).
    -> char cmd[3] : Cha�ne de 3 caract�res de long, destin�e � recevoir le num�ro de la CMD d�termin�e par le groupage (termin�e par le caract�re NULL).
    -> char ghm[5] : Cha�ne de 5 caract�res de long, destin�e � recevoir le num�ro du GHM d�termin� par le groupage (termin�e par le caract�re NULL).
    -> char cret[4]: Cha�ne de 4 caract�res de long, destin�e � recevoir le code retour d�termin� par le groupage. Il s�agit de la
        valeur en caract�re du retour de la fonction grp_2019().Cette cha�ne est termin�e par le caract�re NULL.

    -> int ErrCtrl[MAXERRCTRL_2019] : Tableau de MAXERRCTRL entiers courts correspondant au vecteur des erreurs contenues dans chacun des RUM du RSS.
        Il est construit de la fa�on suivante :
            entier n�1    : (nr) nombre de RUM d�tect�s dans le RSS
            entier n�2    : (n1) nombre d�erreurs d�tect�es dans le RUM n�1�
            entier n�(nr+1)    : nombre d�erreurs d�tect�es dans le RUM n�nr
            entiers n�(nr+2) � (nr+1+n1)    : codes des erreurs du RUM n�1
            entiers n�(nr+n1+2) � (nr+1+n1+n2)    : codes des erreurs du RUM n�2�

    -> int ErrArb[MAXERRARB_2019] : Tableau de MAXERRARB entiers courts correspondant au vecteur des erreurs d�tect�espar l�arbre de d�cision
        de la classification. Les erreurs sont stock�es dans ce vecteur de mani�re s�quentielle. Ce vecteur n�a pas de structure particuli�re.

    -> int ErrImpl[MAXERRIMPL_2019] : Tableau de MAXERRIMPL entiers courts correspondant au vecteur des erreurs dues � l�impl�mentation.
        Les erreurs sont stock�es dans ce vecteur de mani�re s�quentielle. Ce vecteur n�a pas de structure particuli�re.

*Param�tre 3 en sortie: Structure GHS_VALORISATION_2019 contenant :

    -> char ghs[5] : Cha�ne de 5 caract�res de long, destin�e � recevoir le num�ro du GHS d�termin� � partir du GHM obtenu par le groupage,
        et des informations contenues dans le fichier ghsinfo.tab. Dans le cas de la dialyse pour les �tablissements ex-OQN,
        cette variable contient le libell� du forfait dialyse apr�s SROS (commen�ant par D).
        Pour les �tablissements ex-DGF dans le cas de la dialyse, elle contient le num�ro du GHS apr�s SROS.

    -> int nbjxsup : Entier contenant le nombre de journ�es au-del� de la borne extr�me haute pour le GHS consid�r�.
    -> int sejxinf : Entier contenant 0 si la dur�e du s�jour est sup�rieure � la borne extr�me basse du GHS. Sinon, �gal � 1 si exb forfaitaire, 2 si exb � la journ�e, 0 si pas de minoration
    -> float nbjxinf : D�cimal �gal � borneinf-DS si borneinf>DS,0 sinon
    -> int buhcd : Entier utilis� comme bool�en, �gal � 1 si le GHS applique la r�gle sp�cifique aux UHCD, 0 sinon.


    -> int nbjreaR[tab_max];Entier contenant le nombre de suppl�ments de r�animation pour le RUM num�ro i
    -> int nbrepR[tab_max];Entier contenant le nombre de suppl�ments de r�animation p�diatrique pour le RUM num�ro i
    -> int nbjsrcR[tab_max];Entier contenant le nombre de suppl�ments de surveillance continue pour le RUM num�ro i
    -> int nbjstf_issudereaR[tab_max];Entier contenant le nombre de suppl�ments soins intensifs provenant de la r�animation pour le RUM num�ro i
    -> int nbjstf_totR[tab_max];Entier contenant le nombre total de suppl�ments soins intensifs, y compris les suppl�ments provenant de la r�animation, pour le RUM num�ro i
    -> int nbjnnaR[tab_max];Entier contenant le nombre de journ�es suppl�mentaires pour la n�onatologie pour le RUM num�ro i
    -> int nbjnnbR[tab_max];Entier contenant le nombre de journ�es suppl�mentaires pour la n�onatologie avec soins intensifs pour le RUM num�ro i
    -> int nbjnncR[tab_max];Entier contenant le nombre de journ�es suppl�mentaires pour la r�animation n�onatale pour le RUM num�ro i

    -> int nbjrea : Entier contenant le nombre suppl�ments de r�animation.
    -> int nbrep : Entier contenant le nombre de suppl�ments de r�animation p�diatrique.
    -> int nbjsrc : Entier contenant le nombre de suppl�ments de surveillance continue.
    -> int nbjstf_issuderea : Entier contenant le nombre suppl�ments de soins intensifs provenant de la r�animation.
    -> int nbjstf_tot : Entier contenant le nombre total de suppl�ments de soins intensifs, y compris les suppl�ments provenant de la r�animation.

    -> int nbjnna: Entier contenant le nombre de suppl�ments pour la n�onatologie (type 2A).
    -> int nbjnnb: Entier contenant le nombre de suppl�ments pour la n�onatologie avec soins intensifs (type 2B).
    -> int nbjnnc: Entier contenant le nombre de suppl�ments pour la r�animation n�onatale (type 3).

    -> int nbac687: Entier contenant le nombre d�actes suppl�mentaires (hors s�ances) de radioth�rapie menant au GHS 9610.
    -> int nbac688: Entier contenant le nombre d�actes suppl�mentaires (hors s�ances) de radioth�rapie menant au GHS 9611.
    -> int nbac689: Entier contenant le nombre d�actes suppl�mentaires (hors s�ances) de radioth�rapie menant au GHS 9612.

    -> int nbacRX1; Entier contenant le nombre de suppl�ments "Gama Knife" (GHS 6523)
    -> int nbacRX2; Entier contenant le nombre de suppl�ments "Cyber Knife" (GHS 9621)
    -> int nbacRX3; Entier contenant le nombre de suppl�ments "aph�r�se" (GHS 9615)

    -> int nbRCMI; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9622 (RCMI)

    -> int nbactsupp9623; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9623
    -> int nbactsupp9624; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9624
    -> int nbactsupp9625; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9625
    -> int nbactsupp9631; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9631
    -> int nbactsupp9632; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9632
    -> int nbactsupp9633; Entier contenant le nombre d'actes suppl�mentaires menant au GHS 9633

    -> int acprlvtorg: Entier contenant la cat�gorie de pr�l�vement d�organes du s�jour consid�r�. (0, 1, 2 ou 3).

    -> int pih_a : Entier utilis� comme bool�en, contenant 0 en cas d�absence de code diagnostic associ� Z75.80, 1 sinon.
    -> int pih_b : Entier utilis� comme bool�en, contenant 0 si le mode d�entr�e et le mode de sortie est �gal � 0, 1 sinon.

    -> char prestdial[5] : chaine de 5 caract�res utilis�e pour les �tablissements ex-OQN uniquement: contient la prestation dialyse associ�e au GHM obtenu.

    -> int nbhd : Entier contenant le nombre d'actes suppl�mentaires hors s�ances pour l'h�modialyse.
    -> int ent1 : Entier contenant le nombre d'actes suppl�mentaires hors s�ances pour les entra�nements � la dialyse p�riton�ale automatis�e.
    -> int ent2 : Entier contenant le nombre d'actes suppl�mentaires hors s�ances pour les entra�nements � la dialyse p�riton�ale continue ambulatoire.
    -> int ent3 : Entier contenant le nombre d'actes suppl�mentaires hors s�ances pour les entra�nements � l'h�modialyse.
    -> int nbchhypb : Entier contenant le nombre de caissons hyperbare.

    -> int nbseance : Entier contenant le nombre total de s�ances
    -> int nbprot : Entier contenant le nombre d'actes suppl�mentaires menant dans le GHS 9619 (protonth�rapie)
    -> int nbict : Entier contenant le nombre d'actes suppl�mentaires menant dans le GHS 9620 (ICT)


*Param�tre 4 en sortie: Structure RSS_INFO_2019 contenant :
    -> int agea : Entier contenant l��ge en ann�es calcul�e par la fonction groupage
    -> int agej : Entier contenant l��ge en jours calcul�e par la fonction groupage
    -> int dstot: Entier contenant la dur�e totale de s�jour calcul�e par la fonction groupage

    -> DIAGLIST_2019 diaglist    est une structure contenant :
        -    int nbdiag : entier contenant le nombre de diagnostics diff�rents.
        -    char *listdiag : chaine de caract�res contenant la liste de ces diagnostics avec en premi�re position le diagnostic principal,
            puis en deuxi�me position le diagnostic reli�, puis l�ensemble des diagnostics associ�s retenus pour le RSS.

    -> ACTLIST_2019 actlist est une structure contenant :
        -    int nbac : entier contenant le nombre de zones d�actes du RSS.
        -    char *listzac : chaine de caract�res contenant la liste des zones d�actes .
        La pr�sentation de ces zones d�actes est identique � celle du RUM.

    -> INFO_RUM_2019 tabval[tab_max] est un tableau de tab_max �l�ments contenant pour chaque RUM :
        - int brea : entier �gal � 1 si le RUM contient des actes marqueurs de r�animation, 0 sinon.
        - int bsc : entier �gal � 1 si le RUM poss�de les caract�ristiques pour obtenir des suppl�ments de SC sans consid�rer le type d'autorisation de l'UM, 0 sinon.
        - char typum[2] : chaine de 2 caract�res repr�sentant le type d'autorisation du dernier jour pass� dans l'Unit� M�dicale.
        - int dsp : entier contenant la dur�e de s�jour partielle du RUM.

    -> char sigfg[5] : chaine de 5 caract�res contenant la signature de la FG : tables binaires + CLS_ID
    -> char sigrss[20] : chaine de 20 caract�res contenant la signature du RSS
    -> char sigrssghsfg[8];chaine de 8 caract�res contenant la signature de sigrss et du GHS et de sigfg

Valeur en retour : entier
        la fonction grp_2019() renvoie un entier �gal � z�ro si les contr�les n�ont rien d�tect� d�anormal, et si le
        groupage n�a pas retrouv� d�anomalies. Dans le cas contraire, la valeur retourn� est le num�ro de
        l�erreur d�tect�e (sauf pour certaines erreurs de controles non bloquantes non retourn�es par la fonction groupage).

        Toute erreur de contr�le dont la valeur est contenue dans la liste erreursbloq_2019 du fichier fg13.h
        est irr�m�diable, et ne permet pas d�obtenir le groupage du R.S.S. Les erreurs de contr�le non contenues dans cette liste
        ne sont pas graves (autrement dit non bloquantes) en terme de groupage (par exemple : date syst�me improbable).De telles erreurs ne sont d�tect�es que par
        l�analyse du vecteur d�crit plus haut (ErrCtrl).
        Les erreurs d'impl�mentation (tableau ErrImpl) sont aussi irr�m�diables
        Les erreurs de parcours de l'arbre de d�cision (tableau ErrArb) peuvent �tre bloquantes ou non bloquantes.
***********************************************************************************************************/

int grp_2019 (GRP_ENTREE_2019 grp_ent,GRP_RESULTAT_2019 *grp_res,GHS_VALORISATION_2019 *ghs_val,RSS_INFO_2019 *rss_inf);

/*FG 11e:TEM*/
/*pointeurs sur buffers contenant les tables binaires*/
char *h_ptaccam_2019;
char *h_ptmdiag10_2019;
char *h_pttree_2019;
char *h_ghsinf_2019;
char *h_ptfidx_gc_2019;
char *h_ghminf_2019;
char *h_srcdgac_2019;
char *h_innovref_2019;
char *h_tabcombi_2019;
char *h_refauto_2019;
char *h_ptccamdesc_2019;
char *h_ptghsminor_2019;

/*FG 2019 RAAC*/
int critereRAAC;
int indicateurRAAC;

/*type hospitalisation 1er RUM*/
char typehospit1erRUM_exp_2019;
/*s�jour �ligible au forfait diab�te*/
int sejEliForfDiabete_2019;




