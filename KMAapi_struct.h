#define NODATA -999
#define PI 3.141592653589793

#ifndef KMAAPI_STRUCT
#define KMAAPI_STRUCT

typedef struct REQUEST_H		// Keys for hourly requests.
{
	utility::string_t startDt;
	utility::string_t startHh;
	utility::string_t endDt;
	utility::string_t endHh;
	
}req_h;

typedef struct REQUEST_D		// Keys for daily requests.
{
	utility::string_t startDt;
	utility::string_t endDt;

}req_d;

typedef struct REQUEST_S
{
	utility::string_t startDt;
	utility::string_t endDt;
	utility::string_t lat;
	utility::string_t lon;
}req_s;



typedef struct RESPONSE_H	// Received hourly response.
{
	std::string time;		// time data. Must be saved as string to be parsed later.
	int year;				// year data. Must not be smaller than 1900.
	int month;
	int day;
	int hour;


	// list of variables saving response data

	double temp;	// temperature ('C)
	double rain;	// amount of rain (mm)
	double wspd;	// wind speed(m/s)
	double rhum;	// relative humidity(%)
	double dewp;	// dewpoint ('C)
	double srad;	// ghi (MJ/m2)

	double wdir;	// wind direction (deg)
	double vpre;	// vapor pressure (hPa)
	double lpre;	// local pressure (hPa)
	double spre;	// sea level pressure (hPa)
	double sshr;	// sunshine hour (h)
	double sdep;	// snow depth (cm)
	double visi;	// visibility (10m)
	double stem;	// soil surface temperature ('C)


	// recent 3 hour snow, cloud info, weather code, 
	// 5cm & 10cm & 20cm & 30cm underground temperature
	// can be requested but were not utilized (yet).

}res_h;

typedef struct RESPONSE_HSET	// Monthly data comprised of hourly data.
{
	std::list<res_h> hSet;
	int year;

	double altData_h[14] = {
		NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA,
		NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA
	};

}res_hSet;

typedef struct RESPONSE_H2D		// Daily data converted from hourly data.
{
	int year;
	int doy;					// Day of year in 5 digits.

	double tmax;
	double tmin;
	double tavg;

	double rain;	// amount of rain (mm)
	double wspd;	// wind speed(m/s)
	double rhum;	// relative humidity(%)
	double dewp;	// dewpoint ('C)
	double srad;	// ghi (MJ/m2)

	double wdir;	// wind direction (deg)
	double vpre;	// vapor pressure (hPa)
	double lpre;	// local pressure (hPa)
	double spre;	// sea level pressure (hPa)
	double sshr;	// sunshine hour (h)
	double sdep;	// snow depth (cm)
	double visi;	// visibility (10m)
	double stem;	// soil surface temperature ('C)

}res_hd;

typedef struct RESPONSE_H2DSET	// Yearly data comprised of daily data.
{
	std::list<res_hd> hdSet;
	double tav;
	double range;
}res_hdSet;




typedef struct RESPONSE_D
{
	std::string time;		// time data. Must be saved as string to be parsed later.
	int year;				// year data. Must not be smaller than 2000.
	int month;
	int day;
	int doy;				// 3-digit doy. Year not included.

	double tavg;			// Average daily temperature ('C)
	double tmin;			
	double tmax;
	double rain;			// Total daily rainfall (mm)
	
	double avgWspd;			// Average daily wind speed (m/s)
	double avgDewp;			// Average daily dewpoint ('C)
	double avgRhum;			// Average daily relative humidity (%)
	double avgVpre;			// Average daily vapor pressure (hPa)

	double avgLpre;			// Average daily local pressure (hPa)
	double avgSpre;			// Average daily sea level pressure (hPa)

	double sdur;			// Daily sum of possible sunshine duration (hr).
	double sshr;			// Daily sum of actual sunshine duration (hr).
	double srad;			// Daily sum of solar radiation (MJ/m2).

	double sdep;			// Maximum snow depth (cm).
	double stem;			// Average daily soil surface temperature ('C)
	double leva;			// Daily sum of large-scale evaporation (mm)
	double seva;			// Daily sum of small-scale evaporation (mm)

	// Flags are not provided in daily requests.

	double sradEst;			// Estimated srad (MJ/m2).

	
}res_d;

typedef struct RESPONSE_DSET	// Yearly data comprised of daily data.
{
	std::list<res_d> dSet;

	double altData_d[17] = {
		NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA,
		NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA
	};

	double tav;
	double range;

}res_dSet;



typedef struct RESPONSE_S
{
	std::string time;
	int year;
	int month;
	int day;
	int doy;

	double tavg;			// Average daily temperature ('C)
	double tmin;
	double tmax;
	
	double rain;			// Total daily rainfall (mm)
	double srad;			// Total daily solar irradiance (Wh/m2)
	
	double avgWspd;			// Average daily wind speed (m/s)
	double avgRhum;			// Average daily relative humidity (%)
} res_s;

typedef struct RESPONSE_SSET
{
	std::list<res_s> sSet;

	double altData_s[varNum_s] = {
		NODATA, NODATA, NODATA, NODATA, NODATA, NODATA, NODATA
	};

	double tav;
	double range;
} res_sSet;

#endif