extern fn Print(int num): void;



fn Main(int argc): int{
	
	while (3 + 4) {
		Print(4);
	}

	if (1) {
		Print(3);
	}

	if (3) {
		Print(3);
	} else {
		if(4){
			Print(3 - 1);
		}
	}
	
	return 0;
}
