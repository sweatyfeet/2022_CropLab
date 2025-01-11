<COMPILE OPTIONS>
g++ -std=c++11 fileName.cpp -o fileName -lboost_system -lcrypto -lssl -lcpprest

- All four of the complie options are required.
- Additional option of "-lpthread" is needed for different compliers.



<COMMAND LINE OPTIONS>
./fileName (request options) (input file name) (output file name) (output file format)

- request options = h (KMA hourly) || d (KMA daily) || s (SELAB daily)
- The input file name should contain extension (.txt or else)
- The output file name SHOULD NOT contain extension, as it will be automatically appended.
- output file formats = epw (only for hourly requests) || wth || ory
- Different output file formats can be requested simultaneously (examples below)

KMA hourly request for epw & wth & oryza output)
./fileName h input_h.txt output epw wth ory

KMA daily request for wth output)
./fileName d input_d.txt output wth

SELAB daily request for wth & oryza output)
./fileName s input_s.txt output wth ory



<OUTPUT FILE NAMES>
- hourly requests (only for epw formats): Output file contains monthly data,
A name of year and month will be appended between provided output file name and the extension.
ex) EPW file for January of 2021, "output_2021_Jan.epw"

- daily requests: An year will be appended between provided output file name and the extension.
ex) WTH file for 2021, "output_2021.wth"



<INPUT FILE FORMATS>
1. hourly inputs (KMA)

/*
APIKey
parameters to be requested (comma-separated)
stationID,startYear,startMonth,startDay,startHour,endYear,endMonth,endDay,endHour
*/

/*
K6YiG3lc90QzSxbf3QUBLJNwfM8RG56AAx5yzJofFwfeoMVFpQmpW0vzp6hGwf9G1%2FjOivrnlOxFLgWf8Y9Bsg%3D%3D
temp,rain,wspd,rhum,dewp,srad,wdir,vpre,lpre,spre,sshr,sdep,visi,stem
108,2021,1,1,0,2021,12,31,23
*/

- Among the API keys provided from the www.data.go.kr, encoded one should be used.
(The data portal provides two versions of api keys, one encoded and the other decoded)

- Detailed parameter names can be found in DB.h file.
The example above contains every parameter names.

- startHour & endHour should be in range of 0~23, which matches the KMA format.
- The provided range will be automatically cut into monthly and yearly range, 
for the hourly and daily requests, respectively. 
- Therefore hourly and daily requests will be printed in monthly yearly data sets.

ex) hourly request of (2020,3,1,0,2021,5,31,10)
=> { (2020,3,1,0,2020,3,31,23), (2020,4,1,0,2020,4,30,23,) ... (2021,5,1,0,2021,5,31,10) }

ex) daily request of (2020,3,1,0,2021,5,31,10)
=> { (2020,3,1,0,2020,12,31,23), (2021,1,1,0,2021,5,31,10) }



2. daily inputs (KMA)

/*
APIKey
parameters to be requested (comma-separated)
stationID,startYear,startMonth,startDay,endYear,endMonth,endDay
AP coefficient a, AP coefficient b 
*/

/*
K6YiG3lc90QzSxbf3QUBLJNwfM8RG56AAx5yzJofFwfeoMVFpQmpW0vzp6hGwf9G1%2FjOivrnlOxFLgWf8Y9Bsg%3D%3D
tavg,tmin,tmax,rain,avgWspd,avgDewp,avgRhum,avgVpre,avgLpre,avgSpre,sdur,sshr,srad,sdep,stem,leva,seva
108,2021,1,1,2021,12,31
0,0
*/

AP coefficients should be (a, b) = (0, 0) in case of no estimation.



3. daily inputs (SELAB)

/*
latitude,longitude
startYear,startMonth,startDay,endYear,endMonth,endDay
*/

Data before 2021-11-01 is unavailable.
Data from 2021-11-01 is duplicated inside of the API (bug checked 2022.06.30.)
The code will automatically remove the duplicate, but further checking is required.



<KMAapi_DB.h>
- Contains geographical data of KMA ASOS observation points (latitude, longitude, altitude, etc.)
Additional info can be inserted, in case a data from a new observation point is needed.

- Contains observation site names in english, which will be used to translate korean names
provided in the response .json filestream.

- Contains AP coefficients from Choi et al, which is now deprecated.



<KMAapi_struct.h>
- The basic hierarchy of data structures are response_h < response_hSet < response_hList.
- These triple level of structures are mainly accessed via two iterators.

ex) daily data in the yearly data set) diter = yiter->dSet.begin();
ex) hourly data in the monthly data set) hiter = diter->hSet.begin();



<KMAapi_final.cpp>
- Any missing data from the KMA or SELAB will be automatically filled,
with monthly / yearly average data calculated within each of the request set.
- This process is universal regardless of the parameter, 
excluding solar radiation & rain & snow which will be set to 0 in case of missing.

ex) If a temperature data of 2022-06-01 is missing, yearly average temperature of 2022
(which was saved during the initial response saving process) will be used to fill it.

 


****detailed informations (although not organized) can be found in research notes folder.


