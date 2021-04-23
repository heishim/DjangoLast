
#define OffsetVersionRUM_2020 9
#define LgVersionRUM_2020 3


char* RetournChar_2020(const char* Rum,int Offset,int Longueur);

int GetVersion_2020(const char* Rum, int Format);

char* GetFiness_2020(const char* Rum, int Format);

char* GetIdentifiantRSS_2020(const char* Rum, int Format);

char* GetDateDeNaissance_2020(const char* Rum, int Format);

char* GetSex_2020(const char* Rum, int Format);

char* GetUniteMedicale_2020(const char* Rum, int Format);

char* GetDateEntree_2020(const char* Rum, int Format);

char* GetModeEntree_2020(const char* Rum, int Format);

char* GetProvenance_2020(const char* Rum, int Format);

char* GetDateSortie_2020(const char* Rum, int Format);

char* GetModeSortie_2020(const char* Rum, int Format);

char* GetDestination_2020(const char* Rum, int Format);

char* GetCodePostal_2020(const char* Rum, int Format);

char* GetPoidNaissance_2020(const char* Rum, int Format);

char* GetNombreSeances_2020(const char* Rum, int Format);

int GetNombreDiagnosticsNum_2020(const char* Rum, int Format);

int GetNombreDADNum_2020(const char* Rum, int Format);

char* GetNombreDiagnostics_2020(const char* Rum, int Format);

int GetNombreActesNum_2020(const char* Rum, int Format);

char* GetNombreActes_2020(const char* Rum, int Format);

char* TrimDiagnostic_2020(char* Diagnostic);

char* GetDiagnosticPrincipal_2020(const char* Rum, int Format);

char* GetIGS2_2020(const char* Rum, int Format);

/*
	Fonction pour effectuer des copies dans des destinations d�termin�es
	out=>ce qui vat �tre �crit
	in=>la source qui vat �tre d�salou�e
	Last change:  MB   27 Sep 99    2:27 pm
*/
char* CopyDest_2020(char* out,char* in);

/*
  Retourne la taille th�orique du Rum tel qu'ele peut �tre calcul�e � partir
  des premiers champs d'ent�te
*/
int GetTaille_2020(const char* Rum, int Format);


/*
  Prend en entr�e un rum, et rend en sortie un "rum format interne", c'est � dire un rum au format 004
  ce rum est allou�, et il convient au programe appelant de le d�salouer
  En cas de probl�me, un pointeur nul est retourn�
*/
char* RumToInterne_2020(const char* Rum, int Format);
