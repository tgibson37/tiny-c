
bar(int i) { // returns i^2
 return(i*i);
 }

foo() {return(31);}

main () { // 6/4/18
 int w;w=foo();
 printf ("%d",w);
 printf ("%c%d",10,3+5);
 printf ("%c%d%c",10,strlen("I am a string"),10);
 printf ("%d",foo());
 printf ("%ctype a small number ",10);
 int n;
 scanf("%d", &n);
 printf ("%d%c",bar(n),10);
 }

