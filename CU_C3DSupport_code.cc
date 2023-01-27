

class COverlap
	{
	public:
	double id;
	double overlap_size;

	COverlap() {freememory();}
	~COverlap() {freememory();}

	void freememory()
		{
		id=-1;
		overlap_size=0;
		}

    #ifndef DISABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
		{
		ar & overlap_size;
		ar & id;
		}
	#endif


	COverlap& operator= (const COverlap &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			id=rhs.id;
			overlap_size=rhs.overlap_size;
			}
		return *this;    // Return ref for multiple assignment
		}

	// less_than_equal_according_to_id
	bool operator<(const COverlap &rhs) const
		{
		if (id < rhs.id) return(true);
		return(false);
		}

	string output() const
		{
		ostringstream s1;
		s1.str("");
		s1 << id << ":" << overlap_size ;
		return(s1.str());
		}


	};
class COverlap_list
	{
	public:
	vector <COverlap> overlap;

	COverlap_list() {freememory();}
	~COverlap_list() {freememory();}

	void freememory()
		{
		overlap.clear();
		}

    #ifndef DISABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
		{
		ar & overlap;
		}
	#endif

	COverlap_list& operator= (const COverlap_list &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			overlap=rhs.overlap;
			}
		return *this;    // Return ref for multiple assignment
		}

	void add_overlap(double id)
		{
 		bool found=false;

 		vector<COverlap>::iterator low;

		// check if allready here
 		if (overlap.size() > 0)
 		// check if the last element - since it is allready sorted it will have the biggest id - has bigger than current id - otherwise the lower bound will return iterator after the end of the vector and we will have an out of memory bounds read
 		//if (overlap.back().id >= id)
 			{
			// we need to use a object to do the lower_bound search
			COverlap a;
			a.id=id;

	 		low=lower_bound_which_never_returns_an_out_of_bounds_result(overlap.begin(), overlap.end(),a);
			if (low->id==id)
				found=true;
			}

 		// new object
		if (found==false)
 			{
			// add
			COverlap b;
			b.id=id;
			b.overlap_size=1;
			overlap.push_back(b);
			// sort by id - so that we can later use the very fast binary_search
			sort(overlap.begin(), overlap.end());
			}

 		// old object
		else
			low->overlap_size++;
		}

	bool get_object_with_biggest_overlap_size(COverlap &obj) const
		{
		double max=0;
		for (long il=0; il < (long)overlap.size(); il++)
			if (overlap[il].overlap_size > max)
				{
				obj=overlap[il];
				max=overlap[il].overlap_size;
				}
		if (max > 0) return(true);
		return(false);
		}

	string output() const
		{
		ostringstream s1;
		s1.str("");
		s1 << overlap.size() << ":\t";
		for (long il=0; il < (long)overlap.size(); il++ )
			s1 << overlap[il].output() << ", ";
		//s1 << endl;
		return(s1.str());
		}
	};


template <class  T>
class CPoint3Dxyz
	{
	public:
	T x, y, z;
	double a;

	// constructor
	CPoint3Dxyz() {freememory();}
	~CPoint3Dxyz() {freememory();}

	// frees memory
	void freememory()
		{
		x=y=z=BAD_DATA_FLOAT;
		}

	CPoint3Dxyz& operator= (const CPoint3Dxyz &rhs)
		{
		if (this != &rhs) // make sure not same object
			{
			//freememory();                     // Delete old name's memory.
			//if (!rhs.check_if_initilized())
			//	error(AT,FU, "!rhs.check_if_initilized()");
			x=rhs.x;
			y=rhs.y;
			z=rhs.z;
			a=rhs.a;
			}
		return *this;    // Return ref for multiple assignment
		}

	CPoint3Dxyz deep_copy()
		{
		CPoint3Dxyz p;
		p=*this;
		return(p);
		}

	void set(T _x, T _y, T _z)
		{
		x=_x;
		y=_y;
		z=_z;
		}

	CPoint3Dxyz create(T x, T y, T z)
		{
		CPoint3Dxyz p;
		p.set(x,y,z);
		return(p);
		}
	string output()
		{
		ostringstream s1;
		s1.str("");
		s1 << x << "\t" << y << "\t" << z << "\t" << a ;
		return(s1.str());
		}
	};
