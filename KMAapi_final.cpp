#include <iostream>
#include <vector>
#include <iomanip>
#include <list>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

#include "KMAapi_class.h"
#include "KMAapi_struct.h"
#include "KMAapi_DB.h"

using namespace web;                        // Common features including URIs.
using namespace web::http;                  // Common HTTP functionality.
using namespace web::http::client;          // HTTP client features.
using namespace utility;                    // Common utilities including string conversions (especially for Windows & Linux).
using namespace concurrency::streams;
using namespace conversions;				// Functions for unicode string conversions

int main(int argc, char* argv[])
{
	clock_t start = clock();

	if (argc < 4)
	{
		std::cout << "[Error] Incorrect command input. Please provide ouput file format." << std::endl;
		return 1;
	}

	char hd = *argv[1];
	std::string ifName = argv[2];
	std::string ofName = argv[3];

	std::string outFormat[3] = { "null", "null", "null" };
	outFormat[0] = argv[4];

	if (argc > 5) outFormat[1] = argv[5];
	if (argc > 6) outFormat[2] = argv[6];

	if ((hd == 'h') || (hd == 'H'))
	{
		for (int index = 0; index < 3; ++index)
		{
			if (outFormat[index] != "null")
			{
				if ((outFormat[index] != ("epw")) && (outFormat[index] != ("wth")) && (outFormat[index] != ("ory")))
				{
					std::cout << "[Error] Incorrect command input. Please provide ouput file format." << std::endl;
					return 1;
				}
			}
		}

		KMAapi_h kmaapi_h;

		kmaapi_h.setFileName_h(ofName);
		kmaapi_h.getInput_h(ifName);
		kmaapi_h.request_h();
		kmaapi_h.fillMissing_h();


		bool doDaily = false;

		for (int index = 0; index < 3; ++index)
			if ((outFormat[index] == "wth") || (outFormat[index] == "ory"))
				doDaily = true;

		if (doDaily = true)
			kmaapi_h.hourly2daily();

		

		bool doneEpw = false;
		bool doneWth = false;
		bool doneOry = false;

		for (int index = 0; index < 3; ++index)
		{
			if (outFormat[index] == "epw") kmaapi_h.epwOut_h();
			else if (outFormat[index] == "wth") kmaapi_h.wthOut_h();
			else if (outFormat[index] == "ory") kmaapi_h.oryOut_h();
		}
		
		std::cout << "[KMAapi] Hourly data output successful." << std::endl;
		clock_t end = clock();
		std::cout << "[KMAapi] Elasped time: " << (end - start) * 0.001 << "s";
		return 0;
	}
	else if ((hd == 'd') || (hd == 'D'))
	{
		for (int index = 0; index < 2; ++index)
		{
			if (outFormat[index] != "null")
			{
				if ((outFormat[index] != ("wth")) && (outFormat[index] != ("ory")))
				{
					std::cout << "[Error] Incorrect command input. Please provide ouput file format." << std::endl;
					return 1;
				}
			}
		}

		KMAapi_d kmaapi_d;

		kmaapi_d.setFileName_d(ofName);
		kmaapi_d.getInput_d(ifName);
		kmaapi_d.request_d();
		kmaapi_d.fillMissing_d();
		kmaapi_d.calcTemp_d();

		if (kmaapi_d.doEst()) kmaapi_d.estRad_d();
		
		for (int index = 0; index < 2; ++index)
		{
			if (outFormat[index] == "wth") kmaapi_d.wthOut_d();
			else if (outFormat[index] == "ory") kmaapi_d.oryOut_d();
		}

		std::cout << "[KMAapi] Daily data output successful." << std::endl;

		clock_t end = clock();
		std::cout << "[KMAapi] Elasped time: " << (end - start) * 0.001 << "s";
		return 0;
	}
	else if ((hd == 's') || (hd == 'S'))
	{
		for (int index = 0; index < 2; ++index)
		{
			if (outFormat[index] != "null")
			{
				if ((outFormat[index] != ("wth")) && (outFormat[index] != ("ory")))
				{
					std::cout << "[Error] Incorrect command input. Please provide ouput file format." << std::endl;
					return 1;
				}
			}
		}

		KMAapi_s kmaapi_s;

		kmaapi_s.setFileName_s(ofName);
		kmaapi_s.getInput_s(ifName);

		kmaapi_s.request_s();
		kmaapi_s.fillMissing_s();
		kmaapi_s.calcTemp_s();

		for (int index = 0; index < 2; ++index)
		{
			if (outFormat[index] == "wth") kmaapi_s.wthOut_s();
			else if (outFormat[index] == "ory") kmaapi_s.oryOut_s();
		}

		std::cout << "[KMAapi] Selab data output successful." << std::endl;

		clock_t end = clock();
		std::cout << "[KMAapi] Elasped time: " << (end - start) * 0.001 << "s";
		return 0;
	}
	else
	{
		std::cout << "[Error] Incorrect command input. Please try again." << std::endl;
		return 1;
	}


	
}

int calcDoy(int& year, int& month, int& day)
{
	// Montly DOY (Non-leap Year)
	// 1  2  3  4   5   6   7   8   9   10  11  12
	// 31 28 31 30  31  30  31  31  30  31  30  31
	//    59 90 120 151 181 212 243 273 304 334 365

	int doyList[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	int doy = day + doyList[month - 1];

	// Accounting for leap years.
	if ((year % 4 == 0) && month >= 3)
		doy += 1;
	return doy;
}

// Processing hourly inputs.

KMAapi_h::KMAapi_h()
{
	std::cout << "[KMAapi] Initializing KMAapi.cpp..." << std::endl;
}
 
KMAapi_h::~KMAapi_h()
{
	std::cout << "[KMAapi] Program finished. Terminating..." << std::endl;
}


void KMAapi_h::setFileName_h(std::string& ifName)
{
	fileName_h = ifName;
}

void KMAapi_h::getInput_h(std::string& ifName)
{
	std::ifstream input;
	input.open(ifName);
	while (input.fail())
	{
		ifName.clear();
		std::cout << "[Error] Unavailable input file name. Please check the arguments." << std::endl;
		return;
	}

	std::string apiKey;

	int startYr, startMon, startDay, startHr;		
	int endYr, endMon, endDay, endHr;	

	std::stringstream ss;
	char buffer[512];

	input.getline(buffer, 512, '\n');		// Getting API key
	ss << buffer;
	ss >> apiKey;

	key_h = to_string_t(apiKey);
	ss.str("");
	ss.clear();

	// Assume that the given api key is a decoded string
	bool encoded = false;
	for (size_t i = 0; i < key_h.size(); ++i) {
		if (key_h[i] == U('%')) {
			// Check if two characters after '%' are valid hex digits
			if (i + 2 < key_h.size() && std::isxdigit(key_h[i + 1]) && std::isxdigit(key_h[i + 2])) {
				bool encode = true;
				break;
			}
		}
	}
	if (encoded == false) key_h = uri::encode_data_string(key_h);


	input.getline(buffer, 512, '\n');		// Getting parameter names.
	for (int index = 0; index < input.gcount(); ++index)
		if (buffer[index] == ',')
			buffer[index] = ' ';
	ss << buffer;

	while (!ss.eof())
	{
		std::string temp;
		ss >> temp;

		for (int index = 0; index < varNum_h; ++index)
		{
			if (temp == varName_h[index])
			{
				varOpt_h[index] = true;
				break;
			}
		}
	}
	ss.str("");
	ss.clear();



	
	// Getting station ID & date info.

	input.getline(buffer, 64, '\n');		
	for (int index = 0; index < input.gcount(); ++index)
		if (buffer[index] == ',') buffer[index] = ' ';
	
	ss << buffer;
	ss >> stnId_h;
	
	// Finding & translating corresponding station name.
	int numRow = sizeof(siteName) / sizeof(siteName[0]);
	for (int index = 0; index < numRow; ++index)
	{
		if (siteName[index][0] == stnId_h)
			stnNm_h = siteName[index][1];
	}


	ss >> startYr;
	ss >> startMon;
	ss >> startDay;			dayStart_h = startDay;
	ss >> startHr;			hrStart_h = startHr;
	ss >> endYr;
	ss >> endMon;
	ss >> endDay;
	ss >> endHr;
	ss.str("");
	ss.clear();




	// Cutting data sets to monthly set to match request limitations,
	//  which is max. 999 units per request.

	std::vector<int> cutStartDay;
	std::vector<int> cutStartHr;

	std::vector<int> cutEndDay;
	std::vector<int> cutEndHr;

	const int monSize[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


	if ((startYr == endYr) && (startMon == endMon))
	{
		int s = startYr * 10000 + startMon * 100 + startDay;
		int e = endYr * 10000 + endMon * 100 + endDay;

		cutStartDay.push_back(s);
		cutStartHr.push_back(startHr);
		cutEndDay.push_back(e);
		cutEndHr.push_back(endHr);
	}
	else
	{
		int initS = startYr * 10000 + startMon * 100 + startDay;
		int initE = startYr * 10000 + startMon * 100 + monSize[startMon - 1];
		if (((startYr % 4) == 0) && (startMon == 2)) initE += 1;		// Regarding 29th Feb

		cutStartDay.push_back(initS);
		cutStartHr.push_back(startHr);
		cutEndDay.push_back(initE);
		cutEndHr.push_back(23);




		int yearCount = startYr;

		int monCount = startMon + 1;
		if (monCount == 13)
		{
			monCount = 1;
			yearCount += 1;
		}

		while (!((yearCount == endYr) && (monCount == endMon)))
		{
			int tempS = yearCount * 10000 + monCount * 100 + 1;
			int tempE = yearCount * 10000 + monCount * 100 + monSize[monCount - 1];
			if (((yearCount % 4) == 0) && (monCount == 2)) tempE += 1;		// Regarding 29th Feb.

			cutStartDay.push_back(tempS);
			cutStartHr.push_back(0);
			cutEndDay.push_back(tempE);
			cutEndHr.push_back(23);

			++monCount;
			if (monCount == 13)
			{
				monCount = 1;
				yearCount += 1;
			}
		}

		int lastS = endYr * 10000 + endMon * 100 + 1;
		int lastE = endYr * 10000 + endMon * 100 + endDay;

		cutStartDay.push_back(lastS);
		cutStartHr.push_back(0);
		cutEndDay.push_back(lastE);
		cutEndHr.push_back(endHr);
	}
	



	// Storing input data in class member.

	auto sditer = cutStartDay.begin();
	auto shiter = cutStartHr.begin();
	auto editer = cutEndDay.begin();
	auto ehiter = cutEndHr.begin();

	for (; sditer != cutStartDay.end(); ++sditer, ++shiter, ++editer, ++ehiter)
	{
		req_h inputKey;

		std::string startDt = std::to_string(*sditer);		
		
		std::string startHh = "0";
		if (*shiter < 10) startHh.append(std::to_string(*shiter));
		else startHh = std::to_string(*shiter);
		
		std::string endDt = std::to_string(*editer);	

		std::string endHh = "0";
		if (*ehiter < 10) endHh.append(std::to_string(*ehiter));
		else endHh = std::to_string(*ehiter);

		inputKey.startDt = conversions::to_string_t(startDt);
		inputKey.startHh = conversions::to_string_t(startHh);
		inputKey.endDt = conversions::to_string_t(endDt);
		inputKey.endHh = conversions::to_string_t(endHh);

		input_hSet.push_back(inputKey);
	}
	
	
	input.close();
	std::cout << "[KMAapi] File input successful." << std::endl;
}

void KMAapi_h::request_h()
{
	std::list<req_h>::iterator iiter;
	iiter = input_hSet.begin();

	// Required for mass request, as data for max. 999 units can be requested per call.
	const int numOfRows = 999;
	int pageNo = 0, totalCount;

	for (; iiter != input_hSet.end(); iiter++)
	{
		do
		{
			pageNo++;
			web::uri_builder uri;				// A scheme will be set as http://
			uri.set_host(U("apis.data.go.kr"))	// with http_clinet constructor.
				.set_path(U("/1360000/AsosHourlyInfoService/getWthrDataList"))
				.append_query(U("serviceKey"), key_h, false)
				.append_query(U("dataType"), U("JSON"))
				.append_query(U("dataCd"), U("ASOS"))
				.append_query(U("dateCd"), U("HR"))
				.append_query(U("startDt"), iiter->startDt)
				.append_query(U("startHh"), iiter->startHh)
				.append_query(U("endDt"), iiter->endDt)
				.append_query(U("endHh"), iiter->endHh)
				.append_query(U("stnIds"), to_string_t(stnId_h))
				.append_query(U("numOfRows"), numOfRows)
				.append_query(U("pageNo"), pageNo);

			auto requestJson = http_client(uri.to_string())

				.request(methods::GET)

				.then([=](http_response response)
					{
						// Check the status code.
						if (response.status_code() != 200) {
							throw std::runtime_error("[Error] Returned "+ std::to_string(response.status_code()));
						}

						std::cout << "[KMAapi] Received response status code: "
							<< response.status_code() << ". Request successful." << std::endl;
						response.headers().set_content_type(U("application/json;charset=UTF-8"));

						// Convert the response body to JSON object.
						return response.extract_json();
					})

				// Get the items field.
				.then([&totalCount](json::value temp1)
					{
						json::value temp2 = temp1.at(U("response"));
						json::value temp3 = temp2.at(U("body"));
						totalCount = temp3.at(U("totalCount")).as_integer();

						json::value temp4 = temp3.at(U("items"));
						return temp4.at(U("item"));
				})

				// The Json object "items" is a single array named "item".
				// Now parsing the weather details.

				.then([this](json::value root)
				{
					json::array itemList = root.as_array();
					res_hSet monthly;

					std::stringstream buffer;
					std::string date;

					// For alternative data calculation
					double monSum[varNum_h] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
					int hrCount[varNum_h] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

					for (auto iter = itemList.cbegin(); iter != itemList.cend(); ++iter)
					{
						json::value item = *iter;

						// storing json data into class member.
						utility::string_t dateRes;				// e.g. 2020-06-01 00:00
						utility::string_t varRes[varNum_h];			// temp, rain, wspd, rhum, dewp, srad
																// wdir, vpre, pres, spre, sshr, sdep, vsib, stem

						// Flags for data quaility control.
						// Some of the qc flags are not provided; "-1" for missing Qc flags. 

						utility::string_t varFlag[varNum_h];		// tempFlag, rainFlag, wspdFlag, rhumFlag, (null), (null), wdirFlag
															// (null), presFlag, spreFlag, sshrFlag, (null), (null), (null)
						varFlag[4] = U("-1");			
						varFlag[5] = U("-1");
						varFlag[7] = U("-1");
						varFlag[11] = U("-1");
						varFlag[12] = U("-1");
						varFlag[13] = U("-1");
						
						
						dateRes = item[itemKey_h[0][0]].as_string();
						
						for (int index = 0; index < varNum_h; ++index)
							if (varOpt_h[index] == true)
								varRes[index] = item[itemKey_h[1][index]].as_string();

						for (int index = 0; index < varNum_h; ++index)
							if ((varOpt_h[index] == true) && (varFlag[index] != U("-1")))
								varFlag[index] = item[itemKey_h[2][index]].as_string();
							
						

						res_h hourly;

						hourly.time = to_utf8string(dateRes);
						buffer << hourly.time;

						getline(buffer, date, '-');		// Parsing year.
						hourly.year = stoi(date);

						getline(buffer, date, '-');		// Parsing month.
						hourly.month = stoi(date);

						getline(buffer, date, ' ');		// Parsing day.
						hourly.day = stoi(date);

						getline(buffer, date, ':');		// Parsing hour.
						hourly.hour = stoi(date);
						hourly.hour += 1;				// Matching hour range of the response (0~23) and epw format (1~24).

						buffer.str("");
						buffer.clear();
						


						double* varPtr[varNum_h] = {
							&hourly.temp, &hourly.rain, &hourly.wspd,
							&hourly.rhum, &hourly.dewp, &hourly.srad,
							&hourly.wdir, &hourly.vpre, &hourly.lpre, &hourly.spre,
							&hourly.sshr, &hourly.sdep, &hourly.visi, &hourly.stem
						};

						for (int index = 0; index < varNum_h; ++index)
						{
							if (varOpt_h[index] == true)
							{
								*varPtr[index] = checkFlag_h(varRes[index], varFlag[index]);

								if (*varPtr[index] != NODATA)
								{
									monSum[index] += *varPtr[index];
									hrCount[index] += 1;
								}
							}
						}
				
						monthly.hSet.push_back(hourly);
					}

					// Saving monthly names
					auto miter = monthly.hSet.begin();
					std::string monName = fileName_h;
					if (monName == "NoName") monName = stnNm_h;

					monName.append("_").append(std::to_string(miter->year))
						.append("_").append(monthName[miter->month - 1]);
					monName_h.push_back(monName);

					monthly.year = miter->year;

					// Saving alternative data.
					// Default NODATA value will be set if no data is available.
					
					for (int index = 0; index < varNum_h; ++index)
						if (hrCount[index] != 0)
						{
							monthly.altData_h[index] = monSum[index] / hrCount[index];
							hrCount[index] = 0;
						}

					hList.push_back(monthly);
				});

		// Wait for the concurrent tasks to finish.
			try { requestJson.wait(); }
			catch (const std::exception& err)
			{
			std::cout << "[Error] Error exception: " << err.what() << std::endl;
			return;
			}

		} while (pageNo * numOfRows < totalCount);		// Proceed to another request as the responses are provided in page units.
	}

	std::cout << "[KMAapi] Hourly Data request successful." << std::endl;
}

double KMAapi_h::checkFlag_h(utility::string_t &response, utility::string_t &flag)
{
	if (flag == U("-1"))
	{
		if (response == U("")) return NODATA;
		else return stod(to_utf8string(response));
	}
	else if ((flag == U("1")) || (flag == U("9")))		// 0 for default(normal), 1 for error, 9 for missing data
		return NODATA;
	else if ((flag == U("0")) || (flag == U("")))
	{
		if (response == U("")) return 0.0;
		return stod(to_utf8string(response));
	}
	else return NODATA;
}

void KMAapi_h::fillMissing_h()
{
	std::string altName;
	std::stringstream ss;

	std::list<res_hSet>::iterator yiter;
	std::list<res_h>::iterator hiter;

	yiter = hList.begin();
	for (; yiter != hList.end(); ++yiter)
	{
		hiter = yiter->hSet.begin();

		int hrCount;
		int dayCount;

		if (yiter == hList.begin())
		{
			hrCount = hrStart_h + 1;
			dayCount = dayStart_h;
		}
		else
		{
			hrCount = 1;
			dayCount = 1;
		}
			
		
		for (; hiter != yiter->hSet.end(); ++hiter, ++hrCount)
		{
			if (hrCount == 25)
			{
				hrCount = 1;		// Resetting hourCounter every day.
				dayCount += 1;
			}
			
			if (hrCount != hiter->hour)
			{
				res_h missHr;	// Filling missing hourly data.
				std::string time;

				time.append(std::to_string(hiter->year)).append("-")
					.append(std::to_string(hiter->month)).append("-")
					.append(std::to_string(hiter->day)).append(" ")
					.append(std::to_string(hrCount)).append(":00");

				missHr.time = time;
				missHr.year = hiter->year;
				missHr.month = hiter->month;
				missHr.day = dayCount;
				missHr.hour = hrCount;



				double* varMiss[varNum_h] = {
					&missHr.temp, &missHr.rain, &missHr.wspd,
					&missHr.rhum, &missHr.dewp, &missHr.srad,
					&missHr.wdir, &missHr.vpre, &missHr.lpre, &missHr.spre,
					&missHr.sshr, &missHr.sdep, &missHr.visi, &missHr.stem
				};

				for (int count = 0; count < varNum_h; ++count)
					if (varOpt_h[count] == true) *varMiss[count] = NODATA;


				(yiter->hSet).insert(hiter, missHr);		// Set iterator to the inserted hour
				--hiter;
				std::cout << "[KMAapi] Filled missing hourly data " << hiter->time << std::endl;
			}


			double* varNow[varNum_h] = {
				&hiter->temp, &hiter->rain, &hiter->wspd,
				&hiter->rhum, &hiter->dewp, &hiter->srad,
				&hiter->wdir, &hiter->vpre, &hiter->lpre, &hiter->spre,
				&hiter->sshr, &hiter->sdep, &hiter->visi, &hiter->stem
			};
			
			for (int index = 0; index < varNum_h; ++index)
			{
				if ((varOpt_h[index] == true) && (*varNow[index] == NODATA))
				{
					if ((index == 1) || (index == 5) || (index == 11))			// Missing srad, sdep, rain values will be set to 0.
						*varNow[index] = 0;
					else if (yiter->altData_h[index] != NODATA)
						*varNow[index] = yiter->altData_h[index];
					else
					{
						std::cout << "[KMAapi] Missing " << varName_h[index] << " value for " << hiter->time
							<< ".\nIt will be set as default value: " << varDefault_h[index] << std::endl;

						*varNow[index] = varDefault_h[index];
					}
				}
			}
		}
	}
	std::cout << "[KMAapi] Checked and filled missing hourly values." << std::endl;
}

void KMAapi_h::epwOut_h()
{
	std::ofstream epw;

	std::list<res_hSet>::iterator miter;
	std::list<res_h>::iterator hiter;
	std::list<std::string>::iterator niter;

	miter = hList.begin();
	niter = monName_h.begin();


	for (; (miter != hList.end()); ++miter, ++niter)
	{
		std::string ofName = *niter;
		ofName.append(".epw");

		epw.open(ofName);
		if (epw.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open epw output file. Skipping current data set." << std::endl;
			continue;
		}
		

		// Formatting header.

		int info = 0;
		while (siteInfo[info][0] != stoi(stnId_h)) ++info;		// Finding geographic data of the site.

		int timezone = round((siteInfo[info][2]) / 15.0) + 1;

		epw << "LOCATION," << stnNm_h << ",-,-,MN7,999,"
			<< siteInfo[info][1] << "," << siteInfo[info][2] << ","
			<< timezone << "," << siteInfo[info][3] << std::endl;

		epw << "DESIGN CONDITIONS,0\n" << "TYPICAL/EXTREME PERIODS,0\n"
			<< "GROUND TEMPERATURES,1,1.0,,,,10.1,7.4,6.4,7.4,10.1,13.8,17.5,20.1,21.1,20.1,17.5,13.8\n"
			<< "HOLIDAYS/DAYLIGHT SAVINGS,No,0,0,0\n"
			<< "COMMENTS 1,  METEONORM Version 8.0.2.24236\n"
			<< "COMMENTS 2,\n"
			<< "DATA PERIODS,1,1,Data,Sunday,1/1,12/31" << std::endl;
	

		// Formatting data.

		hiter = miter->hSet.begin();
		for (; hiter != miter->hSet.end(); ++hiter)
		{
			// Global radiation value (MJ/m2) must be changed to
			// Wh/m2 in epw files (multiply by pow(10.0, -6.0) * 3600.0).
			// Every minutes value (1~60) will be set to 1.

			// Epw file header format is the following:
			// year, month, day, hour, minute, Datasource,
			// DryBulb{ C }, DewPoint{ C }, RelHum{ % }, Atmos Pressure{ Pa },
			// ExtHorzRad{ Wh/m2 }, ExtDirRad{ Wh/m2 }, HorzIRSky{ Wh/m2 },
			// GloHorzRad{ Wh/m2 }, DirNormRad{ Wh/m2 }, DifHorzRad{ Wh/m2 },
			// GloHorzIllum{ lux }, DirNormIllum{ lux }, DifHorzIllum{ lux }, ZenLum{ Cd/m2 },
			// WindDir{ deg }, WindSpd{ m/s }, TotSkyCvr{ .1 }, OpaqSkyCvr{ .1 },
			// Visibility{ km }, Ceiling Hgt{ m }, PresWeathObs, PresWeathCodes,
			// Precip Wtr{ mm }, Aerosol Opt Depth{ .001 },
			// SnowDepth{ cm }, Days Last Snow, Albedo{ .01 }, Rain{ mm }, Rain Quantity{ hr }

			epw << hiter->year << ',' << hiter->month << ',' << hiter->day << ','
				<< hiter->hour << ',' << "1" << ',' << stnNm_h << ',';

			double pres = NODATA;
			if (hiter->lpre != NODATA) pres = hiter->lpre * 100;		// hPa -> Pa

			double srad = NODATA;										// Wh/m2 -> MJ
			if (hiter->srad != NODATA) srad = hiter->srad / (pow(10.0, -6.0) * 3600.0);



			epw << std::fixed << std::setprecision(1)
				<< hiter->temp << ',' << hiter->dewp << ',' << hiter->rhum << ','
				<< pres << ",-999,-999,-999,"								
				<< srad	<< ",-999,-999,-999,-999,-999,-999,"
				<< hiter->wdir << ',' << hiter->wspd
				<< ",-999,-999,-999,-999,-999,-999,-999,-999,"
				<< hiter->sdep << ",-999,-999,"
				<< hiter->rain << ",-999"
				<< std::endl;
		}
		epw.close();
		epw.clear();
	}
	
	std::cout << "[KMAapi] EPW parsing has been successful." << std::endl;
}

void KMAapi_h::hourly2daily()
{
	std::list<res_h>::iterator hiter;
	std::list<res_hSet>::iterator miter;

	res_hdSet yearly;
	int yrPrev = -1;
	int yrNow = -1;
	bool init = true;

	// Variables to sum & average annual temperatures.

	double tavMonSum = 0.0;		// Yearly sum of monthly average temperature.
	double tavDaySum = 0.0;		// Monthly sum of daily average temperature.
	int daysPerMon = 0;			// The number of days in a month.

	double warmMon = -99.0;
	double coldMon = 99.0;

	double tavYr = 0.0;
	int monsPerYr = 0;

	miter = hList.begin();
	while (miter != hList.end())
	{
		hiter = miter->hSet.begin();

		// Setting flags for year-counting.
		yrPrev = hiter->year;
		if (init == true)
		{
			yrNow = yrPrev;
			init = false;
		}

		while (hiter != miter->hSet.end())
		{
			// Calculating doy.
			res_hd daily;

			int doy;
			doy = calcDoy(hiter->year, hiter->month, hiter->day);

			if (hiter->year > 2000)	doy += (hiter->year - 2000) * 1000;
			else doy += (hiter->year - 1900) * 1000;

			daily.year = hiter->year;
			daily.doy = doy;
			


			double tmax_d = -99.0, tmin_d = 99.0;
			double* varNow[14] = {
				&daily.tavg, &daily.rain, &daily.wspd,
				&daily.rhum, &daily.dewp, &daily.srad,
				&daily.wdir, &daily.vpre, &daily.lpre, &daily.spre,
				&daily.sshr, &daily.sdep, &daily.visi, &daily.stem
			};

			for (int index = 0; index < varNum_h; ++index) *varNow[index] = 0;

			int hrsPerDay = 0;			// Regarding start & end dates,
			int dayNow = hiter->day;	// which can have less than 24 of data.
			
			while (hiter->day == dayNow)
			{
				if (varOpt_h[0] == true)
				{
					*varNow[0] += hiter->temp;
					if (tmax_d < hiter->temp) tmax_d = hiter->temp;	// finding daily max. temperature.
					if (tmin_d > hiter->temp) tmin_d = hiter->temp;	// finding daily min. temperature.
				}

				double varTemp[14] = {
					hiter->temp, hiter->rain, hiter->wspd,
					hiter->rhum, hiter->dewp, hiter->srad,
					hiter->wdir, hiter->vpre, hiter->lpre, hiter->spre,
					hiter->sshr, hiter->sdep, hiter->visi, hiter->stem
				};

				for (int index = 1; index < varNum_h; ++index)
					if (varOpt_h[index] == true) *varNow[index] += varTemp[index];
				
				++hiter; ++hrsPerDay;
				if (hiter == miter->hSet.end()) break;
			}

			for (int index = 0; index < varNum_h; ++index)
			{
				// Getting average daily value
				// excluding rain, solar radiatoin and sunshine hour.
				if (varOpt_h[index] == true)
					if (!((index == 1) || (index == 5) || (index == 10))) *varNow[index] /= hrsPerDay;
			}

			daily.tmax = tmax_d;
			daily.tmin = tmin_d;
			hrsPerDay = 0;

			
			// Saving daily data.
			yearly.hdSet.push_back(daily);
				
			
			// Calculating tav (Average anuual temperature) & amp (Average annual temp. range).
			tavDaySum += daily.tavg;
			
			++daysPerMon;


		} // <- Proceed to next daily data.
		

		tavDaySum /= daysPerMon;
		tavMonSum += tavDaySum;
		tavDaySum = 0;
		daysPerMon = 0;

		// Finding warmest & coldest month.
		if (tavMonSum > warmMon) warmMon = tavMonSum;
		if (tavMonSum < coldMon) coldMon = tavMonSum;

		tavYr += tavMonSum;
		tavMonSum = 0;

		++monsPerYr;
		++miter;

		
		if (miter == hList.end()) yrNow = yrPrev + 1;
		else yrNow = miter->year;

		// Saving yearly data.
		if ((yrNow != yrPrev) || (miter == hList.end()))
		{
			tavYr /= monsPerYr;
			monsPerYr = 0;

			yearly.tav = tavYr;
			yearly.range = warmMon - coldMon;

			hdList.push_back(yearly);
			yrNow = yrPrev;
			yearly.hdSet.clear();

			// Saving yearly file name.
			std::string yrName = fileName_h;

			if (yrName == "NoName") yrName = stnNm_h;
			yrName.append("_").append(std::to_string(yrNow));
			yrName_h.push_back(yrName);
		}

		
	}; // <- Proceed to next monthly data.
	
	std::cout << "[KMAapi] Acquired daily data set." << std::endl;
}

void KMAapi_h::wthOut_h()
{
	std::ofstream wth;

	std::list<res_hdSet>::iterator yiter;
	std::list<res_hd>::iterator diter;
	std::list<std::string>::iterator niter;

	niter = yrName_h.begin();

	const int cellSize = 6;
	const int latLonSize = 9;
	const int doySize = 5;

	yiter = hdList.begin();
	for (; yiter != hdList.end(); ++yiter, ++niter)
	{
		std::string ofName = *niter;
		ofName.append(".wth");

		wth.open(ofName);
		if (wth.fail())
		{
			ofName.clear();
			std::cout << "[KMAapi] Unable to open wth output file. Skipping current data set." << std::endl;
			continue;
		}

		if (!((varOpt_h[0] == 1) && (varOpt_h[1] == 1) && (varOpt_h[5] == 1)))
		{
			// temp, rain, srad
			std::cout << "[Error] Not enough response data for wth output. Please check the input file." << std::endl;
			return;
		}
			

		// Printing file header.
		diter = yiter->hdSet.begin();

		wth << "*WEATHER DATA : " << stnNm_h << "\n\n"
			<< "@ INSI      LAT     LONG  ELEV   TAV   AMP REFHT WNDHT" << std::endl;

		int info = 0;
		while (siteInfo[info][0] != stoi(stnId_h)) ++info;		// Finding geographic data of the site.

		wth << std::right
			<< std::setw(cellSize) << std::right << stnNm_h.substr(0, 4)	
			<< std::setw(latLonSize) << siteInfo[info][1]
			<< std::setw(latLonSize) << siteInfo[info][2]
			<< std::setw(cellSize) << siteInfo[info][3]
			<< std::setw(cellSize) << std::fixed << std::setprecision(2) << yiter->tav
			<< std::setw(cellSize) << std::fixed << yiter->range
			<< std::setw(cellSize) << std::fixed << std::setprecision(1) << 1.5 
			<< std::setw(cellSize) << std::fixed << 2.0 << std::endl;	// default REFHT & WNDHT values are set.

		wth << "@DATE  SRAD  TMAX  TMIN  RAIN";
		if (varOpt_h[4] == true) wth << std::setw(cellSize) << std::right << "DEWP";
		if (varOpt_h[2] == true) wth << std::setw(cellSize) << std::right << "WIND";
		if (varOpt_h[3] == true) wth << std::setw(cellSize) << std::right << "RHUM";
		wth << std::endl;


		// Formatting data.
		for (; diter != yiter->hdSet.end(); diter++)
		{
			wth << std::setw(doySize) << std::right << std::setfill('0') << diter->doy << std::setfill(' ')
				<< std::setw(cellSize) << std::fixed << std::setprecision(1) << diter->srad	// MJ/m2
				<< std::setw(cellSize) << diter->tmax
				<< std::setw(cellSize) << diter->tmin
				<< std::setw(cellSize) << diter->rain;

			if (varOpt_h[4] == true)
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->dewp;

			if (varOpt_h[2] == true) 
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< (diter->wspd) * 86.4;						// m/s -> km/day

			if (varOpt_h[3] == true)
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->rhum;

			wth << std::endl;
		}

		wth.close();
		wth.clear();
	}
	std::cout << "[KMAapi] Hourly wth parsing successful." << std::endl;
}

void KMAapi_h::oryOut_h()
{
	std::ofstream ory;

	std::list<res_hdSet>::iterator yiter;
	std::list<res_hd>::iterator diter;
	std::list<std::string>::iterator niter;

	niter = yrName_h.begin();

	if (!((varOpt_h[0] == 1) && (varOpt_h[1] == 1) && (varOpt_h[2] == 1)
		&& (varOpt_h[3] == 1) && (varOpt_h[7] == 1)))
	{
		// temp, rain, wspd, rhum, vpre
		std::cout << "[Error] Not enough response data for wth output. Please check the input file." << std::endl;
		return;
	}

		
	yiter = hdList.begin();
	for (; yiter != hdList.end(); ++yiter, ++niter)
	{
		std::string ofName = stnNm_h.substr(0, 4);
		ofName.append(stnId_h).append(".")
			.append(niter->substr(niter->length() - 3));		// e.g. Seoul_2019 -> Seou108.019

		ory.open(ofName);
		if (ory.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open oryza output file. Skipping current data set." << std::endl;
			continue;
		}

		// Formatting file header.
		int info = 0;
		while (siteInfo[info][0] != stoi(stnId_h)) ++info;		// Finding geographic data of the site.

		ory << siteInfo[info][2] << ',' << siteInfo[info][1] << ',' << siteInfo[info][3] << ','
			<< 0.00 << ',' << 0.00 << std::endl;


		// Formatting data.
		diter = (yiter->hdSet).begin();
		for (; diter != (yiter->hdSet).end(); ++diter)
		{
			int year2digit;
			if (diter->year >= 2000) year2digit = diter->year - 2000;
			else year2digit = diter->year - 1900;

			ory << stnId_h << ','
				<< diter->year << ','
				<< diter->doy - year2digit * 1000 << ','
				<< diter->srad * 1000 << ','		// MJ -> KJ/(m2 * day)
				<< std::fixed << std::setprecision(1) << diter->tmin << ','
				<< diter->tmax << ','
				<< std::fixed << std::setprecision(2) << diter->vpre * 0.1 << ','			// hPa -> kPa
				<< std::fixed << std::setprecision(1) << diter->wspd << ','					// m/s
				<< diter->rain
				<< std::endl;
		}
		ory.close();
		ory.clear();
	}

	std::cout << "[KMAapi] Oryza file output successful." << std::endl;
}



// Processing daily inputs.

KMAapi_d::KMAapi_d()
{
	std::cout << "[KMAapi] Initializing KMAapi.cpp..." << std::endl;
};

KMAapi_d::~KMAapi_d() {
	std::cout << "[KMAapi] Program finished. Terminating..." << std::endl;
};

void KMAapi_d::setFileName_d(std::string& ofName)
{
	fileName_d = ofName;
}

bool KMAapi_d::doEst()
{
	if ((coef_a != 0) || (coef_b != 0))	return true;
	else return false;
}

void KMAapi_d::getInput_d(std::string& ifName)
{
	std::ifstream input;

	input.open(ifName);
	while (input.fail())
	{
		ifName.clear();
		std::cout << "[Error] Unavailable input file name. Please check the arguments." << std::endl;
		return;
	}

	std::string apiKey;
	int startYr, startMon, startDay;
	int endYr, endMon, endDay;

	std::stringstream ss;
	char buffer[512];

	input.getline(buffer, 512, '\n');		// Getting API key
	ss << buffer;
	ss >> apiKey;
	key_d = to_string_t(apiKey);
	ss.str("");
	ss.clear();

	// Assure that the given api key is encoded
	bool encoded = false;
	for (size_t i = 0; i < key_d.size(); ++i) {
		if (key_d[i] == U('%')) {
			// Check if two characters after '%' are valid hex digits
			if (i + 2 < key_d.size() && std::isxdigit(key_d[i + 1]) && std::isxdigit(key_d[i + 2])) {
				bool encode = true;
				break;
			}
		}
	}
	if (encoded == false) key_d = uri::encode_data_string(key_d); // uri::encode_uri() does not work
	//std::wcout << key_d << std::endl;

	input.getline(buffer, 512, '\n');		// Getting parameter names.
	for (int index = 0; index < input.gcount(); index++)
		if (buffer[index] == ',')
			buffer[index] = ' '; 
	ss << buffer;

	while (!ss.eof())
	{
		std::string temp;
		ss >> temp;

		for (int index = 0; index < varNum_d; ++index)
		{
			if (temp == varName_d[index])
			{
				varOpt_d[index] = true;
				break;
			}
		}
	}
	ss.str("");
	ss.clear();

	input.getline(buffer, 64, '\n');		// Getting date info.
	for (int index = 0; index < input.gcount(); index++)
		if (buffer[index] == ',')
			buffer[index] = ' ';
	ss << buffer;

	ss >> stnId_d;
	ss >> startYr;
	ss >> startMon;			monStart_d = startMon;
	ss >> startDay;			doyStart_d = calcDoy(startYr, startMon, startDay);
	ss >> endYr;
	ss >> endMon;
	ss >> endDay;

	// Finding & translating corresponding station name.
	int numRow = sizeof(siteName) / sizeof(siteName[0]);
	for (int index = 0; index < numRow; ++index)
	{
		if (siteName[index][0] == stnId_d)
			stnNm_d = siteName[index][1];
	}

	ss.str("");
	ss.clear();



	// Getting AP coefficients
	input.getline(buffer, 32, '\n');		

	for (int index = 0; index < input.gcount(); index++)
		if (buffer[index] == ',')
			buffer[index] = ' ';
	ss << buffer;


	double a, b;
	ss >> a;
	ss >> b;

	if ((a == 0) && (b == 0))
	{
		std::cout << "[KMAapi] No estimation using AP coefficients." << std::endl;
		
		if (varOpt_d[14] == 0)
		{
			std::cout << "[Error] No srad request has been made. Please check the inputs." << std::endl;
			return;
		}
	}
	else
	{
		std::cout << "[KMAapi] Solar radiation values will be estimated using the provided coefficients." << std::endl;
		
		if ((varOpt_d[12] == false) || (varOpt_d[13] == false))
		{
			std::cout << "[Error] Both sdur & sshr requests are required. Pleace check the inputs. ";
			return;
		}
	}
	
	coef_a = a;
	coef_b = b;
		


	// Cutting data sets to yearly set to match request limitations,
	// which is max. 999 units per request.

	std::vector<int> cutStart;
	std::vector<int> cutEnd;

	int monSize[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (startYr == endYr)
	{
		int s = startYr * 10000 + startMon * 100 + startDay;
		int e = endYr * 10000 + endMon * 100 + endDay;

		cutStart.push_back(s);
		cutEnd.push_back(e);
	}
	else
	{
		int initStart = startYr * 10000 + startMon * 100 + startDay;
		int initEnd = startYr * 10000 + 1231;

		cutStart.push_back(initStart);
		cutEnd.push_back(initEnd);


		int yearCount = startYr + 1;
		while (yearCount != endYr)
		{
			int tempStart = yearCount * 10000 + 101;		// 20220101 for example
			int tempEnd = yearCount * 10000 + 1231;

			cutStart.push_back(tempStart);
			cutEnd.push_back(tempEnd);
			++yearCount;
		}


		int lastStart = endYr * 10000 + 101;
		int lastEnd = endYr * 10000 + endMon * 100 + endDay;

		cutStart.push_back(lastStart);
		cutEnd.push_back(lastEnd);
	}

	


	// Storing input data in class member.

	auto siter = cutStart.begin();
	auto eiter = cutEnd.begin();

	for (; ((siter != cutStart.end()) && (eiter != cutEnd.end())); ++siter, ++eiter)
	{
		req_d inputKey;

		std::string startDt = std::to_string(*siter);
		std::string endDt = std::to_string(*eiter);

		inputKey.startDt = conversions::to_string_t(startDt);
		inputKey.endDt = conversions::to_string_t(endDt);

		input_dSet.push_back(inputKey);			
	}

	input.close();
	std::cout << "[KMAapi] File input successful." << std::endl;
}

void KMAapi_d::request_d()
{
	std::list<req_d>::iterator iiter;
	iiter = input_dSet.begin();

	// Required for mass request, as max. 999 rows of data can be requested per call.
	const int numOfRows = 999;
	int pageNo = 0, totalCount;

	for (; iiter != input_dSet.end(); iiter++)
	{
		do
		{
			pageNo++;

			auto requestJson = http_client(U("http://apis.data.go.kr/1360000/AsosDalyInfoService/getWthrDataList?"))

				// Build request URI and start the request.
				.request(methods::GET, uri_builder()
					.append_query(U("serviceKey"), key_d, false)
					.append_query(U("dataType"), U("JSON"))
					.append_query(U("dataCd"), U("ASOS"))
					.append_query(U("dateCd"), U("DAY"))
					.append_query(U("startDt"), iiter->startDt)
					.append_query(U("endDt"), iiter->endDt)
					.append_query(U("stnIds"), to_string_t(stnId_d))
					.append_query(U("numOfRows"), numOfRows)
					.append_query(U("pageNo"), pageNo)
					.to_string())

				.then([=](http_response response)
					{
						// Check the status code.
						if (response.status_code() != 200) {
							throw std::runtime_error("[Error] Returned " + std::to_string(response.status_code()));
						}

						std::cout << "[KMAapi] Received response status code: "
							<< response.status_code() << ". Request successful." << std::endl;

						// Set headers to receive content type as JSON
						// Charset should be UTF-8 to avoid errors upon parsing Korean.
						response.headers().set_content_type(U("application/json;charset=UTF-8"));

						// Convert the response body to JSON object.
						return response.extract_json();
					})

				// Get the items field.
				.then([&totalCount](json::value temp1)
					{
						json::value temp2 = temp1.at(U("response"));
						json::value temp3 = temp2.at(U("body"));
						totalCount = temp3.at(U("totalCount")).as_integer();

						json::value temp4 = temp3.at(U("items"));
						return temp4.at(U("item"));
					})

				// The Json object "items" is a single Json array named "item".
				// Now parsing the weather details.

				.then([this, &iiter](json::value root)
					{
						json::array itemList = root.as_array();
						res_dSet yearly;

						std::stringstream buffer;
						std::string date;

						// For alternative data calculation
						double yrSum[varNum_d] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
						int dayCount[varNum_d] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

						for (auto iter = itemList.cbegin(); iter != itemList.cend(); ++iter)
						{
							json::value item = *iter;

							// storing json data into class member.
							utility::string_t dateRes;			
							utility::string_t varRes[varNum_d];

							dateRes = item[itemKey_d[0][0]].as_string();

							for (int index = 0; index < varNum_d; ++index)
								if (varOpt_d[index] == true)
									varRes[index] = item[itemKey_d[1][index]].as_string();
									
										

							res_d daily;

							daily.time = to_utf8string(dateRes);
							buffer << daily.time;

							getline(buffer, date, '-');		// Parsing year.
							daily.year = stoi(date);

							getline(buffer, date, '-');		// Parsing month.
							daily.month = stoi(date);

							getline(buffer, date);		// Parsing day.
							daily.day = stoi(date);

							buffer.str("");
							buffer.clear();

							daily.doy = calcDoy(daily.year, daily.month, daily.day);
							


							// Qc flags for daily requests are not provided.

						

							double* varPtr[varNum_d] = {
								&daily.tavg, &daily.tmin, &daily.tmax, &daily.rain,
								&daily.avgWspd, &daily.avgDewp, &daily.avgRhum,
								&daily.avgVpre, &daily.avgLpre, &daily.avgSpre,
								&daily.sdur, &daily.sshr, &daily.srad,
								&daily.sdep, &daily.stem, &daily.leva, &daily.seva
							};
								
							for (int index = 0; index < varNum_d; ++index)
							{
								if (varOpt_d[index] == true)
								{
									if (varRes[index] == U(""))
										*varPtr[index] = NODATA;
									else
									{
										*varPtr[index] = stod(to_utf8string(varRes[index]));
										
										yrSum[index] += *varPtr[index];
										dayCount[index] += 1;
									}
											
								}
							}

							yearly.dSet.push_back(daily);
						}

						// Saving yearly names for epw outputs.

						auto yiter = yearly.dSet.begin();
						std::string yrName = fileName_d;

						if (yrName == "NoName") yrName = stnId_d;

						yrName.append("_").append(std::to_string(yiter->year));
						yrName_d.push_back(yrName);

						

						// Saving alternative data.
						// Default NODATA value will be set if no data is available.

						for (int index = 0; index < varNum_d; ++index)
							if (dayCount[index] != 0)
								yearly.altData_d[index] = (yrSum[index] /= dayCount[index]);
						
						dList.push_back(yearly);
				});

				// Waiting for the concurrent tasks to finish.
				try {
					requestJson.wait();
				}
				catch (const std::exception& e) {
					std::cout << "[Error] Error exception: " << e.what() << std::endl;
				}
		} while (pageNo * numOfRows < totalCount);
	}

	std::cout << "[KMAapi] Daily data request successful." << std::endl;	// Proceed to another request as the responses are provided in page units.
}

void KMAapi_d::fillMissing_d()
{
	std::string altName;
	std::stringstream ss;

	std::list<res_dSet>::iterator yiter;
	std::list<res_d>::iterator diter;

	int doyList[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	// Filling missing daily data.
	yiter = dList.begin();
	for (; yiter != dList.end(); yiter++)
	{
		diter = yiter->dSet.begin();
		
		int yrNow = diter->year;
		int monCount;
		int doyCount;

		
		if (yiter == dList.begin())
		{
			monCount = monStart_d;
			doyCount = doyStart_d; 
		}
		else
		{
			monCount = 1;
			doyCount = 1;
		}

		for (; diter != yiter->dSet.end(); ++diter)
		{
			if (doyCount != diter->doy)
			{
				res_d missDay;	// Filling missing daily data.
				std::string time;

				// Calculating day from doy
				int day;
				if ((yrNow % 4 == 0) && (doyCount > 59))		// Regarding leap year.
					day = doyCount - (doyList[monCount - 1] - 1);
				else day = doyCount - doyList[monCount - 1];
				

				time.append(std::to_string(yrNow)).append("-")
					.append(std::to_string(monCount)).append("-")
					.append(std::to_string(day));

				missDay.time = time;
				missDay.year = diter->year;
				missDay.month = monCount;
				missDay.day = day;
				missDay.doy = doyCount;

				

				double* varMiss[varNum_d] = {

					&missDay.tavg, &missDay.tmin, &missDay.tmax, &missDay.rain,
					&missDay.avgWspd, &missDay.avgDewp, &missDay.avgRhum,
					&missDay.avgVpre, &missDay.avgLpre, &missDay.avgSpre,
					&missDay.sdur, &missDay.sshr, &missDay.srad, &missDay.sdep,
					&missDay.stem, &missDay.leva, &missDay.seva
				};

				for (int count = 0; count < varNum_d; ++count)
					if (varOpt_d[count] == true) *varMiss[count] = NODATA;


				(yiter->dSet).insert(diter, missDay);
				--diter;	// Set iterator to the inserted day to fill rest of the data.
				std::cout << "[KMAapi] Filled missing daily data " << diter->time << std::endl;
			}

			double* varNow[varNum_d] = {
				&diter->tavg, &diter->tmin, &diter->tmax, &diter->rain,
				&diter->avgWspd, &diter->avgDewp, &diter->avgRhum,
				&diter->avgVpre, &diter->avgLpre, &diter->avgSpre,
				&diter->sdur, &diter->sshr, &diter->srad, &diter->sdep,
				&diter->stem, &diter->leva, &diter->seva
			};

			for (int index = 0; index < varNum_d; ++index)
			{
				if ((varOpt_d[index] == true) && (*varNow[index] == NODATA))
				{
					if ((index == 3) || (index == 12) || (index == 13))			// Missing srad, sdep, rain values will be set to 0.
						*varNow[index] = 0;
					else if (yiter->altData_d[index] != NODATA)
						*varNow[index] = yiter->altData_d[index];
					else
					{
						std::cout << "[KMAapi] Missing " << varName_d[index] << " value for " << diter->time
							<< ".\nIt will be set as default value: " << varDefault_d[index];

						*varNow[index] = varDefault_d[index];
					}
				}
			}

			

			if ((yrNow % 4 == 0) && (doyCount > 59) && (doyCount - 1 == doyList[monCount]))
				++monCount;
			else if ((yrNow % 4 == 0) && (doyCount == 31))
				++monCount;
			else if ((yrNow % 4 != 0) && (doyCount == doyList[monCount]))
				++monCount;
			
			++doyCount;
			
		}
	}
			
	std::cout << "[KMAapi] Missing daily values have been alternated." << std::endl;
}

void KMAapi_d::calcTemp_d()
{
	std::list<res_d>::iterator diter;
	std::list<res_dSet>::iterator yiter;

	yiter = dList.begin();
	for (; yiter != dList.end(); ++yiter)
	{
		diter = yiter->dSet.begin();
		for (; diter != yiter->dSet.end();)
		{
			// Variables to sum & average annual temperatures.

			double tav = 0;
			double warmMon = -99.0;
			double coldMon = 99.0;

			int monPrev = diter->month;
			int monCount = 0;

			int count = 0;
			int daysPerMon = 0;
			
			while (count < (yiter->dSet).size())
			{
				double monSum = 0.0;

				// Summing monthly average temperature.
				while (monPrev == diter->month)
				{
					monSum += diter->tavg;
					++daysPerMon;
					++diter;
					++count;

					if (diter == yiter->dSet.end()) break;
				}

				// Getting monthly average temperature.
				monSum /= daysPerMon;
				tav += monSum;
				
				if (monSum > warmMon) warmMon = monSum;
				if (monSum < coldMon) coldMon = monSum;

				daysPerMon = 0;
				if (diter != yiter->dSet.end()) monPrev = diter->month;
				++monCount;
			}

			tav /= monCount;
			yiter->tav = tav;
			yiter->range = warmMon - coldMon;
		}
	}

	std::cout << "[KMAapi] Calculated annual average temperature & range." << std::endl;
}

void KMAapi_d::estRad_d()
{
	std::string ifName;
	std::stringstream ss;

	std::list<res_dSet>::iterator dsiter;
	std::list<res_d>::iterator diter;

	double rowCount = 0, lat_rad = 0;

	int info = 0;
	while (siteInfo[info][0] != stoi(stnId_d)) ++info;		// Finding geographic data of the site.
	lat_rad = siteInfo[info][1] * (PI / 180.0);				// Degree -> Radian

	
	dsiter = dList.begin();
	for (; dsiter != dList.end(); dsiter++)
	{
		res_dSet yearly;
		diter = dsiter->dSet.begin();

		for (; diter != dsiter->dSet.end(); diter++)
		{
			int doy_year;

			if (diter->year > 2000) doy_year = diter->doy + (diter->year - 2000) * 1000;
			else doy_year = diter->doy + (diter->year - 1900) * 1000;

			// Estimated daily solar radiation, external solar radiation,
			// latitude (radian), daily declination (radian), daily sunset hour angle (radian),
			// relative distance between the sun and the earth,
			// solar constant in MJ(m2*min), respectively.

			double estSrad, extRad, dailyDecl, ssHrAngle, relDis;
			const double sol = 0.082;

			dailyDecl = 0.409 * sin((2.0 * PI / 365.0) * diter->doy - 1.39);
			ssHrAngle = acos((-1.0) * tan(lat_rad) * tan(dailyDecl));
			relDis = 1 + 0.033 * cos((2 * PI / 365.0) * diter->doy);

			extRad = (24.0 * 60.0 / PI) * sol * relDis *
				(ssHrAngle * sin(lat_rad) * sin(dailyDecl)
					+ cos(lat_rad) * cos(dailyDecl) * sin(ssHrAngle));

			estSrad = extRad * (coef_a + coef_b * (diter->sshr / diter->sdur));

			diter->sradEst = estSrad;
		}
	}

	std::cout << "[KMAapi] Estimated solar radiation successfully." << std::endl;
}

void KMAapi_d::wthOut_d()
{
	std::ofstream wth;

	std::list<res_dSet>::iterator yiter;
	std::list<res_d>::iterator diter;
	std::list<std::string>::iterator niter;

	yiter = dList.begin();
	niter = yrName_d.begin();

	const int cellSize = 6;
	const int latLonSize = 9;
	const int doySize = 5;

	bool outEst = false;
	if ((coef_a != 0) || (coef_b != 0)) outEst = true;
	

	if (!((varOpt_d[0] == 1) && (varOpt_d[1] == 1) && (varOpt_d[2] == 1)))
	{
		// tmin, tmax, rain
		std::cout << "[Error] Not enough response data for wth output. Please check the input file." << std::endl;
		return;
	}

	for (; yiter != dList.end(); ++yiter, ++niter)
	{
		std::string ofName = *niter;
		ofName.append(".wth");

		wth.open(ofName);
		if (wth.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open wth output file. Skipping current data set." << std::endl;
			continue;
		}

		// Printing file header.
		diter = yiter->dSet.begin();

		wth << "*WEATHER DATA : " << stnNm_d << "\n\n"
			<< std::right
			<< std::setw(cellSize) << "@ INSI"
			<< std::setw(latLonSize) << "LAT"
			<< std::setw(latLonSize) << "LONG"
			<< std::setw(cellSize) << "ELEV"
			<< std::setw(cellSize) << "TAV"
			<< std::setw(cellSize) << "AMP"
			<< std::setw(cellSize) << "REFHT"
			<< std::setw(cellSize) << "WNDHT"
			<< std::endl;

		int info = 0;
		while (siteInfo[info][0] != stoi(stnId_d)) ++info;		// Finding geographic data of the site.

		wth << std::setw(cellSize) << std::right << stnNm_d.substr(0, 4)
			<< std::setw(latLonSize) << siteInfo[info][1]
			<< std::setw(latLonSize) << siteInfo[info][2]
			<< std::setw(cellSize) << siteInfo[info][3]
			<< std::setw(cellSize) << std::fixed << std::setprecision(2) << yiter->tav
			<< std::setw(cellSize) << yiter->range
			<< std::setw(cellSize) << std::fixed << std::setprecision(1) << 1.5
			<< std::setw(cellSize) << 2.0
			<< std::endl;	// default REFHT & WNDHT values are set.
		
		wth << "@DATE"
			<< std::right
			<< std::setw(cellSize) << "SRAD"
			<< std::setw(cellSize) << "TMAX"
			<< std::setw(cellSize) << "TMIN"
			<< std::setw(cellSize) << "RAIN";

		if (varOpt_d[7] == true) wth << std::setw(cellSize) << std::right << "DEWP";
		if (varOpt_d[6] == true) wth << std::setw(cellSize) << std::right << "WIND";
		if (varOpt_d[8] == true) wth << std::setw(cellSize) << std::right << "RHUM";
		wth << std::endl;



		// Formatting data.
		for (; diter != yiter->dSet.end(); diter++)
		{
			int year2digit;
			if (diter->year >= 2000) year2digit = diter->year - 2000;
			else year2digit = diter->year - 1900;

			wth << std::setw(doySize) << std::right << std::setfill('0') << diter->doy + year2digit * 1000 << std::setfill(' ');

			if (outEst == false)
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->srad;				// MJ/m2
			else
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->sradEst;			

			wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right << diter->tmax
				<< std::setw(cellSize) << diter->tmin
				<< std::setw(cellSize) << diter->rain;
			
			if (varOpt_d[7] == true) 
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->avgDewp;

			if (varOpt_d[6] == true)
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< (diter->avgWspd) * 86.4;						// m/s -> km/day

			if (varOpt_d[8] == true)
				wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->avgRhum;

			wth << std::endl;
		}

		wth.close();
		wth.clear();
	}
	std::cout << "[KMAapi] Daily wth parsing successful." << std::endl;
};

void KMAapi_d::oryOut_d()
{
	std::ofstream ory;

	std::list<res_dSet>::iterator yiter;
	std::list<res_d>::iterator diter;
	std::list<std::string>::iterator niter;

	niter = yrName_d.begin();

	bool outEst = false;
	if ((coef_a != 0) || (coef_b != 0)) outEst = true;

	if (!((varOpt_d[1] == 1) && (varOpt_d[2] == 1) && (varOpt_d[3] == 1)
		&& (varOpt_d[4] == 1) && (varOpt_d[7] == 1)))
	{
		// tmin, tmax, rain, avgWspd, avgVpre
		std::cout << "[Error] Not enough response data for wth output. Please check the input file." << std::endl;
		return;
	}

	yiter = dList.begin();
	for (; yiter != dList.end(); ++yiter, ++niter)
	{
		diter = (yiter->dSet).begin();

		std::string ofName = stnNm_d;
		if (outEst == true) ofName.append("Est");
		ofName.append(stnId_d).append(".").append(niter->substr(niter->length() - 3));		// e.g. Seoul_2019 -> Seou108.019

		ory.open(ofName);
		if (ory.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open oryza output file. Skipping current data set." << std::endl;
			continue;
		}


		// Formatting file header.

		int info = 0;
		while (siteInfo[info][0] != stoi(stnId_d)) ++info;		// Finding geographic data of the site.

		ory << siteInfo[info][2] << ',' << siteInfo[info][1] << ',' << siteInfo[info][3] << ','
			<< coef_a << ',' << coef_b << std::endl;


		// Formatting data.
		for (; diter != (yiter->dSet).end(); ++diter)
		{
			ory << stnId_d << ',' << diter->year << ',' << diter->doy << ',';

			if (outEst == false) ory << diter->srad * 1000 << ',';		// MJ -> KJ/(m2 * day)
			else ory << diter->sshr << ',';

			ory << std::fixed << std::setprecision(1) << diter->tmin << ','
				<< diter->tmax << ','
				<< std::fixed << std::setprecision(2) << diter->avgVpre * 0.1 << ','			// hPa -> kPa
				<< std::fixed << std::setprecision(1) << diter->avgWspd << ','					// m/s
				<< diter->rain
				<< std::endl;
		}
		ory.close();
		ory.clear();
	}

	std::cout << "[KMAapi] Oryza file output successful." << std::endl;
}


// Processing SELAB inputs.
KMAapi_s::KMAapi_s()
{
	std::cout << "[KMAapi] Initializing KMAapi.cpp..." << std::endl;
};

KMAapi_s::~KMAapi_s()
{
	std::cout << "[KMAapi] Program finished. Terminating..." << std::endl;
}

void KMAapi_s::setFileName_s(std::string& ofName)
{
	fileName_s = ofName;
}

void KMAapi_s::getInput_s(std::string& ifName)
{
	std::ifstream input;
	input.open(ifName);
	while (input.fail())
	{
		ifName.clear();
		std::cout << "[Error] Unavailable input file name. Please check the arguments." << std::endl;
		return;
	}



	int startYr, startMon, startDay;
	int endYr, endMon, endDay;

	std::stringstream ss;
	char buffer[512];

	input.getline(buffer, 32, '\n');		// Getting input data.
	for (int index = 0; index < input.gcount(); index++)
		if (buffer[index] == ',')
			buffer[index] = ' ';
	ss << buffer;

	ss >> lat;
	ss >> lon;
	ss.str("");
	ss.clear();

	input.getline(buffer, 32, '\n');		// Getting input data.
	for (int index = 0; index < input.gcount(); index++)
		if (buffer[index] == ',')
			buffer[index] = ' ';
	ss << buffer;

	ss >> startYr;
	ss >> startMon;			monStart_s = startMon;
	ss >> startDay;			doyStart_s = calcDoy(startYr, startMon, startDay);
	ss >> endYr;
	ss >> endMon;
	ss >> endDay;

	ss.str("");
	ss.clear();



	// Cutting data sets to yearly set to match request limitations,
	// which is max. 999 units per request.

	std::vector<std::string> cutStart;
	std::vector<std::string> cutEnd;

	int monSize[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (startYr == endYr)
	{
		std::string s = std::to_string(startYr);
		
		if (startMon < 10) s.append("-0");
		else s.append("-");
		s.append(std::to_string(startMon));
		
		if (startDay < 10) s.append("-0");
		else s.append("-");
		s.append(std::to_string(startDay));

		std::string e = std::to_string(endYr);
		
		if (endMon < 10) s.append("-0");
		else s.append("-");
		s.append(std::to_string(endMon));
		
		if (endDay < 10) s.append("-0");
		else s.append("-");
		s.append(std::to_string(endDay));

		cutStart.push_back(s);
		cutEnd.push_back(e);
	}
	else
	{
		std::string initStart = std::to_string(startYr);

		if (startMon < 10) initStart.append("-0");
		else initStart.append("-");
		initStart.append(std::to_string(startMon));
		if (startDay < 10) initStart.append("-0");
		else initStart.append("-");
		initStart.append(std::to_string(startDay));

		std::string initEnd = std::to_string(startYr);
		initEnd.append("-12-31");

		cutStart.push_back(initStart);
		cutEnd.push_back(initEnd);



		int yearCount = startYr + 1;
		while (yearCount != endYr)
		{
			std::string tempStart = std::to_string(yearCount);
			tempStart.append("-01-01");

			std::string tempEnd = std::to_string(yearCount);
			tempEnd.append("-12-31");

			cutStart.push_back(tempStart);
			cutEnd.push_back(tempEnd);
			++yearCount;
		}

		std::string lastStart = std::to_string(endYr);
		lastStart.append("-01-01");

		std::string lastEnd = std::to_string(endYr);
		
		if (endMon < 10) lastEnd.append("-0");
		else lastEnd.append("-");
		lastEnd.append(std::to_string(endMon));

		if (endDay < 10) lastEnd.append("-0");
		else lastEnd.append("-");
		lastEnd.append(std::to_string(endDay));

		cutStart.push_back(lastStart);
		cutEnd.push_back(lastEnd);
	}


	// Storing input data in class member.

	auto siter = cutStart.begin();
	auto eiter = cutEnd.begin();

	for (; ((siter != cutStart.end()) && (eiter != cutEnd.end())); ++siter, ++eiter)
	{
		req_s inputKey;

		inputKey.startDt = conversions::to_string_t(*siter);
		inputKey.endDt = conversions::to_string_t(*eiter);

		input_sSet.push_back(inputKey);
	}

	input.close();
	std::cout << "[KMAapi] File input successful." << std::endl;
}

void KMAapi_s::request_s()
{
	std::list<req_s>::iterator iiter;
	iiter = input_sSet.begin();

	for (; iiter != input_sSet.end(); iiter++)
	{
		web::uri_builder uri;				// A scheme will be set as http://
		uri.set_host(U("livinglab.selab.cloud"))	// with http_clinet constructor.
			.set_path(U("/api/seamlessForecast"))
			.append_query(U("startDate"), iiter->startDt)
			.append_query(U("endDate"), iiter->endDt)
			.append_query(U("latitude"), to_string_t(std::to_string(lat)))
			.append_query(U("longitude"), to_string_t(std::to_string(lon)));

		auto requestJson = http_client(uri.to_string())

			.request(methods::GET)

			.then([=](http_response response)
				{
					// Check the status code.
					if (response.status_code() != 200) {
						throw std::runtime_error("[Error] Returned " + std::to_string(response.status_code()));
					}

					std::cout << "[KMAapi] Received response status code: "
						<< response.status_code() << ". Request successful." << std::endl;

					// Set headers to receive content type as JSON
					// Charset should be UTF-8 to avoid errors upon parsing Korean.
					response.headers().set_content_type(U("application/json;charset=UTF-8"));

					// Convert the response body to JSON object.
					return response.extract_json();
			})

			// Get the data field.
			.then([](json::value temp) { return temp.at(U("data"));
				})

			.then([this, &iiter](json::value root)
			{
				json::array itemList = root.as_array();
				res_sSet yearly;

				std::stringstream buffer;
				std::string date;

				// For alternative data calculation
				double yrSum[varNum_s] = { 0, 0, 0, 0, 0, 0, 0 };
				int dayCount[varNum_s] = { 0, 0, 0, 0, 0, 0, 0 };
				bool checkInit = false;

				for (auto iter = itemList.cbegin(); iter != itemList.cend(); ++iter)
				{
					json::value item = *iter;

					// storing json data into class member.
					utility::string_t dateRes;
					double varRes[varNum_s];

					dateRes = item[itemKey_s[0]].as_string();

					for (int index = 0; index < varNum_s; ++index)
						varRes[index] = item[itemKey_s[index + 1]].as_double();


					res_s daily;

					daily.time = to_utf8string(dateRes);
					buffer << daily.time;

					getline(buffer, date, '-');		// Parsing year.
					daily.year = stoi(date);

					getline(buffer, date, '-');		// Parsing month.
					daily.month = stoi(date);

					getline(buffer, date);		// Parsing day.
					daily.day = stoi(date);

					buffer.str("");
					buffer.clear();

					daily.doy = calcDoy(daily.year, daily.month, daily.day);


					// Regarding the duplicated data (checked 2022. 6. 29.)
					if ((checkInit == false) && (daily.year == 2021) && (daily.month == 11) && (daily.day == 1))
						{
							checkInit = true;
							continue;
						}
					

					// Qc flags for selab requests are not provided.

					double* varPtr[varNum_s] = {
						&daily.tmax, &daily.tmin, &daily.tavg, &daily.avgRhum,
						&daily.rain, &daily.srad, &daily.avgWspd
					};

					for (int index = 0; index < varNum_s; ++index)
					{
							if (varRes[index] == NAN)
								*varPtr[index] = NODATA;
							else
							{
								*varPtr[index] = varRes[index];

								yrSum[index] += *varPtr[index];
								dayCount[index] += 1;
							}
					}
					
					yearly.sSet.push_back(daily);
				}

				// Saving yearly names for epw outputs.

				auto yiter = yearly.sSet.begin();
				std::string yrName = fileName_s;

				yrName.append("_").append(std::to_string(yiter->year));
				yrName_s.push_back(yrName);


				// Saving alternative data.
				// Default NODATA value will be set if no data is available.

				for (int index = 0; index < varNum_s; ++index)
					if (dayCount[index] != 0)
						yearly.altData_s[index] = (yrSum[index] /= dayCount[index]);

				sList.push_back(yearly);
			});

		// Waiting for the concurrent tasks to finish.
		try {
			requestJson.wait();
		}
		catch (const std::exception& e) {
			std::cout << "[Error] Error exception: " << e.what() << std::endl;
		}
	}

	std::cout << "[KMAapi] Daily data request successful." << std::endl;
}

void KMAapi_s::fillMissing_s()
{
	std::string altName;
	std::stringstream ss;

	std::list<res_sSet>::iterator yiter;
	std::list<res_s>::iterator diter;

	int doyList[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	

	// Filling missing daily data.
	yiter = sList.begin();
	for (; yiter != sList.end(); yiter++)
	{
		diter = yiter->sSet.begin();

		int yrNow = diter->year;
		int monCount;
		int doyCount;


		if (yiter == sList.begin())
		{
			monCount = monStart_s;
			doyCount = doyStart_s;
		}
		else
		{
			monCount = 1;
			doyCount = 1;
		}

		for (; diter != yiter->sSet.end(); ++diter)
		{
			if (doyCount != diter->doy)
			{
				res_s missDay;	// Filling missing daily data.
				std::string time;

				// Calculating day from doy
				int day;
				if ((yrNow % 4 == 0) && (doyCount > 59))		// Regarding leap year.
					day = doyCount - (doyList[monCount - 1] - 1);
				else day = doyCount - doyList[monCount - 1];


				time.append(std::to_string(yrNow)).append("-")
					.append(std::to_string(monCount)).append("-")
					.append(std::to_string(day));

				missDay.time = time;
				missDay.year = diter->year;
				missDay.month = monCount;
				missDay.day = day;
				missDay.doy = doyCount;



				double* varMiss[varNum_s] = {

					&missDay.tmax, &missDay.tmin, &missDay.tavg, &missDay.avgRhum,
					&missDay.rain, &missDay.srad, &missDay.avgWspd
				};

				for (int count = 0; count < varNum_s; ++count) *varMiss[count] = NODATA;


				(yiter->sSet).insert(diter, missDay);
				--diter;	// Set iterator to the inserted day to fill rest of the data.
				std::cout << "[KMAapi] Filled missing daily data " << diter->time << std::endl;
			}

			double* varNow[varNum_s] = {
				&diter->tmax, &diter->tmin, &diter->tavg, &diter->avgRhum,
				&diter->rain, &diter->srad, &diter->avgWspd
			};

			for (int index = 0; index < varNum_s; ++index)
			{
				if (*varNow[index] == NODATA)
				{
					if ((index == 3) || (index == 4))			// Missing srad, rain values will be set to 0.
						*varNow[index] = 0;
					else if (yiter->altData_s[index] != NODATA)
						*varNow[index] = yiter->altData_s[index];
					else
					{
						std::cout << "[KMAapi] Missing " << varName_s[index] << " value for " << diter->time
							<< ".\nIt will be set as default value: " << varDefault_s[index];

						*varNow[index] = varDefault_s[index];
					}
				}
			}



			if ((yrNow % 4 == 0) && (doyCount > 59) && (doyCount - 1 == doyList[monCount]))
				++monCount;
			else if ((yrNow % 4 == 0) && (doyCount == 31))
				++monCount;
			else if ((yrNow % 4 != 0) && (doyCount == doyList[monCount]))
				++monCount;

			++doyCount;

		}
	}

	std::cout << "[KMAapi] Missing daily values have been alternated." << std::endl;
}

void KMAapi_s::calcTemp_s()
{
	std::list<res_s>::iterator diter;
	std::list<res_sSet>::iterator yiter;

	yiter = sList.begin();
	for (; yiter != sList.end(); ++yiter)
	{
		diter = yiter->sSet.begin();
		for (; diter != yiter->sSet.end();)
		{
			// Variables to sum & average annual temperatures.

			double tav = 0;
			double warmMon = -99.0;
			double coldMon = 99.0;

			int monPrev = diter->month;
			int monCount = 0;

			int count = 0;
			int daysPerMon = 0;

			while (count < (yiter->sSet).size())
			{
				double monSum = 0.0;

				// Summing monthly average temperature.
				while (monPrev == diter->month)
				{
					monSum += diter->tavg;
					++daysPerMon;
					++diter;
					++count;

					if (diter == yiter->sSet.end()) break;
				}

				// Getting monthly average temperature.
				monSum /= daysPerMon;
				tav += monSum;

				if (monSum > warmMon) warmMon = monSum;
				if (monSum < coldMon) coldMon = monSum;

				daysPerMon = 0;
				if (diter != yiter->sSet.end()) monPrev = diter->month;
				++monCount;
			}

			tav /= monCount;
			yiter->tav = tav;
			yiter->range = warmMon - coldMon;
		}
	}

	std::cout << "[KMAapi] Calculated annual average temperature & range." << std::endl;
}

void KMAapi_s::wthOut_s()
{
	std::ofstream wth;

	std::list<res_sSet>::iterator yiter;
	std::list<res_s>::iterator diter;
	std::list<std::string>::iterator niter;

	yiter = sList.begin();
	niter = yrName_s.begin();

	const int cellSize = 6;
	const int latLonSize = 9;
	const int doySize = 5;

	bool ckInit = false;

	for (; yiter != sList.end(); ++yiter, ++niter)
	{
		std::string ofName = *niter;
		ofName.append(".wth");

		wth.open(ofName);
		if (wth.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open wth output file. Skipping current data set." << std::endl;
			continue;
		}

		// Printing file header.
		diter = yiter->sSet.begin();

		wth << "*WEATHER DATA : " << fileName_s << "\n\n"
			<< std::right
			<< std::setw(cellSize) << "@ INSI"
			<< std::setw(latLonSize) << "LAT"
			<< std::setw(latLonSize) << "LONG"
			<< std::setw(cellSize) << "ELEV"
			<< std::setw(cellSize) << "TAV"
			<< std::setw(cellSize) << "AMP"
			<< std::setw(cellSize) << "REFHT"
			<< std::setw(cellSize) << "WNDHT"
			<< std::endl;

		wth << std::setw(cellSize) << std::right << fileName_s.substr(0, 4)
			<< std::setw(latLonSize) << lat
			<< std::setw(latLonSize) << lon
			<< std::setw(cellSize) << 0
			<< std::setw(cellSize) << std::fixed << std::setprecision(2) << yiter->tav
			<< std::setw(cellSize) << yiter->range
			<< std::setw(cellSize) << std::fixed << std::setprecision(1) << 1.5
			<< std::setw(cellSize) << 2.0
			<< std::endl;	// default REFHT & WNDHT values are set.

		wth << "@DATE"
			<< std::right
			<< std::setw(cellSize) << "SRAD"
			<< std::setw(cellSize) << "TMAX"
			<< std::setw(cellSize) << "TMIN"
			<< std::setw(cellSize) << "RAIN"
			<< std::setw(cellSize) << "WIND"
			<< std::setw(cellSize) << "RHUM"
			<< std::endl;



		// Formatting data.
		for (; diter != yiter->sSet.end(); diter++)
		{
			// Regarding current api bug (2022. 6. 29)
			if ((ckInit == false) && (diter->year == 2021) && (diter->month == 11) && (diter->day == 1))
			{
				++diter;
				ckInit = true;
			}

			int year2digit;
			if (diter->year >= 2000) year2digit = diter->year - 2000;
			else year2digit = diter->year - 1900;

			wth << std::setw(doySize) << std::right << std::setfill('0') << diter->doy + year2digit * 1000 << std::setfill(' ');

			wth << std::setw(cellSize) << std::fixed << std::setprecision(1) << std::right
				<< diter->srad * 3600.0 * 24* pow(10.0, -6.0)				// Wh/m2 -> MJ/m2
				<< std::setw(cellSize) << diter->tmax
				<< std::setw(cellSize) << diter->tmin
				<< std::setw(cellSize) << diter->rain
				<< std::setw(cellSize) << (diter->avgWspd) * 86.4		// m/s -> km/day
				<< std::setw(cellSize) << diter->avgRhum
				<< std::endl;
		}

		wth.close();
		wth.clear();
	}
	std::cout << "[KMAapi] Daily wth parsing successful." << std::endl;
};

void KMAapi_s::oryOut_s()
{
	std::ofstream ory;

	std::list<res_sSet>::iterator yiter;
	std::list<res_s>::iterator diter;
	std::list<std::string>::iterator niter;

	niter = yrName_s.begin();

	yiter = sList.begin();
	for (; yiter != sList.end(); ++yiter, ++niter)
	{
		diter = (yiter->sSet).begin();

		int year2digit;
		if (diter->year >= 2000) year2digit = diter->year - 2000;
		else year2digit = diter->year - 1900;

		std::string year3digit = "0";
		year3digit.append(std::to_string(year2digit));

		std::string ofName = fileName_s;
		ofName.append(".").append(year3digit);		// e.g. Seou.019

		ory.open(ofName);
		if (ory.fail())
		{
			ofName.clear();
			std::cout << "[Error] Unable to open oryza output file. Skipping current data set." << std::endl;
			continue;
		}


		// Formatting file header.
		ory << lon << ',' << lat << ',' << 0 << ',' << 0 << ',' << 0 << std::endl;


		// Formatting data.
		for (; diter != (yiter->sSet).end(); ++diter)
		{

			double tmaxK = diter->tmax + 273.15;
			double tminK = diter->tmin + 273.15;

			double tmaxSatVpre = 0.06108 * exp((17.27 * tmaxK) / (237.3 + tmaxK));
			double tminSatVpre = 0.06108 * exp((17.27 * tminK) / (237.3 + tminK));

			double meanVpre = (tmaxSatVpre + tminSatVpre) / 2;
			double vpre = diter->avgRhum * 0.01 * meanVpre;

			ory << 1 << ',' << diter->year << ',' << diter->doy << ','			// Station no. is set to 1.
				<< diter->srad * 3600.0 * 24 *(pow(10.0, -6.0)) * 1000 << ',';		// Wh/m2 -> MJ/m2 -> KJ/m2

			ory << std::fixed << std::setprecision(1) << diter->tmin << ','
				<< diter->tmax << ','
				<< std::fixed << std::setprecision(2) << vpre * 0.1 << ','			// hPa -> kPa
				<< std::fixed << std::setprecision(1) << diter->avgWspd << ','					// m/s
				<< diter->rain
				<< std::endl;
		}

		ory.close();
		ory.clear();
	}

	std::cout << "[KMAapi] Oryza file output successful." << std::endl;
}
