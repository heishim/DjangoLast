
#define OffsetVersionRUM_2019 9
#define LgVersionRUM_2019 3


char* RetournChar_2019(const char* Rum,int Offset,int Longueur);

int GetVersion_2019(const char* Rum, int Format);

char* GetFiness_2019(const char* Rum, int Format);

char* GetIdentifiantRSS_2019(const char* Rum, int Format);

char* GetDateDeNaissance_2019(const char* Rum, int Format);

char* GetSex_2019(const char* Rum, int Format);

char* GetUniteMedicale_2019(const char* Rum, int Format);

char* GetDateEntree_2019(const char* Rum, int Format);

char* GetModeEntree_2019(const char* Rum, int Format);

char* GetProvenance_2019(const char* Rum, int Format);

char* GetDateSortie_2019(const char* Rum, int Format);

char* GetModeSortie_2019(const char* Rum, int Format);

char* GetDestination_2019(const char* Rum, int Format);

char* GetCodePostal_2019(const char* Rum, int Format);

char* GetPoidNaissance_2019(const char* Rum, int Format);

char* GetNombreSeances_2019(const char* Rum, int Format);

int GetNombreDiagnosticsNum_2019(const char* Rum, int Format);

int GetNombreDADNum_2019(const char* Rum, int Format);

char* GetNombreDiagnostics_2019(const char* Rum, int Format);

int GetNombreActesNum_2019(const char* Rum, int Format);

char* GetNombreActes_2019(const char* Rum, int Format);

char* TrimDiagnostic_2019(char* Diagnostic);

char* GetDiagnosticPrincipal_2019(const char* Rum, int Format);

char* GetIGS2_2019(const char* Rum, int Format);

/*
	Fonction pour effectuer des copies dans des destinations déterminées
	out=>ce qui vat être écrit
	in=>la source qui vat être désalouée
	Last change:  MB   27 Sep 99    2:27 pm
*/
char* CopyDest_2019(char* out,char* in);

/*
  Retourne la taille théorique du Rum tel qu'ele peut être calculée à partir
  des premiers champs d'entête
*/
int GetTaille_2019(const char* Rum, int Format);


/*
  Prend en entrée un rum, et rend en sortie un "rum format interne", c'est à dire un rum au format 004
  ce rum est alloué, et il convient au programe appelant de le désalouer
  En cas de problème, un pointeur nul est retourné
*/
char* RumToInterne_2019(const char* Rum, int Format);
