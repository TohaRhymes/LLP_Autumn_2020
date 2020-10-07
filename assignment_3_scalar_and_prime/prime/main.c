#include <stdio.h>

int is_prime(unsigned long n) {
   
    if (n <= 1UL) 
        return 0; 
    if (n == 2UL)
        return 1;

    for (unsigned long i = 2UL; i * i <= n; ++i)
        if (!(n % i))
            return 0;

    return 1;
}

int main() {
    while (1){
        unsigned long n = 0UL;

        while (!scanf("%lu", &n));
        
        if (is_prime(n))
            printf("%lu is a prime number!\n", n);
        else
            printf("%lu isn't a prime number!\n", n);

    }
}
