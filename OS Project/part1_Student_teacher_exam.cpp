#include <iostream>
#include <thread> // for multithreading
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

mutex m;
condition_variable cv;
bool pen_available = false;
bool paper_available = false;
bool question_paper_available = false;

void teacher()
{
    while (true)
    {
        int item1, item2;
        item1 = rand() % 3;
        do
        {
            item2 = rand() % 3;
        } while (item2 == item1);
        cout << "Teacher placed ";
        if (item1 == 0)
        {
            cout << "pen";
            pen_available = true;
        }
        else if (item1 == 1)
        {
            cout << "paper";
            paper_available = true;
        }
        else
        {
            cout << "question paper";
            question_paper_available = true;
        }
        cout << " and ";
        if (item2 == 0)
        {
            cout << "pen";
            pen_available = true;
        }
        else if (item2 == 1)
        {
            cout << "paper";
            paper_available = true;
        }
        else
        {
            cout << "question paper";
            question_paper_available = true;
        }
        cout << " on table" << endl;

        cv.notify_all();

        unique_lock<mutex> lock(m);
        cv.wait(lock);
        cout << "Teacher collected completed assignment" << endl
             << endl;

        pen_available = false;
        paper_available = false;
        question_paper_available = false;
    }
}

void student(int id, bool &item1_available, bool &item2_available)
{
    while (true)
    {

        unique_lock<mutex> lock(m);
        cv.wait(lock, []
                { return (pen_available && paper_available) ||
                         (pen_available && question_paper_available) ||
                         (paper_available && question_paper_available); });

        if (item1_available && item2_available)
        {
            cout << "Student " << id << " has taken ";
            if (pen_available)
            {
                cout << "pen and ";
                pen_available = false;
            }
            if (paper_available)
            {
                cout << "paper and ";
                paper_available = false;
            }
            if (question_paper_available)
            {
                cout << "question paper and ";
                question_paper_available = false;
            }
            cout << "started doing the assignment" << endl;

            this_thread::sleep_for(chrono::seconds(5));

            cv.notify_all();
            lock.unlock();
            cout << "Student " << id << " completed the assignment" << endl;
        }
        else
        {
            lock.unlock();
        }
    }
}

int main()
{

    thread student1(student, 1, ref(pen_available), ref(paper_available));
    thread student2(student, 2, ref(paper_available), ref(question_paper_available));
    thread student3(student, 3, ref(pen_available), ref(question_paper_available));

    thread teacher_thread(teacher);

    student1.join();
    student2.join();
    student3.join();
    teacher_thread.join();

    return 0;
}
