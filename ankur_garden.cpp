#include<iostream>
#include<string>
using namespace std;

class DataStorage;  // forward declaration

// ─────────────────────────────────────────
// abstract base class
// ─────────────────────────────────────────
class DepartmentModule
{
protected:
    int id;
    string name;
    static int totalModules;

public:
    DepartmentModule(int i, string n)
    {
        id = i;
        name = n;
        totalModules++;
    }

    virtual void execute() = 0;  // pure virtual

    static int getTotal() { return totalModules; }
    string getName()      { return name; }

    virtual ~DepartmentModule() { totalModules--; }
};
int DepartmentModule::totalModules = 0;


// dummy class - empty concrete class, just to instantiate DepartmentModule
class Dummy : public DepartmentModule
{
public:
    Dummy() : DepartmentModule(0, "Dummy") {}
    void execute() {}
};


// ─────────────────────────────────────────
// abstract task class
// ─────────────────────────────────────────
class ActivityTask
{
protected:
    int taskId;
    string zone;

public:
    ActivityTask(int t, string z) : taskId(t), zone(z) {}
    virtual void execute() = 0;
    virtual ~ActivityTask() {}
};

// hierarchical inheritance from ActivityTask
// 4 classes all inherit from same parent ActivityTask
class WaterTask : public ActivityTask
{
public:
    WaterTask(int t, string z) : ActivityTask(t, z) {}
    void execute() { cout << "Watering zone: " << zone << endl; }
};

class GrassTrimTask : public ActivityTask
{
public:
    GrassTrimTask(int t, string z) : ActivityTask(t, z) {}
    void execute() { cout << "Trimming grass in: " << zone << endl; }
};

class DeWeedTask : public ActivityTask
{
public:
    DeWeedTask(int t, string z) : ActivityTask(t, z) {}
    void execute() { cout << "Removing weeds in: " << zone << endl; }
};

class TreeShapeTask : public ActivityTask
{
public:
    TreeShapeTask(int t, string z) : ActivityTask(t, z) {}
    void execute() { cout << "Shaping trees in: " << zone << endl; }
};


// ─────────────────────────────────────────
// datastorage - friend of Registration and ResourceTracker
// ─────────────────────────────────────────
class DataStorage
{
private:
    string* logs;
    string* database;
    int logCount, logSize;
    int dbCount, dbSize;

public:
    DataStorage(int lSize, int dSize)
    {
        logSize = lSize;
        dbSize  = dSize;
        logCount = dbCount = 0;
        logs     = new string[logSize];
        database = new string[dbSize];
    }

    void save(string entry)
    {
        if(logCount < logSize) logs[logCount++] = entry;
        else cout << "log full" << endl;
    }

    void saveDB(string entry)
    {
        if(dbCount < dbSize) database[dbCount++] = entry;
        else cout << "db full" << endl;
    }

    void load()
    {
        cout << "\n-- Logs --" << endl;
        for(int i = 0; i < logCount; i++)
            cout << i+1 << ". " << logs[i] << endl;
        cout << "-- Database --" << endl;
        for(int i = 0; i < dbCount; i++)
            cout << i+1 << ". " << database[i] << endl;
    }

    friend class ResourceTracker;
    friend class Registration;

    ~DataStorage()
    {
        delete[] logs;
        delete[] database;
    }
};


// ─────────────────────────────────────────
// hierarchical inheritance from DepartmentModule
// Registration, TaskManager, ResourceTracker all inherit from same parent
// ─────────────────────────────────────────

class Registration : public DepartmentModule
{
private:
    string* users;
    string* zones;
    int userCount, maxUsers;
    int zoneCount, maxZones;

public:
    Registration(int id, int uMax, int zMax) : DepartmentModule(id, "Registration")
    {
        maxUsers = uMax;
        maxZones = zMax;
        userCount = zoneCount = 0;
        users = new string[maxUsers];
        zones = new string[maxZones];
    }

    void registerUser(string uname, DataStorage &ds)
    {
        if(userCount < maxUsers)
        {
            users[userCount++] = uname;
            ds.save("Registered: " + uname);   // friend access
            cout << "Registered: " << uname << endl;
        }
        else cout << "user list full" << endl;
    }

    void addZone(string zone, DataStorage &ds)
    {
        if(zoneCount < maxZones)
        {
            zones[zoneCount++] = zone;
            ds.saveDB("Zone added: " + zone);
            cout << "Zone added: " << zone << endl;
        }
    }

    void execute()
    {
        cout << "\nRegistration - users: " << userCount << " | zones: " << zoneCount << endl;
        for(int i = 0; i < userCount; i++)
            cout << "  user: " << users[i] << endl;
        for(int i = 0; i < zoneCount; i++)
            cout << "  zone: " << zones[i] << endl;
    }

    ~Registration()
    {
        delete[] users;
        delete[] zones;
    }
};


class TaskManager : public DepartmentModule
{
private:
    ActivityTask** taskList;
    int taskCount, maxTasks;

public:
    TaskManager(int id, int max) : DepartmentModule(id, "TaskManager")
    {
        maxTasks = max;
        taskCount = 0;
        taskList = new ActivityTask*[maxTasks];
    }

    void createTask(ActivityTask* t)
    {
        if(taskCount < maxTasks) taskList[taskCount++] = t;
        else cout << "task list full" << endl;
    }

    void assignTask(int index)
    {
        if(index >= 0 && index < taskCount)
            taskList[index]->execute();   // polymorphic call
        else
            cout << "invalid index" << endl;
    }

    void execute()
    {
        cout << "\nRunning all tasks:" << endl;
        for(int i = 0; i < taskCount; i++)
            taskList[i]->execute();
    }

    ~TaskManager()
    {
        for(int i = 0; i < taskCount; i++) delete taskList[i];
        delete[] taskList;
    }
};


// ─────────────────────────────────────────
// MULTIPLE INHERITANCE
// Scheduler inherits from both DepartmentModule AND ActivityTask
// so it is both a module and can behave like a task
// ─────────────────────────────────────────
class Scheduler : public DepartmentModule, public ActivityTask
{
private:
    string* workSchedule;
    int schedCount, maxSched;

public:
    Scheduler(int id, int max) 
        : DepartmentModule(id, "Scheduler"), ActivityTask(id, "Garden")
    {
        maxSched = max;
        schedCount = 0;
        workSchedule = new string[maxSched];
    }

    void scheduleWork(string entry)
    {
        if(schedCount < maxSched)
        {
            workSchedule[schedCount++] = entry;
            cout << "Scheduled: " << entry << endl;
        }
    }

    void checkStatus()
    {
        cout << "\n-- Schedule --" << endl;
        for(int i = 0; i < schedCount; i++)
            cout << i+1 << ". " << workSchedule[i] << endl;
    }

    // overrides execute() from both parents - required for multiple inheritance
    void execute()
    {
        cout << "\nScheduler - " << schedCount << " entries:" << endl;
        checkStatus();
    }

    ~Scheduler() { delete[] workSchedule; }
};


class ResourceTracker : public DepartmentModule
{
private:
    string* inventory;
    int invCount, maxInv;

public:
    ResourceTracker(int id, int max) : DepartmentModule(id, "ResourceTracker")
    {
        maxInv = max;
        invCount = 0;
        inventory = new string[maxInv];
    }

    void updateInventory(string item)
    {
        if(invCount < maxInv) inventory[invCount++] = item;
    }

    // friend access - reads ds.logCount which is private
    void calculateUsage(DataStorage &ds)
    {
        cout << "Logs saved: " << ds.logCount << " | DB entries: " << ds.dbCount << endl;
    }

    void execute()
    {
        cout << "\nInventory:" << endl;
        for(int i = 0; i < invCount; i++)
            cout << "  " << inventory[i] << endl;
    }

    ~ResourceTracker() { delete[] inventory; }
};


// ─────────────────────────────────────────
// copy constructor demo
// ─────────────────────────────────────────
class WorkerProfile
{
public:
    string name;
    int id;

    WorkerProfile(string n, int i) : name(n), id(i) {}

    WorkerProfile(const WorkerProfile &src)
    {
        name = src.name;
        id   = src.id;
        cout << "copy constructor called for: " << name << endl;
    }

    void display() { cout << "Name: " << name << " | ID: " << id << endl; }
};


void printMenu()
{
    cout << "\n==== ANKUR Garden System ====" << endl;
    cout << "1. Registration" << endl;
    cout << "2. Task Manager" << endl;
    cout << "3. Scheduler" << endl;
    cout << "4. Resource Tracker" << endl;
    cout << "5. View Logs" << endl;
    cout << "6. Copy Constructor Demo" << endl;
    cout << "7. Active Module Count" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter choice: ";
}


int main()
{
    cout << "Welcome to ANKUR Garden Management System" << endl;

    Dummy d;   // dummy class object - just to have a DepartmentModule instance
    DataStorage ds(20, 20);

    Registration reg(1, 10, 5);
    TaskManager tm(2, 10);
    Scheduler sch(3, 10);
    ResourceTracker rt(4, 10);

    // pre-load tasks
    tm.createTask(new WaterTask(101, "North Garden"));
    tm.createTask(new GrassTrimTask(102, "South Lawn"));
    tm.createTask(new DeWeedTask(103, "East Bed"));
    tm.createTask(new TreeShapeTask(104, "Main Entrance"));

    // pre-load inventory
    rt.updateInventory("Fertilizer 50kg");
    rt.updateInventory("Pesticide 20L");
    rt.updateInventory("Water Pipes 10 units");

    int choice;
    do
    {
        printMenu();
        cin >> choice;

        if(choice == 1)
        {
            cout << "Enter username: ";
            string uname; cin >> uname;
            reg.registerUser(uname, ds);

            cout << "Enter zone name: ";
            string zone; cin >> zone;
            reg.addZone(zone, ds);

            reg.execute();
        }
        else if(choice == 2)
        {
            tm.execute();
        }
        else if(choice == 3)
        {
            cout << "Enter schedule entry: ";
            cin.ignore();
            string entry; getline(cin, entry);
            sch.scheduleWork(entry);
            sch.execute();
        }
        else if(choice == 4)
        {
            rt.execute();
            rt.calculateUsage(ds);
        }
        else if(choice == 5)
        {
            ds.load();
        }
        else if(choice == 6)
        {
            WorkerProfile w1("Gurram", 1001);
            WorkerProfile w2 = w1;   // triggers copy constructor
            w1.display();
            w2.display();
        }
        else if(choice == 7)
        {
            cout << "Active modules: " << DepartmentModule::getTotal() << endl;
        }
        else if(choice != 0)
        {
            cout << "invalid choice" << endl;
        }

    } while(choice != 0);

    cout << "Exiting..." << endl;
    return 0;
}
