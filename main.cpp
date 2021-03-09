#include <chrono>
#include <iostream>
#include <thread>

void threadFunction()
{
    std::cout << "The other thread Thread id = " << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // simulate work
    std::cout << "Finished work 1 in thread\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Finished work 2 in thread\n";
}

class Vehicle
{
public:
    Vehicle(int id)
        : _id(id)
    {}
    void operator()()
    {
        std::cout << "Vehicle #" << _id << " has been created" << std::endl;
    }

private:
    int _id;
};

int main()
{
    /*
     * Threads started by functions, and objects with overloaded function call operator
     */
    std::cout << "Hello concurrent world from main! Thread id = " << std::this_thread::get_id() << std::endl;
    unsigned int nCores = std::thread::hardware_concurrency();
    std::cout << "This machine supports concurrency with " << nCores << " cores available" << std::endl;

    // create thread
    std::thread t {threadFunction};
    std::thread tV {Vehicle(1)};

    // do something in main()
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // simulate work
    std::cout << "Finished work 1 in main\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Finished work 2 in main\n";

    // wait for thread to finish
    t.join();
    tV.join();

    /*
     * lambdas
     */
    int id = 0;  // Define an integer variable

    // auto f0 = []() { std::cout << "ID = " << id << std::endl; }; // Error: 'id' cannot be accessed
    // auto f3 = [id]() { std::cout << "ID = " << ++id << std::endl; }; // Error, 'id' may not be modified

    id++;
    auto f1 = [id] {
        std::cout << "ID = " << id << std::endl;
    };  // OK, 'id' is captured by value
    id++;
    auto f2 = [&id]() {
        std::cout << "ID = " << id << std::endl;
    };  // OK, 'id' is captured by reference
    auto f4 = [id]() mutable {
        std::cout << "ID = " << ++id << std::endl;
    };  // OK, 'id' may be modified
    auto f5 = [](const int id) {
        std::cout << "ID = " << id << std::endl;
    };  // ID is passed as a parameter

    f1();
    f2();
    f4();
    f5(id);

    /*
     * Lambdas in threads
     */

    int idForLambdas = 0;  // Define an integer variable

    // starting a first thread (by reference)
    auto f0 = [&idForLambdas]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "a) ID in Thread (call-by-reference) = " << idForLambdas << std::endl;
    };
    std::thread t1(f0);

    // starting a second thread (by value)
    std::thread t2([idForLambdas]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "b) ID in Thread (call-by-value) = " << idForLambdas << std::endl;
    });

    // increment and print id in main
    ++idForLambdas;
    std::cout << "c) ID in Main (call-by-value) = " << idForLambdas << std::endl;

    // wait for threads before returning
    t1.join();
    t2.join();


    /*
     * Starting a Thread with Variadic Templates and Member Functions
     */

    return 0;
}
