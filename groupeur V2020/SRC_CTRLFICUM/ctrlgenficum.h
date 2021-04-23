/*CD gestion du fichier d'UM <<brut>>*/

/*messages d'erreur renvoyée par CtrlFicUM*/
#define ERR_UM_OUVERTURE_FICHIER 1
#define ERR_UM_CREATION_FICHIERUM 2
#define ERR_UM_CREATION_FICHIERLOG 3
#define ERR_UM_FORMAT 4
#define ERR_UM_DOUBLONS 5
#define ERR_UM_SYSTEME 6
#define ERR_UM_NBLIGNESMAX 7
#define ERR_UM_TYPENONAUTORISE_EN_GLOBAL 8
#define ERR_UM_FICHIER_CORROMPU 9
#define ERR_UM_DOUBLONS_AUTGLOBFGEO 10
#define ERR_UM_TYPEAUT_PLUSACCEPTEE 11

#define MAX_LIGNE_UM 80000

/*13.11d : FINESS GEOGRAPHIQUE OBLIGATOIRE
#define TAILLE_LIGNE_UM_INITIAL_FORMAT1 17*/
/*taille d'une ligne d'un fichier d'UM non trié*/
#define TAILLE_LIGNE_UM_INITIAL_FORMAT2 17+9 /*idem mais nouveau format (ajout du N° FINESS géographique)*/
#define TAILLE_LIGNE_UM_INITIAL_FORMAT3 17+9+2 /*idem mais nouveau format (type dautorisation sur 3 car a lieu de 2, et ajout du champ "mode d'hospitalisation sur 1 car)*/

/*nom du fichier d'UM vérifié et trié généré*/
/*#define TABLE_FICUM "ficum.txt"*/
#define TABLE_AUTOREFS_BINAIRE "autorefs.tab"

#define MAX_AUTOR_GLOBALES 9

/*structure représentant une ligne du fichier d'UM généré*/
struct stUM
{
    char um[4];
    /*13.11d : FINESS GEOGRAPHIQUE OBLIGATOIRE*/
    char finessgeo[9];
    char autorisation[3];
    char datedebautor[8];
    char nblits[3];
    char typehospit;
    char datefinautor[8];
};

struct stUM tabums_source[MAX_LIGNE_UM];

/*nombre de ligne d'UM dans le fichier d'UM*/
int nbum_source;

//indice du 1er enregistrement avec um=$$$$ dans tabums_source
int indice_tabumsource$$$$;


/*structure donnant l'info : pour telle UM, le finess geographique le plus récent associé à cette UM est ...*/
struct stUMfinessgeo
{
    char um[4];
    char finessgeo_leplusrecent[9];
};

struct stUMfinessgeo ums_finessgeo[MAX_LIGNE_UM];
int nbums_declarees;/*nombre de numéros d'UM différents déclarés dans le fichier d'UM*/


/*signature du fichier d'UM généré*/
unsigned char crcum[11];

struct _autref_gen_13e
{
    int typum[100];
    int typeportee[100];/*1 : globale, 0 : pour une UM seulement*/
    int nbaut;
};
typedef struct _autref_gen_13e autref_gen_13e;

/*CD autorisations d'UM validees par la table de référence*/
autref_gen_13e umrefs_gen_13e;

int chargeRefAutorisations();
int porteeGlobale(char ptypum[]);

typedef unsigned char uchar;
/* Definition de la structure d'entete de table */
struct ENTETE_gen_13e {
    char sign[8];
    char verc[2];
    char revc[2];
    char datc[6];
    char nomt[8];
    uchar ffic;
    uchar nstb;
    uchar kgen[4];
} entete_gen_table_13e;
/* Definition de la structure d'entete de sous-table */
struct entete_gen_13e {
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
}entete_gen_stab_13e;

/*Fonction CtrlFicUM
nficumin : nom et chemin complet du fichier d'UM à contrôler et transformer
nreptabfg : nom et chemin complet du répertoire des tables de la fonction groupage dans lequel sera généré le fichier "ficum.txt" (finissant par /)
int bgenerelog : différent de zéro si on veut qu'un fichier de log soit généré dans le même répertoire que le fichier source nficumin, égal à zéro sinon.
int bgestionPortee : gestion de la portée globale du type d'autorisation 0:non 1:oui
*/
int CtrlFicUM(char *nficumin, char *nficumout, char *nreptabfg, int bgenerelog, int bgestionPortee,int bgenerefinessgeo);

void pad_droite(char *pchaine);


/*renvoie la liste des autorisations globales valide par rapport a un intervalle de temps donné*/
void ListeAutorGlobaleValides(int pindiceum, char date_entree[], char date_sortie[], int autorV[], int *nbAutV);

