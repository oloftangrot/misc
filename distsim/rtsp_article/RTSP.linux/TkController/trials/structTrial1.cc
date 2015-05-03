struct a {
	int size;
	char const * const c[3];
} s = {
	3, 
	{ "test1", "test2","test3" }
};

struct b {
	int size;
	int val;
	struct bb {
		char const * const c1;
		char const * const c2;
	} c[3];
} ss = {
	3, 
	4,
	{
		{ "test11", "test12" },
		{ "test21", "test22" },
		{ "test31", "test32" }
	}
};

