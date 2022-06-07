
extern fn Print(int num): void;

fn Factorial(int num) : int {
	if (num == 0) {
		return 1;
	} else {
		return num * Factorial(num - 1);
	}
}

fn Main(int argc): int{
		
	int i = 0;
	while(i < 5){
		Print(Factorial(i));
		i++;
	}
	return 0;
}
