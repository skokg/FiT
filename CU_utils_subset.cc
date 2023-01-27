
// error function
void error(const char *location,const char *function, string txt)
	{
	cout << "ERROR: " << location << " " << function << ": " <<  txt << endl;
	exit(1);
	}


// is value inside interval test
bool is_inside_interval(double lower_limit, double upper_limit, double value)
	{
	if (value >= lower_limit && value <= upper_limit) return(true);
	return(false);
	}



// for long number il it adds 0 in front so the number in mest chaacters long
string output_leading_zero_string(long il,int mest)
	{
	int ix;

	ostringstream s1;
	s1.str("");
	int stevke;
	string s;

	if (il==0) stevke=1;
	else stevke=(int)floor(log10((double)il))+1;

	s1 << il;
	s=s1.str();
	s1.str("");
	for (ix=0;ix< mest-stevke;ix++)
		{
		s1 << "0" << s;
		s=s1.str();
		s1.str("");
		}
	return(s);
	}

bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    blnReturn = true;
  } else {
    blnReturn = false;
  }

  return(blnReturn);
}

// creates directory (including all needed parent directories)
// checks first if directory allready exists
void mkpath(string path)
{
string command;
if (!FileExists(path))
	{
	command = "mkdir -p " + path;
	//cout << path << endl;
	system(command.c_str());
	}
}

// similar to mkpath except the "path" mush inlude the filename
void mkpath_from_path_with_filenane(string path)
{
	// find the last occurence of slash
	size_t found;
	found=path.find_last_of("/\\");

	if (found==string::npos)
		error(AT,FU, path + " does not include / !");

	mkpath(path.substr(0,found));
}



string output_logic_as_string(bool a)
	{
	if (a==true) return("true");
	else return("false");
	}


void write_string_to_new_file(string filename, string s)
	{
	ofstream myfile;
	myfile.open (filename.c_str());
	myfile << s;
	myfile.close();
	}

void write_string_to_new_file_and_create_dir_if_necessary(string filename, string s)
	{
	// find the last occurence of slash
	size_t found;
	found=filename.find_last_of("/\\");
	if (found!=string::npos)
		mkpath_from_path_with_filenane(filename);

	write_string_to_new_file(filename, s);
	}


string extract_value_from_a_string(string s)
	{
	// remove what is in front of = sign
	s=s.substr(s.find("=")+1,s.size());
	// remove all spaces
	while (s.find(" ") != s.npos)
		s=s.erase( s.find(" "), 1 );
	//cout << "'" << s << "'" << endl;
	return(s);
	}


// number to string conversion
string n2s(double x)
	{
	ostringstream s1;
	s1.str("");
	s1 << x;
	return(s1.str());
	}


// read lines of a txt file and make them string vector
vector <string> get_each_line_as_string_from_txt_file(string filename)
	{
	vector <string> out;
	string line;

	if (!FileExists(filename))
		error(AT,FU, filename + " does not exist" );

	ifstream myfile(filename.c_str());
	if (!myfile.is_open())
		error(AT,FU, "could not open file: " + filename);

	while (! myfile.eof() )
		{
		getline (myfile,line);

		// remove carrigae return if present - usualy if file is edited with windows
		line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
		//if (line.find_first_of("\r"))
		//	line=line.substr(0,line.find_first_of("\r"));

		out.push_back(line);
		}

	myfile.close();

	return(out);
	}


template<typename T>
    void removeDuplicates(std::vector<T>& vec)
    {
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }


template <class ForwardIterator, class T>
  ForwardIterator lower_bound_which_never_returns_an_out_of_bounds_result (ForwardIterator first, ForwardIterator last, const T& val)
{
  ForwardIterator it;
  typename iterator_traits<ForwardIterator>::difference_type count, step;
  count = distance(first,last);
  while (count>0)
  {
    it = first; step=count/2; advance (it,step);
    if (*it<val) {
      first=++it;
      count-=step+1;
    }
    else count=step;
  }

  if (first == last)
  	return last-1;
  else
	return first;
}

// rounds a double to digits
double round_to_digits(double x, int digits)
	{
	return(round(pow(10,(double)digits)*x)/pow(10,(double)digits));
	}


double squared_euclidian_distance(double x1, double y1, double x2, double y2)
	{
	return((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	}


double euclidian_distance(double x1, double y1, double x2, double y2)
	{
	return(sqrt(squared_euclidian_distance(x1,y1,x2,y2)));
	}


double euclidian_distance_domain_periodic_in_x_dimension(double x1, double y1, double x2, double y2, int dimx)
	{
	double dx=fabs(x1-x2);
	if (dx > (double)dimx/2)
		dx=(double)dimx - dx;
	return(sqrt(dx*dx + (y2-y1)*(y2-y1)));
	}



class CMy_time_format
	{
	public:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	time_t timestamp;


	int season; // DJF=1, MAM=2, JJA=3, SON=4
	string season_string;

	string month_leading_zero;
	string day_leading_zero;
	string hour_leading_zero;

	// constructor timestamp
	CMy_time_format(time_t Ttimestamp)
		{
		timestamp=Ttimestamp;
		timestamp_to_date();
		};

	// constructor date
	CMy_time_format(int Tyear,int Tmonth,int Tday,int Thour,int Tminute,int Tsecond)
		{
		year=Tyear;
		month=Tmonth;
		day=Tday;
		hour=Thour;
		minute=Tminute;
		second=Tsecond;
		if (month < 10) month_leading_zero="0"; else  month_leading_zero="";
		if (day < 10) day_leading_zero="0"; else  day_leading_zero="";
		if (hour < 10) hour_leading_zero="0"; else  hour_leading_zero="";
		date_to_timestamp();
		set_season();
		};

	// constructor empty
	CMy_time_format()
		{
		timestamp=0;
		}

	bool compare_are_they_the_same_by_comparing_timestamp(const CMy_time_format &tt) const
		{
		if (timestamp != tt.timestamp) return(false);
		return(true);
		}

	// overload operators == nad !=
	bool operator== (const CMy_time_format& tt) const
		{return(compare_are_they_the_same_by_comparing_timestamp(tt));}
	bool operator!= (const CMy_time_format& tt) const
		{return(!compare_are_they_the_same_by_comparing_timestamp(tt));}
	bool operator< (const CMy_time_format& tt) const
		{
		if (timestamp < tt.timestamp) return(true);
		return(false);
		}
	bool operator> (const CMy_time_format& tt) const
		{
		if (*this < tt || *this==tt) return(false);
		return(true);
		}
	bool operator>= (const CMy_time_format& tt) const
		{
		if (*this < tt) return(false);
		return(true);
		}
	bool operator<= (const CMy_time_format& tt) const
		{
		if (*this > tt) return(false);
		return(true);
		}

	CMy_time_format& operator= (const CMy_time_format &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			timestamp=rhs.timestamp;
			timestamp_to_date();
			}
		return *this;    // Return ref for multiple assignment
		}

	CMy_time_format& operator+= (const long &rhs)
		{
 		timestamp+=rhs;
		timestamp_to_date();
		return *this;    // Return ref for multiple assignment
		}

	CMy_time_format& operator-= (const long &rhs)
		{
 		timestamp-=rhs;
		timestamp_to_date();
		return *this;    // Return ref for multiple assignment
		}

	const CMy_time_format operator+ (const long &rhs) const
		{
	    CMy_time_format result = *this;     // Make a copy of myself.  Same as MyClass result(*this);
	    result += rhs;            // Use += to add other to the copy.
    	return result;              // All done!
		}


	// set date
	void set_by_date(int Tyear,int Tmonth,int Tday,int Thour,int Tminute,int Tsecond)
		{
		year=Tyear;
		month=Tmonth;
		day=Tday;
		hour=Thour;
		minute=Tminute;
		second=Tsecond;
		if (month < 10) month_leading_zero="0"; else  month_leading_zero="";
		if (day < 10) day_leading_zero="0"; else  day_leading_zero="";
		if (hour < 10) hour_leading_zero="0"; else  hour_leading_zero="";
		date_to_timestamp();
		set_season();
		}


	// set date
	void set_by_timestamp(long temp_timestamp)
		{
		timestamp=temp_timestamp;
		timestamp_to_date();
		}

	// set by_hours_since_1900_01_01 - this is the ECMWF format for data
	void set_by_hours_since_1900_01_01(double hours_since_1900_01_01)
		{
		// time_in_data is in hours since 1900-01-01 00:00:0.0
		CMy_time_format ttemp(1970,1,1,0,0,0);
		// substract 613608 hours - hours betwwen 1900 and 1970
		double time_in_data=hours_since_1900_01_01-613608;
		set_by_timestamp(ttemp.timestamp +  time_in_data*3600);
		}


	// converts the intigers representing the date to timestamp
	void date_to_timestamp()
		{
		tm now;
		now.tm_year=year-1900;
		now.tm_mon=month-1;
		now.tm_mday=day;
		now.tm_hour=hour;
		now.tm_min=minute;
    	now.tm_sec=second;
	    timestamp=timegm(&now);
		}

	// converts timestamp to date
	void timestamp_to_date()
		{
		tm *now = gmtime(&timestamp);
		year=now->tm_year+1900;
		month=now->tm_mon+1;
		day=now->tm_mday;
		hour=now->tm_hour;
		minute=now->tm_min;
		second=now->tm_sec;
		if (month < 10) month_leading_zero="0"; else  month_leading_zero="";
		if (day < 10) day_leading_zero="0"; else  day_leading_zero="";
		if (hour < 10) hour_leading_zero="0"; else  hour_leading_zero="";
		set_season();
		}



	string output_date_string(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "_" << output_leading_zero_string(month,2) << "_" << output_leading_zero_string(day,2) << "_" << output_leading_zero_string(hour,2) << ":" << output_leading_zero_string(minute,2) << ":" << output_leading_zero_string(second,2);
		return (s1.str());
		}

	string output_date_string_with_custom_seperator(string sep) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "_" << output_leading_zero_string(month,2) << "_" << output_leading_zero_string(day,2) << "_" << output_leading_zero_string(hour,2) << "_" << output_leading_zero_string(minute,2) << "_" << output_leading_zero_string(second,2);
		return (s1.str());
		}

	string output_date_string_to_minute(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "_" << output_leading_zero_string(month,2) << "_" << output_leading_zero_string(day,2) << "_" << output_leading_zero_string(hour,2) << "_" << output_leading_zero_string(minute,2);
		return (s1.str());
		}

	string output_date_string_to_hour(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "_" << output_leading_zero_string(month,2) << "_" << output_leading_zero_string(day,2) << "_" << output_leading_zero_string(hour,2) ;
		return (s1.str());
		}


	string output_date_string_to_day(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "_" << output_leading_zero_string(month,2) << "_" << output_leading_zero_string(day,2);
		return (s1.str());
		}

	string yyyy()
		{return(output_leading_zero_string(year,4));}
	string mm()
		{return(output_leading_zero_string(month,2));}
	string dd()
		{return(output_leading_zero_string(day,2));}
	string hh()
		{return(output_leading_zero_string(hour,2));}
	string minmin()
		{return(output_leading_zero_string(minute,2));}


	string output_date_string_to_hour_no_leading_zeroes(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << year << "_" << month << "_" << day << "_" <<hour ;
		return (s1.str());
		}


	string output_date_string_to_day_with_minuses(void) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << "-" << output_leading_zero_string(month,2) << "-" << output_leading_zero_string(day,2);
		return (s1.str());
		}

	string output_date_string_to_day_with_custom_separator(string sep) const
		{
		ostringstream s1;
		s1.str("");
		s1 << output_leading_zero_string(year,4) << sep << output_leading_zero_string(month,2) << sep << output_leading_zero_string(day,2);
		return (s1.str());
		}

	void set_by_julian_day(double julianday)
		{
		// substract linux epoch expressed in julianday notantion = 2440587.5;
		timestamp=(long)((julianday - 2440587.5)*3600*24);
		timestamp_to_date();
		}

	void set_by_modified_julian_day(double modified_julianday)
		{
		// http://en.wikipedia.org/wiki/Julian_day
		set_by_julian_day(modified_julianday + 2400000.5);
		}

	void set_season()
		{
		if (month==12 || month == 1 || month == 2) {season=1; season_string="DJF";}
		if (month== 3 || month == 4 || month == 5) {season=2; season_string="MAM";}
		if (month== 6 || month == 7 || month == 8) {season=3; season_string="JJA";}
		if (month== 9 || month == 10 || month == 11) {season=4; season_string="SON";}
		if (month== 0) {season=0; season_string="annual";}
		}

	string get_month_string_from_month_number(int mnumber) const
	    {
		string out;
		     if (mnumber==1) out="JAN";
		else if (mnumber==2) out="FEB";
		else if (mnumber==3) out="MAR";
		else if (mnumber==4) out="APR";
		else if (mnumber==5) out="MAY";
		else if (mnumber==6) out="JUN";
		else if (mnumber==7) out="JUL";
		else if (mnumber==8) out="AUG";
		else if (mnumber==9) out="SEP";
		else if (mnumber==10) out="OCT";
		else if (mnumber==11) out="NOV";
		else if (mnumber==12) out="DEC";
		else
			error(AT,FU, "Wrong mnumber!");

	    return (out);
	    }

	  string get_month_string_from_month_number_include_annual_for_zero(int mnumber) const
	    {
		string out;
	    if (mnumber==0) out="Annual";
		else out=get_month_string_from_month_number(mnumber);
	    return (out);
	    }


	  string get_season_string_from_month_number(int mnumber) const
	    {
	    CMy_time_format begin(2000,mnumber,1,3,0,0);
	    return (begin.season_string);
	    }

	  int get_season_number_from_month_number(int mnumber) const
	    {
	      CMy_time_format begin(2000,mnumber,1,3,0,0);
	      return (begin.season);
	    }

	  string get_season_sting_from_season_number(int snumber) const
	     {
	       CMy_time_format begin(2000,1,1,3,0,0);

	       int ix=1;
	       while (begin.season != snumber && ix <= 12)
		 {
		   begin.set_by_date(2000,ix,1,3,0,0);
		   ix++;
		 }

	       if (snumber==0) begin.season_string="Annual";

	       return (begin.season_string);
	     }



	};


// display start and end times of program run
void timer2(CMy_time_format &timer_start, CMy_time_format &timer_last_checkpoint, string text)
	{
	// start timer if timestamp not set and display time
	if (timer_start.timestamp == 0)
		{
		timer_start.timestamp = time(NULL);
		timer_start.timestamp_to_date();
		timer_last_checkpoint= timer_start;

		cout << "Timer START: ";
		if (timer_start.hour < 10) cout << "0";
		cout << timer_start.hour << ":";
		if (timer_start.minute < 10) cout << "0";
		cout << timer_start.minute << ":";
		if (timer_start.second < 10) cout << "0";
		cout << timer_start.second << endl;
		}

	else
		{
		CMy_time_format tt;
		tt.timestamp = time(NULL);
		tt.timestamp_to_date();

		cout << "Timer TIME:  ";
		if (tt.hour < 10) cout << "0";
		cout << tt.hour << ":";
		if (tt.minute < 10) cout << "0";
		cout << tt.minute << ":";
		if (tt.second < 10) cout << "0";
		cout << tt.second;

		long all_seconds = tt.timestamp - timer_start.timestamp;
		long hours = all_seconds / 3600;
		long minutes = (all_seconds  - hours * 3600)/60;
		long seconds = all_seconds  - hours * 3600 - minutes * 60;

		double all_minutes = (double)all_seconds/60;
		double all_minutes_from_last_checkup = (double)(tt.timestamp - timer_last_checkpoint.timestamp)/60;
		//double all_seconds_from_last_checkup = tt.timestamp - timer_last_checkpoint.timestamp;

		cout << "     " << hours << "h " << minutes << "min " << seconds << "sec                 " << all_minutes << "m    LAST:"<< all_minutes_from_last_checkup <<"m OR " << tt.timestamp - timer_last_checkpoint.timestamp << "s" << endl;

		timer_last_checkpoint=tt;
		}
	}



// ----------------------------------
// random number generator (RNG) based on random.h library (requires C++11).
// --------------------------------------

std::mt19937 rng_state_global;  // global instance of RNG state

// wrapper function for random number generator based on random.h library - it is not threadsafe
double ran2(long *idum)
	{
	if (*idum < 0)
		{
		rng_state_global.seed(-*idum);
		*idum=-*idum;
		}
	return(std::uniform_real_distribution<double>(0.0, 1.0)(rng_state_global));
	}


// wrapper function for random number generator based on random.h library - it is threadsafe as long as each thread uses its own rng_state instance
double ran2_threadsafe(long *idum, std::mt19937 &rng_state)
	{
	if (*idum < 0)
		{
		rng_state.seed(-*idum);
		*idum=-*idum;
		}
	return(std::uniform_real_distribution<double>(0.0, 1.0)(rng_state));
	}
