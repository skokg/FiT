
// ----------------------------------------------------------------------------------
// A class to represent properties of a 3D frame Object slice at a certain time.
// ----------------------------------------------------------------------------------
class C3DFrameObject
	{
	public:
	double object_id;
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	int zmin;
	int zmax;

	COverlap_list overlap_list;

	double new_id;

	double centerx,centery,centerz;
	double pieces;

	double size;

	double orientation,symetry;


	C3DFrameObject() {freememory();}
	~C3DFrameObject() {freememory();}

	void freememory()
		{
		object_id=new_id=0;
		centerx=centery=centerz=0;
		xmin=ymin=zmin=100000;
		xmax=ymax=zmax=-1;
		pieces=0;
		size=0;
		orientation=symetry=BAD_DATA_FLOAT;
		//overlaps.clear();
		overlap_list.freememory();
		//overlap_list_new_id.freememory();
		}

	C3DFrameObject& operator= (const C3DFrameObject &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			object_id=rhs.object_id;
			new_id=rhs.new_id;
			xmin=rhs.xmin;
			xmax=rhs.xmax;
			ymin=rhs.ymin;
			ymax=rhs.ymax;
			zmin=rhs.zmin;
			zmax=rhs.zmax;
			//overlaps=rhs.overlaps;
			overlap_list=rhs.overlap_list;
			//overlap_list_new_id=rhs.overlap_list_new_id;
			centerx=rhs.centerx;
			centery=rhs.centery;
			centerz=rhs.centerz;
			pieces=rhs.pieces;
			size=rhs.size;
			orientation=rhs.orientation;
			symetry=rhs.symetry;
			}
		return *this;    // Return ref for multiple assignment
		}

    #ifndef DISABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
		{
		ar & object_id;
		ar & new_id;
		ar & xmin;
		ar & xmax;
		ar & ymin;
		ar & ymax;
		ar & zmin;
		ar & zmax;
		ar & overlap_list;
		//ar & overlap_list_new_id;
		ar & centerx;
		ar & centery;
		ar & centerz;
		ar & pieces;
		ar & size;
		ar & orientation;
		ar & symetry;
		}
	#endif

	C3DFrameObject deep_copy() const
		{
		C3DFrameObject out;
		out=*this;
		return(out);
		}

	C3DFrameObject create_new_object_from_merging_of_objects(vector <C3DFrameObject*> fos, bool domain_periodic_in_x_dimension, int dimx)
		{
		long io;
		C3DFrameObject fo;

		fo.object_id=fos[0]->new_id;
		fo.new_id=fos[0]->new_id;
		fo.pieces=fos.size();

		for (io=0; io < (long)fos.size(); io++)
			{
			fo.size+=fos[io]->size;
			if (fos[io]->xmin < fo.xmin) fo.xmin = fos[io]->xmin;
			if (fos[io]->ymin < fo.ymin) fo.ymin = fos[io]->ymin;
			if (fos[io]->zmin < fo.zmin) fo.zmin = fos[io]->zmin;
			if (fos[io]->xmax > fo.xmax) fo.xmax = fos[io]->xmax;
			if (fos[io]->ymax > fo.ymax) fo.ymax = fos[io]->ymax;
			if (fos[io]->zmax > fo.zmax) fo.zmax = fos[io]->zmax;

			fo.centerx+= fos[io]->centerx*fos[io]->size;
			fo.centery+= fos[io]->centery*fos[io]->size;
			fo.centerz+= fos[io]->centerz*fos[io]->size;

			//cout << "- aa\t" << fo.object_id << "\t" << fos[io]->centerx << "\t" << fos[io]->size << endl;

			}

		fo.centerx*=1.0/fo.size;
		fo.centery*=1.0/fo.size;
		fo.centerz*=1.0/fo.size;


		// redo the calculation of fo.centerx if domain is periodic in x dimension - https://en.wikipedia.org/wiki/Center_of_mass#cite_note-10
		if (domain_periodic_in_x_dimension)
			{
			double xx_sum=0;
			double yy_sum=0;
			for (io=0; io < (long)fos.size(); io++)
				{
				double fi=(double)fos[io]->centerx/(double)dimx*2*M_PI;
				xx_sum+=fos[io]->size*cos(fi);
				yy_sum+=fos[io]->size*sin(fi);
				}

			if (xx_sum != 0 || yy_sum != 0)
				{
				double fi_avg=atan2(-yy_sum,-xx_sum) + M_PI;
				fo.centerx=fi_avg/(2*M_PI)*(double)dimx;
				}
			else
				fo.centerx=(double)dimx/2;
			}

		//cout << "- bb\t" << fo.object_id << "\t" << fo.centerx << "\t" << fos[io]->size << endl;


		return(fo);
		}




	string output() const
		{
		ostringstream s1;
		s1.str("");
		s1 << std::setprecision(12);
		s1 << object_id << "\tx:" << xmin << "-" << xmax << "\ty:" << ymin << "-" << ymax << "\tz:" << zmin << "-" << zmax << "\tcenter:" << centerx << ","<<  centery << "," << centerz << "\tpieces:" << pieces << "\tsize:" << size << "\tnew_id:" << new_id << "\toverlaps:" << overlap_list.output();
		//for (int ix=0; ix < (long)overlaps.size(); ix++)
		//	s1 << overlaps[ix] << ",";
		return(s1.str());
		}


	string output2() const
		{
		ostringstream s1;
		s1.str("");
		s1 << std::setprecision(12);
//		s1 << "x:" << xmin << "-" << xmax << "\ty:" << ymin << "-" << ymax << "\tz:" << zmin << "-" << zmax << "\tcnt:" << round(centerx) << ","<<  round(centery) << "," << round(centerz) << "\tpcs:" << pieces << "\tsize:" << size ;
		s1 << "x:" << xmin << "-" << xmax << "\ty:" << ymin << "-" << ymax << "\tz:" << zmin << "-" << zmax << "\tcnt:" << round_to_digits(centerx,1) << ","<<  round_to_digits(centery,1) << "," << round_to_digits(centerz,1) << "\tpcs:" << pieces << "\tsize:" << size << "\toverlaps:" << overlap_list.output();
		return(s1.str());
		}


	};


// ----------------------------------------------------------------------------------
// A class to represent ALL 3D frame Objects at a certain time step.
// ----------------------------------------------------------------------------------
class C3DFrameObjects_in_timestep
	{
	public:

	vector <C3DFrameObject> FrameObject;

	C3DFrameObjects_in_timestep() {freememory();}
	~C3DFrameObjects_in_timestep() {freememory();}

	void freememory()
		{
		FrameObject.clear();
		}

    #ifndef DISABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
		{
		ar & FrameObject;
		}
	#endif

	C3DFrameObjects_in_timestep& operator= (const C3DFrameObjects_in_timestep &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			FrameObject=rhs.FrameObject;
			}
		return *this;    // Return ref for multiple assignment
		}

	bool does_object_with_new_id_exist_in_this_set(double new_id, vector <C3DFrameObject*> &index)
		{
		index.clear();
		for (long io=0; io < (long)FrameObject.size(); io++)
			if (FrameObject[io].new_id == new_id)
				index.push_back(&FrameObject[io]);
		if (index.size() > 0) return(true);
		return(false);
		}

	vector <double> get_a_list_of_all_new_ids()
		{
		vector <double> list;
		for (long io=0; io < (long)FrameObject.size(); io++)
			if (FrameObject[io].size > 0)
			list.push_back(FrameObject[io].new_id);
		removeDuplicates(list);
		return(list);
		}

	C3DFrameObjects_in_timestep deep_copy() const
		{
		C3DFrameObjects_in_timestep out;
		out=*this;
		return(out);
		}

	string output() const
		{
		ostringstream s1;
		s1.str("");
		for (long il=0; il < (long)FrameObject.size(); il++ )
			s1 << il << "\t" << FrameObject[il].output() << endl;
		return(s1.str());
		}


	string output_larger_than_zero_size() const
		{
		ostringstream s1;
		s1.str("");
		for (long il=0; il < (long)FrameObject.size(); il++ )
			if (FrameObject[il].size > 0)
				s1 << il << "\t" << FrameObject[il].output() << endl;
		return(s1.str());
		}
	};


// ----------------------------------------------------------------------------------
// A class to represent ALL 3D frame Objects at ALL times
// ----------------------------------------------------------------------------------
class C3DFrameObjects_all
	{
	public:

	vector <C3DFrameObjects_in_timestep> FrameObjects_in_timestep;

	C3DFrameObjects_all() {freememory();}
	~C3DFrameObjects_all() {freememory();}

	C3DFrameObjects_all(long il)
		{
		freememory();
		C3DFrameObjects_in_timestep temp;
		FrameObjects_in_timestep.assign(il,temp);
		}

	void freememory()
		{
		FrameObjects_in_timestep.clear();
		}

    #ifndef DISABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
		{
		ar & FrameObjects_in_timestep;
		}
	#endif

	C3DFrameObjects_all& operator= (const C3DFrameObjects_all &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			FrameObjects_in_timestep=rhs.FrameObjects_in_timestep;
			}
		return *this;    // Return ref for multiple assignment
		}

	C3DFrameObjects_all deep_copy() const
		{
		C3DFrameObjects_all out;
		out=*this;
		return(out);
		}

	void set_new_ids_from_overlaps_for_one_timestep_using_FIT_logic(long timestep, double &new_id_counter, double new_id_counter_start, double horizontal_distance_limit_for_merging,  bool domain_periodic_in_x_dimension, int dimx)
		{
		long io;

		if (timestep >= (long)FrameObjects_in_timestep.size())
			error(AT,FU, "timestep >= FrameObjects_in_timestep.size()");

		// calculate max_id
		double max_id=0;
		for (io=0; io < (long)FrameObjects_in_timestep[timestep].FrameObject.size(); io++)
			if (FrameObjects_in_timestep[timestep].FrameObject[io].object_id > max_id)
				max_id=FrameObjects_in_timestep[timestep].FrameObject[io].object_id;

		if (max_id > new_id_counter_start || new_id_counter < new_id_counter_start)
			error(AT,FU, "max_id > new_id_counter_start || new_id_counter < new_id_counter_start");

		//cout << max_id << endl;



		COverlap overlap;

		// start setting the new_ids
		for (io=0; io < (long)FrameObjects_in_timestep[timestep].FrameObject.size(); io++)
			{
			C3DFrameObject *fo;
			fo = &FrameObjects_in_timestep[timestep].FrameObject[io];
		//ycout << "aaaa" << endl;


			// dont do anything for zero sized objects - this are objects with id-s up to lower_obj_index_limit
			if (fo->size > 0)
				{
				// no overlap - new object
				if (fo->overlap_list.overlap.size() == 0)
					{
					fo->new_id=new_id_counter;
					new_id_counter++;
					}

				// one or more overlaps - new_id is the same as new_id of overlaping object in previous time step which has the biggest overlapping size
				else
					{
					if (!fo->overlap_list.get_object_with_biggest_overlap_size(overlap))
						error(AT,FU, "!overlap_list.get_object_with_biggest_overlap_size(overlap)");

					C3DFrameObject *fo2;
					fo2 = &FrameObjects_in_timestep[timestep-1].FrameObject[overlap.id];
					fo->new_id=	fo2->new_id;
					//cout << overlap.id << "\t" << fo->new_id << "\t" << fo2->new_id << endl;
					}
				}
			}

		// Reverse the merging of objects that are too far away from the center of the 4D object they belong to
		// This criteria is disabled if horizontal_distance_limit_for_merging < 0
		if (horizontal_distance_limit_for_merging >= 0)
			{
			// First calculate the centers of all 4D objects in the current timestep
			vector <double> new_id_list = FrameObjects_in_timestep[timestep].get_a_list_of_all_new_ids();
			vector <double> new_id_list_max_overlap_id;
			vector <C3DFrameObject> fonp_vector;
			//cout << output_vector_as_string(new_id_list, ", ") << endl;
			for (unsigned long il=0; il < new_id_list.size(); il++)
				{
				vector <C3DFrameObject*> ptr_fos;
				if (FrameObjects_in_timestep[timestep].does_object_with_new_id_exist_in_this_set(new_id_list[il], ptr_fos))
					{
					C3DFrameObject fonp;
					C3DFrameObject fo_tmp;
					fonp=fo_tmp.create_new_object_from_merging_of_objects(ptr_fos, domain_periodic_in_x_dimension, dimx );
					fonp_vector.push_back(fonp.deep_copy());
					//cout << fonp.new_id << "\t" << fonp.output2() << endl;

					// calculate the id of the object that has the largest overlap with the new object in the previous timestep
					double max_overlap_size = 0;
					double max_overlap_id = -1;
					// loop over all obects that have the new id in the current timestep
					for (unsigned long io=0; io < ptr_fos.size(); io++)
						{
						// loop over all overlaps
						double current_overlap_size = 0;
						for (unsigned long ioverlap=0; ioverlap < ptr_fos[io]->overlap_list.overlap.size(); ioverlap++)
							{
							COverlap overlap;
							overlap = ptr_fos[io]->overlap_list.overlap[ioverlap];
							if (FrameObjects_in_timestep[timestep-1].FrameObject.at(overlap.id).new_id == new_id_list[il])
								current_overlap_size+=overlap.overlap_size;
							}
						if (current_overlap_size > max_overlap_size)
							{
							max_overlap_size = current_overlap_size;
							max_overlap_id =   ptr_fos[io]->object_id;
							}
						}
					//ERRORIF(max_overlap_id <= 0);
					new_id_list_max_overlap_id.push_back(max_overlap_id);
					//cout << "max_overlap_id: " << new_id_list[il] << "\t" << max_overlap_id << "\t" << max_overlap_size << endl;
					}
				else
					{
					error(AT,FU, "FrameObjects_in_timestep[timestep].does_object_with_new_id_exist_in_this_set(new_id_list[il], ptr_fos)");
					}
				}
			ERRORIF(new_id_list.size() != new_id_list_max_overlap_id.size() );
			ERRORIF(new_id_list.size() != fonp_vector.size() );

			//cout << FrameObjects_in_timestep[timestep].output_larger_than_zero_size() << endl;

			// Next, check whether individual merged objects are not too far from the center of the 4D object they belong to
			for (io=0; io < (long)FrameObjects_in_timestep[timestep].FrameObject.size(); io++)
				{
				C3DFrameObject *fo;
				fo = &FrameObjects_in_timestep[timestep].FrameObject[io];
				// check only objects that have overlap
				if (fo->overlap_list.overlap.size() > 0)
					// find which 4D object does this 3D object belong to
					for (unsigned long il=0; il < fonp_vector.size(); il++)
						if (fonp_vector[il].new_id == fo->new_id)
							// only do it if 4D object has more than one piece
							if (fonp_vector[il].pieces > 1)
								{
								// calculate centroid distance
								double horizontal_centroid_distance=euclidian_distance(fonp_vector[il].centerx, fonp_vector[il].centery, fo->centerx, fo->centery);

								// recalculate distance if domain is periodic
								if (domain_periodic_in_x_dimension)
									horizontal_centroid_distance=euclidian_distance_domain_periodic_in_x_dimension(fonp_vector[il].centerx, fonp_vector[il].centery, fo->centerx, fo->centery, dimx);

								//cout << fonp_vector[il].new_id << "\t" <<  fonp_vector[il].centerx << "\t" << fo->centerx << "\t" << horizontal_centroid_distance << endl;

								// if the distance is too large start a new 4D object insted of merging (this logic ignores the overlaps with other objects)
								if (horizontal_centroid_distance > horizontal_distance_limit_for_merging)
									{
									// Check if this is the object with the biggest overlap - in this case don't un-merge it
									if (new_id_list_max_overlap_id[il] != fo->object_id)
										{
										fo->new_id=new_id_counter;
										new_id_counter++;
										}
									}
								}
				}
			}
		}

	void set_new_ids_from_overlaps_for_all_timesteps_using_FIT_logic(double &new_id_counter, double new_id_counter_start, double horizontal_distance_limit_for_merging, bool domain_periodic_in_x_dimension, int dimx)
		{
		for (long it=0; it < (long)FrameObjects_in_timestep.size(); it++)
			set_new_ids_from_overlaps_for_one_timestep_using_FIT_logic(it, new_id_counter,  new_id_counter_start, horizontal_distance_limit_for_merging, domain_periodic_in_x_dimension, dimx);
		}


	// this simply list all 3D objects with old id-s - so the id-s are not unique.
	string output_the_3D_objects_tree()
		{
		long it, io;
		ostringstream s1;
		s1.str("");
		s1 << std::setprecision(12);
		C3DFrameObject *fo;

		s1 << "object id (unique only in timestep)" << "\t" << "timestep" << "\t" << "x extent" << "\t" << "y extent" << "\t" << "z extent" << "\t" << "object center" << "\t" << "number of pieces" << "\t" << "object volume (num. of gridpoints)" << endl;

		for (it=0; it < (long)FrameObjects_in_timestep.size(); it++)
			for (io=0; io < (long)FrameObjects_in_timestep[it].FrameObject.size(); io++)
				if (FrameObjects_in_timestep[it].FrameObject[io].size > 0)
					{
					fo=&FrameObjects_in_timestep[it].FrameObject[io];

					s1 << fo->object_id << "\t" << output_leading_zero_string(it,4) << "\t" << fo->output2() << endl;
					}

		return (s1.str());
		}


	string output_the_4D_objects_tree(bool domain_periodic_in_x_dimension, int dimx)
		{
		long it, io, it2, lifespan;
		C3DFrameObject fonp;
		C3DFrameObject *fo;
		bool found_in_previous_timestep;
		vector <C3DFrameObject*> ptr_fos;
		ostringstream s1;
		s1.str("");
		s1 << std::setprecision(12);

		s1 << "unique object id" << "\t" << "lifestep of object" << "\t" << "timestep" << "\t" << "x extent" << "\t" << "y extent" << "\t" << "z extent" << "\t" << "object center" << "\t" << "number of pieces" << "\t" << "object volume (num. of gridpoints)" << endl;

		for (it=0; it < (long)FrameObjects_in_timestep.size(); it++)
			for (io=0; io < (long)FrameObjects_in_timestep[it].FrameObject.size(); io++)
				if (FrameObjects_in_timestep[it].FrameObject[io].size > 0)
					{
					fo = &FrameObjects_in_timestep[it].FrameObject[io];

					// check if object existed in previous timestep
					found_in_previous_timestep=false;
					if (it != 0)
						found_in_previous_timestep=FrameObjects_in_timestep[it-1].does_object_with_new_id_exist_in_this_set(fo->new_id, ptr_fos);

					lifespan=0;
					if (!found_in_previous_timestep)
						{
						// display object info from this timestep
						fo->pieces=1;
						s1 << fo->new_id << "\t" << lifespan << "\t" << output_leading_zero_string(it,4) << "\t" << fo->output2() << endl;
						// check in next timesteps
						for (it2=it+1; it2 < (long)FrameObjects_in_timestep.size(); it2++)
							{
							if (FrameObjects_in_timestep[it2].does_object_with_new_id_exist_in_this_set(fo->new_id, ptr_fos))
								{
								lifespan++;
								fonp=fo->create_new_object_from_merging_of_objects(ptr_fos, domain_periodic_in_x_dimension, dimx);
								s1 << fonp.new_id << "\t" << lifespan << "\t" << output_leading_zero_string(it2,4) << "\t" << fonp.output2() << endl;
								}

							// no need to check further in the future in object was not find at it2 timestep
							else break;
							}
						}

					}

		return (s1.str());
		}



	string output() const
		{
		ostringstream s1;
		s1.str("");
		for (long il=0; il < (long)FrameObjects_in_timestep.size(); il++ )
			{
			s1 << "Timestep:" << il << "    -----------------------------------------------------------" << endl;
			s1 << FrameObjects_in_timestep[il].output() << endl;
			}
		return(s1.str());
		}


	};
