#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
static int studentCounter;
static int teacherCounter;
//  visitor class to store visitor_category
class Visitor
{
public:
    string visitor_category;
    int timeEntered;
    int visitorCount = 0;
    // constructor to assign vistor_category
    Visitor(string category, int time)
    {
        visitor_category = category;
        timeEntered = time;
        if (visitor_category == "Student")
        {
            visitorCount = ++studentCounter;
        }
        else
        {
            visitorCount = ++teacherCounter;
        }
    }
};

queue<Visitor> student_queue, teacher_queue; // queue to hold Visitir class objects
mutex m;                                     // queue mutex
condition_variable cv;                       // for notifying when visitor arrived and book issued

// function to issue book  and  show on console
void issueBook(Visitor v)
{
    // Book issueing time
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Issued  book to ";
    if (v.visitor_category == "Student")
    {
        cout << "student number: ";
    }
    else
    {
        cout << "teacher number: ";
    }
    cout << v.visitorCount << " arrived at time: " << v.timeEntered << endl;
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

    if (v.visitor_category == "Student")
    {
        student_queue.push(v);
    }
    else
    {
        teacher_queue.push(v);
    }

    cv.notify_one(); // it will notify only one thread that someone entered queue
}

int main()
{
    thread t(chooseVisitor);
    // to add student and teacher randomly in library queue
    int time = 0;
    while (true)
    {
        bool check_if_timealready_incremented = false; // to make sure time not increment twice if both student and teacher come in same iteration

        // Add new visitors to the library at random times
        if (rand() % 2 == 0) // high probabilty to add student asmore chance to get %2=0 then %5 to
        // mmodified time incrementation and  used %5 for teacher to reduce the nummber of teachers coming
        {
            addVisitor(Visitor("Student", time));
            time++;
            check_if_timealready_incremented = true;
        }
        if (rand() % 5 == 0) // and this is not if else ladder these are 2 sperate if which means two students can arrive at same time
        {
            addVisitor(Visitor("Teacher", time));
            if (!check_if_timealready_incremented)
            {
                time++;
            }
        }
        this_thread::sleep_for(chrono::seconds(1));
        // time++; this will increase time in every iteration even if no one entered  commenting this and
        // incrementing time only if someone enter for better understanding
    }

    t.join();
    return 0;
}
