#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <memory>
#include <chrono>

#include "log.h"
#include "blockingconcurrentqueue.h"
#include "concurrentqueue.h"

int example_0()
{
    moodycamel::ConcurrentQueue<int> q;
    int dequeued[100] = { 0 };
    std::thread threads[20];

    // Producers
    for (int i = 0; i != 10; ++i) {
        threads[i] = std::thread([&](int i) {
            for (int j = 0; j != 10; ++j) {
                q.enqueue(i * 10 + j);
            }
        }, i);
    }
    
    // Consumers
    for (int i = 10; i != 20; ++i) {
        threads[i] = std::thread([&]() {
            int item;
            for (int j = 0; j != 20; ++j) {
                if (q.try_dequeue(item)) {
                    ++dequeued[item];
                }
            }
        });
    }
    
    // Wait for all threads
    for (int i = 0; i != 20; ++i) {
        threads[i].join();
    }
    
    // Collect any leftovers (could be some if e.g. consumers finish before producers)
    int item;
    while (q.try_dequeue(item)) {
        ++dequeued[item];
    }
    
    // Make sure everything went in and came back out!
    for (int i = 0; i != 100; ++i) {
        assert(dequeued[i] == 1);
    }

    LOG_D("done!!");
    return 0;
}

class ClassA {
public:
    ClassA() {}

    ClassA(int t0, int t1)
        :m_t0(t0), m_t1(t1) {
    }

    ~ClassA() {
        LOG_D("deconstruct!!");
    }

public:
    int m_t0 = 0;
    int m_t1 = 1;
};

int example_1()
{
    using ClassAPtr = ClassA*;
    moodycamel::ConcurrentQueue<ClassAPtr> q;
    
    ClassAPtr ptr = new ClassA(2, 2);
    q.enqueue(ptr);
    LOG_D("0: ptr= %p, t0= %d, t1= %d", ptr, ptr->m_t0, ptr->m_t1);

    ptr = new ClassA(3, 3);
    q.enqueue(ptr);
    LOG_D("1: ptr= %p, t0= %d, t1= %d", ptr, ptr->m_t0, ptr->m_t1);

    if (q.try_dequeue(ptr)) {
        LOG_D("2: ptr= %p, t0= %d, t1= %d", ptr, ptr->m_t0, ptr->m_t1);
        delete ptr;
    }

    if (q.try_dequeue(ptr)) {
        LOG_D("3: ptr= %p, t0= %d, t1= %d", ptr, ptr->m_t0, ptr->m_t1);
        delete ptr;
    }

    LOG_D("done!!");
    return 0;
}

int example_2()
{
    using ClassASharedPtr = std::shared_ptr<ClassA>;
    moodycamel::ConcurrentQueue<ClassASharedPtr> q;

    ClassASharedPtr ptr = std::make_shared<ClassA>(2, 2);
    q.enqueue(ptr);
    LOG_D("0: ptr= %p, t0= %d, t1= %d", ptr.get(), ptr->m_t0, ptr->m_t1);

    ptr = std::make_shared<ClassA>(3, 3);
    q.enqueue(ptr);
    LOG_D("1: ptr= %p, t0= %d, t1= %d", ptr.get(), ptr->m_t0, ptr->m_t1);

    if (q.try_dequeue(ptr)) {
        LOG_D("2: ptr= %p, t0= %d, t1= %d", ptr.get(), ptr->m_t0, ptr->m_t1);
    }

    if (q.try_dequeue(ptr)) {
        LOG_D("3: ptr= %p, t0= %d, t1= %d", ptr.get(), ptr->m_t0, ptr->m_t1);
    }

    LOG_D("done!!");
    return 0;
}

int example_3()
{
    using ClassASharedPtr = std::shared_ptr<ClassA>;
    
    moodycamel::BlockingConcurrentQueue<ClassASharedPtr> q;
    
    std::thread thd = std::thread([&]() {
        static constexpr std::int64_t SLEEP_DURATION = 10; // µ¥Î»: ºÁÃë

        ClassASharedPtr ptr;
        for (;;) {
            if (q.wait_dequeue_timed(ptr, std::chrono::milliseconds(SLEEP_DURATION))) {
                LOG_D("0: ptr= %p, t0= %d, t1= %d", ptr.get(), ptr->m_t0, ptr->m_t1);
            }
        }
    });

    ClassASharedPtr ptr = std::make_shared<ClassA>(2, 2);
    q.enqueue(ptr);
    thd.join();

    LOG_D("done!!");
    return 0;
}

int example_4()
{
    moodycamel::ConcurrentQueue<ClassA> q;

    moodycamel::ProducerToken ptok(q);
    moodycamel::ConsumerToken ctok(q);

    ClassA a(4, 4);
    q.enqueue(ptok, std::move(a));
    q.try_dequeue(ctok, a);
    LOG_D("t0= %d, t1= %d", a.m_t0, a.m_t1);

    return 0;
}

int main(int argc, char *argv[])
{
    //(void)example_0();
    //(void)example_1();
    //(void)example_2();
    //(void)example_3();
    (void)example_4();

    return 0;
}

