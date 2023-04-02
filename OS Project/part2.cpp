#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

//  visitor class to store visitor_category
class Visitor
{
public:
    string visitor_category;
    // constructor to assign vistor_category
    Visitor(string category)
    {
        visitor_category = category;
    }
};

queue<Visitor> student_queue, teacher_queue; // queue to hold Visitir class objects
mutex m;                                     // queue mutex
condition_variable cv;

// function to issue book  and  show on console
void issueBook(Visitor v)
{
    // Book issueing time
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Issued  book to ";
    if (v.visitor_category == "Student")
    {
        cout << "student\n";
    }
    else
    {
        cout << "teacher\n";
    }
}

void chooseVisitor()
{
    while (true)
    {

        unique_lock<mutex> lock(m);
        cv.wait(lock, []
                { return !student_queue.empty() || !teacher_queue.empty(); }); // wait for a visitor to arrive by checking cv condition

        // first check teacher if there is any otherwise process student later hence giving priority to teachers
        if (!teacher_queue.empty())
        {
            Visitor v = teacher_queue.front();
            teacher_queue.pop();
            lock.unlock();
            issueBook(v);
        }
        else
        {
            Visitor v = student_queue.front();
            student_queue.pop();
            lock.unlock();
            issueBook(v);
        }
    }
}

void addVisitor(Visitor v)
{
    m.lock();
    if (v.visitor_category == "Student")
    {
        student_queue.push(v);
    }
    else
    {
        teacher_queue.push(v);
    }

    m.unlock();
    cv.notify_one(); // it will notify only one thread that lock is released and that willcheck condition and proceed
}

int main()
{
    thread t(chooseVisitor);

    while (true)
    {
        // Add new visitors to the library at random times
        if (rand() % 2 == 0) // high probabilty to add student asmore chance to get %2=0 then %3
        {
            addVisitor(Visitor("Student"));
        }
        if (rand() % 3 == 0)
        {
            addVisitor(Visitor("Teacher"));
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    t.join();
    return 0;
}
