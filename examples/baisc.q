

fn Sum(int a, int b): int 
	return a + b;

extern fn Println(int number): void;

fn Main(): int {
	int n = 3 * (4 + 2) + 4 * 4 + 3;

	Println(n);

	return 0;
}