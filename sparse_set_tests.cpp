// sparse_array.cpp : Defines the entry point for the console application.
//
// <author>Mikhail Semenov</author>
// <date>2015-01-03</date>
// <summary>Contains Sparse Tests</summary>

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <limits>
#include <cmath>

#include <chrono>
#include <random>
#include <iomanip>
#include <set>
#include <bitset>
#include <algorithm>
#include <unordered_map>


using namespace std::chrono;
std::default_random_engine generator;
std::uniform_int_distribution<unsigned> uint_distribution(0, 4294967295);

unsigned random_uint() // between 0 and 4294967295
{ /* Generates numbers between 0 and 1. */
    return uint_distribution(generator);
}

void reset_random_uint()
{
    uint_distribution.reset();
    generator.seed(0x11111111);
}

typedef std::chrono::high_resolution_clock clk;
typedef duration<double, std::ratio<1, 1000>>    time_in_msec;


//#define USE_BOOST

#ifdef USE_BOOST
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#endif

const unsigned steps = 100000000;
const unsigned repeat = 1;

#include "sparse_sets.h"

int Eratosthenes_Bounded_Set(unsigned n)
{
    bounded_set test_set(n + 1);

    test_set.insert(2);

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set.insert(i);
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set.test(i))
        {
            for (unsigned j = i + i; j <= n; j += i)
            {                
                test_set.erase(j);                
            }
        }
    }
    
    return test_set.count();
}

int Eratosthenes_Unordered_Sparse_Set(unsigned n)
{
    unordered_sparse_set test_set(n + 1);

    test_set.insert(2);

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set.insert(i);
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set.test(i))
        {
            for (unsigned j = i + i; j <= n; j += i)
            {
                test_set.erase(j);
            }
        }
    }

    return test_set.count();
}

#include <set>
int Eratosthenes_Set(unsigned n)
{
    std::set<unsigned> test_set;

    test_set.insert(2);

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set.insert(i);
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set.find(i) != test_set.end())
        {
            for (unsigned j = i + i; j <= n; j += i)
            {
                test_set.erase(j);
            }
        }
    }

    return test_set.size();
}



int Eratosthenes_Sparse_Set(unsigned n)
{
    sparse_set test_set(n + 1);

    test_set.insert(2);

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set.insert(i);
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set.test(i))
        {
            for (unsigned j = i + i; j <= n; j += i)
            {               
                test_set.erase(j);                
            }
        }
    }

    return test_set.count();
}

int Eratosthenes_Bool_Vector(unsigned n)
{
    std::vector<bool> test_set(n + 1);

    test_set[2] = true;

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set[i] = true;
    }    

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set[i])
        {
            for (unsigned j = i + i; j <= n; j += i)
            {                
                test_set[j] = false;                
            }
        }
    }

    int counter = 0;
    for (auto x: test_set)
    {
        if (x)
        {
            counter++;
        }
    }
    return counter;
}

int Eratosthenes_Char_Vector(unsigned n)
{
    std::vector<char> test_set(n + 1);

    test_set[2] = 'T';

    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set[i] = 'T';
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set[i] == 'T')
        {
            for (unsigned j = i + i; j <= n; j += i)
            {              
                test_set[j] = '\0';                
            }
        }
    }

    int counter = 0;
    for (auto x: test_set)
    {
        if (x == 'T')
        {
            counter++;
        }
    }
    return counter;
}


#include <bitset>

template <unsigned n>
unsigned Eratosthenes_Bitset()
{
    std::bitset<n+1> test_set;

    test_set[2] = true;
    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set[i] = true;
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set[i])
        {
            for (unsigned j = i + i; j <= n; j += i)
            {
                test_set[j] = false;
            }
        }
    }
    
    return test_set.count();
}

template <unsigned k, unsigned n>
struct Test_Eratosthenes_Bitset_Impl
{
    static unsigned Run()
    {
        return Eratosthenes_Bitset<k + n>() + Test_Eratosthenes_Bitset_Impl<k,n-1>::Run();
    }
};

template <unsigned k>
struct Test_Eratosthenes_Bitset_Impl<k,0>
{
    static unsigned Run()
    {
        return Eratosthenes_Bitset<k>();
    }
};

template <unsigned k>
void Test_Eratosthenes_Bitset()
{    
    static const unsigned iterations = 200;
    std::cout << "________________________________________________________________________" << std::endl;
    std::cout << "Test Eratosthenes sieve. Range [" << k << ", " << (k + iterations - 1) << "]" << std::endl;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    unsigned counter = Test_Eratosthenes_Bitset_Impl<k, iterations-1>::Run();    

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Bitset. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;           
}


#ifdef USE_BOOST
int Eratosthenes_Boost_Dynamic_Bitset(unsigned n)
{
    boost::dynamic_bitset<std::size_t> test_set(n + 1);

    test_set[2] = true;
    for (unsigned i = 3; i <= n; i += 2)
    {
        test_set[i] = true;
    }

    for (unsigned i = 3; i*i <= n; i += 2)
    {
        if (test_set[i])
        {
            for (unsigned j = i + i; j <= n; j += i)
            {
                test_set[j] = false;
            }
        }
    }
    
    return test_set.count();
}
#endif



//double f(unsigned n)
//{
//    return n*(double)n;// 1000.0 * sin(n);
//}

#define f(n) n



void Test_Bitset(const unordered_sparse_set& values)
{
    std::size_t length = values.size();
    std::size_t selection = values.count();

    if (values.size() > 1000000)
        return;
    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "bitset. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;

    //std::vector<unsigned> v(n);
    clk::time_point t1 = high_resolution_clock::now();
    //std::vector<bool> vx(n);
    std::bitset<1000000> vx;
        
    for (auto x: values)
    {       
        vx[x] = true;        
    };

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "bitset. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;
    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (vx[k1])
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "bitset random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();
    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (unsigned p = 0; p < length; p++)
        {
            if (vx[p])
            {
                double coeff = random_uint();
                sum += p * coeff;
                counter2++;
            }
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "bitset summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;


    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;
    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        vx[k1] = false;
    }
    */

    for (auto x : values)
    {
        vx[x] = false;
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    counter = vx.count();

    std::cout << "counter: " << counter << std::endl;
    std::cout << "bitset random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}

void Test_Set(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();
    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "Set. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;
       
    clk::time_point t1 = high_resolution_clock::now();

    std::set<unsigned> vx;
    for (auto x : values)
    {
        vx.insert(x);
    };
    
    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "Set. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;

    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (vx.find(k1) != vx.end())
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "Set random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();
    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (auto p: vx)
        {
            double coeff = random_uint();
            sum += p * coeff;
            counter2++;
        }        
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "Set summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;       

    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;

    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        vx.erase(k1);
        if (vx.empty())
            break;
    }
    */

    for (auto x : values)
    {
        vx.erase(x);
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    counter = vx.size();

    std::cout << "counter: " << counter << std::endl;
    std::cout << "Set random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}


void Test_Vector_of_Bool(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "vector<bool>. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;

    reset_random_uint();
    
    reset_random_uint();    
    clk::time_point t1 = high_resolution_clock::now();
    
    std::vector<bool> vx(length);

    for (auto x : values)
    {
        vx[x] = true;
    }
        
    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "vector<bool>. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (vx[k1])
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "vector<bool> random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();
    sum = 0;
    unsigned long long counter2 = 0;


    for (unsigned k = 0; k < repeat; k++)
    {
        unsigned p = 0;
        for (std::vector<bool>::const_iterator it = vx.cbegin(), itStop = vx.cend(); it != itStop; ++it)
        {
            if (*it)
            {
                double coeff = random_uint();
                sum += p * coeff;
                counter2++;
            }
            p++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "vector<bool> summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;

    //unsigned total_elements = counter2 / repeat;
    //double density = total_elements / (double)length * 100.0;
    //std::cout << " length: " << length << " total elements:" << total_elements << " Density: " << density << "%" << std::endl;



    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;
    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        vx[k1] = false;
        bool empty = true;
        for (auto x : vx)
        {
            if (x)
            {
                empty = false;
                break;
            }
        }
        if (empty)
            break;
    }
    */

    for (auto x : values)
    {
        vx[x] = false;
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    
    for (auto x: vx)
    {
        if (x)
        {            
            counter++;
        }        
    }

    std::cout << "counter: " << counter << std::endl;
    std::cout << "vector<bool> random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}



void Test_Vector_of_Char(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "vector<char>. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;

    clk::time_point t1 = high_resolution_clock::now();    
    std::vector<char> vx(length);

    for (auto x : values)
    {
        vx[x] = 'T';
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "vector<char>. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (vx[k1] == 'T')
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "vector<bool> random access. It took " << time_span.count() << " milliseconds." << std::endl;


    reset_random_uint();
    t1 = high_resolution_clock::now();
    sum = 0;
    unsigned long long counter2 = 0;


    for (unsigned k = 0; k < repeat; k++)
    {
        unsigned p = 0;
        for (std::vector<char>::const_iterator it = vx.cbegin(), itStop = vx.cend(); it != itStop; ++it)
        {
            if (*it == 'T')
            {
                double coeff = random_uint();
                sum += p * coeff;
                counter2++;
            }
            p++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "vector<char> summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;
    
    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;


    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        vx[k1] = '\0';
        bool empty = true;
        for (auto x : vx)
        {
            if (x == 'T')
            {
                empty = false;
                break;
            }
        }
        if (empty)
            break;
    }
    */

    for (auto x : values)
    {
        vx[x] = '\0';
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    
    for (auto x: vx)
    {
        if (x == 'T')
        {            
            counter++;
        }        
    }

    std::cout << "counter: " << counter << std::endl;
    std::cout << "vector<char> random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}

void Test_Bounded_Set(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "BOUNDED SET. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;
    reset_random_uint();


    clk::time_point t1 = high_resolution_clock::now();
    bounded_set test_set(length);
    
    for (auto x : values)
    {
        test_set.insert(x);
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Bounded set. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (test_set.test(k1))
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "bounded set random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();

    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (bounded_set::const_iterator it = test_set.begin(), itStop = test_set.end(); it != itStop; ++it)
        {
            double coeff = random_uint();
            sum += *it * coeff;//f(*it);
            counter2++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;

    timing = time_span.count();
    std::cout << "Bounded set summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;


    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        test_set.erase(k1);
        if (test_set.empty())
            break;
    }
    */

    for (auto x : values)
    {
        test_set.erase(x);
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    counter = test_set.count();

    std::cout << "counter: " << counter << std::endl;
    std::cout << "bounded set random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}


void Test_Unordered_Sparse_Set(const unordered_sparse_set& values)
{
    //std::cout << "start test" << std::endl;
    const unsigned length = values.size();
    //std::cout << "after length" << std::endl;
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "UNORDERED SPARSE SET. length:" << length << " selection: " << selection << " density: " << (selection/(double)length * 100.0) << "%" << std::endl;
    reset_random_uint();


    clk::time_point t1 = high_resolution_clock::now();
    unordered_sparse_set test_set(length);
    //std::cout << "unordered sparse set created" << std::endl;

    for (auto x : values)
    {
        test_set.insert(x);
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Unordered Sparse set. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (test_set.test(k1))
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "Unordered Sparse Set random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();

    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (unordered_sparse_set::iterator it = test_set.begin(), itStop = test_set.end(); it != itStop; ++it)
        {
            double coeff = random_uint();
            sum += *it * coeff;//f(*it);
            counter2++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;

    timing = time_span.count();
    std::cout << "Unordered Sparse Set summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;


    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        test_set.erase(k1);
        if (test_set.empty())
            break;
    }
    */
    for (auto x : values)
    {
        //unsigned k1 = random_uint() % length;
        test_set.erase(x);
        //if (test_set.empty())
        //    break;
    }


    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    counter = test_set.count();

    std::cout << "counter: " << counter << std::endl;
    std::cout << "Unordered Sparse Set random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}

#ifdef USE_BOOST
void Test_Boost_Dynamic_Bitset(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "BOOST DYNAMIC BITSET. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;
    reset_random_uint();


    clk::time_point t1 = high_resolution_clock::now();
    boost::dynamic_bitset<std::size_t> test_set(length);

    for (auto x : values)
    {
        test_set[x] = true;
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Boost Dynamic Bitset. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (test_set.test(k1))
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "Boost Dynamic Bitset random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();

    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        std::size_t bit = test_set.find_first();
        while (bit != test_set.npos)
        {
            double coeff = random_uint();
            sum += bit * coeff;//f(*it);
            counter2++;
            bit = test_set.find_next(bit);
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;

    timing = time_span.count();
    std::cout << "Boost Dynamic Bitset summation. It took " << (time_span.count()/repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;

    //unsigned total_elements = counter2 / repeat;
    //double density = total_elements / (double)length * 100.0;
    //std::cout << " length: " << length << " total elements:" << total_elements << " Density: " << density << "%" << std::endl;
    //std::cout << " random access: units*density: " << (units * density) << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;


    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        test_set[k1] = false;
        std::size_t bit = test_set.find_first();
        if (bit == test_set.npos)
            break;
    }
    */

    for (auto x : values)
    {
        //unsigned k1 = random_uint() % length;
        test_set[x] = false;
        //if (test_set.empty())
        //    break;
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    std::size_t bit = test_set.find_first();
    counter = 0;
    while (bit != test_set.npos)
    {        
        counter++;
        bit = test_set.find_next(bit);
    }

    std::cout << "counter: " << counter << std::endl;
    std::cout << "Boost Dynamic Bitset random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}
#endif

void Test_Sparse_Set(const unordered_sparse_set& values)
{
    const unsigned length = values.size();
    const unsigned selection = values.count();

    std::cout << "_____________________________________________________" << std::endl;
    std::cout << "Sparse Set. length:" << length << " selection: " << selection << " density: " << (selection / (double)length * 100.0) << "%" << std::endl;
    reset_random_uint();
    clk::time_point t1 = high_resolution_clock::now();
    sparse_set test_set(length);

    for (auto x : values)
    {
        test_set.insert(x);
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Sparse Set. Generation. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();
    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;    

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        if (test_set.test(k1))
        {
            //sum += f(k1);
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    //double units = (time_span.count() / (double)counter)*10000.0;
    std::cout << "counter: " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << "Sparse Set random access. It took " << time_span.count() << " milliseconds." << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();

    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (sparse_set::const_iterator it = test_set.begin(), itStop = test_set.end(); it != itStop; ++it)
        {
            double coeff = random_uint();
            sum += *it * coeff;
            counter2++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << "counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;
    timing = time_span.count();
    std::cout << "Sparse Set summation. It took " << (time_span.count() / repeat) << " milliseconds." << std::endl;
    std::cout << "per element (in one iteration)" << 1000000.0*time_span.count() / (double)counter2 << " nanoseconds." << std::endl;
    
    //unsigned total_elements = counter2 / repeat;
    //double density = total_elements / (double)length * 100.0;
    //std::cout << " length: " << length << " total elements:" << total_elements << " Density: " << density << "%" << std::endl;
    //std::cout << " random access: units*density: " << (units * density) << std::endl;

    reset_random_uint();

    t1 = high_resolution_clock::now();


    sum = 0;
    counter = 0;

    /*
    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % length;
        test_set.erase(k1);
        if (test_set.empty())
            break;
    }
    */

    for (auto x : values)
    {        
        test_set.erase(x);     
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    counter = test_set.count();

    std::cout << "counter: " << counter << std::endl;
    std::cout << "Sparse Set random deletion. It took " << time_span.count() << " milliseconds." << std::endl;
    reset_random_uint();
}


void Test_Eratosthenes(unsigned n)
{
    unsigned iterations = 200;
    std::cout << "________________________________________________________________________" << std::endl;
    std::cout << "Test Eratosthenes sieve. Range [" << n << ", " << (n + iterations - 1) << "]" << std::endl;

    clk::time_point t1 = high_resolution_clock::now();

    unsigned counter = 0;
    
    for (unsigned i = 0; i < iterations; i++)
    {
        counter += Eratosthenes_Bounded_Set(n+i);
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << "Bounded bitset. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;        

    t1 = high_resolution_clock::now();

    counter = 0;
    for (unsigned i = 0; i < iterations; i++)
    {
        counter += Eratosthenes_Bool_Vector(n + i);
    }
    
    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    timing = time_span.count();
    std::cout << "Vector of bool. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;


    t1 = high_resolution_clock::now();
    counter = 0;
    for (unsigned i = 0; i < iterations; i++)
    {
        counter += Eratosthenes_Sparse_Set(n + i);
    }    

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    timing = time_span.count();
    std::cout << "Sparse Set. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;

#ifdef USE_BOOST
    t1 = high_resolution_clock::now();
    counter = 0;
    for (unsigned i = 0; i < iterations; i++)
    {
        counter += Eratosthenes_Boost_Dynamic_Bitset(n + i);
    }    

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    timing = time_span.count();
    std::cout << "Boost Dynamic Bitset. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;
#endif

    if (n <= 10000000)
    {
        t1 = high_resolution_clock::now();
        counter = 0;
        for (unsigned i = 0; i < iterations; i++)
        {
            counter += Eratosthenes_Set(n + i);
        }        

        t2 = clk::now();
        time_span = duration_cast<time_in_msec>(t2 - t1);

        timing = time_span.count();
        std::cout << "Set. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;
    }

    if (n <= 100000000)
    {
        t1 = high_resolution_clock::now();
        counter = 0;
        for (unsigned i = 0; i < iterations; i++)
        {
            counter += Eratosthenes_Char_Vector(n + i);
        }        

        t2 = clk::now();
        time_span = duration_cast<time_in_msec>(t2 - t1);

        timing = time_span.count();
        std::cout << "Vector of char. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;

        t1 = high_resolution_clock::now();
        counter = 0;
        for (unsigned i = 0; i < iterations; i++)
        {
            counter += Eratosthenes_Unordered_Sparse_Set(n + i);
        }        

        t2 = clk::now();
        time_span = duration_cast<time_in_msec>(t2 - t1);

        timing = time_span.count();
        std::cout << "Unordered sparse set. Counter: " << counter << " took " << time_span.count() << " milliseconds." << std::endl;
    }
}


int main()
{   
    bounded_set x(100);
    x.insert(5);
    x.insert(20);
    x.insert(21);
    x.insert(25);
    x.insert(30);
    x.insert(31);
    x.insert(35);

    bounded_set::iterator itStop = x.upper_bound(30);
    for (bounded_set::iterator it = x.lower_bound(21); it != itStop; ++it)
    {
        std::cout << *it << std::endl;
    }

    std::set<int> y;
    y.insert(5);
    y.insert(20);
    y.insert(21);
    y.insert(25);
    y.insert(30);
    y.insert(31);
    y.insert(35);

    std::set<int>::iterator itStop2 = y.upper_bound(30);
    for (std::set<int>::iterator it2 = y.lower_bound(21); it2 != itStop2; ++it2)
    {
        std::cout << *it2 << std::endl;
    }

    Test_Eratosthenes_Bitset<1000>();
    Test_Eratosthenes_Bitset<10000>();
    Test_Eratosthenes_Bitset<100000>();
    //Test_Eratosthenes_Bitset<1000000>();
    //Test_Eratosthenes_Bitset<5000000>();
    
    Test_Eratosthenes(1000);
    Test_Eratosthenes(10000);
    Test_Eratosthenes(100000);
    //Test_Eratosthenes(1000000);
    //Test_Eratosthenes(5000000);
    //Test_Eratosthenes(10000000);
    //Test_Eratosthenes(50000000);
       

    unsigned lengths[] = { 100000, 1000000, 10000000, 50000000 };// , 50000000};
    unsigned selections[] = { 100000 };

    unordered_sparse_set values;
    for (auto length : lengths)
    {
        values.resize(length);
        for (auto selection : selections)
        {
            values.clear();
            reset_random_uint();
            unsigned select_counter = 0;
            do
            {
                unsigned k1 = random_uint() % length;
                if (values.insert(k1))
                {
                    select_counter++;
                }
            } while (select_counter != selection);

            Test_Unordered_Sparse_Set(values);
#ifdef USE_BOOST
            Test_Boost_Dynamic_Bitset(values);
#endif
            Test_Bounded_Set(values);
            Test_Sparse_Set(values);
            Test_Vector_of_Bool(values);
            Test_Vector_of_Char(values);
        }        
    }
    
    int zz;
    std::cin >> zz;

    return 0;
}






