/**************************************************************************************************************/
/***************************************ATIH grpstruct1311d.h 21/11/2011***************************************/

#define tab_max 99                /* Nombre maximum de RUM dans un RSS*/

#define MAXERRCTRL_2019 5000        /* Nombre maximum d'erreurs de contrôle pour un RSS */
#define MAXERRARB_2019  5           /* Nombre maximum d'erreurs de parcours de l'arbre de décision*/
#define MAXERRIMPL_2019 20          /* Nombre maximum d'erreurs d'implémentation*/

/*structure représentant les données à fournir en entrée à la fonction groupage*/
struct GENT_2019
{
    char *rum2[tab_max]; /*les RUM*/
    char dirtab[BUFSIZ]; /*chemin des tables de groupage finissant par '/' */
    int ns;              /*nombre de RUM*/
    int type_etab;       /*1:ex-DGF 2:ex-OQN*/

    char ficum[BUFSIZ];  /*chemin complet du fichier d'autorisation d'UM*/
};
typedef struct GENT_2019 GRP_ENTREE_2019 ;

/*structure représentant les données de groupage calculées par la fonction groupage*/
struct GRES_2019
{
    int ns_sor;/*nombre de séjours en sortie*/
    int sejp;/*numéro du RUM contenant le DP retenu*/
    char version[3];/*version de la classification, finissant par le caractère NULL*/
    char cmd[3];/*CMD, finissant par le caractère NULL*/
    char ghm[5];/*GHM, finissant par le caractère NULL*/
    char cret[4];/*Code retour, finissant par le caractère NULL*/
    int ErrCtrl[MAXERRCTRL_2019];/*Tableau des erreurs de contrôle des RUM du RSS*/
    int ErrArb[MAXERRARB_2019];/*Tableau des erreurs de parcours de l'arbre de groupage*/
    int ErrImpl[MAXERRIMPL_2019];/*Tableau des erreurs d'implémentation*/
};
typedef struct GRES_2019 GRP_RESULTAT_2019 ;


/*structure représentant les éléments de valorisation calculées par la fonction groupage*/
struct GVAL_2019
{
    char ghs[5];  /*valeur du GHS terminant par le caractère '\n'*/
                  /*Dans le cas de la dialyse, c'est le forfait dialyse après SROS (pour public et privé)*/
    int nbjxsup;  /*nombre de journée au delà de la borne extrême haute*/
    
    int sejxinf ; /*entier égal à 0 si durée de séjour supérieure à borne extrême basse, sinon = 1 si exb forfaitaire, =  2 si exb à la journée, =0 si pas de minoration de l'exb*/
    float nbjxinf;/*décimal égal à borneinf-DS si borneinf>DS,0 sinon. Si décès, égal à zéro*/

    int buhcd;    /*booleen indiquant que le GHS applique la règle spécifique aux UHCD*/

    /********************************************************************************************************/
    /************************ SUPPLEMENTS TYPéS (dépendant de l'autorisation d'UM)***************************/
    /********************************************************************************************************/

    /*on calcule désormais les suppléments par RUM :*/
    int nbjreaR[tab_max];/*nombre de suppléments de réanimation pour le RUM numéro i*/
    int nbrepR[tab_max];/*nombre de suppléments de réanimation pédiatrique pour le RUM numéro i*/
    int nbjsrcR[tab_max];/*nombre de suppléments de surveillance continue pour le RUM numéro i*/
    int nbjstf_issudereaR[tab_max];/*Nombre de suppléments soins intensifs provenant de la réanimation pour le RUM numéro i*/
    int nbjstf_totR[tab_max];/*Nombre total de suppléments soins intensifs, y compris les suppléments provenant de la réanimation, pour le RUM numéro i*/
    int nbjnnaR[tab_max];/*nombre de journées supplémentaires pour la néonatologie pour le RUM numéro i*/
    int nbjnnbR[tab_max];/*nombre de journées supplémentaires pour la néonatologie avec soins intensifs pour le RUM numéro i*/
    int nbjnncR[tab_max];/*nombre de journées supplémentaires pour la réanimation néonatale pour le RUM numéro i*/

    /*suppléments totaux, c'est à dire pour le séjour entier (l'ensemble du RSS):*/
    int nbjrea; /*nombre de suppléments de réanimation*/
    int nbrep; /*nombre de suppléments de réanimation pédiatrique*/
    int nbjsrc; /*nombre de suppléments surveillance continue*/
    int nbjstf_issuderea;/*Nombre de suppléments soins intensifs provenant de la réanimation*/
    int nbjstf_tot;/*Nombre total de suppléments soins intensifs, y compris les suppléments provenant de la réanimation*/
    int nbjnna; /*nombre de journées supplémentaires pour la néonatologie*/
    int nbjnnb; /*nombre de journées supplémentaires pour la néonatologie avec soins intensifs */
    int nbjnnc; /*nombre de journées supplémentaires pour la réanimation néonatale */

    /********************************************************************************************************/
    /******************** SUPPLEMENTS NON TYPéS (non dépendant de l'autorisation d'UM)***********************/
    /********************************************************************************************************/

    int nbac687;/*nombre d'actes supplémentaires menant dans le GHS 9610*/
    int nbac688;/*nombre d'actes supplémentaires menant dans le GHS 9611*/
    int nbac689;/*nombre d'actes supplémentaires menant dans le GHS 9612*/

    int nbacRX1;/*nombre de suppléments "Gamaknife" GHS 6523*/
    int nbacRX2;/*nombre de suppléments "Cyberknife" GHS 9621*/
    int nbacRX3;/*nombre de suppléments "aphérèse" GHS 9615*/

    int nbRCMI;/*nombre d'actes supplémentaires menant au GHS 9622 (RCMI)*/

    int nbactsupp9623; /*nombre d'actes supplémentaires menant dans le GHS 9623*/
    
    /*SUPPR EN 11d
    int nbactsupp9624;*/ /*nombre d'actes supplémentaires menant dans le GHS 9624*/
    
    int nbactsupp9625; /*nombre d'actes supplémentaires menant dans le GHS 9625*/
    int nbactsupp9631; /*nombre d'actes supplémentaires menant dans le GHS 9631*/
    int nbactsupp9632; /*nombre d'actes supplémentaires menant dans le GHS 9632*/
    int nbactsupp9633; /*nombre d'actes supplémentaires menant dans le GHS 9633*/
   
    int acprlvtorg;/*indicateur de prélèvement d'organes*/
    
    int pih_a;
    int pih_b;
    
    char ghsavantInnov[5];/*GHS avant innovation, si pas d'innovation alors ghsavantInnov=ghs*/
    
    /*seances dialyses en sus :*/
    int nbhd; /*nb d'actes supplémentaires hors séances pour l'hémodialyse*/
    int nbent1;/*nb d'actes supplémentaires hors séances pour les entraînements à la dialyse péritonéale automatisée*/
    int nbent2;/*nb d'actes supplémentaires hors séances pour les entraînements à la dialyse péritonéale continue ambulatoire*/
    int nbent3;/*nb d'actes supplémentaires hors séances pour les entraînements à l'hémodialyse*/

    int nbchypb; /*nombre de séances d'oxygénothérape hyperbare menant dans le GHS 9614*/

    int nbseance;/*nombre de séances*/

    int nbprot;/*nombre d'actes supplémentaires menant dans le GHS 9619 (protonthérapie)*/
    int nbict;/*nombre d'actes supplémentaires menant dans le GHS 9620 (ICT)*/

    int nbantepartum;/*nombre de suppléments journaliers pour l'antepartum se terminant par un accouchement*/
    int nbradiopedia; /*supplément radiothérapie pédiatrique*/

    /*FG 11g*/
    int ghsminor;/*indicateur de GHS minoré*/
    
	
	/*int nbsupp_1;*//*supplément1 en en attente des modialités de calcul*/
	int supplSDC; /* supplément SDC : 1=> oui, 0=> non*/
	
	int nbsupp_2;/*supplément2 en en attente des modialités de calcul*/

    int flagAvastin;
};
typedef struct GVAL_2019 GHS_VALORISATION_2019 ;

/*liste des diagnostics du RSS*/
struct LD_2019
{
    int nbdiag; /*nombre de diagnostics dans la liste*/
    char *listdiag; /*liste des diagnostics du RSS*/
};
typedef struct LD_2019 DIAGLIST_2019;

/*liste des actes du RSS*/
struct LA_2019
{
    int nbac; /*nombre d’actes du RSS*/
    char *listzac; /*liste des actes du RSS */
};
typedef struct LA_2019 ACTLIST_2019;

/*informations propre à un RUM*/
struct _INFO_RUM_2019
{
    int brea; /*booleen permettant de savoir si le RUM a des actes marqueurs de réanimation*/
    int bsc; /*booleen indicateur de surveillance continue*/
    char typum[2];/*type d'autorisation de l'UM*/
    int dsp;/*durée de séjour partielle du RUM*/
};
typedef struct _INFO_RUM_2019 INFO_RUM_2019;

/*structure représentant des informations sur le RSS renvoyées par la fonction groupage*/
struct RSSI_2019
{
    int agea; /*age en années*/
    int agej; /*age en jours*/
    int dstot; /*durée de séjour totale*/
    DIAGLIST_2019 diaglist ; /*liste de tous les diagnostics*/
    ACTLIST_2019 actlist; /*liste de tous les actes*/

    /*indicateur, RUM par RUM, de réa, de type d'Unité Médicale (UM) et de durée de séjour partielle*/
    INFO_RUM_2019 tabval[tab_max];

    char sigfg[5];/*signature de la FG : tables binaires + CLS_ID*/
    char sigrss[20];/*signature du RSS*/
    char sigrssghsfg[8];/*signature de sigrss du GHS et de sigfg*/
};
typedef struct RSSI_2019 RSS_INFO_2019;

/***********************************************************************************************************/
