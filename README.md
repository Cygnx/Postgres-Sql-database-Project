# Postgres-Sql-database-Project
Files Included:
Parse.cpp

	Requirements:
	Needs to be compiled with the -O3 and -lpxx parameters
      **Assumes that a database has been created under the name "testdb" prior to running this program
	Remember to change the username accordingly 
	The program assumes that the needed csv files are in the same working directory
	
	Instructions:
	Simply compile the file with make file, and run it. 
	The program will automate everything.
	When it finishes running, the "testdb" database should be populated with 7 tables including data from the 7 csv files
	
	What it does:
	This program will read in the 7 csv files one at a time, creates a database based on the schema described in 
	answer 1, and inserts all the appropriate data into the tables.
	The program will periodically display its read-in progress for NHTS csv files, but not for the EIA files


	Technical Details for testdb:
	FILES --------------------------------> Created table names
	"EIA_CO2_Transportation_2014.csv" ----> EIA_CT
	"EIA_MkWh_2014.csv" ------------------> EIA_E
	"EIA_CO2_Electric_2014.csv" ----------> EIA_CE
	
	"HHV2PUB.CSV" ------------------------> HH
	"PERV2PUB.CSV" ----------------------->	PER
	"VEHV2PUB.CSV" ----------------------->	VEH
	"DAYV2PUB.CSV" -----------------------> DAY
	
Answers.cpp	

	Requirements:
	Needs to be compiled with the -O3 and -lpxx parameters
      **Assumes that a database has been created under the name "testdb" prior to running this program
	It also assumes that this "testdb" database has been populated with the appropriate data tables from running the output file of "parse.cpp"
	Remember to change the username accordingly 
	
	Instructions:
	Simply compile the file with the above parameters, and run it. 
	The program will automate everything.
	
	What it does:
	The program will announce which question the current query is going to answer,
	execute the query, and display the answer. 
	

Instructions
	0) Prepare an empty database called "testdb", using the createdb command
	1) Change the "username = " part of the parse.cpp file (near the bottom of the file)
	   to the appropriate username of the user running the file
	2) Run the make file, which will compile both programs
	3) Run parse.out to setup the database	
	4) Run answer.out to parse the database for the answers 
