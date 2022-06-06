
extern fn Print(int num): void;

fn Variables(): void{
    int n = 3;
    int d = n;
    {
        int v = 2228;
    }
}

fn UnaryOps(): void{
    int ini = 3;
    int np = ini++;
    int nm = ini--;
    int pn = ((++ini));
    int mn = --ini;
    int inv = (-ini);
}

fn Main(int argc): int{

}
