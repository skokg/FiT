
#define DISABLE_BOOST_SERIALIZATION
#define DISABLE_PNGWRITER
#define DISABLE_THINGS_NOT_NECESSARY_FOR_FIT

#include "CU_Header_Include_FiT.cc"


C3DField ReadC3DField_in_this_version_of_software(const CNamelist_for_4D_analysis &namelist, const string fname)
	{
	C3DField f;

	if (namelist.input_file_type == 1)
		f.Read_From_Binary_Fortran_File(fname, fortran_binary_header, namelist.dimx ,namelist.dimy ,namelist.dimz);
	else if (namelist.input_file_type == 2)
		f.ReadField(fname);
	else if (namelist.input_file_type == 3)
		f.ReadField_from_CSV_file(fname, namelist.dimx ,namelist.dimy ,namelist.dimz);
	else
		error(AT,FU, "Something is wrong withe the input_file_type");
	// Allowed are only options 1,2 and 3

	return(f);
	}

void WriteC3DField_in_this_version_of_software(const CNamelist_for_4D_analysis &namelist, const string fname, const C3DField &f)
	{
	if (namelist.output_file_type == 1)
		f.Write_To_Binary_Fortran_File(fname, fortran_binary_header);
	else if (namelist.output_file_type == 2)
		f.WriteField_compressed_and_create_path(fname,2);
	else if (namelist.output_file_type == 3)
		f.WriteField_to_CSV_file(fname);
	else
		error(AT,FU, "Something is wrong withe the ouput_file_type");
	// Allowed are only options 1,2 and 3
	}


int main(int argc, char *argv[])
{
long it;
ostringstream s1,s2,s3,s4,s5,s6,s7,dataset, subdir1, subdir2;
CMy_time_format tt,tt2,tt1;

CMy_time_format timer_start, timer_last_checkpoint;
timer2(timer_start, timer_last_checkpoint, "");

C3DField f,f2;

double new_id_counter=new_id_counter_start;


// -----------------------------------
// Reading namelist
// -----------------------------------
CNamelist_for_4D_analysis namelist;
if (argc != 2)
	error(AT,FU, "namelist file not specified. Usage '" + (string)argv[0] + " namelist_file'");
namelist.read_namelist_from_file((string)argv[1]);

cout << namelist.echo_namelist() << endl;

double number_of_found_3Dobjects;

// ----------------------------------------------------
// Some initialization
// ----------------------------------------------------
f.allocatememory(namelist.dimx,namelist.dimy,namelist.dimz);

C3DFrameObjects_all objects3D (namelist.number_of_timesteps);

// ----------------------------------------------------
// First time loop - to get 3D objects and overlaps
///----------------------------------------------------
for (it=0; it < namelist.number_of_timesteps; it++)
	{
	cout << "-----  Timestep " << output_leading_zero_string(it,number_of_digits_in_input_files) << endl;

	// -----------------------------------
	// Read input file
	// -----------------------------------
	s1.str("");
	s1 << namelist.input_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) << namelist.input_file_extension();
	cout << "Reading file: " << s1.str() << endl;
	f=ReadC3DField_in_this_version_of_software(namelist,s1.str());

	// check dimenions of read file
	if (namelist.dimx != f.dimx || namelist.dimy != f.dimy || namelist.dimz != f.dimz)
		error(AT,FU, "namelist.dimx != f.dimx || namelist.dimy != f.dimy || namelist.dimz != f.dimz");

	// -----------------------------------
	// Identify 3D objects
	// -----------------------------------
	cout << "Identifying 3D objects" << endl;
	f.Identify_3D_objects_using_floodfill_and_cascading_threshold(namelist.number_of_thresholds,lower_obj_index_limit,number_of_found_3Dobjects,namelist.domain_periodic_in_x_dimension);
	cout << "Found " << number_of_found_3Dobjects << " 3D objects" << endl;

	// -----------------------------------
	// Write 3D object files
	// -----------------------------------
	// write 3D object netcdf file
	if (namelist.output_3D_objects)
		{
		s1.str("");
		s1 << namelist.output_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) +"_3Dobjects" << namelist.output_file_extension();
		cout << "Writing file: " << s1.str() << endl;

		WriteC3DField_in_this_version_of_software(namelist, s1.str(), f);
		//f.WriteField_compressed_and_create_path(s1.str(),2);
		}

	// write 3D object PNG file
	if (namelist.output_3D_objects && namelist.output_png_files)
		{
		s1.str("");
		s1 << namelist.output_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) +"_3Dobjects_flattened.png";
		cout << "Writing file: " << s1.str() << endl;
		f.WriteField_PNG_projected_to_2d_with_objects_on_white_background(s1.str());
		}

	// -----------------------------------
	// Calculate object properties
	// -----------------------------------
	cout << "Calculating 3D object attributes" << endl;
	f.Calculate_object_attributes(objects3D.FrameObjects_in_timestep[it].FrameObject, lower_obj_index_limit, upper_obj_index_limit, namelist.domain_periodic_in_x_dimension);

	// -----------------------------------
	// Calculate overlaps - overlap with objects in PREVIOUS timestep
	// -----------------------------------
	cout << "Calculating overlaps with objects in PREVIOUS timestep" << endl;
	if (it != 0)
		{
		f.Calculate_object_overlaps(f2, objects3D.FrameObjects_in_timestep[it].FrameObject, lower_obj_index_limit, upper_obj_index_limit);
		}

	// -----------------------------------
	// Make a copy of this field to be used at the next time step
	// -----------------------------------
	f2=f;

	// ----------------------------------------------------
	// Calclate 4D objects according to FiT - the 4D index of object is denoted by new_ids
	///----------------------------------------------------
	if (namelist.output_4D_objects)
		{
		cout << "Defining 4D objects from overlaping information according to FiT  " << endl;
		objects3D.set_new_ids_from_overlaps_for_one_timestep_using_FIT_logic(it,new_id_counter, new_id_counter_start, namelist.horizontal_distance_limit_for_merging, namelist.domain_periodic_in_x_dimension, namelist.dimx );


		// reindex objects in field - values set by new_ids
		//cout << "reindex objects in field - values set by new_ids" << endl;
		f.Reindex_objects_in_field_so_they_corespond_to_new_ids(objects3D.FrameObjects_in_timestep[it].FrameObject);

		// create a new set of objects for 4D objects
		//f.create

		}

	// -----------------------------------
	// write 4D object files
	// -----------------------------------

	// write 4D object netcdf file
	if (namelist.output_4D_objects)
		{
		s1.str("");
		s1 << namelist.output_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) +"_4Dobjects" << namelist.output_file_extension();
		cout << "Writing file: " << s1.str() << endl;
		WriteC3DField_in_this_version_of_software(namelist, s1.str(), f);
		//f.WriteField_compressed(s1.str(),2);
		}

	// write 4D object PNG file
	if (namelist.output_4D_objects && namelist.output_png_files)
		{
		// write png
		s1.str("");
		s1 << namelist.output_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) +"_4Dobjects_flattened.png";
		cout << "Writing file: " << s1.str() << endl;
		f.WriteField_PNG_projected_to_2d_with_objects_on_white_background(s1.str());
		}
	//cout << objects.FrameObjects_in_timestep[it].output() << endl;

	}

// -----------------------------------
// Write 3D object tree in txt file
// -----------------------------------
if (namelist.output_3D_objects)
	{
	s1.str("");
	s1 << namelist.output_file_prefix << +"3Dobject_tree.txt";
	cout << "Writing file: " << s1.str() << endl;
	write_string_to_new_file_and_create_dir_if_necessary(s1.str(),objects3D.output_the_3D_objects_tree());
	}


// -----------------------------------
// Write 4D object tree in txt file
// -----------------------------------
if (namelist.output_4D_objects)
	{
	s1.str("");
	s1 << namelist.output_file_prefix << +"4Dobject_tree.txt";
	cout << "Writing file: " << s1.str() << endl;
	write_string_to_new_file_and_create_dir_if_necessary(s1.str(),objects3D.output_the_4D_objects_tree(namelist.domain_periodic_in_x_dimension, namelist.dimx));
	}

timer2(timer_start, timer_last_checkpoint, "");
}




