#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
static int studentCounter;
static int teacherCounter;
//  visitor class to store visitor_category , arrival time and count
class Visitor
{
public:
    string visitor_category;
    int timeEntered;
    int visitorCount = 0; // wil store count among teacher and student seperately
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
int seconds = 0; // to keep the timer record
void timerSeconds()
{

    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(990)); // 990 miliseconds
        seconds++;
    }
}

queue<Visitor> student_queue, teacher_queue; // queue to hold Visitir class objects
mutex m;                                     // mutex for protecting queue
condition_variable cv;                       // for notifying when visitor arrived

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
    cout << v.visitorCount << " arrived at time: " << v.timeEntered << " seconds"
         << "  current time is " << seconds << " seconds" << endl;
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
            issueBook(v);
            lock.unlock();
        }
        else
        {
            Visitor v = student_queue.front();
            student_queue.pop();
            issueBook(v);
            lock.unlock();
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

    cv.notify_one(); // it will notify chooseVisitor to choose one visitor
}

int main()
{
    thread t(chooseVisitor);
    thread timer(timerSeconds);
    // to add student and teacher randomly in library queue
    int num_of_visitors = 0; // ta make the program finite
    while (true)
    {
        if (num_of_visitors >= 20)
        {
            break;
        }

        // Add new visitors to the library at random times
        if (rand() % 2 == 0) // high probabilty to add student asmore chance to get %2=0 then %5 to
        // mmodified time incrementation and  used %5 for teacher to reduce the nummber of teachers coming
        {
            addVisitor(Visitor("Student", seconds));

            num_of_visitors++;
        }
        if (rand() % 4 == 0) // and this is not if else ladder these are 2 sperate if which means two students can arrive at same time
        {
            addVisitor(Visitor("Teacher", seconds));
            num_of_visitors++;
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
    timer.join();
    t.join();
    return 0;
}
