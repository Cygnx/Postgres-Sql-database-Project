//Ling Pei Li & Qingwei Zeng

#include <iostream>
#include <fstream> 
#include <pqxx/pqxx>  
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace pqxx;
 
void sql_q(string s)									// Wrapper function for sql querying
{
	
	connection C("dbname=testdb user=qztama password=1"); 

    if (!C.is_open()) 
      cout << "Can't open database" << endl;  
	 
	work W(C);
	result r;
	r = W.exec(s); 
	W.commit();  
	C.disconnect();

		std::cout << r[0][0].as<float>() << std::endl;
}

int main()
{
	string sql = "";

	cout<<endl << "=============ANSWER FOR 3A===============" << endl;
	for (int i = 5 ; i <= 100; i+= 5)
	 {
		cout << i << ",";
	 sql = 
	" SELECT people/1.0/total AS answer			"
	"	FROM									"
	"	(SELECT COUNT(*) AS people FROM 		"
	"		(SELECT SUM(TRPMILES) AS dayMile	"
	"	 	FROM DAY							"
	"		WHERE TRPMILES > 0 					"
	"	 	GROUP BY HOUSEID, PERSONID) AS ssid	"
	"	 WHERE dayMile < " +to_string(i)+") AS a "
	"	,										"
	"	(SELECT COUNT(*) AS total FROM 			"
	"		(SELECT HOUSEID, PERSONID			"
	"	 	FROM DAY							"
	"	 	GROUP BY HOUSEID, PERSONID) AS D	"
	"	) AS b 									";		
		sql_q(sql);
	} 
	
	 
	cout<<endl << "=============ANSWER FOR 3B===============" << endl;
	for(int i = 5; i <= 100; i += 5)
	{
	cout << i << ",";
	sql = 
"	SELECT TOTALMILES/TOTALGALLONS AS ANSWER		"
"	FROM											"
"													"
"		(											"
"		SELECT SUM(GALLONS) as TOTALGALLONS			"
"		FROM  										"
"			(										"
"			SELECT TRPMILES/1.0/EPATMPG AS GALLONS	"
"			FROM									"
"				(									"
"				(SELECT	HOUSEID, VEHID, EPATMPG		"
"				FROM VEH) AS T1						"
"													"
"				JOIN								"
"													"
"				(SELECT	HOUSEID, VEHID, TRPMILES	"
"				FROM DAY							"
"				WHERE TRPMILES > 0 AND TRPMILES < " + to_string(i) + "AND VEHID > 0) AS T2"
"				ON T1.HOUSEID = T2.HOUSEID AND T1.VEHID = T2.VEHID"
"				) AS CombT									"
"			) AS GG 										"
"		) AS TotalFuel,									"
"		(											"
"		SELECT	SUM(TRPMILES) AS TOTALMILES			"
"		FROM DAY									"
"		WHERE TRPMILES < " + to_string(i) + "AND VEHID > 0			"
"		) AS TotalMiles									"
"													";
	sql_q(sql);
	}
	
	cout<<endl << "=============ANSWER FOR 3C===============" << endl;
	 for(int i = 200803; i <= 200904; i++)
	 {
	 
		cout << i << ",";
		sql = 
	"	SELECT TOTAL_CO2HHEm / MotorCO2 AS answer"
	"	FROM"
	"	("
	"		SELECT AvgGPerHH * 117538000 * 0.008887 AS TOTAL_CO2HHEm"
	"		FROM"
	"		("
				/*Finds average gallons per household 	*/
	"			SELECT AVG(GallonsPerHH) AS AvgGPerHH"
	"			FROM"
	"			("
	"				SELECT HOUSEID, SUM(TRPMILES/1.0/EPATMPG) * 30.44 AS GallonsPerHH"
	"				FROM"
	"					("
							/*Natural Joining VEH and DAY, show houseid, vehid, trpmiles, epatmg 
							for a given month*/
	"						SELECT VEH.HOUSEID, TRPMILES, EPATMPG"
	"						FROM VEH JOIN DAY "
	"							  ON VEH.HOUSEID = DAY.HOUSEID AND VEH.VEHID = DAY.VEHID"
	"						WHERE DAY.TRPMILES > 0 AND DAY.TDAYDATE = " + to_string(i)+
	"					) AS GivenMonthTable"
	"				GROUP BY HOUSEID"
	"			) AS GPerHH"
	"		) AS AGPerHH"
	"	) AS T_CO2HHEm,"
	"	("
	"		SELECT TEACEUS * 1000000 AS MotorCO2"
	"		FROM eia_ct"
	"		WHERE YYYYMM = " + to_string(i)+						
	"	) AS M_CO2"	;
		
		sql_q(sql);	
		if(i % 100 == 12)
			i+= 88;
	}

cout<<endl << "=============ANSWER FOR 3D===============" << endl;
for(int j = 20; j <= 60; j+= 20)
{
for(int i = 200803; i <= 200904; i++ )
{
	cout << j <<  "," << i <<",";
	sql =
	"SELECT oldTotalCO2Em - (oldTotalCO2Em + newTotalElectricalEnergyCO2Emissions - TotTransCO2Loss) AS ChangeOfCO2"
" FROM"
//" /*find the old total CO2 Emissions for specific month*/"
" ("
"   SELECT TEACEUS + TXEIEUS AS oldTotalCO2Em"
"   FROM EIA_CE, EIA_CT"
"   WHERE EIA_CE.YYYYMM = "+ to_string(i) +" AND EIA_CT.YYYYMM ="  + to_string(i) +
" ) AS old,"

"("
"     SELECT RateOfMonth * newTotalElec AS newTotalElectricalEnergyCO2Emissions"
"     FROM                                        "
//"     /*Get the rate CO2/Million metric ton*/"
"     ("
"       SELECT TXEIEUS / ELETPUS AS RateOfMonth"
"       FROM                                          "
"       (SELECT TXEIEUS FROM EIA_CE WHERE YYYYMM =  "+ to_string(i) +" ) AS CO2,"
"       (SELECT ELETPUS FROM EIA_E WHERE YYYYMM =  "+ to_string(i) +" ) AS Energy"
"     ) AS rate,                                        "
""
//"     /*Total Electrical Energy MkWh*/"
"     ("
"     SELECT TotalGasPartOfElecMonth AS newTotalElec  "
"	  FROM"
"       ("
//"       /*Gets the gas part of electricity energy in MkWh, table includes multiple vehicles*/ "
"       SELECT (ElecEnergyLE20Month + ElecEnergyGT20Month) / 1000000.0 AS TotalGasPartOfElecMonth"
"       FROM"
//"         /*HOUSEID, VEHID, ElecEnergyLE20Month*/"
"         ("
"         SELECT AVG(kWhPerDay) * 30.44 * 117538000 AS ElecEnergyLE20Month"
"         FROM"
"           ("
"           SELECT HOUSEID, GallonsPerHHPerDay * 33.1 / 3 AS kWhPerDay"
"           FROM"
"             ("
//"             /*Sum up the amount of gas of each household scaled to a month, TotalTripMiles <= 20*/"
"             SELECT HOUSEID, SUM(TotalTripMilesPerVehicle/EPATMPG) AS GallonsPerHHPerDay"
"             FROM"
//"               /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle, EPATMPG where EPATMPG is the fe for the tuple's specific vehicle,"
//"               should still have one tuple for each vehicle*/"
"               ("
"               SELECT DISTINCT VEH.HOUSEID, VEH.VEHID, TotalTripMilesPerVehicle, EPATMPG"
"               FROM"
//"                 /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle that is grouped by houseid and vehid,"
//"                 should only have one tuple of each vehicle*/"
"                 ("
"                 SELECT HOUSEID, VEHID, SUM(TRPMILES) AS TotalTripMilesPerVehicle"
"                 FROM"
"                   ("
//"                     /*Contains HOUSEID, VEHID, TRPMILES > 0 for a given month*/"
"                     SELECT HOUSEID, VEHID, TRPMILES"
"                     FROM DAY"
"                     WHERE DAY.TRPMILES > 0 AND DAY.TDAYDATE = " + to_string(i)+
"                   ) AS GivenMonthTable1"
"                 GROUP BY HOUSEID, VEHID"
"                 ) AS VEH_TRPMILES,"
"                 VEH"
"               WHERE VEH.HOUSEID = VEH_TRPMILES.HOUSEID AND VEH.VEHID = VEH_TRPMILES.VEHID"
"               ) AS HVTotE"
"             WHERE TotalTripMilesPerVehicle <=  "+ to_string(j) +
"             GROUP BY HOUSEID"
"             ) AS GalPerHHPerDay"
"           ) AS kWhPerDay"
"         ) AS ElecEnergyLessEqual20,"
"         "
""
//"         /*HOUSEID, VEHID, ElecEnergyGT20Month*/"
"         ("
"         SELECT AVG(kWhPerDay) * 30.44 * 117538000 AS ElecEnergyGT20Month"
"         FROM"
"           ("
"           SELECT HOUSEID, GallonsPerHHPerDay * 33.1 / 3 AS kWhPerDay"
"           FROM"
"             ("
//"             /*Sum up the amount of gas of each household scaled to a month, TotalTripMiles > 20*/"
"             SELECT HOUSEID, SUM(((TotalTripMilesPerVehicle*0)+20)/EPATMPG) AS GallonsPerHHPerDay"
"             FROM"
//"               /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle, EPATMPG where EPATMPG is the fe for the tuple's specific vehicle,"
//"               should still have one tuple for each vehicle*/"
"               ("
"               SELECT DISTINCT VEH.HOUSEID, VEH.VEHID, TotalTripMilesPerVehicle, EPATMPG"
"               FROM"
//"                 /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle that is grouped by houseid and vehid,"
//"                 should only have one tuple of each vehicle*/"
"                 ("
"                 SELECT HOUSEID, VEHID, SUM(TRPMILES) AS TotalTripMilesPerVehicle"
"                 FROM"
"                   ("
//"                     /*Contains HOUSEID, VEHID, TRPMILES > 0 for a given month*/"
"                     SELECT HOUSEID, VEHID, TRPMILES"
"                     FROM DAY"
"                     WHERE DAY.TRPMILES > 0 AND DAY.TDAYDATE = " + to_string(i)+
"                   ) AS GivenMonthTable1"
"                 GROUP BY HOUSEID, VEHID"
"                 ) AS VEH_TRPMILES,"
"                 VEH"
"               WHERE VEH.HOUSEID = VEH_TRPMILES.HOUSEID AND VEH.VEHID = VEH_TRPMILES.VEHID"
"               ) AS HVTotE"
"             WHERE TotalTripMilesPerVehicle >  "+ to_string(j) +
"             GROUP BY HOUSEID"
"             ) AS GalPerHHPerDay"
"           ) AS kWhPerDay"
"         ) AS ElecEnergyGreat20"
"       ) AS TotalGasPOEM"
"     ) AS electric"
"   ) AS newElecCO2,"

"("
"		SELECT TransCO2LE20Month + TransCO2GT20Month AS TotTransCO2Loss "
"		FROM "
"         ("
"         SELECT AVG(HHGallonsPerMonth) * 1.0 * 117538000 * 0.008887 / 1000000 AS TransCO2LE20Month"
"         FROM"
"           ("
"           SELECT HOUSEID, GallonsPerHHPerDay * 30.44 AS HHGallonsPerMonth"
"           FROM"
"             ("
//"             /*Sum up the amount of gas of each household scaled to a month, TotalTripMiles <= 20*/"
"             SELECT HOUSEID, SUM(TotalTripMilesPerVehicle/EPATMPG) AS GallonsPerHHPerDay"
"             FROM"
//"               /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle, EPATMPG where EPATMPG is the fe for the tuple's specific vehicle,"
//"               should still have one tuple for each vehicle*/"
"               ("
"               SELECT DISTINCT VEH.HOUSEID, VEH.VEHID, TotalTripMilesPerVehicle, EPATMPG"
"               FROM"
//"                 /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle that is grouped by houseid and vehid,"
//"                 should only have one tuple of each vehicle*/"
"                 ("
"                 SELECT HOUSEID, VEHID, SUM(TRPMILES) AS TotalTripMilesPerVehicle"
"                 FROM"
"                   ("
//"                     /*Contains HOUSEID, VEHID, TRPMILES > 0 for a given month*/"
"                     SELECT HOUSEID, VEHID, TRPMILES"
"                     FROM DAY"
"                     WHERE DAY.TRPMILES > 0 AND DAY.TDAYDATE = " + to_string(i)+
"                   ) AS GivenMonthTable1"
"                 GROUP BY HOUSEID, VEHID"
"                 ) AS VEH_TRPMILES,"
"                 VEH"
"               WHERE VEH.HOUSEID = VEH_TRPMILES.HOUSEID AND VEH.VEHID = VEH_TRPMILES.VEHID"
"               ) AS HVTotE"
"             WHERE TotalTripMilesPerVehicle <=  "+ to_string(j) +
"             GROUP BY HOUSEID"
"             ) AS GalPerHHPerDay"
"           ) AS HHGalMonth"
"         ) AS TCO2LessEqual20,"


"         ("
"         SELECT AVG(HHGallonsPerMonth) * 0.008887 / 1000000 * 117538000 AS TransCO2GT20Month"
"         FROM"
"           ("
"           SELECT HOUSEID, GallonsPerHHPerDay * 30.44 AS HHGallonsPerMonth"
"           FROM"
"             ("
//"             /*Sum up the amount of gas of each household scaled to a month, TotalTripMiles > 20*/"
"             SELECT HOUSEID, SUM(((TotalTripMilesPerVehicle*0)+20)/EPATMPG) AS GallonsPerHHPerDay"
"             FROM"
//"               /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle, EPATMPG where EPATMPG is the fe for the tuple's specific vehicle,"
//"               should still have one tuple for each vehicle*/"
"               ("
"               SELECT DISTINCT VEH.HOUSEID, VEH.VEHID, TotalTripMilesPerVehicle, EPATMPG"
"               FROM"
//"                 /*Contains HOUSEID, VEHID, TotalTripMilesPerVehicle that is grouped by houseid and vehid,"
//"                 should only have one tuple of each vehicle*/"
"                 ("
"                 SELECT HOUSEID, VEHID, SUM(TRPMILES) AS TotalTripMilesPerVehicle"
"                 FROM"
"                   ("
//"                     /*Contains HOUSEID, VEHID, TRPMILES > 0 for a given month*/"
"                     SELECT HOUSEID, VEHID, TRPMILES"
"                     FROM DAY"
"                     WHERE DAY.TRPMILES > 0 AND DAY.TDAYDATE = " + to_string(i)+
"                   ) AS GivenMonthTable1"
"                 GROUP BY HOUSEID, VEHID"
"                 ) AS VEH_TRPMILES,"
"                 VEH"
"               WHERE VEH.HOUSEID = VEH_TRPMILES.HOUSEID AND VEH.VEHID = VEH_TRPMILES.VEHID"
"               ) AS HVTotE"
"             WHERE TotalTripMilesPerVehicle >  "+ to_string(j) +
"             GROUP BY HOUSEID"
"             ) AS GalPerHHPerDay"
"           ) AS HHGalMonth"
"         ) AS TCO2GREAT20"
") AS TotTCO2Loss"
	;

sql_q(sql);
if(i%100 == 12)
	i+=88;
}	
}	
	return 0;
}
