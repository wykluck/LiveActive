//
// bsc.h
//
// interface to browser information in a .bsc file
//

#ifndef __BSC_INCLUDED__
#define __BSC_INCLUDED__

//#include <windef.h>			// for BOOL, BYTE, ULONG, USHORT
#include <windows.h>
typedef ULONG  NI;          /* name index */
typedef ULONG  IINST;
typedef ULONG  IREF;
typedef ULONG  IDEF;
typedef USHORT IMOD;
typedef USHORT LINE;
typedef BYTE   TYP;
typedef USHORT ATR;
typedef ULONG  ATR32;
typedef ULONG  MBF;
typedef ULONG  BOB;

typedef char*  SZ;
typedef const char*	SZ_CONST;

#ifndef BSCCALL
#define BSCCALL  __cdecl
#endif

#if	defined(BSC_LIBRARY)
#define BSCAPI(RTYPE)	RTYPE BSCCALL
#else
#define BSCAPI(RTYPE)	__declspec(dllimport) RTYPE BSCCALL
#endif

#define	IN			/* in parameter, parameters are IN by default */
#define	OUT			/* out parameter */

typedef struct Bsc Bsc;

#define HTARGET ULONG_PTR

// The constant IINST value for the "Globals" class
#define IINST_GLOBALS	0xFFFF0001
#define isTheGlobalIinst(iinst) ((iinst) == IINST_GLOBALS)

typedef enum
{
	addOp,
	delOp,
	changeOp,
	changeIinstOp,
	refreshAllOp,
	noOp
}  OPERATION;

typedef struct IinstInfo
{
    IINST       m_iinst;
    SZ_CONST    m_szName; // REVIEW: must be deleted (use Ni instead)!
    NI	        m_ni; 
} IinstInfo;

typedef struct BSC_STAT
{
	ULONG	cDef;
	ULONG	cRef;
	ULONG	cInst;
	ULONG	cMod;
	ULONG	cUseLink;
	ULONG	cBaseLink;
} BSC_STAT;

typedef struct NiQ
{
    IINST m_iinstOld;
	IinstInfo m_iInfoNew;
	OPERATION m_op;
	TYP		m_typ;
} NiQ;

typedef BOOL (*pfnNotifyChange) (NiQ * rgQ, ULONG cQ, HTARGET hTarget);

// these are the supported query types
typedef enum _qy_ { 
	qyNil, /* reserved */
	qyContains, 
    qyCalls, 
	qyCalledBy,
	qyRefs,
	qyDefs,
	qyBaseOf, 
	qyDervOf, 
	qyImpMembers, 
	qyMac	/* reserved */
} QY;

#if __cplusplus

// open a browser database
BSCAPI( BOOL ) openBrowser(SZ_CONST szName, OUT Bsc** ppbsc);

struct Bsc
{
	// open by name or by .pdb 
	static	BOOL open(SZ_CONST szName, OUT Bsc** ppbsc) {return ::openBrowser(szName, ppbsc);}
	virtual BOOL close() = 0;

	// primitives for getting the information that underlies a handle
	virtual BOOL iinstInfo(IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR *patr) = 0;
	virtual BOOL irefInfo(IREF iref, OUT SZ *pszModule, OUT LINE *piline) = 0;
	virtual BOOL idefInfo(IDEF idef, OUT SZ *pszModule, OUT LINE *piline) = 0;
	virtual BOOL imodInfo(IMOD imod, OUT SZ *pszModule) = 0;
	virtual SZ   szFrTyp(TYP typ) = 0;
	virtual SZ   szFrAtr(ATR atr) = 0;

	// primitives for managing object instances (iinst)
	virtual BOOL getIinstByvalue(SZ sz, TYP typ, ATR atr, OUT IINST *piinst) = 0;
	virtual BOOL getOverloadArray(SZ sz, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;	
	virtual BOOL getUsedByArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;
	virtual BOOL getUsesArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;
	virtual BOOL getBaseArray(IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;
	virtual BOOL getDervArray(IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst)  = 0;
	virtual BOOL getMembersArray(IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;

	// primitives for getting definition and reference information	
	virtual BOOL getDefArray(IINST iinst, OUT IREF **ppidef, OUT ULONG *pciidef) = 0;
	virtual BOOL getRefArray(IINST iinst, OUT IREF **ppiref, OUT ULONG *pciiref) = 0;

	// primitives for managing source module contents
	virtual BOOL getModuleContents(IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;
	virtual BOOL getModuleByName(SZ sz, OUT IMOD *pimod) = 0;
	virtual BOOL getAllModulesArray(OUT IMOD **ppimod, OUT ULONG *pcimod) = 0;
	
	// call this when a computed array is no longer required
	virtual void disposeArray(void *pAnyArray) = 0;

	// call this to get a pretty form of a decorated name	
	virtual SZ  formatDname(SZ szDecor) = 0;
	
	// call this to do category testing on instances
	virtual BOOL fInstFilter(IINST iinst, MBF mbf) = 0;

	// primitives for converting index types
	virtual IINST iinstFrIref(IREF) = 0;
	virtual IINST iinstFrIdef(IDEF) = 0;
	virtual IINST iinstContextIref(IREF) = 0;

	// general size information
	virtual	BOOL getStatistics(struct BSC_STAT *) = 0;
	virtual	BOOL getModuleStatistics(IMOD, struct BSC_STAT *) = 0;

	// case sensitivity functions
	virtual BOOL fCaseSensitive() = 0;
	virtual BOOL setCaseSensitivity(BOOL) = 0;

	// handy common queries which can be optimized
	virtual BOOL getAllGlobalsArray(MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst) = 0;
	virtual BOOL getAllGlobalsArray(MBF mbf, OUT IinstInfo **ppiinstinfo, OUT ULONG *pciinst) = 0;

	// needed for no compile browser
	// get parameters (iinst must be a function type)
	virtual SZ  getParams (IINST iinst) = 0;
	virtual USHORT getNumParam (IINST iinst) = 0;
	virtual SZ getParam (IINST iinst, USHORT index) = 0;
	// get return type/variable type
	virtual SZ  getType (IINST iinst) = 0;
	// register call back for notification
	// THIS SHOULD BE DELETED SOON!
	virtual BOOL regNotify (pfnNotifyChange pNotify) = 0;
	// register to make sure that NCB will create change queue
	virtual BOOL regNotify () = 0;
	virtual BOOL getQ (OUT NiQ ** ppQ, OUT ULONG * pcQ) = 0;
	virtual BOOL checkParams (IINST iinst, SZ * pszParam, ULONG cParam) = 0;
	virtual BOOL fHasMembers (IINST iinst, MBF mbf) = 0;
	
	// needed for class view for optimization
	virtual SZ szFrNi (NI ni) = 0;
	virtual BOOL niFrIinst (IINST iinst, NI *ni) = 0;
	virtual BOOL lock() = 0;
	virtual BOOL unlock() = 0;

    // Methods allowing usage of big ATR field
	virtual SZ   szFrAtr2(ATR32 atr) = 0;
	virtual BOOL iinstInfo2(IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr) = 0;
	virtual BOOL getIinstByvalue2(SZ sz, TYP typ, ATR32 atr, OUT IINST *piinst) = 0;
};

#endif

// C Bindings
#if __cplusplus
extern "C" {
#endif

BSCAPI( BOOL ) BSCOpen(SZ_CONST szName, OUT Bsc** ppbsc);
BSCAPI( BOOL ) BSCClose(Bsc* pbsc);
BSCAPI( BOOL ) BSCIinstInfo(Bsc* pbsc, IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR *patr);
BSCAPI( BOOL ) BSCIrefInfo(Bsc* pbsc, IREF iref, OUT SZ *pszModule, OUT LINE *piline);
BSCAPI( BOOL ) BSCIdefInfo(Bsc* pbsc, IDEF idef, OUT SZ *pszModule, OUT LINE *piline);
BSCAPI( BOOL ) BSCImodInfo(Bsc* pbsc, IMOD imod, OUT SZ *pszModule);
BSCAPI( SZ )  BSCSzFrTyp(Bsc* pbsc, TYP typ);
BSCAPI( SZ )  BSCSzFrAtr(Bsc* pbsc, ATR atr);
BSCAPI( BOOL ) BSCGetIinstByvalue(Bsc* pbsc, SZ sz, TYP typ, ATR atr, OUT IINST *piinst);
BSCAPI( BOOL ) BSCGetOverloadArray(Bsc* pbsc, SZ sz, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);	
BSCAPI( BOOL ) BSCGetUsedByArray(Bsc* pbsc, IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetUsesArray(Bsc* pbsc, IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetBaseArray(Bsc* pbsc, IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetDervArray(Bsc* pbsc, IINST iinst, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetMembersArray(Bsc* pbsc, IINST iinst, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetDefArray(Bsc* pbsc, IINST iinst, OUT IREF **ppidef, OUT ULONG *pciidef);
BSCAPI( BOOL ) BSCGetRefArray(Bsc* pbsc, IINST iinst, OUT IREF **ppiref, OUT ULONG *pciiref);
BSCAPI( BOOL ) BSCGetModuleContents(Bsc* pbsc, IMOD imod, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);
BSCAPI( BOOL ) BSCGetModuleByName(Bsc* pbsc, SZ sz, OUT IMOD *pimod);
BSCAPI( BOOL ) BSCGetAllModulesArray(Bsc* pbsc, OUT IMOD **ppimod, OUT ULONG *pcimod);
BSCAPI( void ) BSCDisposeArray(Bsc* pbsc, void *pAnyArray);
BSCAPI( SZ ) BSCFormatDname(Bsc* pbsc, SZ szDecor);
BSCAPI( BOOL ) BSCFInstFilter(Bsc* pbsc, IINST iinst, MBF mbf);
BSCAPI( IINST ) BSCIinstFrIref(Bsc* pbsc, IREF);
BSCAPI( IINST ) BSCIinstFrIdef(Bsc* pbsc, IDEF);
BSCAPI( IINST ) BSCIinstContextIref(Bsc* pbsc, IREF);
BSCAPI(	BOOL ) BSCGetStatistics(Bsc* pbsc, BSC_STAT *);
BSCAPI(	BOOL ) BSCGetModuleStatistics(Bsc* pbsc, IMOD, BSC_STAT *);
BSCAPI( BOOL ) BSCFCaseSensitive(Bsc* pbsc);
BSCAPI( BOOL ) BSCSetCaseSensitivity(Bsc* pbsc, BOOL fCaseIn);
BSCAPI( BOOL ) BSCGetAllGlobalsArray(Bsc* pbsc, MBF mbf, OUT IINST **ppiinst, OUT ULONG *pciinst);

BSCAPI( SZ   ) BSCSzFrAtr2(Bsc* pbsc, ATR32 atr);
BSCAPI( BOOL ) BSCIinstInfo2(Bsc* pbsc, IINST iinst, OUT SZ *psz, OUT TYP *ptyp, OUT ATR32 *patr);
BSCAPI( BOOL ) BSCGetIinstByvalue2(Bsc* pbsc, SZ sz, TYP typ, ATR32 atr, OUT IINST *piinst);

// BSC Query functionality provided for backward compatibility
// (partial compatibility only -- see help file)

BSCAPI( BOOL ) OpenBSCQuery(Bsc* pbsc);
BSCAPI( BOOL ) CloseBSCQuery();
BSCAPI( BOOL ) InitBSCQuery(QY qy, BOB bob);
BSCAPI( BOB ) BobNext();
BSCAPI( BOB ) BobFrName(SZ sz);
BSCAPI( SZ ) LszNameFrBob(BOB bob);

#if __cplusplus
}
#endif

#if __cplusplus
// CBSCLock
//
// Note, Bsc::lock returns BOOL, however, from current usage, the result is never tested, so
// this class assumes it need not be tested.
//

class CBSCLock
{
private :

	Bsc * const m_pBsc;

public :

	CBSCLock(Bsc * const pBsc) : m_pBsc(pBsc)	{ if (m_pBsc) m_pBsc->lock();	}
	~CBSCLock(void)								{ if (m_pBsc) m_pBsc->unlock();	}
};
#endif

// these are the bit values for Bsc::instInfo()

// this is the type part of the result, it describes what sort of object
// we are talking about.  Note the values are sequential -- the item will
// be exactly one of these things
//

#define INST_TYP_FUNCTION       0x01
#define INST_TYP_LABEL          0x02
#define INST_TYP_PARAMETER      0x03
#define INST_TYP_VARIABLE       0x04
#define INST_TYP_CONSTANT       0x05
#define INST_TYP_MACRO          0x06
#define INST_TYP_TYPEDEF        0x07
#define INST_TYP_STRUCNAM       0x08
#define INST_TYP_ENUMNAM        0x09
#define INST_TYP_ENUMMEM        0x0A
#define INST_TYP_UNIONNAM       0x0B
#define INST_TYP_SEGMENT        0x0C
#define INST_TYP_GROUP          0x0D
#define INST_TYP_PROGRAM        0x0E
#define INST_TYP_CLASSNAM       0x0F
#define INST_TYP_MEMFUNC        0x10
#define INST_TYP_MEMVAR         0x11
#define INST_TYP_INCL           0x12
#define INST_TYP_MSGMAP         0x13
#define INST_TYP_MSGITEM        0x14
#define INST_TYP_DIALOGID       0x15 // dialog ID for MFC
// idl stuff
#define INST_TYP_IDL_ATTR       0x16 // idl attributes are stored as iinst
#define INST_TYP_IDL_COCLASS    0x17
#define INST_TYP_IDL_IFACE      0x18
#define INST_TYP_IDL_DISPIFACE  0x19
#define INST_TYP_IDL_LIBRARY    0x1A
#define	INST_TYP_IDL_MODULE     0x1B
#define INST_TYP_IDL_IMPORT     0x1C
#define INST_TYP_IDL_IMPORTLIB  0x1D
#define INST_TYP_IDL_MFCCOMMENT 0x1E // idl interface/dispinterface can have mfc comment
// java stuff
#define INST_TYP_JAVA_IFACE     0x1F // java (NOT COM) interfaces
// 0x20 to 0x30 left for Java
// some more C++ stuff
#define INST_TYP_TEMPLATE       0x31
#define INST_TYP_NAMESPACE      0x32
#define INST_TYP_IMPORT         0x33
#define INST_TYP_BASECLASS      0x34
#define INST_TYP_ATTRIBUTE      0x35
#define INST_TYP_INTERFACE		0x36
#define INST_TYP_PROPERTY		0x37 // MC++ __property

// these are the attributes values, they describes the storage
// class and/or scope of the instance.  Any combination of the bits
// might be set by some language compiler, but there are some combinations
// that don't make sense.

#define INST_ATR_LOCAL       0x001
#define INST_ATR_STATIC      0x002
#define INST_ATR_SHARED      0x004
#define INST_ATR_NEAR        0x008
#define INST_ATR_COMMON      0x010
#define INST_ATR_DECL_ONLY   0x020
#define INST_ATR_PUBLIC      0x040
#define INST_ATR_NAMED       0x080
#define INST_ATR_MODULE      0x100
#define INST_ATR_VIRTUAL     0x200
#define INST_ATR_PRIVATE     0x400
#define INST_ATR_PROTECT     0x800

#define IMODE_VIRTUAL        0x001
#define IMODE_PRIVATE        0x002
#define IMODE_PUBLIC         0x004
#define IMODE_PROTECT        0x008

#define mbfNil       0x000
#define mbfVars      0x001
#define mbfFuncs     0x002
#define mbfMacros    0x004
#define mbfTypes     0x008
#define mbfClass     0x010
#define mbfIncl      0x020
#define mbfMsgMap    0x040
#define mbfDialogID  0x080
#define mbfLibrary   0x100
#define mbfImport    0x200
#define mbfTemplate  0x400
#define mbfNamespace 0x800
#define mbfAll       0xFFF

#define irefNil  ((IREF)-1)
#define idefNil  ((IDEF)-1)
#define iinstNil ((IINST)-1)

// BOB = browser object, general index holder 

typedef ULONG BOB;

#define bobNil 0L

typedef USHORT CLS;

#define clsMod  1
#define clsInst 2
#define clsRef  3
#define clsDef  4

#define BobFrClsIdx(cls, idx)  ((((ULONG)(cls)) << 28) | (idx))
#define ClsOfBob(bob)	(CLS)((bob) >> 28)

#define ImodFrBob(bob)	((IMOD) ((bob) & 0xfffffffL))
#define IinstFrBob(bob)	((IINST)((bob) & 0xfffffffL))
#define IrefFrBob(bob)	((IREF) ((bob) & 0xfffffffL))
#define IdefFrBob(bob)	((IDEF) ((bob) & 0xfffffffL))

#define BobFrMod(x)  (BobFrClsIdx(clsMod,  (x)))
#define BobFrInst(x) (BobFrClsIdx(clsInst, (x)))
#define BobFrRef(x)  (BobFrClsIdx(clsRef,  (x)))
#define BobFrDef(x)  (BobFrClsIdx(clsDef,  (x)))

#endif // __BSC_INCLUDED__
