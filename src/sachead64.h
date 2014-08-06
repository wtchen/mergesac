/*		SAC Header Include File

*	This include file should be used when ever the SAC header
*	is accessed.
*	Definitions are for those items that the UofW `tosac'
*	program includes from the UW-PING type data and associated files.
*	Values of `-12345' are used for any varriable which is UNKNOWN.

*/

#define SACHEADERSIZE	632	/* # of bytes in SAC header */

#define SACVERSION	6	/* version number of SAC */
#define SAC_I_D		2.0	/* SAC I.D. number */

#define SACWORD		float	/* SAC data is an array of floats */
#define SACUNDEF	-12345	/* undefined value */
#define SACSTRUNDEF	"-12345  "	/* undefined value for strings */

#define SACTEMP4	11

#define SACTEMP6	3
#define SACTEMP7	10

#define	KEVNMLEN	16	/* length of event name */
#define K_LEN		8	/* length of all other string fields */

/*	This is the actual data structure which is used to enter the header
	variables.
*/
struct SAChead
{
    /* floating point fields */
    float delta;		/* incement between evenly spaced data (sec) */
    float depmin;
    float depmax;
    float scale;
    float odelta;
    float b;		/* beginning value of indep var (0. sec) */
    float e;		/* ending value of indep var (sec) */
    float o;		/* event origin time, rel to zero (sec) */
    float a;		/* first arrival time, rel to zero (sec) */
    float internal1;
    float t0;		/* S-wave arrival time, rel to zero (sec) */
    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    float t8;
    float t9;
    float f;		/* End of event time, rel to zero (sec) */
    float resp0;
    float resp1;
    float resp2;
    float resp3;
    float resp4;
    float resp5;
    float resp6;
    float resp7;
    float resp8;
    float resp9;
    float stla;		/* station latitude (deg north) */
    float stlo;		/* station longitude (deg east */
    float stel;
    float stdp;
    float evla;		/* event location, latitude (deg north) */
    float evlo;		/* event location, longitude (deg east) */
    float evel;
    float evdp;		/* event depth (km) */
    float blank1;
    float user0;		/* event magnitude */
    float user1;		/* event location rms (sec) */
    float user2;		/* event location gap (deg) */
    float user3;		/* event location standard error (km) */
    float user4;
    float user5;		/* number of stations used in location */
    float user6;
    float user7;
    float user8;
    float user9;
    float dist;		/* event-station epicentral distance (km) */
    float az;		/* event to station azimuth (deg) */
    float baz;		/* event to station back-azimuth (deg) */
    float gcarc;		/* event to station arc distance (deg) */
    float internal2;
    float internal3;
    float depmen;
    float cmpaz;		/* component azimuth (deg) */
    float cmpinc;		/* component inclination (deg) */
    float blank4[SACTEMP4];

    /* integer fields */
    int nzyear;		/* Trace beginning time (year) */
    int nzjday;		/*  (julian day) */
    int nzhour;		/*  (hour) */
    int nzmin;		/*  (minute) */
    int nzsec;		/*  (second) */
    int nzmsec;		/*  (millisecond) */
    int nvhdr;
    int internal5;
    int internal6;
    int npts;		/* number of points in trace */
    int internal7;
    int internal8;
    int blank6[SACTEMP6];
    int iftype;		/* Type of data  =1 for time series */
    int idep;		/* Type of dependent data =UNKNOWN */
    int iztype;		/* zero time equivalence  =1 for beginning */
    int iblank6a;
    int iinst;
    int istreg;
    int ievreg;
    int ievtyp;
    int iqual;
    int isynth;
    int blank7[SACTEMP7];
    unsigned int leven;	/* =1 for evenly spaced data */
    unsigned int lpspol;	/* =1 for correct polarity, 0 for reversed */
    unsigned int lovrok;
    unsigned int lcalda;
    unsigned int lblank1;

    /* character string fields */
    char kstnm[K_LEN];	/* station name (blank padded) */
    char kevnm[KEVNMLEN];	/* event name (UofW pickfile name) */
    char khole[K_LEN];
    char ko[K_LEN];		/* Origin time lable */
    char ka[K_LEN];		/* First arrival time lable */
    char kt0[K_LEN];	/* S-wave time lable */
    char kt1[K_LEN];
    char kt2[K_LEN];
    char kt3[K_LEN];
    char kt4[K_LEN];
    char kt5[K_LEN];
    char kt6[K_LEN];
    char kt7[K_LEN];
    char kt8[K_LEN];
    char kt9[K_LEN];
    char kf[K_LEN];
    char kuser0[K_LEN];
    char kuser1[K_LEN];
    char kuser2[K_LEN];
    char kcmpnm[K_LEN];
    char knetwk[K_LEN];
    char kdatrd[K_LEN];
    char kinst[K_LEN];
};

/*	This is a structure of the same size as 'SAChead', but in a form
	that is easier to initialize
*/

#define NUM_FLOAT	70	/* number of floats in header */
#define MAXINT		35	/* number of ints in header */
#define MAXSTRING	24	/* number of strings in header */

struct SAChead2
{
    float SACfloat[NUM_FLOAT];
    int SACint[MAXINT];
    unsigned int SACun[5];
    char SACstring[MAXSTRING][K_LEN];
};

/* eof sachead.h */
