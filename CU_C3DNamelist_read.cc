

class CNamelist_for_4D_analysis
	{
	public:
	int dimx;
	int dimy;
	int dimz;
	long number_of_timesteps;
	string input_file_prefix;
	string output_file_prefix;
	long number_of_thresholds;
	double horizontal_distance_limit_for_merging;
	bool domain_periodic_in_x_dimension;

	long input_file_type;
	long output_file_type;
	bool output_3D_objects;
	bool output_4D_objects;
	bool output_png_files;


	CNamelist_for_4D_analysis()
		{
		dimx=BAD_DATA_FLOAT;
		dimy=BAD_DATA_FLOAT;
		dimz=BAD_DATA_FLOAT;
		number_of_timesteps=BAD_DATA_FLOAT;
		input_file_prefix="";
		output_file_prefix="";
		number_of_thresholds=0;
		horizontal_distance_limit_for_merging=BAD_DATA_FLOAT;
		domain_periodic_in_x_dimension=false;
		input_file_type=0;
		output_file_type=0;
		output_3D_objects=false;
		output_4D_objects=false;
		output_png_files=false;
		}

	void check_values()
		{
		if (dimx < 1)
			error(AT,FU,"namelist: dimx < 1!");
		if (dimy < 1)
			error(AT,FU,"namelist: dimy < 1!");
		if (dimz < 1)
			error(AT,FU,"namelist: dimz < 1!");
		if (number_of_timesteps < 1)
			error(AT,FU,"namelist: number_of_timesteps < 1!");
		if (number_of_thresholds < 1)
			error(AT,FU,"namelist: number_of_thresholds < 1!");
		if (input_file_type < 1 || input_file_type > 3)
			error(AT,FU,"input_file_type < 1 || input_file_type > 3");
		if (output_file_type < 1 || output_file_type > 3)
			error(AT,FU,"output_file_type < 1 || output_file_type > 3");
		if (horizontal_distance_limit_for_merging < -1 )
			error(AT,FU,"horizontal_distance_limit_for_merging < -1");
		}

	void read_namelist_from_file(string filename)
		{
		if (!FileExists(filename))
			error(AT,FU, filename + " file does not exist!");

		vector <string> file_line = get_each_line_as_string_from_txt_file(filename);

		for (long ix = 0; ix < (long)file_line.size(); ix++)
		// ignore lines witch start with #
		if (file_line[ix].find("#") != 0 )
			{
			if (file_line[ix].find("dimx") == 0 )
				dimx=atoi(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("dimy") == 0 )
				dimy=atoi(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("dimz") == 0 )
				dimz=atoi(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("number_of_timesteps") == 0 )
				number_of_timesteps=atol(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("input_file_prefix") == 0 )
				input_file_prefix=extract_value_from_a_string(file_line[ix]);

			else if (file_line[ix].find("output_file_prefix") == 0 )
				output_file_prefix=extract_value_from_a_string(file_line[ix]);

			else if (file_line[ix].find("number_of_thresholds") == 0 )
				number_of_thresholds=atol(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("horizontal_distance_limit_for_merging") == 0 )
				horizontal_distance_limit_for_merging=atof(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("domain_periodic_in_x_dimension") == 0 )
				{
				long temp=atol(extract_value_from_a_string(file_line[ix]).c_str());
				if (temp==0) domain_periodic_in_x_dimension=false;
				else if (temp==1) domain_periodic_in_x_dimension=true;
				else error(AT,FU,"domain_periodic_in_x_dimension != 0 || domain_periodic_in_x_dimension != 1");
				}

			else if (file_line[ix].find("input_file_type") == 0 )
				input_file_type=atol(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("output_file_type") == 0 )
				output_file_type=atol(extract_value_from_a_string(file_line[ix]).c_str());

			else if (file_line[ix].find("output_3D_objects") == 0 )
				{
				long temp=atol(extract_value_from_a_string(file_line[ix]).c_str());
				if (temp==0) output_3D_objects=false;
				else output_3D_objects=true;
				}

			else if (file_line[ix].find("output_4D_objects") == 0 )
				{
				long temp=atol(extract_value_from_a_string(file_line[ix]).c_str());
				if (temp==0) output_4D_objects=false;
				else output_4D_objects=true;
				}

			else if (file_line[ix].find("output_png_files") == 0 )
				{
				long temp=atol(extract_value_from_a_string(file_line[ix]).c_str());
				if (temp==0) output_png_files=false;
				else output_png_files=true;
				}

			}
		check_values();
		}

	string echo_namelist()
		{
		ostringstream s;
		s.str("");
		s << "dimx =\t" << dimx << endl;
		s << "dimy =\t" << dimy << endl;
		s << "dimz =\t" << dimz << endl;
		s << "number_of_timesteps =\t" << number_of_timesteps << endl;
		s << "input_file_prefix =\t" << input_file_prefix << endl;
		s << "output_file_prefix =\t" << output_file_prefix << endl;
		s << "number_of_thresholds =\t" << number_of_thresholds << endl;
		s << "horizontal_distance_limit_for_merging =\t" << horizontal_distance_limit_for_merging << endl;
		s << "domain_periodic_in_x_dimension =\t" << output_logic_as_string(domain_periodic_in_x_dimension) << endl;
		s << "input_file_type =\t" << input_file_type << endl;
		s << "output_file_type =\t" << output_file_type << endl;
		s << "output_3D_objects =\t" << output_logic_as_string(output_3D_objects) << endl;
		s << "output_4D_objects =\t" << output_logic_as_string(output_4D_objects) << endl;
		s << "output_png_files =\t" << output_logic_as_string(output_png_files) << endl;

		return (s.str());
		}

	string input_file_extension()
		{
		if (input_file_type == 1) return(".dat");
		else if (input_file_type == 2) return(".nc");
		else if (input_file_type == 3) return(".csv");
		else
			error(AT,FU, "input_file_type problem");
		return("");
		}

	string output_file_extension()
		{
		if (output_file_type == 1) return(".dat");
		else if (output_file_type == 2) return(".nc");
		else if (output_file_type == 3) return(".csv");
		else
			error(AT,FU, "output_file_type problem");
		return("");
		}
	};
