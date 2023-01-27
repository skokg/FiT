
#define DISABLE_PNGWRITER
#define DISABLE_THINGS_NOT_NECESSARY_FOR_FIT

#include "mpi.h"

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
int id;
int p;
long is;

ostringstream s1,s2,s3,s4,s5,s6,s7,dataset, subdir1, subdir2;
CMy_time_format tt,tt2,tt1;

std::stringstream oss;
std::stringstream iss;
std::string s;
long length;
int idx;
long itx;


C3DField f,f2;

//  Initialize MPI.
MPI::Status status;
MPI::Init ( argc, argv );
//  Get the number of processes.
p = MPI::COMM_WORLD.Get_size (  );
//  Determine this processes's rank.
id = MPI::COMM_WORLD.Get_rank ( );

CMy_time_format timer_start, timer_last_checkpoint;
if (id==0)
	timer2(timer_start, timer_last_checkpoint, "");


double new_id_counter=new_id_counter_start;

// -----------------------------------
// Reading namelist
// -----------------------------------
CNamelist_for_4D_analysis namelist;
if (argc != 2)
	error(AT,FU, "namelist file not specified. Usage '" + (string)argv[0] + " namelist_file'");
namelist.read_namelist_from_file((string)argv[1]);
//namelist.read_namelist_from_file("namelist_sample.dat");

if (id==0)
	{
	cout << namelist.echo_namelist() << endl;
	cout << "  The number of processes available is " << p << "\n";
	}
double number_of_found_3Dobjects;

// ----------------------------------------------------
// Some initialization
// ----------------------------------------------------
f.allocatememory(namelist.dimx,namelist.dimy,namelist.dimz);
f2=f;

C3DFrameObjects_all objects (namelist.number_of_timesteps);

// this is the number of steps needed by each process
int number_of_steps;
number_of_steps=ceil((double)namelist.number_of_timesteps/(double)p);

if (p < 2)
	error(AT,FU, "The parallel version of the software should be run with at least two processes !!! ");



// ----------------------------------------------------
// First time loop - to get 3D objects and overlaps
///----------------------------------------------------
for (is=0; is < number_of_steps; is++)
	{
	// this is the timestep analyzed by this process
	it=p*is + id;


	if (id==0)
		{
		cout << "---- Now analyzing the following timesteps on " << p << " processors: ";
		for (idx=0; idx < p; idx++)
			{
			itx=p*is + idx;
			if ( itx < namelist.number_of_timesteps)
				cout << output_leading_zero_string(itx,number_of_digits_in_input_files) << " ";
			}
		cout << endl;
		}
  	MPI::COMM_WORLD.Barrier ( );

	// only analyze as long as we dont pass the last timestep
	if (it < namelist.number_of_timesteps)
		{

		//cout << "-----  Process " << id << " reading timestep " << output_leading_zero_string(it,number_of_digits_in_input_files) << endl;

		//usleep(100);

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
		//cout << "Identifying 3D objects" << endl;
		f.Identify_3D_objects_using_floodfill_and_cascading_threshold(namelist.number_of_thresholds,lower_obj_index_limit,number_of_found_3Dobjects,namelist.domain_periodic_in_x_dimension);
		cout << "Found " << number_of_found_3Dobjects << " 3D objects in timestep " << output_leading_zero_string(it,number_of_digits_in_input_files) << endl;

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
		f.Calculate_object_attributes(objects.FrameObjects_in_timestep[it].FrameObject, lower_obj_index_limit, upper_obj_index_limit, namelist.domain_periodic_in_x_dimension);

		//cout << objects.FrameObjects_in_timestep[it].output() << endl;

		}

  	// wait till all processes prepare these fields before copying
  	MPI::COMM_WORLD.Barrier ( );

	if (id==0)
		cout << "Calculating overlaps with objects in PREVIOUS timesteps" << endl;


	if (it < namelist.number_of_timesteps)
		{
		// Send data to the next process (next timestep) - dont do it for the last process
		if (id != p -1 && it < namelist.number_of_timesteps - 1)
			MPI::COMM_WORLD.Send ( &(f.data).front(), f.size(), MPI::DOUBLE, id+1, 1 );
		// Receive data from the previous process (timestep) - dont do it for the first process
		if (id != 0)
			MPI::COMM_WORLD.Recv ( &(f2.data).front(), f.size(), MPI::DOUBLE, id-1, 1, status );
		}

  	// wait till copy finishes
	MPI::COMM_WORLD.Barrier ( );

	// -----------------------------------
	// Calculate overlaps - overlap with objects in PREVIOUS timestep
	// -----------------------------------
	if (it < namelist.number_of_timesteps)
		{
		if (it != 0)
			{
			f.Calculate_object_overlaps(f2, objects.FrameObjects_in_timestep[it].FrameObject, lower_obj_index_limit, upper_obj_index_limit);
			}
		}

	// -----------------------------------
	// Make a copy of this field to be used at the next time step
	// -----------------------------------
	// Send data from the last process f field to f2 field of the first process - so this field will be ready in the next iteration as the previous field to the first process
	MPI::COMM_WORLD.Barrier ( );
	// dont do this in the last iteration
	if (is <  number_of_steps - 1)
		{
		if (id == p -1)
			MPI::COMM_WORLD.Send ( &(f.data).front(), f.size(), MPI::DOUBLE, 0, 1 );
		if (id == 0)
			MPI::COMM_WORLD.Recv ( &(f2.data).front(), f.size(), MPI::DOUBLE, p -1, 1, status );
		}
	MPI::COMM_WORLD.Barrier ( );


	// -----------------------------------
	// Copy all the object data to the master process
	// -----------------------------------
	// Send the data from all other processes to the main process
	if (id > 0 && it < namelist.number_of_timesteps)
 		// first the data must be serialized using boost serialize library
		{
		oss.str("");
		s.clear();

		boost::archive::binary_oarchive oa(oss);
		oa << objects.FrameObjects_in_timestep[it].FrameObject;

		s=oss.str();
		length=s.size();

		// first we send the lenght of the data parameter
		MPI::COMM_WORLD.Send ( &length, 1, MPI::LONG, 0, 1 );
		// then we send the data itself - check first that it contains any data
		if (length > 0)
			MPI::COMM_WORLD.Send ( &s[0], length, MPI::CHAR, 0, 1 );

		//cout << "creating archive for timestep " << it << " by processor " << id << " of length " << length << endl;
		//if (id==2)
		//	write_string_to_new_file("ggg1.txt",objects.output_the_3D_objects_tree());


		}
	// Receive tha data by the main process
	if (id == 0 && it < namelist.number_of_timesteps)
		for (idx=1; idx < p; idx++)
			{
			itx=p*is + idx;
			// check for the last iteration that we do not expecting the data from some processes that will not send it
			if ( itx < namelist.number_of_timesteps)
				{
				// first we receive the lenght of the data parameter
				MPI::COMM_WORLD.Recv ( &length, 1, MPI::LONG, idx, 1 , status);
				s.clear();
				if (length > 0)
					{
					s.assign(length, ' ');
					MPI::COMM_WORLD.Recv ( &s[0], length, MPI::CHAR, idx, 1 , status);
					}
				// deserialize the data into the FrameObject
				iss.str("");
				iss << s;
				//cout << "reconstructing archive for timestep " << itx << " by processor " << idx << " of length " << length << endl;
				boost::archive::binary_iarchive ia(iss);
				ia >> objects.FrameObjects_in_timestep[itx].FrameObject;
				}

			}

	MPI::COMM_WORLD.Barrier ( );

	// ----------------------------------------------------
	// Calclate 4D objects according to FiT - the 4D index of object is denoted by new_ids
	///----------------------------------------------------
	if (id==0)
		if (namelist.output_4D_objects)
			{
			cout << "Defining 4D objects from overlaping information according to FiT  " << endl;
			for (idx=0; idx < p; idx++)
				{
				itx=p*is + idx;
				if ( itx < namelist.number_of_timesteps)
					{
					//objects.set_new_ids_from_overlaps_for_one_timestep_using_FIT_logic(itx,new_id_counter, new_id_counter_start);
					objects.set_new_ids_from_overlaps_for_one_timestep_using_FIT_logic(itx,new_id_counter, new_id_counter_start, namelist.horizontal_distance_limit_for_merging, namelist.domain_periodic_in_x_dimension, namelist.dimx );
					// reindex objects in field - values set by new_ids
					//cout << "reindex objects in field - values set by new_ids" << endl;
					}
				}
			}

	MPI::COMM_WORLD.Barrier ( );

	// -----------------------------------
	// Copy all the 4D object data from the master process to all other processes - so the 3D fields can be reindexed accordingly
	// -----------------------------------
	if (id==0)
		if (namelist.output_4D_objects)
			{
			for (idx=1; idx < p; idx++)
				{
				itx=p*is + idx;
				if ( itx < namelist.number_of_timesteps)
					{
					// serialize data
					oss.str("");
					s.clear();
					boost::archive::binary_oarchive oa(oss);
					oa << objects.FrameObjects_in_timestep[itx].FrameObject;
					s=oss.str();
					length=s.size();
					// first we send the lenght of the data parameter
					MPI::COMM_WORLD.Send ( &length, 1, MPI::LONG, idx, 1 );
					// then we send the data itself - check first that it contains any data
					if (length > 0)
						MPI::COMM_WORLD.Send ( &s[0], length, MPI::CHAR, idx, 1 );
					}
				}
			}
	if (id > 0 && it < namelist.number_of_timesteps)
		{
		// first we receive the lenght of the data parameter
		MPI::COMM_WORLD.Recv ( &length, 1, MPI::LONG, 0, 1 , status);
		s.clear();
		if (length > 0)
			{
			s.assign(length, ' ');
			MPI::COMM_WORLD.Recv ( &s[0], length, MPI::CHAR, 0, 1 , status);
			}
		// deserialize the data into the FrameObject
		iss.str("");
		iss << s;
		boost::archive::binary_iarchive ia(iss);
		ia >> objects.FrameObjects_in_timestep[it].FrameObject;
		}

	MPI::COMM_WORLD.Barrier ( );

	// reindex the objects
	if (it < namelist.number_of_timesteps)
		if (namelist.output_4D_objects )
			f.Reindex_objects_in_field_so_they_corespond_to_new_ids(objects.FrameObjects_in_timestep[it].FrameObject);


	// -----------------------------------
	// write 4D object files
	// -----------------------------------

	// write 4D object netcdf file
	if (it < namelist.number_of_timesteps)
	if (namelist.output_4D_objects)
		{
		s1.str("");
		s1 << namelist.output_file_prefix + output_leading_zero_string(it,number_of_digits_in_input_files) +"_4Dobjects" << namelist.output_file_extension();
		cout << "Writing file: " << s1.str() << endl;
		WriteC3DField_in_this_version_of_software(namelist, s1.str(), f);
		}

	// write 4D object PNG file
	if (it < namelist.number_of_timesteps)
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


MPI::COMM_WORLD.Barrier ( );

// -----------------------------------
// Write 3D object tree in txt file
// -----------------------------------
if (id==0)
if (namelist.output_3D_objects)
	{
	s1.str("");
	s1 << namelist.output_file_prefix << +"3Dobject_tree.txt";
	cout << "Writing file: " << s1.str() << endl;
	write_string_to_new_file_and_create_dir_if_necessary(s1.str(),objects.output_the_3D_objects_tree());
	}


// -----------------------------------
// Write 4D object tree in txt file
// -----------------------------------
if (id==0)
if (namelist.output_4D_objects)
	{
	s1.str("");
	s1 << namelist.output_file_prefix << +"4Dobject_tree.txt";
	cout << "Writing file: " << s1.str() << endl;
	write_string_to_new_file_and_create_dir_if_necessary(s1.str(),objects.output_the_4D_objects_tree(namelist.domain_periodic_in_x_dimension, namelist.dimx));
	}


//  Terminate MPI.
MPI::Finalize ( );

if (id==0)
	timer2(timer_start, timer_last_checkpoint, "");
}


