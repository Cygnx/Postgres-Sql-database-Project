//Ling Pei Li & Qingwei Zeng
#include <iostream>
#include <fstream> 
#include <pqxx/pqxx>  
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#define EIA_E_MIN_DATE 194913
#define EIA_E_MAX_DATE 201407 
#define EIA_CE_MIN_DATE 197301
#define EIA_CE_MAX_DATE 201407 
#define EIA_CT_MIN_DATE 197301
#define EIA_CT_MAX_DATE 201407 

using namespace std;
using namespace pqxx; 
 
bool isNum(string s);
vector<string> parseLine(string s,bool format);
void csv_to_sql(string csv, string tableName);
int determineType(string s);
void sql_q(string s);
bool isNullCell(string s);

int main(int argc, char* argv[]) { 
   string sql; 
    
  try { 


	//create table for transportation
    sql = "CREATE TABLE EIA_CT("  
    "YYYYMM       INT NOT NULL," 
    "CLACEUS      FLOAT," 
    "NGACEUS      FLOAT," 
    "AVACEUS      FLOAT," 
    "DMACEUS      FLOAT,"
    "JFACEUS      FLOAT,"
    "LGACEUS      FLOAT,"
	"LUACEUS      FLOAT,"
	"MMACEUS      FLOAT,"
	"RFACEUS      FLOAT,"
	"PMACEUS      FLOAT,"
	"ESACEUS      FLOAT,"
	"TEACEUS      FLOAT,"
    "PRIMARY KEY(YYYYMM));"; 
 
	sql_q(sql);
    
	//create table for electric 
    sql = "CREATE TABLE EIA_CE("  
    "YYYYMM       INT NOT NULL," 
    "CLEIEUS      FLOAT," 
    "NNEIEUS      FLOAT," 
    "DKEIEUS      FLOAT," 
    "PCEIEUS      FLOAT,"
    "RFEIEUS      FLOAT,"
    "PAEIEUS      FLOAT,"
	"GEEIEUS      FLOAT,"
	"NWEIEUS      FLOAT,"
	"TXEIEUS      FLOAT,"
    "PRIMARY KEY(YYYYMM));"; 
 
	sql_q(sql);

	//create table for electric 
    sql = "CREATE TABLE EIA_E("  
    "YYYYMM       INT NOT NULL," 
	"CLETPUS      FLOAT," 
	"PAETPUS      FLOAT,"
	"NGETPUS      FLOAT,"
	"OJETPUS      FLOAT,"
	"NUETPUS      FLOAT,"
	"HPETPUS      FLOAT,"
	"HVETPUS      FLOAT,"
	"WDETPUS      FLOAT,"
	"WSETPUS      FLOAT,"
	"GEETPUS      FLOAT,"
	"SOETPUS      FLOAT,"
	"WYETPUS      FLOAT,"
	"ELETPUS      FLOAT,"
    "PRIMARY KEY(YYYYMM));"; 
 
	sql_q(sql);

	ifstream fs;
	string myString; 
	vector< vector<string> > cTrans_v;
	vector< vector<string> > Elect_v;
	vector< vector<string> > cElect_v;
	
	// parsing "EIA_CO2_Transportation_2014.csv" 
    fs.open("EIA_CO2_Transportation_2014.csv");
    if (!fs.good()) 
    	return 1; // exit if file not found 
	getline(fs, myString); // gets rid of the first row 
	for(int i = 0;getline(fs, myString); i++)
		cTrans_v.push_back(parseLine(myString,0));
	fs.close();
	
	// parsing "EIA_MkWh_2014.csv" 
    fs.open("EIA_MkWh_2014.csv");
    if (!fs.good()) 
    	return 1; // exit if file not found 
	getline(fs, myString); // gets rid of the first row 
	for(int i = 0;getline(fs, myString); i++)
		Elect_v.push_back(parseLine(myString,0));
		fs.close();	
		
	// parsing "EIA_CO2_Electric_2014.csv" 
	fs.open("EIA_CO2_Electric_2014.csv");
    if (!fs.good()) 
    	return 1; // exit if file not found 
	getline(fs, myString); // gets rid of the first row 
	for(int i = 0;getline(fs, myString); i++)
		cElect_v.push_back(parseLine(myString,0));
	fs.close();
	
	//insert transportation values---------------------------------------------
	for(int i = EIA_CT_MIN_DATE; i < EIA_CT_MAX_DATE;i++)
	{
		string sql;
		string vals  = to_string(i) + ",";
		string col_name = "YYYYMM,";
		
		//loop through and find all matches to specific date i
		//and append MSN and VALUES to col_name and vals respectively
		for(int a = 0; a < cTrans_v.size(); a++)
		{
			if(i ==  atoi(cTrans_v[a][1].c_str()))
			{
				vals = vals + cTrans_v[a][2] + ",";
				col_name = col_name + cTrans_v[a][0] + ",";
			}
		}
		
		//getting rid of last comma
		vals = vals.substr(0,vals.size()-1);
		col_name = col_name.substr(0,col_name.size()-1);

		//build sql insert statement
		sql = "INSERT INTO EIA_CT (" + col_name + ") VALUES("
				+ vals + ");"; 
       
		//executing and committing sql insertion query
	
		sql_q(sql);
		
		//increment the year
		if(i%100 == 13)
			i+=87;
	}
	
	//insert celect values-------------------------------------------------
	for(int i = EIA_CE_MIN_DATE; i < EIA_CE_MAX_DATE;i++)
	{
		string sql;
		string vals  = to_string(i) + ",";
		string col_name = "YYYYMM,";
		
		//loop through and find all matches to specific date i
		//and append MSN and VALUES to col_name and vals respectively
		for(int a = 0; a < cElect_v.size(); a++)
		{
			if(i ==  atoi(cElect_v[a][1].c_str()))
			{
				if(!strcmp(cElect_v[a][2].c_str(),"Not Available")) // value is not available
					vals = vals + "NULL,";
				else
					vals = vals + cElect_v[a][2] + ",";
				
				col_name = col_name + cElect_v[a][0] + ",";
			}
		}
		
		//getting rid of last comma
		vals = vals.substr(0,vals.size()-1);
		col_name = col_name.substr(0,col_name.size()-1);

		//build sql insert statement
		sql = "INSERT INTO EIA_CE (" + col_name + ") VALUES("
				+ vals + ");"; 
       
		//executing and committing sql insertion query
		
		sql_q(sql);
		
		//increment the year
		if(i%100 == 13)
			i+=87;
	}
	
	//insert electric values-------------------------------------------------
	for(int i = EIA_E_MIN_DATE; i < EIA_E_MAX_DATE;i++)
	{
		string sql;
		string vals  = to_string(i) + ",";
		string col_name = "YYYYMM,";
		
		//loop through and find all matches to specific date i
		//and append MSN and VALUES to col_name and vals respectively
		for(int a = 0; a < Elect_v.size(); a++)
		{
			if(i ==  atoi(Elect_v[a][1].c_str()))
			{
				if(!strcmp(Elect_v[a][2].c_str(),"Not Available")) // value is not available
					vals = vals + "NULL,";
				else
					vals = vals + Elect_v[a][2] + ",";
				
				col_name = col_name + Elect_v[a][0] + ",";
			}
		}
		
		//getting rid of last comma
		vals = vals.substr(0,vals.size()-1);
		col_name = col_name.substr(0,col_name.size()-1);

		//build sql insert statement
		sql = "INSERT INTO EIA_E (" + col_name + ") VALUES("
				+ vals + ");"; 
       
		//executing and committing sql insertion query

		sql_q(sql);
		
		//increment the year
		if(i%100 == 13)
			i+=87;
	}
	
	csv_to_sql("HHV2PUB.CSV", "HH");
	csv_to_sql("PERV2PUB.CSV", "PER");
	csv_to_sql("VEHV2PUB.CSV", "VEH");
	csv_to_sql("DAYV2PUB.CSV", "DAY"); 
	

  } catch (const std::exception &e){ 
    cerr << e.what() << std::endl; 
    return 1; 
  } 
  return 0;
}
void csv_to_sql(string csv, string tableName)
{
	/**************
	***0 = int ****
	***1 = float***
	***2 = text ***
	**************/
	
	int rows;
	string sql; 
	ifstream fs;
	string myString = "";
	vector<int> rTypes; 
	vector<string> rTemp;
	vector<string> colTemp;
	
	//-------------------------------------------------------Determines The Type Of each column-----------------------------
	
	fs.open(csv.c_str());
    if (!fs.good()) 
		cout << "failed to open:" + csv << endl;
		
	for(int i = 0; getline(fs, myString); i++)		// i = row #
	{
		if(i % 20000 == 0)
		cout << "Reading " << i << endl;
		
		rTemp = parseLine(myString, false);			// Reads in the current line and saves it in a token'd vector
		
		if(i == 0)									// First row of the file
		{			
			colTemp = parseLine(myString, false);	// Save the names of the first row into colTemp Vector	
			for(int a = 0; a < colTemp.size(); a++)	//	Iterates through the colTemp Vector
				rTypes.push_back(0);				// Populate the Types Vector with the appropriate number of 0's
		}
		
		if(i != 0)									// All rows excluding the first
		{			
			for(int a = 0 ; a < rTemp.size(); a++)	// a = column #
			{
				int type = determineType(rTemp[a]);	// Determine the data type cell(i,a)
				if(type > rTypes[a])				// If not the tightest bound
					rTypes[a] = type;				// Update and make it the tightest bound for this column.
			}
		}	
		rows = i + 1;								// Saves # of rows total in this file into rows
	}
	
	fs.close();
	
	//------------------------------------------------------Creates the Table w/ the appropriate Data types------------------
	
	sql = "CREATE TABLE "+ tableName +"\n"			// Prepares the sql string for a create table query
		  "(\n";
		
	for(int a = 0; a <  rTypes.size(); a++)			// Iterates through the columns
	{
		string temp = "";							// Determines the type of data the current cell is
		if(rTypes[a] == 2)
			temp = "TEXT";
		else if(rTypes[a] == 0)
			temp = "INT";
		else 
			temp = "FLOAT";
		sql += colTemp[a] + "  " + temp + ",\n";	// Adds the NAME  and the DATATYPE of the current cell into the sql CREATE TABLE query
	}
	
	sql = sql.substr(0, sql.size() - 2); 			// removes the last ,\n 
	sql += "\n);";									// Needed for the query format
	cout << sql;
	sql_q(sql);
	
	//------------------------------------------------INSERT-----------------------------------------------------------------
	
	fs.open(csv.c_str());							// Opens the file
    if (!fs.good()) 
		cout << "failed to open:" + csv << endl;
		
	sql = "INSERT INTO " + tableName + " VALUES ";	// Prepares the sql string for a INSERT INTO query
	
	for(int i = 0; getline(fs, myString); i++)		// For each row
	{
		if(i % 20000 == 0)							// Displays progress every 20k rows
		cout << "Pass 2 : Reading " << i << " / " << rows<< endl;
		
		rTemp = parseLine(myString, true);			// Tokenize the current row into a vector
		
		if(i == 0)							
			continue;								// Skips to the next iteration
			
		sql += "(";									// Sql formatting
		
		for(int a = 0 ; a < rTemp.size();a++)		// For each column in this row
		{
			if(isNullCell(rTemp[a])) 				// Check for NULLS
				sql += "NULL,";
			else									// Add value to the SQL query
				sql += rTemp[a];						
		}
		
		sql = sql.substr(0, sql.size() - 1); 		// Removes the last , 
		sql += "),\n";								// Sql formating
		
		if( i % 1000 == 0 || i == rows - 1)			// Do an insertion query every 1k rows
		{
			sql = sql.substr(0, sql.size() - 3); 	// Removes the last ,\n of the last entry
			sql += "\n);";
			sql_q(sql);								// Sends Query
													//Formats the sql string for the next insertion
			sql = "INSERT INTO " + tableName + " VALUES ";	
		}
	}
	//cout << sql;
}
bool isNullCell(string s)							// Determines if a string is NULL
{
	bool isNull = true;
	for(int i = 0; i < s.size(); i++)
	{
		switch(s[i])
		{
			case '.': 
			case 'X':		
			case '-':
		 	case ',':
			case '\'': break;
			default: 
				isNull = false;
		};
	}
	return isNull;
}
int determineType(string s)							// Determines the data type of a string
{
	/**************
	***0 = int ****
	***1 = float***
	***2 = text ***
	**************/

	int iType = 0;
	string ss = "";
	bool isFloat = false, isText = false;
	for(int i = 0; i < s.size(); i++)
	{
		if(s[i] < '0' || s[i] > '9')
			ss+= s[i];
	}
	for(int i = 0; i < ss.size(); i++)
	{
		switch(ss[i])
		{
			case '.': 
				isFloat = true;
					break;
			case 'X':		
			case '-':
		 	case ',':
			case '\'': break;
			default: 
				isText = true;
				iType = 2;
		};
	}
	
	if(isFloat && !isText)
		iType = 1;
		
	 if(s.size() > 10 && !isFloat) 								// Anything above 10 digits is considered a text
		return 2;
		
	return iType;
}
vector<string> parseLine(string s,bool format)		// Parses a row into a vector, optional formatting
{
	vector<string> tempS;
	string buffer;
	istringstream iss(s);
	
	while (getline(iss, buffer, ','))
	{
		if(buffer.find('\r') != string::npos)		//remove \r if there is any
			buffer.erase(buffer.find('\r'));
		if(format)
			buffer = "'"+buffer+"',";
		
		tempS.push_back(buffer);
	}
	return tempS;
}
void sql_q(string s)									// Wrapper function for sql querying
{
	connection C("dbname=testdb user=qztama password=1"); 

    if (!C.is_open()) 
      cout << "Can't open database" << endl;  
	 
	work W(C); 
	W.exec(s); 
	W.commit();  
	C.disconnect(); 
}
