// lecture simple du .PSD en 32 bits

#include <vector>

#define PSD_TRANSPARENT 0x1000000
#define PSD_BLACK 0x0

// message d'erreur
#define PSD_OK 0x0
#define PSD_INVALIDE 0x1
#define PSD_FILE_NOT_FOUND 0x2
#define PSD_ONLY_24_BPP_RGB 0x3
#define PSD_OUT_OF_MEMORY 0x4
#define PSD_TOO_MUCH_COLORS_IN_THE_PICTURE 0x5
#define PSD_TOO_MUCH_COLORS_IN_A_LAYER 0x6
#define PSD_TOO_MUCH_LAYER 0x7

typedef unsigned int psdPixel;

/// Classe qui charge un PSD 24 bits et tous ces layers.
class Cpsd
{
	// quelques types
	typedef std::vector<psdPixel*> layerVector;
	typedef std::vector<char*> layerNameVector;
	// public...
public:
	Cpsd();
	~Cpsd ();
	/// Obtenir la largeur
	int GetWidth() const;
	/// Obtenir la hauteur
	int GetHeight() const;
	/// Obtenir le nombre de layer
	int GetLayerCount() const;
	/// Obtenir un pointeur sur un layer
	/*@Doc: Obtenir un pointeur sur un layer

	Retourne un pointeur sur un tableau de Width*Height psdPixel. 
	Une valeur speciale de pixel, PSD_TRANSPARENT, indique un pixel
	transparent.*/
	psdPixel* GetLayer(int nLayer) const;
	/// Charger un fichier psd
	/*@Doc: Charger un fichier psd.

	Valeurs de retour:
	PSD_OK : pas de problème\\
	PSD_INVALIDE : fichier PSD invalide\\
	PSD_FILE_NOT_FOUND : fichier pas trouvé\\
	PSD_ONLY_24_BPP_RGB : fichier pas en format RGB 24 bits
	PSD_OUT_OF_MEMORY : pas assez de mémoire*/
	int Load(const wchar_t* sFileName);
	/// Verifier que le fichier est un PSD
	static BOOL IsPsd(const wchar_t* sFileName);
	/// Obtenir un pointeur sur l'image complète
	psdPixel* GetMerged();
	/// Reduction 256 couleurs
	/*@Doc: Reduction 256 couleurs

	Passer en parametre une référence sur le pointeur de surface et une reference
	sur le pointeur de palette.
	La fonction retourne PSD_OK et le pointeur sur l'image en 256 couleurs ainsi
	que sue la palette.\\
	Sinon, la fonction retourne soit PSD_TOO_MUCH_COLORS_IN_THE_PICTURE, soit
	PSD_TOO_MUCH_COLORS_IN_A_LAYER ou soit PSD_TOO_MUCH_LAYER. Dans le premier cas, l'image possede plus
	de 256 couleurs avec tous les layers visibles confondus, dans le second cas,
	un layer possède plus de 16 couleurs. Dans ce dernier cas, pour chaque layer possedant plus de 16 couleurs,
	la fonction passe en parametre est appelee avec comme parametre le nom du layer mis en cause.

	Dans le troisieme cas, il y'a plus de 16 layer dans l'image.
	En cas d'erreur, les pointeurs retournés sont
	NULL. ColorReducing retourne autre chose que PSD_TOO_MUCH_COLORS_IN_A_LAYER, la valeur
	de nLayerError est indéfinie.
	
	Les pointeurs retournes doivent etre delete si la fonction retourne PSD_OK. Ils ne sont pas efface par l'objet*/
	int ColorReducing (unsigned char*& pPixel, unsigned short*& pPalette, void(*funError)(void *pParm,const char* layername),void *pParm=NULL);
	// implementation
protected:
	void Clear ();
	int m_nWidth;
	int m_nHeight;
	layerVector m_vectorLayer;
	layerNameVector m_vectorLayerName;
	psdPixel* m_pMerged;
	psdPixel* m_pAlpha;
public:
	static char *s_sPsdMessageError[];
};