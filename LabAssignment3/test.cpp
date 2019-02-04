#include <iostream>
#include <utility>

using std::cout;
using std::endl;
using std::swap;

int main()
{
    double a[10] = { 0 };
    double b[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    
    for (int i = 0; i < 10; i++)
        cout << a[i] << " | " << b[i] << endl;
    
    swap(a, b); 
    
    for (int i = 0; i < 10; i++)
        cout << a[i] << " | " << b[i] << endl;
    
    return 0;
}
