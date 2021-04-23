#ifndef _gestionum2017_h 
#define _gestionum2017_h 

#define TYPUM_BLANC 0
#define TYPUM_REA 1
#define TYPUM_SI 2
#define TYPUM_SC 3
#define TYPUM_NEONAT2A 4
#define TYPUM_NEONAT2B 5
#define TYPUM_NEONAT3 6
#define TYPUM_UHCD 7
#define TYPUM_SP 8
#define TYPUM_SRA 9
#define TYPUM_SSC 10
#define TYPUM_ANESTHCA 11
#define TYPUM_HOSPTP 12
#define TYPUM_REAPEDIA 13
#define TYPUM_SCPEDIA 14
#define TYPUM_CLINOUV 15

#define TYPUM_SIHEMA 16
#define TYPUM_UNVHSI 17
#define TYPUM_SINEUROV 18
#define TYPUM_SEVCOMPL 19

#define TYPUM_CENTREHEMO 21
#define TYPUM_CENTREHEMOENF 22
#define TYPUM_UNITEHEMOMED 23

#define TYPUM_VALVEAORT 60
#define TYPUM_DIABETE 62 /*autorisation globale*/

#define MAX_TYPUM_2020 100 /*numéro max de type d'UM*/
#define MAX_AUTORISATIONS_UM_2020 40 /*nb max d'autorisations déclarées pour une UM*/

#define MAX_UM_2020 2000
#define MAX_LIGNE_UM_2020 80000 /*nombre maximum de ligne d'UM dans le fichier d'UM*/
#define tab_max 99  

/* Positions dans le fichier d'UM*/
/*CD type autor sur 3 carac au lieu de 2 et typhospit sur 1 => donc les positions dans le fichier d'UM changent*/
/*
#define FICUM_TYP_2020 4+9
#define FICUM_LITS_2020 14+9
#define FICUM_DATE_DEB_2020 6+9
#define FICUM_DATE_FIN_2020 17+9
#define FICUM_FINESSGEO_2020 4
*/
#define FICUM_TYP_2020 13
#define FICUM_LITS_2020 24
#define FICUM_DATE_DEB_2020 16
#define FICUM_DATE_FIN_2020 28
#define FICUM_FINESSGEO_2020 4
#define FICUM_TYPHOSPIT_2020 27
/*#define FICUM_LITS 6
#define FICUM_DATE_DEB 9
#define FICUM_DATE_FIN 17*/

#define TAILLE_SIGNATURE_2020 10
/*CD type autor sur 3 carac au lieu de 2 et typhospit sur 1 => donc +2*/
/*13.11d: n° Finess geographique obligatiore dans fichier d'autorisations d'UM*/
/*#define TAILLE_LIGNE_UM_2020 25+9*/
#define TAILLE_LIGNE_UM_2020 25+9+2

struct _AUTORUM_2020
{
    short typum;
    char  datdebeffet[8];
    char  datfineffet[8];
    int   invalidee;/*si l'autorisation est invalidee par la table de réference des autorisations*/
    char  finessgeo[9];/*FINESS Geographique*/
    char  troisemecartypum;
    char  typhospit;/*C,P ou M*/
};

typedef struct _AUTORUM_2020 AUTORUM_2020;

struct _UMINFO_2020
{
	char um[4];                   /*N° de l'unité médicale*/
	short umdeclarees[MAX_TYPUM_2020]; /*tab disant pour chaque typdum (de 0 à 99) si il est déclaré ou non*/
                                 /* si négatif => pas déclaré, si positif, indice dans le tableau*/
                                 /* "autorisations" de la premiere autorisation pour ce type d'UM*/ 
    AUTORUM_2020 autorisations[MAX_AUTORISATIONS_UM_2020];

    short nbautorisations;
};
typedef struct _UMINFO_2020 UMINFO_2020;

/*CD 24/01/07 utilisation du fichier d'UM*/
FILE *fptrum_2020;

/*tableau stockant toutes les UM du fichier d'UM ainsi que l'ensemble de leurs autorisations*/
UMINFO_2020 uminf_2020[MAX_UM_2020];
/*nombre d'éléments du tableau uminf*/
int nbuminf_2020;


/*tableau servant a savoir pour chaque RUM du RSS
le premier indice dans le tableau uminf concernant l'UM de ce RUM*/
int indice_ums_2020[tab_max];

int indice_um$$$$_2020;

int bChargeTableUM_2020(char sFic[]);
int getindicetypUM_2020(char typum[]);
int estdeclare_2020(int indum, int typdum);
int premierindiceTypUM_2020(int indum, int typdum);

int interval_autorise_2020(char datedebutdeffet[], char datefindeffet[], char date_entree[], char date_sortie[], int *nbjours);
int est_autorise_2020(int indum, int typdum, char date_entree[], char date_sortie[], int *nbjours, int *bPremJourAutor);

int trouveindiceUM_2020(char strUM[]);
extern int date_strict_sup_2020(char *date1,char *date2);
int trouveTypeUM_2020(char date[],int indum,int *typdum);
int TypUm_est_autorise_2020(int indum, int typdum, char date_entree[], char date_sortie[], int *nbjours, char date_dde[], char date_dfe[]);

void Ajout_jour_date_2020(char date[],int nbj,int *jour,int *mois,int *annee);
void Soustraire_jour_date_2020(char *date,int nbj,int *jour,int *mois,int *annee);

extern int callos_2020 (char *date,char *dats);
extern int mfgets_2020(char *buf, int taillemax, FILE *ptrfile);
extern void convdate_2020 (char *adat,int *p_jj,int *p_mm,int *p_aa);
extern int contfmt_2020 (char *zone,char format[],int ft,int taille);
extern int contdate_2020 (char *adate);
extern int comparer_dates_2020(char *date1,char *date2);
extern int nombre_jour_1980_2020(char *date);
extern unsigned char ToAscii_2020(unsigned char orig_char);

/*CD FG10B*/
struct _autref_2020
{
    /*int typum[MAX_TYPUM_2020];*/
    int bvalide[MAX_TYPUM_2020];
    int numfoncval[MAX_TYPUM_2020];
    
    /*CD FG13 : portée de l'autorisation : 0 : portée UM, 1 : portée globale (autorisation quelquesoit l'UM)*/
    int typeportee[MAX_TYPUM_2020];
    
    int nbaut;/*nombre d'autorisations dans le fichier de références*/
};
typedef struct _autref_2020 autref_2020;

/*CD autorisations d'UM validees par la table de référence*/
autref_2020 umrefs_2020;
autref_2020 litdedref_2020;


/*CD FG12*/
struct _intervalles_2020
{
    char date_deb[8];
    char date_fin[8];
    int nbjours;
    int typeautor;
    int typesupplement;
    char finessgeo[9];
    int flagpremierjour;
    char troisemecartypum;
    char typhospit;/*C,P ou M*/
};
struct _intervallesAutor_2020
{
    int nbintervalles;
    /*int mixte; plus besoin on mémorise tous les "segments*/
    struct _intervalles_2020 intervalles[MAX_AUTORISATIONS_UM_2020];
};
typedef struct _intervallesAutor_2020 intervallesAutor_2020;

/*pour DEFI*/
void getPlagesValo_2020(intervallesAutor_2020 **lesPlagesValo);

/*historisation des périodes d'autorisation au sein d'un RUM*/
/*seules 2 autorisations sont retenues*/
/*si l'UM change plus de 2 fois d'autorisation ,alors le type d'autorisation de la deuxièmes période sera 99*/
intervallesAutor_2020 plagesAutor_2020[tab_max];

extern int autorisation_valide_2020(int ptypum);
extern int autlitdedie_valide_2020(int ptyplitded);
extern int getnumfoncsupplement_2020(int ptypum);
extern int interval_autorise_hist_2020(char datedebutdeffet[], char datefindeffet[], char date_entree[], char date_sortie[], char** date_debfinale,char** date_finfinale,int *nbjours);

#endif

