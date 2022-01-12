int fact(int a)
{
	if(a <= 1) return 1;
	return a*fact(a-1);
}



int main(){
    int a;

    a = fact(5);
    println(a);


}
