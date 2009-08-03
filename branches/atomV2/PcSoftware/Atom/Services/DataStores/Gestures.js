{ gestures: [
	{
		name : 'INSERT', 
		description : 'V-shape', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'M', 
			f4 : 'M', 
			f5 : 'P', 
			f6 : 'U', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 1, 
			f4 : 1, 
			f5 : 1, 
			f6 : 0, 
			f7_lower : 0,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'JOIN', 
		description : 'Negative horizontal arc', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'P', 
			f4 : 'M', 
			f5 : 'P', 
			f6 : 'U', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 1, 
			f4 : 1, 
			f5 : 1, 
			f6 : 0, 
			f7_lower : 0,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1
		}
	},
	{
		name : 'INSERT_LINE', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'U', 
			f4 : 'P', 
			f5 : 'P', 
			f6 : 'M', 
			f8 : 'M', 
			f9 : 'P'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 0, 
			f4 : 1, 
			f5 : 1, 
			f6 : 1, 
			f7_lower : 0,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'UNDO', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'P', 
			f4 : 'P', 
			f5 : 'M', 
			f6 : 'M', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 9, 
			f2 : 0, 
			f3 : 1, 
			f4 : 1, 
			f5 : 1, 
			f6 : 0, 
			f7_lower : 1,
			f7_upper : 1, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'DELETE', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'U', 
			f4 : 'M', 
			f5 : 'P', 
			f6 : 'M', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 1,
			f1_upper : 9, 
			f2 : 0, 
			f3 : 0,
			f4 : 1, 
			f5 : 1, 
			f6 : 1, 
			f7_lower : 5,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'SWAP', 
		description : '', 
		functions : 
		{
			f2 : 'M', 
			f3 : 'U', 
			f4 : 'M', 
			f5 : 'P', 
			f6 : 'M', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 1,
			f1_upper : 1, 
			f2 : 1, 
			f3 : 0,
			f4 : 0, 
			f5 : 0, 
			f6 : 0, 
			f7_lower : 2,
			f7_upper : 3, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'SPACE', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'U', 
			f4 : 'U', 
			f5 : 'U', 
			f6 : 'M', 
			f8 : 'U', 
			f9 : 'U'
		}, 
		masks :
		{
			f1_lower : 1,
			f1_upper : 9, 
			f2 : 0, 
			f3 : 0,
			f4 : 0, 
			f5 : 0, 
			f6 : 1, 
			f7_lower : 1,
			f7_upper : 1, 
			f8 : 0, 
			f9 : 0 
		}
	},
	{
		name : 'NEW_LINE', 
		description : '', 
		functions : 
		{
			f2 : 'P', 
			f3 : 'U', 
			f4 : 'M', 
			f5 : 'P', 
			f6 : 'P', 
			f8 : 'P', 
			f9 : 'M'
		}, 
		masks :
		{
			f1_lower : 1,
			f1_upper : 1, 
			f2 : 0, 
			f3 : 0,
			f4 : 0, 
			f5 : 0, 
			f6 : 1, 
			f7_lower : 1,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'SELECT', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'M', 
			f4 : 'U', 
			f5 : 'U', 
			f6 : 'U', 
			f8 : 'M', 
			f9 : 'U'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 1,
			f4 : 1, 
			f5 : 0, 
			f6 : 0, 
			f7_lower : 0,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 0 
		}
	},
	{
		name : 'COPY', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'U', 
			f4 : 'M', 
			f5 : 'M', 
			f6 : 'M', 
			f8 : 'M', 
			f9 : 'P'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 0,
			f4 : 1, 
			f5 : 1, 
			f6 : 0, 
			f7_lower : 0,
			f7_upper : 9, 
			f8 : 1, 
			f9 : 1 
		}
	},
	{
		name : 'PASTE', 
		description : '', 
		functions : 
		{
			f2 : 'U', 
			f3 : 'U', 
			f4 : 'U', 
			f5 : 'P', 
			f6 : 'P', 
			f8 : 'M', 
			f9 : 'P'
		}, 
		masks :
		{
			f1_lower : 0,
			f1_upper : 0, 
			f2 : 0, 
			f3 : 0,
			f4 : 0, 
			f5 : 1, 
			f6 : 1, 
			f7_lower : 4,
			f7_upper : 4, 
			f8 : 1, 
			f9 : 1 
		}
	}
]}
