#include "grpstruct2019.h"
#include "grpstruct2020.h"

int grp_2019(GRP_ENTREE_2019, GRP_RESULTAT_2019*, GHS_VALORISATION_2019*, RSS_INFO_2019*);
int grp_2020(GRP_ENTREE_2020, GRP_RESULTAT_2020*, GHS_VALORISATION_2020*, RSS_INFO_2020*);

GRP_ENTREE_2019 grp_ent_2019;
GHS_VALORISATION_2019 ghs_val_2019;
RSS_INFO_2019 rss_inf_2019;

GRP_ENTREE_2020 grp_ent_2020;
GHS_VALORISATION_2020 ghs_val_2020;
RSS_INFO_2020 rss_inf_2020;


GRP_RESULTAT_2020 grp_res_2020;



extern int _igs_2019;
extern int lgdiag_2019;

extern int _igs_2020;
extern int lgdiag_2020;

extern int gradationHDJ_2020;

extern int agegest_renseigne_2019;
extern int datedernregles_renseignee_2019;

extern int agegest_renseigne_2020;
extern int datedernregles_renseignee_2020;

extern void free_ptr_2019();
extern void grpclose_2019();

extern void free_ptr_2020();
extern void grpclose_2020();

extern int mfgets_2020(char *buf, int taillemax, FILE *ptrfile);
extern int contfmt_2020(char *zone,char format[],int ft,int taille);
extern int contdate_2020(char *adate);
extern int date_strict_sup_2020(char *date1,char *date2);

