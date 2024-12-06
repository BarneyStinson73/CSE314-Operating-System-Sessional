#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <chrono>
using namespace std;
int n;
int per_print_time;
int per_bind_time;
int per_read_write_time;
pthread_t *threads;
sem_t *students_sem;
sem_t working_mutex;
sem_t binding_mutex;
sem_t submission_mutex;
sem_t reader_mutex;
sem_t print_mutex;
int printer_state[5];
int rc = 0;
int submission_count = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> programStart;
std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;

class student
{
public:
    int id;
    int group_id;
    int printer_id;
    int state; // 0 for waiting, 1 for printing
    int leader_check;
    student()
    {
        this->id = 0;
        this->group_id = 0;
        state = 5;
        leader_check = 0;
    }
    void set_id(int id)
    {
        this->id = id;
    }
    void set_group_id(int group_id)
    {
        this->group_id = group_id;
    }
};
student *students;
void test(int id)
{
    if (students[id].state == 0 && printer_state[students[id].printer_id] == 0)
    {
        students[id].state = 1;
        printer_state[students[id].printer_id] = 1;
        sem_post(&students_sem[id]);
    }
}
void submission(int group_id)
{

    sem_wait(&submission_mutex);
    sem_wait(&print_mutex);
    submission_count++;

    currentTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration since the program start
    std::chrono::duration<double> elapsedSeconds = currentTime - programStart;
    // Group 2 has submitted the report at time 95
    cout << "Group " << group_id << " has submitted the report at time " << elapsedSeconds.count() << endl;
    sem_post(&print_mutex);
    sleep(per_read_write_time);
    sem_post(&submission_mutex);
}
void *read_by_stuffs(void *staff_id)
{
    int staff;
    while (true)
    {
        sem_wait(&reader_mutex);
        rc = rc + 1;
        if (rc == 1)
        {
            sem_wait(&submission_mutex);
        }
        sem_post(&reader_mutex);
        sem_wait(&print_mutex);
        // Staff 1 has started reading the entry book at time 13. No. of submission = 0

        currentTime = std::chrono::high_resolution_clock::now();
        // Calculate the duration since the program start
        std::chrono::duration<double> elapsedSeconds = currentTime - programStart;

        cout << "Staff " << staff_id << " has started reading the entry book at time " << elapsedSeconds.count() << ". No. of submission = " << submission_count << endl;
        sem_post(&print_mutex);
        sleep(per_read_write_time);
        sem_wait(&reader_mutex);
        rc = rc - 1;
        if (rc == 0)
        {
            sem_post(&submission_mutex);
        }
        sem_post(&reader_mutex);
        sleep(per_read_write_time);
    }
}
void bind(int group_id)
{
    sem_wait(&binding_mutex);
    sem_wait(&print_mutex);

    currentTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration since the program start
    std::chrono::duration<double> elapsedSeconds = currentTime - programStart;
    // Group 2 has started binding at time 80
    cout << "Group " << group_id << " has started binding at time " << elapsedSeconds.count() << endl;
    sem_post(&print_mutex);
    sleep(per_bind_time);
    sem_wait(&print_mutex);
    currentTime = std::chrono::high_resolution_clock::now();
    elapsedSeconds = currentTime - programStart;
    cout << "Group " << group_id << " has finished binding at time " << elapsedSeconds.count() << endl;
    sem_post(&print_mutex);
    sem_post(&binding_mutex);
    submission(group_id);
}
void print_done(int id)
{
    sem_wait(&working_mutex);
    students[id].state = 5;
    printer_state[students[id].printer_id] = 0;
    // test(id);
    for (int t = 1; t <= n; t++)
    {
        if (students[t].group_id == students[id].group_id)
        {
            if (students[t].state == 0 && students[t].printer_id == students[id].printer_id)
                test(t);
        }
    }
    for (int t = 1; t <= n; t++)
    {
        if (students[t].group_id != students[id].group_id)
        {
            if (students[t].state == 0 && students[t].printer_id == students[id].printer_id)
                test(t);
        }
    }
    sem_post(&working_mutex);
    if (students[id].leader_check == 1)
    {
        for (int i = 1; i <= n; i++)
        {
            //cout << students[i].group_id << " " << students[i].leader_check << endl;
            if (students[i].group_id == students[id].group_id && students[i].leader_check != 1)
            {
                // cout << "joined " << id << endl;
                pthread_join(threads[i], NULL);
            }
        }
        cout << "Group " << students[id].group_id << " has finished printing" << endl;
        bind(students[id].group_id);
    }
    else
    {
        pthread_exit(NULL);
    }
}
void *Print(void *threadid)
{
    int id = (int)threadid;
    sem_wait(&working_mutex);
    students[id].state = 0;
    test(id);
    sem_post(&working_mutex);
    sem_wait(&students_sem[id]);
    sem_wait(&print_mutex);

    // Get the current time using high-resolution clock
    currentTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration since the program start
    std::chrono::duration<double> elapsedSeconds = currentTime - programStart;

    printf("Student %d  has arrived at the print station at time %f\n", id, elapsedSeconds.count());
    sem_post(&print_mutex);
    sleep(per_print_time);
    sem_wait(&print_mutex);
    currentTime = std::chrono::high_resolution_clock::now();
    elapsedSeconds = currentTime - programStart;
    cout << "Student " << id << " has finished printing at time " << elapsedSeconds.count() << endl;
    sem_post(&print_mutex);
    print_done(id);
}

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    programStart = std::chrono::high_resolution_clock::now();

    // Get the current time using high-resolution clock
    cin >> n;
    int size_of_group;
    cin >> size_of_group;
    int temp[n + 1];
    cin >> per_print_time;
    cin >> per_bind_time;
    cin >> per_read_write_time;
    // student students[n+1];
    students = (student *)malloc((n + 1) * sizeof(student));
    for (int i = 1; i <= n; i++)
    {
        students[i].set_id(i);
        students[i].set_group_id(((i - 1) / size_of_group) + 1);
        students[i].printer_id = students[i].id % 4 + 1;
        if (students[i].id % size_of_group == 0)
        {
            //cout << i << endl;
            students[i].leader_check = 1;
        }
    }
    for (int i = 0; i <= 4; i++)
    {
        printer_state[i] = 0;
    }
    // pthread_t threads[n+1];

    students_sem = (sem_t *)malloc((n + 1) * sizeof(sem_t));

    sem_init(&working_mutex, 0, 1);
    sem_init(&binding_mutex, 0, 1);
    sem_init(&submission_mutex, 0, 1);
    sem_init(&reader_mutex, 0, 1);
    sem_init(&print_mutex, 0, 1);

    for (int i = 1; i <= n; i++)
    {
        sem_init(&students_sem[i], 0, 0);
    }
    // pthread_t student_threads[n+1];
    threads = (pthread_t *)malloc((n + 1) * sizeof(pthread_t));
    int rc;
    random_device rd;
    mt19937 gen(rd());
    double mean = n / 2.0;
    // Create the Poisson distribution with the specified mean
    std::poisson_distribution<int> poissonDist(mean);
    for (int t = 1; t <= n; t++)
    {
        // printf("In main: creating thread %d\n", t);
        int randomValue = poissonDist(gen);
        while (temp[randomValue % n + 1] == 1)
        {
            randomValue = poissonDist(gen);
        }
        temp[randomValue % n + 1] = 1;
        rc = pthread_create(&threads[randomValue % n + 1], NULL, Print, (void *)(randomValue % n + 1));
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_t reader_thread[3];
    for (int i = 1; i <= 2; i++)
    {
        pthread_create(&reader_thread[i], NULL, read_by_stuffs, (void *)i);
    }

    for (int t = 1; t <= n; t++)
    {
        if(t%size_of_group==0)
        pthread_join(threads[t], NULL);
    }
    for (int i = 1; i <= 2; i++)
    {
        pthread_cancel(reader_thread[i]);
    }
}