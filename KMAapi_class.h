#ifndef KMAAPI_CLASS
#define KMAAPI_CLASS

#include "KMAapi_DB.h"
#include "KMAapi_struct.h"

// Basic hierarchy of stored reponse data is h > hSet > hList.
// 'h' or 'd' following the underline character indicates
// whether the data is from hourly or daily requests.

class KMAapi_h
{
private:

	// Lists for hourly data
	utility::string_t key_h;					// Service key for hourly requests.
	std::string fileName_h = "NoName";			// Output file names passed by argument.

	std::list<std::string> monName_h;			// Monthly data file names.
	std::list<std::string> yrName_h;			// Yearly data file names.

	std::string stnNm_h;						// site name (KOR -> ENG).
	std::string stnId_h;						// site ID in 3 digits.

	bool varOpt_h[varNum_h] = {
		1, 1, 1, 1, 1, 1,						// temp, rain, wspd, rhum, dewp, srad, 				
		0, 0, 0, 0, 0, 0, 0, 0					// wdir, vpre, pres, spre, sshr, sdep, vsib, stem
	};	

	int hrStart_h;								// Marking starting hour
	int dayStart_h;								// for fillMssing_h function

	std::list<req_h> input_hSet;				// Hourly input set.
	std::list<res_hSet> hList;					// Hourly input set.
	std::list<res_hdSet> hdList;				// Hourly -> daily output set.


public:
	KMAapi_h();
	~KMAapi_h();

	void setFileName_h(std::string& ofName);

	// Hourly data request & parsing.
	void getInput_h(std::string& ifName);
	void request_h();
	double checkFlag_h(utility::string_t &response, utility::string_t &flag);
	void fillMissing_h();

	void epwOut_h();
	void hourly2daily();
	void wthOut_h();
	void oryOut_h();
	

};

class KMAapi_d
{
private:

	// Lists for daily data
	utility::string_t key_d;			// Service key for hourly requests.
	bool varOpt_d[varNum_d] = {
		1, 1, 1, 1, 1, 1, 1, 1,			// tavg, tmin, tmax, rain, avgWspd, avgDewp, avgRhum, avgVpre,			
		0, 0, 1, 1, 1, 0, 0, 0, 0		// avgLpre, avgSpre, sdur, sshr, srad, sdep, stem, leva, seva
	};

	int doyStart_d;						// Marking starting day & month
	int monStart_d;						// for fillMssing_d function
	

	std::string fileName_d = "NoName";
	std::list<std::string> yrName_d;		// Names for daily output files.

	std::string stnNm_d;					// site name (KOR -> ENG).
	std::string stnId_d;

	double coef_a = 0.18;					// Angstrom-Prescott coefficients.
	double coef_b = 0.55;

	std::list<req_d> input_dSet;			// Daily input set.
	std::list<res_dSet> dList;				// Daily output set.


public: 
	KMAapi_d();
	~KMAapi_d();

	bool doEst();
	void setFileName_d(std::string& ofName);

	// Daily data request & parsing.
	void getInput_d(std::string& ifName);
	void request_d();
	void fillMissing_d();


	void calcTemp_d();
	void estRad_d();
	void wthOut_d();
	void oryOut_d();

};

class KMAapi_s
{
private:

	double lat;
	double lon;

	int doyStart_s;						// Marking starting day & month
	int monStart_s;						// for fillMssing_d function

	std::string fileName_s = "NoName";
	std::list<std::string> yrName_s;		// Names for daily output files.

	std::list<req_s> input_sSet;			// Daily input set.
	std::list<res_sSet> sList;				// Daily output set.

public:

	KMAapi_s();
	~KMAapi_s();

	void setFileName_s(std::string& ofName);

	void getInput_s(std::string& ifName);
	void request_s();
	void fillMissing_s();
	void calcTemp_s();

	void wthOut_s();
	void oryOut_s();
};

int calcDoy(int& year, int& month, int& day);

#endif