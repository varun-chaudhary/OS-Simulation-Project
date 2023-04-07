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

bool std1_done = false;
bool std2_done = false;
bool std3_done = false;

void teacher()
{

    while (!std1_done || !std2_done || !std3_done) // will run untill all the students are done with assignmenet
    {
        bool selected_2_items_successfully = false;
        int random_choice = 0;
        random_choice = rand() % 3;
        do
        {
            selected_2_items_successfully = false;
            random_choice = rand() % 3;
            cout << "Teacher placed ";
            switch (random_choice)
            {
            case 0:
                cout << "pen and paper on the table\n";
                if (std1_done) // checking if student with alternate thing is already done or not
                {
                    cout << "but student 1 (with question paper) is already done\n\n";
                    continue;
                }
                else
                {
                    pen_available = true;
                    paper_available = true;
                    selected_2_items_successfully = true;
                }

                break;
            case 1:
                cout << "paper and question paper on the table\n";
                if (std2_done)
                {
                    cout << "but student 2 (with pen) is already done\n\n";
                    continue;
                }
                else
                {
                    paper_available = true;
                    question_paper_available = true;
                    selected_2_items_successfully = true;
                }
                break;

            case 2:
                cout << "pen and question paper on the table\n";
                if (std3_done)
                {
                    cout << "but student 3  (with paper) is already done\n\n";
                    continue;
                }
                else
                {
                    pen_available = true;
                    question_paper_available = true;
                    selected_2_items_successfully = true;
                }

                break;
            }
        } while (!selected_2_items_successfully); // continue the loop untill 2 items are selected succesfuuly

        cv.notify_all(); // this will notify(invoke) all other threads cv(conditional vaariable)
                         // and they will check further the condition is satisfied , in short- table is realesed now for  students to give assignment

        unique_lock<mutex> lock(m); // unique_lock object is used as argument in cv(conditional variable) ,otherwise we could have just used m.lock();

        cv.wait(lock); // will wait for student to complete the assignment and notify
        cout << "Teacher collected completed assignment" << endl
             << endl;
        this_thread::sleep_for(chrono ::seconds(2));

        // setting everything unuavailable after comletion
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
                         (paper_available && question_paper_available); }); // this will check for lock and then check then check if teacher made  2 things available
                                                                                                                                                                 //    condition if two things  things available together not , if available only then it will  proceed

        if (item1_available && item2_available) // if required  item are  available then startdoing assignment otherwise realease the lock
        {
            // Checking individual thing available and printiing on console what things are got to start dong assignment
            cout << "Student " << id << " has taken ";
            if (pen_available && paper_available)
            {
                cout << "pen and paper ";
                std1_done = true;
                pen_available = false;
                paper_available = false;
            }
            else if (paper_available && question_paper_available)
            {
                cout << "paper and question paper ";
                std2_done = true;
                paper_available = false;
                question_paper_available = false;
            }
            else if (question_paper_available && pen_available)
            {
                cout << "pen and question paper ";
                std3_done = true;
                question_paper_available = false;
                pen_available = false;
            }
            cout << "and started doing the assignment " << endl;

            this_thread::sleep_for(chrono::seconds(5)); // sleep for 5 seconds to stimulate writing an assignment
            cout << "Student " << id << " completed the assignment" << endl
                 << endl;

            lock.unlock();
            cv.notify_all(); // tellimg teacher assignment is done on completeion as per question
            // this will notify(invoke) all other threads cc(conditional vaariable)
            // and they will check further the condition is satisfied , in short -shared table is realesed now for other student to give assignment
            pthread_exit(NULL); // student thread exit after doing assignment
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
    thread student1(student, 1, ref(pen_available), ref(paper_available));            // have question paper
    thread student2(student, 2, ref(paper_available), ref(question_paper_available)); // have pen
    thread student3(student, 3, ref(pen_available), ref(question_paper_available));   // have paper
    thread teacher_thread(teacher);

    // join method will hold main fuction till the executions of threads
    student1.join();
    student2.join();
    student3.join();
    teacher_thread.join();

    return 0;
}
