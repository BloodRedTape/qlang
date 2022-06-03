
extern fn Println(int number): void;

fn Factorial(int num): int{
    if(num == 0)
        return 1;
    else
        return Factorial(num - 1) * num;
}

fn Main(): int{
    int[5] table;

    for(int i = 0; i<5; i++)
        table[i] = Factorial(i);

    for(int i = 0; i<5; i++)
        Println(table[i]);

    return 0;
}