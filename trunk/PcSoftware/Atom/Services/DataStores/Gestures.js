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
			f9 : 'P', 
		}
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
			f9 : 0, 
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
			f9 : 'P', 
		}
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
			f8 : 0, 
			f9 : 1, 
		}
	},
]}
