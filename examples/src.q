
extern fn Print(int num): void;
extern fn Print(string value): void;
extern fn Print(bool value): void;

fn StringTest() :void {
	string str = "Hello World\n";
	Print(str);
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
		Print("Factorial of ");
		Print(i);
		Print(" is ");
		Print(Factorial(i));
		Print("\n");
		i++;
	}
	return 0;
}
