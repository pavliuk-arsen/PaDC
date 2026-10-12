1.2.1
#include <iostream>
#include <thread>

void Thread1() {
    std::cout << 1 << std::endl;
}

void Thread2() {
    std::cout << 2 << std::endl;
}

int main() {
    std::thread t1(Thread1);
    std::thread t2(Thread2);
    //1.2.2
    t1.detach();
    t2.detach();

    return 0;
}


//1.2.2
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <chrono>

std::list<int> l;

void AddToList(int startValue) {
    for (int i = 0; i < 10; ++i) {
        l.push_back(startValue + i);
        std::cout << "Added: " << startValue + i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void ListContains(int targetValue) {
    for (int i = 0; i < 10; ++i) {
        bool found = (std::find(l.begin(), l.end(), targetValue) != l.end());
        std::cout << "Value " << targetValue << (found ? " found" : " not found") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main() {
    int startVal = 5;
    std::thread t1(AddToList, startVal);
    std::thread t2(ListContains, startVal);

    t1.join();
    t2.join();
    return 0;
}


//1.2.4
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <chrono>

std::list<int> l;
std::mutex mtx;

void AddToList(int startValue) {
    for (int i = 0; i < 10; ++i) {
        mtx.lock();
        l.push_back(startValue + i);
        std::cout << "Added: " << startValue + i << std::endl;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ListContains(int targetValue) {
    for (int i = 0; i < 10; ++i) {
        mtx.lock();
        bool found = (std::find(l.begin(), l.end(), targetValue) != l.end());
        std::cout << "Value " << targetValue << (found ? " found" : " not found") << std::endl;
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    int startVal = 5;
    std::thread t1(AddToList, startVal);
    std::thread t2(ListContains, startVal);

    t1.join();
    t2.join();
    return 0;
}


//1.2.5
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>

std::list<int> l;
std::mutex mtx;

void AddToList(int value) {
    std::lock_guard<std::mutex> lock(mtx);
    l.push_back(value);
    std::cout << "Added: " << value << std::endl;
}

void ListContains(int targetValue) {
    std::lock_guard<std::mutex> lock(mtx);
    bool found = (std::find(l.begin(), l.end(), targetValue) != l.end());
    std::cout << "Check " << targetValue << ": " << (found ? "Found" : "Not Found") << std::endl;
}

int main() {
    int target = 100;
    for (int i = 0; i < 10; ++i) {
        std::thread t1(AddToList, target + i);
        std::thread t2(ListContains, target);
        t1.detach();
        t2.detach();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}


//1.2.6
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

struct someData {
    std::string name;
    std::string surname;
    std::string address;
    int age;
};

class exchangePerson {
public:
    someData data;
    std::mutex mtx;

    static void JohnDoe(exchangePerson& p) {
        std::lock_guard<std::mutex> lock(p.mtx);
        p.data.name = "John"; 
        p.data.surname = "Doe"; 
        p.data.address = "Unknown"; 
        p.data.age = 120; 
    }

    static void JacobSmith(exchangePerson& p) {
        std::lock_guard<std::mutex> lock(p.mtx);
        p.data.name = "Jacob"; 
        p.data.surname = "Smith"; 
        p.data.address = "Known"; 
        p.data.age = 1; 
    }

    static void Swap(exchangePerson& p1, exchangePerson& p2) {
        if (&p1 == &p2) return;

        std::lock(p1.mtx, p2.mtx);

        std::lock_guard<std::mutex> lock1(p1.mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(p2.mtx, std::adopt_lock);

        std::swap(p1.data, p2.data);
    }

    void PrintInfo() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << data.name << " " << data.surname << " | "
            << data.address << " | Age: " << data.age << std::endl;
    }
};

int main() {
    exchangePerson p1, p2;

    std::thread t1(exchangePerson::JohnDoe, std::ref(p1));
    std::thread t2(exchangePerson::JacobSmith, std::ref(p2));

    t1.detach();
    t2.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Before swap:" << std::endl;
    p1.PrintInfo();
    p2.PrintInfo();

    std::thread t3(exchangePerson::Swap, std::ref(p1), std::ref(p2));
    t3.join(); 

    std::cout << "After swap:" << std::endl;
    p1.PrintInfo();
    p2.PrintInfo();

    return 0;
}