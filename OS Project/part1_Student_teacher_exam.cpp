#include <iostream>
#include <thread>
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
    while (true) // will run infinite time
    {
        // selecting two items at random
        //  let 0= pen ,1=paper , 2= question paper , rand%3 will give number from 0 to 3
        int item1, item2;
        item1 = rand() % 3;
        do
        {
            item2 = rand() % 3;
        } while (item2 == item1); // keep doing untill both items are differnt
        // teacher placing available item on the table
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

        cv.notify_all(); // this will notify(invoke) all other threads cc(conditional vaariable)
                         // and they will check further the condition is satisfied , in short table is realesed now for  students to give exam

        unique_lock<mutex> lock(m); // unique_lock object is used as argument in cv(conditional variable) ,otherwise we could have just used m.lock();

        cv.wait(lock); // will wait for student to complete the exam
        cout << "Teacher collected completed assignment" << endl
             << endl;
        // setting everything to false after comletion
        pen_available = false;
        paper_available = false;
        question_paper_available = false;
    }
}

void student(int id, bool &item1_available, bool &item2_available)
{
    while (true)
    {

        unique_lock<mutex> lock(m); // creating unique_lock object on mutex m
        cv.wait(lock, []
                { return (pen_available && paper_available) ||
                         (pen_available && question_paper_available) ||
                         (paper_available && question_paper_available); }); // this will check for lock and then check the
                                                                                                                                                                 //    condition if two things  things available together not , if available only then it will  proceed

        if (item1_available && item2_available) // if item available then startdoing exam otherwise realease the lock
        {
            // Checking individual thing available and printiing on console what things are got to start dong exam
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

            this_thread::sleep_for(chrono::seconds(5)); // sleep for 5 seconds to stimulate writing an exam
            lock.unlock();
            cv.notify_all(); // this will notify(invoke) all other threads cc(conditional vaariable)
            // and they will check further the condition is satisfied , in short table is realesed now for other student to give exam

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
    // multiple threads of student and teacher is created
    thread student1(student, 1, ref(pen_available), ref(paper_available));
    thread student2(student, 2, ref(paper_available), ref(question_paper_available));
    thread student3(student, 3, ref(pen_available), ref(question_paper_available));

    thread teacher_thread(teacher);
    // join method will hold main fuction till the executions of threads
    student1.join();
    student2.join();
    student3.join();
    teacher_thread.join();

    return 0;
}
