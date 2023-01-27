// ******************************************************
// ******************************************************
// Class CField. Stores one 3-D field with dimension atributes and raw data.
//
class C3DField {
	public:
	vector <double> data;
	int dimx,dimy,dimz;

	// constructor
	C3DField() {freememory();}
	~C3DField() {freememory();}

	// frees memory
	void freememory()
		{
		data.clear();
		dimx=0;
		dimy=0;
		dimz=0;
		}

	// allocates memory
	void allocatememory(int x,int y,int z)
		{
		dimx=x;
		dimy=y;
		dimz=z;
		data.assign(long(x)*long(y)*long(z),0);
		}

	bool check_if_initilized() const
		{
		if (data.size()==0 || size() != long(dimx)*long(dimy)*long(dimz) ) return (false);
		return(true);
		}

	// area size of field
	long size() const
		{
		return((long)data.size());
		}

	bool compare_have_they_the_same_dimensions(const C3DField &f) const
		{
		if (dimx != f.dimx) return(false);
		if (dimy != f.dimy) return(false);
		if (dimz != f.dimz) return(false);
		return(true);
		}


	bool compare_are_they_the_same(const C3DField &f) const
		{
		if (data != f.data) return(false);
		if (!compare_have_they_the_same_dimensions(f)) return(false);
		return(true);
		}

	// overload operators == nad !=
	bool operator== (const C3DField& f) const
		{return(compare_are_they_the_same(f));}
	bool operator!= (const C3DField& f) const
		{return(!compare_are_they_the_same(f));}

	bool check_coordianes(int x, int y, int z)  const
		{
		if (x >= 0 && x < dimx && y >= 0 && y < dimy && z >=0 && z < dimz) return(true);
		else return(false);
		}

	long get_index_from_xyz (int x, int y, int z) const
		{
		return((long)z*(long)dimy*(long)dimx +  (long)y*(long)dimx + (long)x );
		}

	long get_z_from_index (long il) const
		{
		return(il/((long)dimy*(long)dimx));
		}

	long get_y_from_index (long il) const
		{
		return(il%((long)dimy*(long)dimx)/(long)dimx);
		}

	long get_x_from_index (long il) const
		{
		return(il%((long)dimy*(long)dimx)%(long)dimx);
		}

	// gets value at x,y,z
	double get(int x, int y, int z) const
		{
		return(data[get_index_from_xyz (x, y, z)]);
		}

	// sets value at x,y,z
	double set(int x, int y, int z, double val)
		{
		return(data[get_index_from_xyz (x, y, z)]=val);
		}


	C3DField& operator= (const C3DField &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			if (!rhs.check_if_initilized())
				error(AT,FU, "!rhs.check_if_initilized()");
			data=rhs.data;
			dimx = rhs.dimx;
			dimy = rhs.dimy;
			dimz = rhs.dimz;
			}
		return *this;    // Return ref for multiple assignment
		}



	// sets all values of a field to value
	void Set_all_values(double value)
		{
		for (long il=0; il < size(); il++)
			data[il]=value;
		}

	void Set_all_values_in_rectangular_area(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax, double value)
		{
		for (int x=xmin; x <= xmax; x++)
		for (int y=ymin; y <= ymax; y++)
		for (int z=zmin; z <= zmax; z++)
			set(x, y, z, value);
		}

	void Set_all_values_in_rectangular_area_periodic_x_dimension(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax, double value)
		{
		for (int x=xmin; x <= xmax; x++)
		for (int y=ymin; y <= ymax; y++)
		for (int z=zmin; z <= zmax; z++)
			{
			int xnew=x % dimx;
			if (xnew < 0) xnew+=dimx;
			set(xnew, y, z, value);
			}
		}


	void multiply_by_double(double x)
		{
		for (long il=0; il < size(); il++)
			if (data[il] != BAD_DATA_FLOAT)
				data[il]*=x;
		}

	double get_maximum() const
		{
		double max=BAD_DATA_FLOAT;
		for (long il=0; il < size(); il++)
			if (data[il] > max)
				max=data[il];
		return(max);
		}

	void crop(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax)
		{
		if (!check_coordianes(xmin,ymin,zmin) || !check_coordianes(xmax,ymax,zmax))
			error(AT,FU, "!check_coordianes(xmin,ymin,zmin) || !check_coordianes(xmax,ymax,zmax)");

		if (xmax < xmin || ymax < ymin || zmax < zmin)
			error(AT,FU, "xmax < xmin || ymax < ymin || zmax < zmin");


		C3DField f;
		f=*this;

		freememory();
		allocatememory(xmax-xmin+1,ymax-ymin+1,zmax-zmin+1);

		for (int x=xmin; x <= xmax; x++)
		for (int y=ymin; y <= ymax; y++)
		for (int z=zmin; z <= zmax; z++)
			set(x-xmin, y-ymin, z-zmin, f.get(x,y,z));

		f.freememory();
		}

    #ifndef DISABLE_THINGS_NOT_NECESSARY_FOR_FIT
	CField create_z_level_CField(int z) const
		{
		CField f;
		f.dimx=dimx;
		f.dimy=dimy;
		f.allocatememory();
		f.allocatememory_lat_lon_array();
		for (int x=0; x < dimx; x++)
		for (int y=0; y < dimy; y++)
			f.set(x,y,get(x,y,z));
		return(f);
		}
	#endif



	// creates a "pancake" from 3D to 2D that it projects the value from the lowest elevation with non-zero value. At locations where the are no non-zero values in the column the resulting values is 0
    #ifndef DISABLE_THINGS_NOT_NECESSARY_FOR_FIT
	CField create_2D_field_with_lowest_non_zero_values(CField &f_count) const
		{
		double v;
		CField f;
		f.dimx=dimx;
		f.dimy=dimy;
		f.allocatememory();
		f.allocatememory_lat_lon_array();
		f_count=f;

		for (int x=0; x < dimx; x++)
		for (int y=0; y < dimy; y++)
			{
			v=0;
			for (int z=dimz-1; z >= 0; z--)
				if (get(x,y,z) > 0)
					{
					v=get(x,y,z);
					f_count.data[f_count.XY2DataLoc(x,y)]++;
					}
			f.set(x,y,v);
			}
		return(f);
		}
	#endif


    #ifndef DISABLE_THINGS_NOT_NECESSARY_FOR_FIT
	void create_3D_field_from_2D_CField(const CField &f)
		{
		ERRORIF(!f.check_if_data_is_initilized());
		ERRORIF(f.dimx*f.dimy == 0);
		allocatememory(f.dimx,f.dimy,1);
		for (long ix=0; ix < dimx; ix++)
		for (long iy=0; iy < dimy; iy++)
			set(ix,iy,0,f.get(ix,iy));
		}
	#endif


	void WriteField_PNG_projected_to_2d_with_objects_on_white_background(string fname) const
		{
		int ix, iy, iz;
		gdImagePtr image;

		std::mt19937 rng_state;

		mkpath_from_path_with_filenane(fname);

		image = gdImageCreateTrueColor(dimx, dimy);

		// generate index list for the lowest lying objects
		double previous_index=-1;
		vector <double> previous_rgb {0,0,0};
		double r,g,b;
		r=g=b=0;

		for (iy=0;iy<dimy;iy++)
		for (ix=0;ix<dimx;ix++)
			{
			double index_of_lowest_object=0;
			double counter=0;
			for (iz=0;iz<dimz;iz++)
				if (get(ix,iy,iz) > 0)
					{
					counter++;
					if (index_of_lowest_object == 0)
						index_of_lowest_object = get(ix,iy,iz);
					}

			// no objects
			if (counter == 0)
				{
				gd_draw_pixel(image, ix, iy, 1.0, 1.0, 1.0);
				}

			// object found
			else
				{

				// check if the same object was in the previous pixel
				if (index_of_lowest_object == previous_index)
					{
					r=previous_rgb[0];
					g=previous_rgb[1];
					b=previous_rgb[2];
					}
				else
					{
					long idum=(long)-index_of_lowest_object-1;

					r=g=b=0;
					// eliminate colors that are too dark or light - since they are hard to discern
					while (r+g+b < 2 || r+g+b > 2.5  )
						{
						r=ran2_threadsafe(&idum, rng_state);
						g=ran2_threadsafe(&idum, rng_state);
						b=ran2_threadsafe(&idum, rng_state);
						}
					//cout << ix << "\t" << iy << "\t" << index_of_lowest_object << "\t" << idum << "\t" << r << "\t" << g << "\t" << b << endl;
					}

				previous_index=index_of_lowest_object;
				previous_rgb = {r,g,b};

				// scale color so a higher count is more dark
				r*=((double)dimz-counter+1)/(double)dimz;
				g*=((double)dimz-counter+1)/(double)dimz;
				b*=((double)dimz-counter+1)/(double)dimz;

				gd_draw_pixel(image, ix, iy, r, g, b);
				}


			}

		//int	color=gdImageColorAllocate(image, (int)1*255.,(int)0*255.,(int)0*255.);
		//gdImageSetPixel( image , 3, 3, color );
		//color=gdImageColorAllocate(image, (int)0*255.,(int)1*255.,(int)0*255.);
		//gdImageSetPixel( image , 4, 3, color );

		Write_gd_image_to_png(image, fname);
		gdImageDestroy(image);
		}


    #ifndef DISABLE_THINGS_NOT_NECESSARY_FOR_FIT
	void calculate_object_orientation_and_symetry(vector <C3DFrameObject> &I3DFrameObject)
		{
		long io;

		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		CField f;
		f.allocatememory_and_set_dimx_dimy(dimx, dimy);
		f.Set_all_values(0);

		C3DFrameObject *fo;

		// loop over all objects
		for (io=0; io < (long)I3DFrameObject.size(); io++)
			if (I3DFrameObject[io].size > 1)
				{
				fo=&I3DFrameObject[io];
				// create a projection onto 2D
				for (int ix=fo->xmin; ix <= fo->xmax; ix++ )
				for (int iy=fo->ymin; iy <= fo->ymax; iy++ )
				for (int iz=fo->zmin; iz <= fo->zmax; iz++ )
					if (get(ix,iy,iz) == fo->object_id) f.set(ix-fo->xmin,iy-fo->ymin,1);

				f.calculate_object_orientation_and_symetry_using_image_algebra_book_algorithm(fo->xmin, fo->xmax, fo->ymin, fo->ymax, fo->orientation, fo->symetry);

				// does not work with periodic domains
				f.Replace_values_only_in_certain_area(1, 0, fo->xmin, fo->xmax, fo->ymin, fo->ymax);
				}
		}
	#endif

	// old is the original_value_which_is_to_be_replaced, new_value in the new value
	void floodfill_stack_based_and_log_the_touching_values_and_make_a_list_of_all_affected_points(int x, int y, int z, double old, double new_value, vector <double> &touching_values_list, double dont_log_values_lower_than_this, vector <long> &list_of_affected_points, bool domain_periodic_in_x_dimension)
		{

		double val;

		CPoint3Dxyz<int> p,t;

 		//1. Set Q to the empty queue.
		vector <CPoint3Dxyz<int> > Q;
		// 2. Add node to the end of Q.
		Q.push_back(t.create(x,y,z));
		// 4. While Q is not empty:
		while (Q.size() > 0)
			{
			// 5.     Set n equal to the last element of Q.
			p=Q.back();
			// 7.     Remove last element from Q.
			Q.pop_back();

			val=get(p.x,p.y,p.z);

			// 8.     If the color of n is equal to target-color:
			if (val==old)
				{
				//9.         Set the color of n to replacement-color.
				set(p.x,p.y,p.z,new_value);
				list_of_affected_points.push_back(get_index_from_xyz (p.x,p.y,p.z));
		 		//10.        Add surrounding nodes to end of Q.
				if (p.x < dimx - 1) Q.push_back(t.create(p.x+1, p.y  , p.z  ));
				if (p.x > 0       ) Q.push_back(t.create(p.x-1, p.y  , p.z  ));
				if (domain_periodic_in_x_dimension)
					{
					if (p.x == dimx - 1) Q.push_back(t.create(0, p.y  , p.z  ));
					if (p.x == 0) Q.push_back(t.create(dimx - 1, p.y  , p.z  ));
					}

				if (p.y < dimy - 1) Q.push_back(t.create(p.x  , p.y+1, p.z  ));
				if (p.y > 0       ) Q.push_back(t.create(p.x  , p.y-1, p.z  ));
				if (p.z < dimz - 1) Q.push_back(t.create(p.x  , p.y  , p.z+1));
				if (p.z > 0       ) Q.push_back(t.create(p.x  , p.y  , p.z-1));
				}
			else if (val >= dont_log_values_lower_than_this && val != new_value)
				{
				// search if this value was allready logged
				if (!binary_search(touching_values_list.begin(), touching_values_list.end(),val))
					{
					//cout << val << endl;
					// add the value to list
					touching_values_list.push_back(val);
					// sort - so that we can later use the very fast binary_search
					sort(touching_values_list.begin(), touching_values_list.end());
					}
				}

			}

		}



	void Identify_3D_objects_using_floodfill_and_cascading_threshold(int number_of_thresholds, double lowest_object_id, double &number_of_found_objects, bool domain_periodic_in_x_dimension)
		{
		long il,it;
		double val;

		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		if (number_of_thresholds < 1)
			error(AT,FU, "thrs_list.size() < 1");

		// check if lowest_object_id is high enough
		if (number_of_thresholds +1 > lowest_object_id)
			error(AT,FU, "thrs_list.size() +1 > lowest_object_id");

		// check if all values fell between -1 and thrs_list.size()
		for (il=0; il < size(); il++)
			if (data[il] < -1 || data[il] > number_of_thresholds )
				{
				error(AT,FU, "data[il] < -1 || data[il] > number_of_thresholds. data[il]="+n2s(data[il]));
				}

		// test write fields
		//ostringstream s1;
		//s1.str("");
		//s1 << "aaa";
		//WriteField(s1.str()+".nc",f);
		//WriteField_PNG(s1.str()+".png",  f);

		// do the cascading object identification
		double counter=lowest_object_id;
		vector <double> touching_values_list;
		vector <long> list_of_affected_points;
		vector <long> list_of_affected_points_new;
		for (it=number_of_thresholds; it > 0 ; it--)
			{
			val=it;
			for (int x=0; x < dimx; x++)
			for (int y=0; y < dimy; y++)
			for (int z=0; z < dimz; z++)
				if (get(x,y,z) == val)
					{
					// do the flood fill
					touching_values_list.clear();
					list_of_affected_points.clear();
					floodfill_stack_based_and_log_the_touching_values_and_make_a_list_of_all_affected_points(x,y,z, val, counter,touching_values_list, lowest_object_id, list_of_affected_points,domain_periodic_in_x_dimension);

					//cout << val << "\t" << x << ":" << y << "\t" << list_of_affected_points.size() << "\t" << touching_values_list.size() << ":" ;
					//for (long io=0; io < (long)touching_values_list.size(); io++)
					//	cout << touching_values_list[io] << ",";
					//cout << endl;

					// if there is only one touching value - set to id of the touching value
					if (touching_values_list.size() == 1)
						for (il=0; il < (long)list_of_affected_points.size(); il++)
							data[list_of_affected_points[il]]=touching_values_list.back();

					// if there is more than one touching object - set the ids to -2 so that the growing algorithm can be applied later
					if (touching_values_list.size() > 1)
						for (il=0; il < (long)list_of_affected_points.size(); il++)
							data[list_of_affected_points[il]]=-2;

					// increase counter only if it is a new object
					if (touching_values_list.size() == 0)
						counter++;
					}

			Grow_object_area(-2, lowest_object_id, 1E100, domain_periodic_in_x_dimension);

			// test write fields
			//ostringstream s1;
			//s1.str("");
			//s1 << "aaa" << it;
			//WriteField(s1.str()+".nc",f);
			//WriteField_PNG(s1.str()+".png",  f);
			}

		number_of_found_objects=counter-lowest_object_id;

		//return(f);
		}



	void Calculate_object_attributes(vector <C3DFrameObject> &I3DFrameObject, double lower_obj_index_limit, double upper_obj_index_limit, bool domain_periodic_in_x_dimension) const
		{
		long il;

		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		// check if any 1 is present in the dataset
		for (il=0; il < size(); il++)
			if (data[il] == 1 )
				error(AT,FU, "data[il] == 1!");

		// get max_object_id
		double max_object_id=get_maximum();

		// assign memory to object list
		C3DFrameObject temp_o;
		C3DFrameObject *op;
		I3DFrameObject.assign((long)max_object_id+1,temp_o);

		// Area size and center
		for (int x=0; x < dimx; x++)
		for (int y=0; y < dimy; y++)
		for (int z=0; z < dimz; z++)
			if (get(x,y,z) >= lower_obj_index_limit)
				{
				op=&I3DFrameObject[(long)get(x,y,z)];
				op->size++;
				if (x < op->xmin) op->xmin=x;
				if (x > op->xmax) op->xmax=x;
				if (y < op->ymin) op->ymin=y;
				if (y > op->ymax) op->ymax=y;
				if (z < op->zmin) op->zmin=z;
				if (z > op->zmax) op->zmax=z;
				op->centerx+=(double)x;
				op->centery+=(double)y;
				op->centerz+=(double)z;
				}
		// normalize center
		for (il=0; il <= max_object_id; il++)
			if (I3DFrameObject[il].size > 0)
				{
				op=&I3DFrameObject[il];
				op->centerx*=1.0/op->size;
				op->centery*=1.0/op->size;
				op->centerz*=1.0/op->size;
				}
		// set object ids
		for (il=0; il <= max_object_id; il++)
			I3DFrameObject[il].object_id = il;

		// redo the calculation of x coordinates if domain is periodic in x dimension
		if (domain_periodic_in_x_dimension)
			{
			vector <double> xx_sum ((long)max_object_id+1,0);
			vector <double> yy_sum ((long)max_object_id+1,0);
			for (int x=0; x < dimx; x++)
			for (int y=0; y < dimy; y++)
			for (int z=0; z < dimz; z++)
				if (get(x,y,z) >= lower_obj_index_limit)
					{
					double fi=(double)x/(double)dimx*2*M_PI;
					xx_sum[(long)get(x,y,z)]+=cos(fi);
					yy_sum[(long)get(x,y,z)]+=sin(fi);
					}
			// calculate center
			for (il=0; il <= max_object_id; il++)
				if (I3DFrameObject[il].size > 0)
					{
					op=&I3DFrameObject[il];
					if (xx_sum[il] != 0 || yy_sum[il] != 0)
						{
						double fi_avg=atan2(-yy_sum[il],-xx_sum[il]) + M_PI;
						op->centerx=fi_avg/(2*M_PI)*(double)dimx;
						}
					// center cannot be determined - set ceter to dimx/2
					else
						op->centerx=(double)dimx/2;
					}
			}
		}

	void Calculate_object_overlaps(const C3DField &f, vector <C3DFrameObject> &I3DFrameObject, double lower_obj_index_limit, double upper_obj_index_limit) const
		{
		long il;

		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		if (!f.check_if_initilized())
			error(AT,FU, "!f.check_if_initilized()");

		for (il=0; il < size(); il++)
			if (is_inside_interval(lower_obj_index_limit, upper_obj_index_limit, data[il]))
				if (is_inside_interval(lower_obj_index_limit, upper_obj_index_limit, f.data[il]))
					I3DFrameObject[(long)data[il]].overlap_list.add_overlap(f.data[il]);

		}


	bool Check_if_any_neigboring_point_has_a_value_in_interval(int x, int y, int z, double lower_limit, double upper_limit, double &v, bool domain_periodic_in_x_dimension) const
		{
		if (x < dimx - 1) {v=get(x+1, y  , z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		if (x > 0       ) {v=get(x-1, y  , z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		if (domain_periodic_in_x_dimension)
			{
			if (x == dimx - 1) {v=get(0, y  , z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
			if (x == 0) {v=get(dimx - 1, y  , z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
			}
		if (y < dimy - 1) {v=get(x  , y+1, z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		if (y > 0       ) {v=get(x  , y-1, z  ); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		if (z < dimz - 1) {v=get(x  , y  , z+1); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		if (z > 0       ) {v=get(x  , y  , z-1); if (is_inside_interval(lower_limit, upper_limit, v)) return(true);}
		return(false);
		}

	// in field f the allowed growth area is defined with allow_grow_index
	void Grow_object_area(double allow_grow_index, double lower_obj_index_limit, double upper_obj_index_limit, bool domain_periodic_in_x_dimension)
		{
		long il;
		int x,y,z;
		double a;

		double temp_obj;

		vector <CPoint3Dxyz<int> > old_points;
		vector <CPoint3Dxyz<int> > new_points;
		CPoint3Dxyz<int> p;
		CPoint3Dxyz<int> *t;

		// identify starting points
		for (x=0;x<dimx;x++)
		for (y=0;y<dimy;y++)
		for (z=0;z<dimz;z++)
			if (get(x,y,z) == allow_grow_index)
				if (Check_if_any_neigboring_point_has_a_value_in_interval(x,y,z,lower_obj_index_limit,upper_obj_index_limit,temp_obj, domain_periodic_in_x_dimension))
					{
					p.set(x,y,z);
					p.a=temp_obj;
					old_points.push_back(p.deep_copy());

					//cout << p.output() << endl;

					}

		while (old_points.size() != 0)
			{
			//cout << old_points.size() << endl;
			new_points.clear();
			for (il=0; il < (long)old_points.size(); il++)
				{
				//cout << il << endl;
				t=&old_points[il];
				x=t->x;
				y=t->y;
				z=t->z;
				a=t->a;


				//check if the value was allready set by one of the previous objects - so we dont repeat ourselves
				if (get(x,y,z) == allow_grow_index)
					{
					// set the value of old_point
					set(x,y,z,a);

					// check neigboring points
					if (x < dimx - 1) if (get(x+1, y  , z  ) == allow_grow_index) {p.set(x+1, y  , z  ); p.a=a; new_points.push_back(p.deep_copy());}
					if (x > 0       ) if (get(x-1, y  , z  ) == allow_grow_index) {p.set(x-1, y  , z  ); p.a=a; new_points.push_back(p.deep_copy());}
					if (domain_periodic_in_x_dimension)
						{
						if (x == dimx - 1) if (get(0, y  , z  ) == allow_grow_index) {p.set(0, y  , z  ); p.a=a; new_points.push_back(p.deep_copy());}
						if (x == 0) if (get(dimx - 1, y  , z  ) == allow_grow_index) {p.set(dimx - 1, y  , z  ); p.a=a; new_points.push_back(p.deep_copy());}
						}
					if (y < dimy - 1) if (get(x  , y+1, z  ) == allow_grow_index) {p.set(x  , y+1, z  ); p.a=a; new_points.push_back(p.deep_copy());}
					if (y > 0       ) if (get(x  , y-1, z  ) == allow_grow_index) {p.set(x  , y-1, z  ); p.a=a; new_points.push_back(p.deep_copy());}
					if (z < dimz - 1) if (get(x  , y  , z+1) == allow_grow_index) {p.set(x  , y  , z+1); p.a=a; new_points.push_back(p.deep_copy());}
					if (z > 0       ) if (get(x  , y  , z-1) == allow_grow_index) {p.set(x  , y  , z-1); p.a=a; new_points.push_back(p.deep_copy());}
					}
				}

			//exit(1);
			old_points=new_points;
			}

		//for (il=0; il < old_points.size(); il++)
		//	cout << il << "\t" << old_points[il].x  << "\t" << old_points[il].y  << "\t" << old_points[il].lat << endl;

//		return(f_grow);

		}



	// replace all values oldV with newV
	void Replace_values_only_in_certain_area(double oldV, double newV, int xmin, int xmax, int ymin, int ymax, int zmin, int zmax)
		{
		if ( !check_coordianes(xmin, ymin, zmin) || !check_coordianes(xmax, ymax, zmax))
			error(AT,FU, "!check_coordianes(xmin, ymin, zmin) || !check_coordianes(xmax, ymax, zmax)");

		int x,y,z;

		for (x=xmin ; x<=xmax ; x++)
		for (y=ymin ; y<=ymax ; y++)
		for (z=zmin ; z<=zmax ; z++)
				if (get(x,y,z) == oldV)
					set(x,y,z,newV);
		}

	void Reindex_objects_in_field_so_they_corespond_to_new_ids(vector <C3DFrameObject> &I3DFrameObject)
		{
		long iof;

		C3DFrameObject *fo;

		for (iof=0; iof < (long)I3DFrameObject.size(); iof++ )
			if (I3DFrameObject[iof].size > 0)
				{
				fo=&I3DFrameObject[iof];
				Replace_values_only_in_certain_area(fo->object_id, fo->new_id, fo->xmin, fo->xmax, fo->ymin, fo->ymax, fo->zmin, fo->zmax);
				}
		}

	void Read_From_Binary_Fortran_File(string filename, float &fortran_binary_header, int dimx_, int dimy_, int dimz_)
		{
		// fortran binary uses 4 byte floats and not 8 bytes double such as C
		float temp;

		if (!FileExists(filename))
			error(AT,FU, filename + " file does not exist!");

		allocatememory(dimx_,dimy_,dimz_);

		// check file size is correct
		struct stat filestatus;
		stat( filename.c_str(), &filestatus );
		//cout << filestatus.st_size << endl;
		//cout << (size()+2)*(long)sizeof(float) << endl;
		if ((long)filestatus.st_size != (size()+2)*(long)sizeof(float))
			error(AT,FU, filename + ". File not the right size with regard to specified x,y,z dimensions - probably wrong dimensions were specified!");


		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		ifstream myFile (filename.c_str(), ios::binary);
		// read 4 byte header
		myFile.read ((char *) &fortran_binary_header, sizeof(float));
		//cout << "val: " << temp << endl;

		for (long il=0; il < size(); il++)
			{
			//cout << myFile.tellg() << endl;
			myFile.read ((char *) &temp, sizeof(float));
			data[il]=temp;
			//cout << "val: " << temp << endl;
			}

		// close file
		myFile.close();
		}


	void Write_To_Binary_Fortran_File(string filename, float &fortran_binary_header) const
		{
		//cout << sizeof(float) << "\t" << sizeof(float)*size() << endl;


		// fortran binary uses 4 byte floats and not 8 bytes double such as C
		float temp;

		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		mkpath_from_path_with_filenane(filename);

		ofstream myFile (filename.c_str(), ios::binary);
		// write 4 byte header
		myFile.write ((char *) &fortran_binary_header, sizeof(float));

		for (long il=0; il < size(); il++)
			{
			//cout << myFile.tellg() << endl;
			temp=data[il];
			myFile.write ((char *) &temp, sizeof(float));
			//data[il]=temp;
			//cout << "val: " << temp << endl;
			}

		// write 4 byte footer
		myFile.write ((char *) &fortran_binary_header, sizeof(float));

		// close file
		myFile.close();
		}

	// ******************************************************
	// ******************************************************
	// PISANJE Fields v NetCDF fajle z C interfacom
	//

	#ifndef DISABLE_NETCDF4
	void WriteField_compressed(string fname, int compression_level) const
	{

		int ncid, x_dimid, y_dimid, z_dimid, varid1;
		int dimids[3];
		int retval;

		// check the basics
		if (!check_if_initilized())
			error(AT,FU, "!check_if_initilized()");

		// Create the file. The NC_NETCDF4 flag tells netCDF to create a netCDF-4/HDF5 file.
		if ((retval = nc_create(fname.c_str(), NC_CLOBBER|NC_NETCDF4, &ncid)))
			error(AT,FU, "retval = nc_create(fname.c_str(), NC_NOCLOBBER|NC_NETCDF4, &ncid)");

		// Define the dimensions. NetCDF will hand back an ID for each.
		if ((retval = nc_def_dim(ncid, "z", dimz, &z_dimid)))
			error(AT,FU, "retval");
		if ((retval = nc_def_dim(ncid, "y", dimy, &y_dimid)))
			error(AT,FU, "retval");
		if ((retval = nc_def_dim(ncid, "x", dimx, &x_dimid)))
			error(AT,FU, "retval");


		 //The dimids array is used to pass the IDs of the dimensions of the variable.
		dimids[0] = z_dimid;
		dimids[1] = y_dimid;
		dimids[2] = x_dimid;

		//Define the variable. The type of the variable in this case is NC_DOUBLE
		if ((retval = nc_def_var(ncid, "value", NC_DOUBLE, 3, dimids, &varid1)))
			error(AT,FU, "retval");

		double missing=BAD_DATA_FLOAT;

		// Assign units attributes to coordinate variables.
		if ((retval = nc_put_att_double(ncid, varid1, "_FillValue", NC_DOUBLE, 1,&missing)))
			error(AT,FU, "retval");

		// compress precipiation variable
		// compress only if compression_level > 0
		int deflate=0;
		int deflate_level=compression_level;
		if (compression_level > 0) deflate=1;
		nc_def_var_deflate(ncid, varid1, 0, deflate, deflate_level);

		//End define mode. This tells netCDF we are done defining metadata.
		if ((retval = nc_enddef(ncid)))
			error(AT,FU, "retval");

		// Write the data to the file. Although netCDF supports reading and writing subsets of data, in this case we write all the data in one operation.
		if ((retval = nc_put_var_double(ncid, varid1, &data.at(0) )))
			error(AT,FU, "retval");

		// Close the file. This frees up any internal netCDF resources associated with the file, and flushes any buffers.
		if ((retval = nc_close(ncid)))
			error(AT,FU, "retval");
	}
	#endif

	// same as WriteField but also create the necessary path if it does not allready exists
	#ifndef DISABLE_NETCDF4
	void WriteField_compressed_and_create_path(string fname, int compression_level) const
		{
		mkpath_from_path_with_filenane(fname);
		WriteField_compressed(fname, compression_level);
		}
	#endif



	#ifndef DISABLE_NETCDF4
	string C3DField_get_filename_path_from_opened_netcdf_file(const int ncid)
		{
		unsigned long MAX_FILENAME_LENGTH=1024;

		int  status;
		// get filename
		size_t pathlen;;
		status = nc_inq_path (ncid, &pathlen, NULL);
		if (status != NC_NOERR)
			error(AT,FU, "Can't read file path for netcdf file with specified ncid");

		char path[MAX_FILENAME_LENGTH];
		if (pathlen > MAX_FILENAME_LENGTH -1)
			error(AT,FU, "netcdf filename/path too long");

		status = nc_inq_path (ncid, &pathlen, path);
		if (status != NC_NOERR)
			error(AT,FU, "Can't read file path for netcdf file with specified ncid");

		return((string)path);
		}
	#endif


	#ifndef DISABLE_NETCDF4
	void C3DField_inquire_about_netcdf_variable_from_opened_netcdf_file(const int ncid, const string var_name, vector <long> &var_dim_sizes, vector <string> &var_dim_names, long &total_variable_size)
		{
		int  status;

		string fname=C3DField_get_filename_path_from_opened_netcdf_file(ncid);

		// get varible id
		int var_id;
		status = nc_inq_varid (ncid, var_name.c_str(), &var_id);
		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't read "+ var_name +" variable from netcdf file: " + fname);

		// get info about a variable
		nc_type var_type;
		int var_dimids[NC_MAX_VAR_DIMS];
		int var_ndims;
		int var_natts;
		status = nc_inq_var (ncid, var_id, 0, &var_type, &var_ndims, var_dimids, &var_natts);
		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't read "+ var_name +" variable from netcdf file: " + fname);

		char recname[NC_MAX_NAME+1];
		size_t length;

		var_dim_sizes.clear();
		var_dim_names.clear();

		// loop over all variable dimensions
		for (long id=0; id < var_ndims; id++)
			{
			status = nc_inq_dim( ncid, var_dimids[id], recname, &length);
			if (status != NC_NOERR)
				error(AT,FU, (string)"Can't read "+ var_name +" variable from netcdf file: " + fname);

			var_dim_sizes.push_back(length);
			var_dim_names.push_back((string)recname);
			}

		// get total variable size
		total_variable_size=1;
		for (unsigned long id=0; id < var_dim_sizes.size(); id++)
			total_variable_size*=var_dim_sizes[id];
		}
	#endif


	#ifndef DISABLE_NETCDF4
	void C3DField_Read_whole_netcdf_double_variable_from_opened_netcdf_file(const int ncid, const string var_name,  vector <double> &var_data, vector <long> &var_dim_sizes, vector <string> &var_dim_names)
		{
		int  status;

		string fname=C3DField_get_filename_path_from_opened_netcdf_file(ncid);

		// get dimension info about a variable
		long total_variable_size;
		C3DField_inquire_about_netcdf_variable_from_opened_netcdf_file(ncid, var_name , var_dim_sizes, var_dim_names, total_variable_size);

		// get varible id
		int var_id;
		status = nc_inq_varid (ncid, var_name.c_str(), &var_id);
		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't read "+ var_name +" variable from netcdf file: " + fname);

		var_data.assign(total_variable_size,0);
		status = nc_get_var_double(ncid, var_id, &var_data[0]);
		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't read "+ var_name +" variable from netcdf file: " + fname);
		}
	#endif

	#ifndef DISABLE_NETCDF4
	void ReadField(string fname)
		{
		if (!FileExists(fname))
			error(AT,FU, fname + " file does not exist!");

		int status;
		int ncid;

		status = nc_open(fname.c_str(), NC_NOWRITE, &ncid);
		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't open input file: " + fname);

		vector <long> var_dim_sizes;
		vector <string> var_dim_names;

		C3DField_Read_whole_netcdf_double_variable_from_opened_netcdf_file( ncid, "value",  data, var_dim_sizes, var_dim_names);

		//cout <<  output_vector_as_string_with_index_and_newline(var_dim_names);

		if (var_dim_sizes.size() != 3 || var_dim_names[0] != "z" || var_dim_names[1] != "y" || var_dim_names[2] != "x")
			error(AT,FU, "Error openning input file: " + fname + ". The variable 'value' does not have dimensions z,y,x!! ");

		dimx=var_dim_sizes[2];
		dimy=var_dim_sizes[1];
		dimz=var_dim_sizes[0];

		status = nc_close(ncid);

		if (status != NC_NOERR)
			error(AT,FU, (string)"Can't close input file: " + fname);
		}
	#endif

	// the fields should be separated by ','. Newlines are ignored. The fastest changing dimenstion is x then y and then z
	void ReadField_from_CSV_file(string filename, int dimx_, int dimy_, int dimz_)
		{
		if (!FileExists(filename))
			error(AT,FU, filename + " file does not exist!");

		allocatememory(dimx_,dimy_,dimz_);

		ifstream file ( filename.c_str() ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		string value;
		long counter=0;
		while ( file.good() )
			{
		    getline ( file, value, ',' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		    // remove new line charcters
			value.erase( std::remove(value.begin(), value.end(), '\r'), value.end() );
			value.erase( std::remove(value.begin(), value.end(), '\n'), value.end() );
		    // ignore empty lines
			if (value != "")
				{
				data[counter]=atof(value.c_str());
				counter++;
		    	//cout << counter << "\t" << value << endl ; // display value removing the first and the last character from it
				}
			if (counter > size())
				error(AT,FU, "number of numbers in the ascii CSV file does not corespond to the number of elements of the field. There is probably something wrong with the input CSV file or the wrong dimensions are set in the namelist.");
			}
		if (counter != size())
			error(AT,FU, "number of numbers in the ascii CSV file does not corespond to the number of elements of the field. There is probably something wrong with the input CSV file or the wrong dimensions are set in the namelist.");
		}


	// Writes Field to CSV file
	void WriteField_to_CSV_file(string filename) const
		{
		ostringstream s2;

		if (!check_if_initilized())
			error(AT,FU, "f.check_if_initilized()");

		mkpath_from_path_with_filenane(filename);


		s2.str("");
		for (long iz=0; iz < dimz; iz++)
			{
			for (long iy=0; iy < dimy; iy++)
				{
				for (long ix=0; ix < dimx; ix++)
					{
					s2 << get(ix,iy,iz) << ",";
					}
				s2 << endl;
				}
			//s2 << endl;
			}
		write_string_to_new_file_and_create_dir_if_necessary(filename, s2.str());
		}

	// this function draws the pixel with gd and you dont have to worry about assigning new colors
	// the figure is inverted in y dimension - so it is the same as in pngwriter
	// r,g,b components should be in the range 0-1
	void gd_draw_pixel(gdImagePtr image, int x, int y, double r, double g, double b)  const
		{
		int color;
		color=gdImageColorExact(image,(int)(r*255.),(int)(g*255.),(int)(b*255.));
		// if color not defined previously define it now
		if (color == -1)
			color=gdImageColorAllocate(image, (int)(r*255.),(int)(g*255.),(int)(b*255.));
		gdImageSetPixel( image , x, image->sy - y - 1, color );
		}

	// write gd image to jpg or png file
	void Write_gd_image_to_file(gdImagePtr image, string filename, string file_type)  const
		{
		FILE *out;
		out = fopen(filename.c_str(), "wb");

		if (file_type == "jpg")
			gdImageJpeg(image, out, 80);

		if (file_type == "png")
			gdImagePng(image, out);

		fclose(out);
		}

	// write gd image to jpg or png file
	void Write_gd_image_to_png(gdImagePtr image, string filename)  const
		{
		FILE *out;
		out = fopen(filename.c_str(), "wb");
		gdImagePng(image, out);
		fclose(out);
		}

	void Write_y_inverted_gd_image_to_file(gdImagePtr image, string filename, string file_type)  const
		{
		int ix, iy, c;
		gdImagePtr im_out;
		im_out = gdImageCreateTrueColor(image->sx, image->sy);
		for (ix=0; ix < image->sx; ix++)
			for (iy=0; iy < image->sy; iy++)
				{
				c = gdImageGetPixel(image,ix, iy);
				gdImageSetPixel  ( im_out , ix, image->sy - iy -1, c );
				}
		Write_gd_image_to_file(im_out, filename, file_type);
		gdImageDestroy(im_out);
		}

};
