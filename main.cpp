#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

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
    explicit Vehicle(int id)
        : _id(id)
    {}
    void operator()() const
    {
        std::cout << "Vehicle #" << _id << " has been created" << std::endl;
    }

private:
    int _id;
};

class VehicleWithMemberFunctions
{
public:
    VehicleWithMemberFunctions()
        : _id(0)
    {}
    void addID(int id)
    {
        _id = id;
    }
    void printID() const
    {
        std::cout << "Vehicle ID=" << _id << std::endl;
    }
    void setName(std::string name)
    {
        _name = std::move(name);
    }
    void printName()
    {
        std::cout << "Vehicle name=" << _name << std::endl;
    }

private:
    int _id;
    std::string _name;
};

void printID(int id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "ID = " << id << std::endl;
}

void printIDAndName(int id, std::string const& name)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "ID = " << id << ", name = " << name << std::endl;
}

void printName(std::string name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    std::cout << "Name (from Thread) = " << name << std::endl;
}

void printNameWithRef(std::string& name, int waitTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    name += " (from Thread)";
    std::cout << name << std::endl;
}

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
    int idForVariadicTemplates = 0;  // Define an integer variable

    // starting threads using variadic templates
    std::thread t3(printID, idForVariadicTemplates);
    std::thread t4(printIDAndName, ++idForVariadicTemplates, "MyString");

    // wait for threads before returning
    t3.join();
    t4.join();

    std::string name1 = "MyThread5";
    std::string name2 = "MyThread6";

    // starting threads using value-copy and move semantics
    std::thread t5(printName, name1, 50);
    std::thread t6(printName, std::move(name2), 100);

    // wait for threads before returning
    t5.join();
    t6.join();



    // print name from main
    // The console output shows how using copy-by-value and std::move affect the string parameters:
    std::cout << "Name1 (from Main) = " << name1 << std::endl;
    std::cout << "Name2 (from Main) = " << name2 << std::endl;

    std::string name3("MyThread name3");

    // starting thread
    std::thread t7(printNameWithRef, std::ref(name3), 50);

    // wait for thread before returning
    t7.join();

    // print name from main
    name3 += " (from Main)";
    std::cout << name3 << std::endl;
    // Even though the code works, we are now sharing mutable data between threads - a primary source for concurrency
    // bugs.


    /*
     * Starting threads with member functions
     */
    // create thread
    VehicleWithMemberFunctions v1, v2;
    std::thread t8 = std::thread(&VehicleWithMemberFunctions::addID, v1, 1);   // call member function on object v
    std::thread t9 = std::thread(&VehicleWithMemberFunctions::addID, &v2, 2);  // call member function on object v

    // wait for thread to finish
    t8.join();
    t9.join();

    // print Vehicle id
    v1.printID();
    v2.printID();

    // An alternative is to use a heap-allocated object and a reference-counted pointer such as std::shared_ptr<Vehicle>
    // to ensure that the object lives as long as it takes the thread to finish its work. The following example shows how
    // this can be implemented:
    auto v3 = std::make_shared<VehicleWithMemberFunctions>();
    std::thread t10 = std::thread(&VehicleWithMemberFunctions::addID, v3, 3); // call member function on object v

    // wait for thread to finish
    t10.join();

    // print Vehicle id
    v3->printID();

    // create thread
    auto v4 = std::make_shared<VehicleWithMemberFunctions>();
    std::thread t11 = std::thread(&VehicleWithMemberFunctions::addID, v3, 4); // call member function on object v
    std::thread t12 = std::thread(&VehicleWithMemberFunctions::setName, v4, "Jeep"); // call member function on object v

    // wait for thread to finish
    t11.join();
    t12.join();

    // print Vehicle id
    v4->printID();
    v4->printName();

    // End of Lesson1 section 6 here.

    return 0;
}
