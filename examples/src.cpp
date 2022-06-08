
#include <cstdint>
#include <cstdio>

using s64 = std::int64_t;

void Println(s64 num){
	printf("%lld\n", num);	
}

void Println(s64 num);
void StringTest();
s64 Factorial(s64 num);
s64 Main();
void StringTest(){
const char * str = "Hello World\n";
}
s64 Factorial(s64 num){
if(num==0)
{
return 1;
}
else
{
return num*Factorial(num-1);
}
}
s64 Main(){
bool shit = false;
bool shit2 = true;
s64 i = 0;
while(i<5)
{
Println(Factorial(i));
((i)++);
}
return 0;
}

int main(int argc, char **argv){
	return Main();
}
