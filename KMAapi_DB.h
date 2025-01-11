#ifndef KMAAPI_DB
#define KMAAPI_DB

const int varNum_h = 14;
const int varNum_d = 17;
const int varNum_s = 7;

const std::string varName_h[14] = {
	"temp", "rain", "wspd", "rhum", "dewp", "srad",
	"wdir", "vpre", "lpre", "spre", "sshr", "sdep", "visi", "stem"

	// temperature ('C), amount of rain (mm), wind speed(m/s),
	// relative humidity(%), dewpoint ('C), ghi (MJ/m2),
	// wind direction (deg), vapor pressure (hPa), local pressure (hPa), sea level pressure (hPa),
	// sunshine hour (h), snow depth (cm), visibility (10m), soil surface temperature ('C)
};

const std::string varName_d[17] = {
	"tavg", "tmin", "tmax", "rain"
	"avgWspd", "avgDewp", "avgRhum", "avgVpre", "avgLpre", "avgSpre",

	"sdur", "sshr", "srad", "sdep", "stem", "leva", "seva"
	// Daily sum of possible sunshine duration (hr), Daily sum of actual sunshine duration (hr),
	// Daily sum of solar radiation (MJ/m2), Estimated srad (MJ/m2)
	// Maximum snow depth (cm), Average daily soil surface temperature ('C),
	// Daily sum of large-scale evaporation (mm), Daily sum of small-scale evaporation (mm)
};

const std::string varName_s[7] = {
	"tmax", "tmin", "tavg", "avgRhum", "rain", "srad", "avgWspd"
};

const double varDefault_h[14] = {

	12.5, 0, 0, 61.8, 5.36, 0,
	0, 524.57, 1000, 1000, 6.47, 0, 2000, 12.5

	// temp & rhum: Based on 30-year average climate data in Seoul (108).
	// dewp & vpre: Calculated from temp & rhum.
	// lpre & spre: Arbitrarily set to 1000hpa.
	// sshr: Based on KOSIS 2020 annual sum of sunshine hours.
	// visi: Arbitrarily set to 20km.
};

const double varDefault_d[17] = {
	12.5, 12.5, 12.5, 0,
	0, 61.8, 5.36, 524.57, 1000, 1000,
	12, 6.43, 0,
	0, 12.5, 2, 2
};

const double varDefault_s[7] = {
	12.5, 12.5, 12.5, 0, 0, 0, 61.8
};



const utility::string_t itemKey_h[3][14]= {
	{ U("tm"), U("stnId"), U("stnNm") },									// Time data

	{ U("ta"), U("rn"), U("ws"), U("hm"), U("td"), U("icsr"), U("wd"),		// request variables
		U("pv"), U("pa"), U("ps"), U("ss"), U("dsnw"), U("vs"), U("ts") },

	{ U("taQcflg"), U("rnQcflg"), U("wsQcflg"), U("hmQcflg"), U("-1"), U("-1"), U("wdQcflg"),	// this comma				// qc flags
		U("-1"), U("paQcflg"), U("psQcflg"), U("ssQcflg"), U("-1"), U("-1"), U("-1")}
};



const utility::string_t itemKey_d[2][17] = {
	{ U("tm"), U("stnId"), U("stnNm") },
	{ U("avgTa"), U("minTa"), U("maxTa"), U("sumRn"),
		U("avgWs"), U("avgTd"), U("avgRhm"), U("avgPv"), U("avgPa"), U("avgPs"),
		U("ssDur"), U("sumSsHr"), U("sumGsr"), U("ddMes"), U("avgTs"), U("sumLrgEv"), U("sumSmlEv") }
};

const utility::string_t itemKey_s[8] = {
	U("date"), U("ht"), U("lt"), U("at"), U("h"), U("r"), U("swdown"), U("ws")
};

const double siteInfo[64][6] = {

	// Site id, latitude, longitude, elevation, TAV(annual average temp), AMP(Annual range)
	{100, 37.677, 128.718, 772, 7.1,  26.6},	// Taegwallyong
	{101, 37.903, 127.736, 76,  11.4, 29.1},	// Chunchon
	{105, 37.751, 128.891, 27,  13.5, 24.1},	// Kangnung
	{108, 37.571, 126.966, 86,  12.8, 28.0},	// Seoul
	{112, 37.478, 126.625, 69,  12.5, 27.1},	// Inchon
	{119, 37.257, 126.983, 40,  17.7, 28.1},	// Suwon
	{129, 36.777, 126.494, 25,  17.5, 26.9},	// Sosan
	{131, 36.639, 127.441, 59,  18.4, 27.7},	// Chongju
	{135, 36.220, 127.996, 245, 11.8, 25.9},	// Chupungnyong
	{136, 36.573, 128.707, 141, 12.2, 26.9},	// Andong
	{138, 36.032, 129.380, 4,   14.6, 23.8},	// Pohang
	{143, 35.878, 128.653, 54,  14.4, 25.6},	// Taegu
	{146, 35.841, 127.117, 60,  13.6, 26.5},	// Chonju
	{156, 35.173, 126.892, 70,  14.1, 25.5},	// Kwangju
	{159, 35.105, 129.032, 70,  15.0, 22.5},	// Pusan
	{165, 34.817, 126.382, 45,  14.0, 24.5},	// Mokpo
	{184, 33.514, 126.530, 21,  16.2, 21.1},	// Cheju
	{192, 35.164, 128.040, 29,  13.4, 25.6}		// Chinju
};

const std::string siteName_legacy[18][2]{

	// Site Id, site name
	{"100", "Taegwallyong"},
	{"101", "Chunchon"},
	{"105", "Kangnung"},
	{"108", "Seoul"},
	{"112", "Incheon"},
	{"119", "Suwon"},
	{"129", "Sosan"},
	{"131", "Chonju"},
	{"135", "Chupungnyong"},
	{"136", "Andong"},
	{"138", "Pohang"},
	{"143", "Taegu"},
	{"146", "Chonju"},
	{"156", "Kwangju"},
	{"159", "Pusan"},
	{"165", "Mokpo"},
	{"184", "Cheju"},
	{"192", "Chinju"}
};

const std::string siteName[18][2]{

	// Site Id, site name
	{"100", "Daegwallyeong"},
	{"101", "Chuncheon"},
	{"105", "Gangneung"},
	{"108", "Seoul"},
	{"112", "Incheon"},
	{"119", "Suwon"},
	{"129", "Seosan"},
	{"131", "Chungju"},
	{"135", "Chupungnyeong"},
	{"136", "Andong"},
	{"138", "Pohang"},
	{"143", "Daegu"},
	{"146", "Jeonju"},
	{"156", "Gwangju"},
	{"159", "Busan"},
	{"165", "Mokpo"},
	{"184", "Jeju"},
	{"192", "Jinju"}
};

const std::string monthName[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const double coef_Frere[3][2] = {
	{0.18, 0.55},			// Cold & temperate climate
	{0.25, 0.45},			// Dry tropical climate
	{0.29, 0.42}			// Humid tropical climate
};

const double coef_Choi_daily[18][3] = {
	{100, 0.175, 0.559},	// Taegwallyong
	{101, 0.207, 0.476},	// Chunchon
	{105, 0.217, 0.482},	// Kangnung
	{108, 0.197, 0.452},	// Seoul
	{112, 0.192, 0.476},	// Inchon
	{119, 0.199, 0.459},	// Suwon
	{129, 0.222, 0.484},	// Sosan
	{131, 0.198, 0.491},	// Chongju
	{135, 0.181, 0.485},	// Chupungnyong
	{136, 0.200, 0.471},	// Andong
	{138, 0.201, 0.493},	// Pohang
	{143, 0.204, 0.463},	// Taegu
	{146, 0.206, 0.470},	// Chonju
	{156, 0.211, 0.495},	// Kwangju
	{159, 0.200, 0.471},	// Pusan
	{165, 0.230, 0.500},	// Mokpo
	{184, 0.197, 0.506},	// Cheju
	{192, 0.194, 0.477}		// Chinju
};

#endif
