
extern fn Println(int num): void;

fn StringTest() :void {
	string str = "Hello World";
}

fn Factorial(int num) : int {
	if (num == 0) {
		return 1;
	} else {
		return num * Factorial(num - 1);
	}
}

fn Main(): int{
	bool shit = false;
	bool shit2 = true;

	int i = 0;
	while(i < 5){
		Println(Factorial(i));
		i++;
	}
	return 0;
}
