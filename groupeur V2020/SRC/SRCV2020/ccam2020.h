/******************************************************************************/
/* Agence Technique de l'Information sur l'Hospitalisation (A.T.I.H.)         */
/* Date de création : 04 janvier 2006                                         */
/* Types, constantes et prototypes pour la gestion des gestes complémentaires */
/******************************************************************************/


#define TAILLE_ACTE_C_2020 9
#define  _tentete_2020 33           /* Taille des entetes de table */

#define NB_SOUSTABLES_GC_2020 4       /* Nombre de sous tables */
#define _nbpref_ccam_2020 17576     /* Nombre de prefixes */

typedef unsigned char uchar;

/* Definition de la structure d'entete de table */
struct ET_2020 {

    char sign[8];
    char verc[2];
    char revc[2];
    char datc[6];
    char nomt[8];
    uchar ffic;
    uchar nstb;
    uchar kgen[4];

} ;

typedef struct ET_2020 ENTETE_TABLE_2020 ;

/* Definition de la structure d'entete de sous-table */
struct EST_2020 {

  char sgstn[8];
  char vcstn[2];
  char rcstn[2];
  char dgstn[6];
  uchar nastn[2];
  uchar lastn[2];/*taille article de la table*/
  uchar lgstn[4];/*taille totale de la sous table*/
  uchar ofstn[4];
  uchar dpstn;
  uchar lpstn;
  char restn;

};
typedef struct EST_2020 ENTETE_SSTAB_2020 ;

struct SSI_2020 {
  long lgtot;
  long offset;
  /*CD FG11*/
  /*short taille;*/
  int taille;
};

typedef struct SSI_2020 SSTAB_INFO_2020;


SSTAB_INFO_2020 sstab_info_2020[NB_SOUSTABLES_GC_2020];

int tableauvide_2020(int tableau[],int *nbelements);
int actelisteGC_2020(char code[],char activite,char *res);
int liste_compl_2020(char code_in[],char phase,char activite,char **res);
int est_gc_2020(char[]);

extern FILE	*ptfidx_gc_2020;        /* Pointeur sur la table des gestes complémentaires */

extern unsigned long carlong_2020 (unsigned char c[]);
/*CD 22/01/07
short opaccam_gc(char dxtab[],FILE **ptfidx_lp);*/
short opaccam_gc_2020(FILE *ptfidx_lp, long offsettable);

extern char *h_ptfidx_gc_2020;

