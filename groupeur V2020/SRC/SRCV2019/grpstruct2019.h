/**************************************************************************************************************/
/***************************************ATIH grpstruct1311d.h 21/11/2011***************************************/

#define tab_max 99                /* Nombre maximum de RUM dans un RSS*/

#define MAXERRCTRL_2019 5000        /* Nombre maximum d'erreurs de contr�le pour un RSS */
#define MAXERRARB_2019  5           /* Nombre maximum d'erreurs de parcours de l'arbre de d�cision*/
#define MAXERRIMPL_2019 20          /* Nombre maximum d'erreurs d'impl�mentation*/

/*structure repr�sentant les donn�es � fournir en entr�e � la fonction groupage*/
struct GENT_2019
{
    char *rum2[tab_max]; /*les RUM*/
    char dirtab[BUFSIZ]; /*chemin des tables de groupage finissant par '/' */
    int ns;              /*nombre de RUM*/
    int type_etab;       /*1:ex-DGF 2:ex-OQN*/

    char ficum[BUFSIZ];  /*chemin complet du fichier d'autorisation d'UM*/
};
typedef struct GENT_2019 GRP_ENTREE_2019 ;

/*structure repr�sentant les donn�es de groupage calcul�es par la fonction groupage*/
struct GRES_2019
{
    int ns_sor;/*nombre de s�jours en sortie*/
    int sejp;/*num�ro du RUM contenant le DP retenu*/
    char version[3];/*version de la classification, finissant par le caract�re NULL*/
    char cmd[3];/*CMD, finissant par le caract�re NULL*/
    char ghm[5];/*GHM, finissant par le caract�re NULL*/
    char cret[4];/*Code retour, finissant par le caract�re NULL*/
    int ErrCtrl[MAXERRCTRL_2019];/*Tableau des erreurs de contr�le des RUM du RSS*/
    int ErrArb[MAXERRARB_2019];/*Tableau des erreurs de parcours de l'arbre de groupage*/
    int ErrImpl[MAXERRIMPL_2019];/*Tableau des erreurs d'impl�mentation*/
};
typedef struct GRES_2019 GRP_RESULTAT_2019 ;


/*structure repr�sentant les �l�ments de valorisation calcul�es par la fonction groupage*/
struct GVAL_2019
{
    char ghs[5];  /*valeur du GHS terminant par le caract�re '\n'*/
                  /*Dans le cas de la dialyse, c'est le forfait dialyse apr�s SROS (pour public et priv�)*/
    int nbjxsup;  /*nombre de journ�e au del� de la borne extr�me haute*/
    
    int sejxinf ; /*entier �gal � 0 si dur�e de s�jour sup�rieure � borne extr�me basse, sinon = 1 si exb forfaitaire, =  2 si exb � la journ�e, =0 si pas de minoration de l'exb*/
    float nbjxinf;/*d�cimal �gal � borneinf-DS si borneinf>DS,0 sinon. Si d�c�s, �gal � z�ro*/

    int buhcd;    /*booleen indiquant que le GHS applique la r�gle sp�cifique aux UHCD*/

    /********************************************************************************************************/
    /************************ SUPPLEMENTS TYP�S (d�pendant de l'autorisation d'UM)***************************/
    /********************************************************************************************************/

    /*on calcule d�sormais les suppl�ments par RUM :*/
    int nbjreaR[tab_max];/*nombre de suppl�ments de r�animation pour le RUM num�ro i*/
    int nbrepR[tab_max];/*nombre de suppl�ments de r�animation p�diatrique pour le RUM num�ro i*/
    int nbjsrcR[tab_max];/*nombre de suppl�ments de surveillance continue pour le RUM num�ro i*/
    int nbjstf_issudereaR[tab_max];/*Nombre de suppl�ments soins intensifs provenant de la r�animation pour le RUM num�ro i*/
    int nbjstf_totR[tab_max];/*Nombre total de suppl�ments soins intensifs, y compris les suppl�ments provenant de la r�animation, pour le RUM num�ro i*/
    int nbjnnaR[tab_max];/*nombre de journ�es suppl�mentaires pour la n�onatologie pour le RUM num�ro i*/
    int nbjnnbR[tab_max];/*nombre de journ�es suppl�mentaires pour la n�onatologie avec soins intensifs pour le RUM num�ro i*/
    int nbjnncR[tab_max];/*nombre de journ�es suppl�mentaires pour la r�animation n�onatale pour le RUM num�ro i*/

    /*suppl�ments totaux, c'est � dire pour le s�jour entier (l'ensemble du RSS):*/
    int nbjrea; /*nombre de suppl�ments de r�animation*/
    int nbrep; /*nombre de suppl�ments de r�animation p�diatrique*/
    int nbjsrc; /*nombre de suppl�ments surveillance continue*/
    int nbjstf_issuderea;/*Nombre de suppl�ments soins intensifs provenant de la r�animation*/
    int nbjstf_tot;/*Nombre total de suppl�ments soins intensifs, y compris les suppl�ments provenant de la r�animation*/
    int nbjnna; /*nombre de journ�es suppl�mentaires pour la n�onatologie*/
    int nbjnnb; /*nombre de journ�es suppl�mentaires pour la n�onatologie avec soins intensifs */
    int nbjnnc; /*nombre de journ�es suppl�mentaires pour la r�animation n�onatale */

    /********************************************************************************************************/
    /******************** SUPPLEMENTS NON TYP�S (non d�pendant de l'autorisation d'UM)***********************/
    /********************************************************************************************************/

    int nbac687;/*nombre d'actes suppl�mentaires menant dans le GHS 9610*/
    int nbac688;/*nombre d'actes suppl�mentaires menant dans le GHS 9611*/
    int nbac689;/*nombre d'actes suppl�mentaires menant dans le GHS 9612*/

    int nbacRX1;/*nombre de suppl�ments "Gamaknife" GHS 6523*/
    int nbacRX2;/*nombre de suppl�ments "Cyberknife" GHS 9621*/
    int nbacRX3;/*nombre de suppl�ments "aph�r�se" GHS 9615*/

    int nbRCMI;/*nombre d'actes suppl�mentaires menant au GHS 9622 (RCMI)*/

    int nbactsupp9623; /*nombre d'actes suppl�mentaires menant dans le GHS 9623*/
    
    /*SUPPR EN 11d
    int nbactsupp9624;*/ /*nombre d'actes suppl�mentaires menant dans le GHS 9624*/
    
    int nbactsupp9625; /*nombre d'actes suppl�mentaires menant dans le GHS 9625*/
    int nbactsupp9631; /*nombre d'actes suppl�mentaires menant dans le GHS 9631*/
    int nbactsupp9632; /*nombre d'actes suppl�mentaires menant dans le GHS 9632*/
    int nbactsupp9633; /*nombre d'actes suppl�mentaires menant dans le GHS 9633*/
   
    int acprlvtorg;/*indicateur de pr�l�vement d'organes*/
    
    int pih_a;
    int pih_b;
    
    char ghsavantInnov[5];/*GHS avant innovation, si pas d'innovation alors ghsavantInnov=ghs*/
    
    /*seances dialyses en sus :*/
    int nbhd; /*nb d'actes suppl�mentaires hors s�ances pour l'h�modialyse*/
    int nbent1;/*nb d'actes suppl�mentaires hors s�ances pour les entra�nements � la dialyse p�riton�ale automatis�e*/
    int nbent2;/*nb d'actes suppl�mentaires hors s�ances pour les entra�nements � la dialyse p�riton�ale continue ambulatoire*/
    int nbent3;/*nb d'actes suppl�mentaires hors s�ances pour les entra�nements � l'h�modialyse*/

    int nbchypb; /*nombre de s�ances d'oxyg�noth�rape hyperbare menant dans le GHS 9614*/

    int nbseance;/*nombre de s�ances*/

    int nbprot;/*nombre d'actes suppl�mentaires menant dans le GHS 9619 (protonth�rapie)*/
    int nbict;/*nombre d'actes suppl�mentaires menant dans le GHS 9620 (ICT)*/

    int nbantepartum;/*nombre de suppl�ments journaliers pour l'antepartum se terminant par un accouchement*/
    int nbradiopedia; /*suppl�ment radioth�rapie p�diatrique*/

    /*FG 11g*/
    int ghsminor;/*indicateur de GHS minor�*/
    
	
	/*int nbsupp_1;*//*suppl�ment1 en en attente des modialit�s de calcul*/
	int supplSDC; /* suppl�ment SDC : 1=> oui, 0=> non*/
	
	int nbsupp_2;/*suppl�ment2 en en attente des modialit�s de calcul*/

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
    int nbac; /*nombre d�actes du RSS*/
    char *listzac; /*liste des actes du RSS */
};
typedef struct LA_2019 ACTLIST_2019;

/*informations propre � un RUM*/
struct _INFO_RUM_2019
{
    int brea; /*booleen permettant de savoir si le RUM a des actes marqueurs de r�animation*/
    int bsc; /*booleen indicateur de surveillance continue*/
    char typum[2];/*type d'autorisation de l'UM*/
    int dsp;/*dur�e de s�jour partielle du RUM*/
};
typedef struct _INFO_RUM_2019 INFO_RUM_2019;

/*structure repr�sentant des informations sur le RSS renvoy�es par la fonction groupage*/
struct RSSI_2019
{
    int agea; /*age en ann�es*/
    int agej; /*age en jours*/
    int dstot; /*dur�e de s�jour totale*/
    DIAGLIST_2019 diaglist ; /*liste de tous les diagnostics*/
    ACTLIST_2019 actlist; /*liste de tous les actes*/

    /*indicateur, RUM par RUM, de r�a, de type d'Unit� M�dicale (UM) et de dur�e de s�jour partielle*/
    INFO_RUM_2019 tabval[tab_max];

    char sigfg[5];/*signature de la FG : tables binaires + CLS_ID*/
    char sigrss[20];/*signature du RSS*/
    char sigrssghsfg[8];/*signature de sigrss du GHS et de sigfg*/
};
typedef struct RSSI_2019 RSS_INFO_2019;

/***********************************************************************************************************/
