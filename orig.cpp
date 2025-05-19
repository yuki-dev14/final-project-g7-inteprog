#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <memory>
#include <sstream>
#include <stdexcept>

using namespace std;

// Logger Singleton
class Logger {
private:
    static Logger* instance;
    ofstream logFile;
    Logger() { logFile.open("log.txt", ios::app); }
public:
    static Logger* getInstance() {
        if (!instance)
            instance = new Logger();
        return instance;
    }
    void log(const string& action) {
        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty() && dt.back() == '\n') dt.pop_back();
        logFile << "[" << dt << "] " << action << endl;
    }
    ~Logger() {
        if (logFile.is_open())
            logFile.close();
    }
};
Logger* Logger::instance = nullptr;

// Utility: trim whitespace
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// User base class
class User {
protected:
    string id, name, email, password;
public:
    User(const string& id, const string& name, const string& email, const string& password)
        : id(id), name(name), email(email), password(password) {}
    virtual ~User() {}
    string getId() const { return id; }
    string getName() const { return name; }
    virtual void menu() = 0;
    virtual bool handleOption(int opt) = 0;
};

// Admin class
class Admin : public User {
public:
    Admin(const string& id, const string& name, const string& email, const string& password)
        : User(id, name, email, password) {}
    void menu() override {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Student\n";
        cout << "2. Add Course\n";
        cout << "3. View All Students\n";
        cout << "4. View All Courses\n";
        cout << "5. View Students per Course\n";
        cout << "6. Edit Student\n";
        cout << "7. Edit Course\n";
        cout << "8. Delete Student\n";
        cout << "9. Delete Course\n";
        cout << "10. Logout\n";
    }
    bool handleOption(int opt) override;
};

// Student class
class Student : public User {
public:
    Student(const string& id, const string& name, const string& email, const string& password)
        : User(id, name, email, password) {}
    void menu() override {
        cout << "\n--- Student Menu ---\n";
        cout << "1. View Profile\n";
        cout << "2. Enroll in Course\n";
        cout << "3. View Enrolled Courses\n";
        cout << "4. Edit Profile\n";
        cout << "5. Drop Course\n";
        cout << "6. Logout\n";
    }
    bool handleOption(int opt) override;
};

// --- File helpers ---
bool studentExists(const string& id) {
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid;
        getline(iss, sid, ',');
        if (trim(sid) == id) return true;
    }
    return false;
}
bool courseExists(const string& code) {
    ifstream fin("courses.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode;
        getline(iss, ccode, ',');
        if (trim(ccode) == code) return true;
    }
    return false;
}
bool isEnrolled(const string& sid, const string& ccode) {
    ifstream fin("enrollments.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, code;
        getline(iss, id, ',');
        getline(iss, code, ',');
        if (trim(id) == sid && trim(code) == ccode) return true;
    }
    return false;
}

// --- Admin Features ---
void addStudent() {
    string id, name, email, age, program, password;
    cout << "Enter Student ID: "; getline(cin, id);
    if (studentExists(id)) { cout << "Student ID already exists.\n"; return; }
    cout << "Enter Name: "; getline(cin, name);
    cout << "Enter Email: "; getline(cin, email);
    cout << "Enter Age: "; getline(cin, age);
    cout << "Enter Program: "; getline(cin, program);
    cout << "Enter Password: "; getline(cin, password);
    ofstream fout("students.txt", ios::app);
    fout << id << "," << name << "," << email << "," << age << "," << program << "," << password << endl;
    Logger::getInstance()->log("Admin added student " + id);
    cout << "Student added.\n";
}
void addCourse() {
    string code, name, units;
    cout << "Enter Course Code: "; getline(cin, code);
    if (courseExists(code)) { cout << "Course code already exists.\n"; return; }
    cout << "Enter Course Name: "; getline(cin, name);
    cout << "Enter Units: "; getline(cin, units);
    ofstream fout("courses.txt", ios::app);
    fout << code << "," << name << "," << units << endl;
    Logger::getInstance()->log("Admin added course " + code);
    cout << "Course added.\n";
}
void viewAllStudents() {
    ifstream fin("students.txt");
    string line;
    cout << "\nID\tName\tEmail\tAge\tProgram\n";
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, name, email, age, program, pwd;
        getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        cout << id << "\t" << name << "\t" << email << "\t" << age << "\t" << program << endl;
    }
}
void viewAllCourses() {
    ifstream fin("courses.txt");
    string line;
    cout << "\nCode\tName\tUnits\n";
    while (getline(fin, line)) {
        istringstream iss(line);
        string code, name, units;
        getline(iss, code, ','); getline(iss, name, ','); getline(iss, units, ',');
        cout << code << "\t" << name << "\t" << units << endl;
    }
}
void viewStudentsPerCourse() {
    cout << "Enter Course Code: ";
    string code; getline(cin, code);
    if (!courseExists(code)) { cout << "Course not found.\n"; return; }
    cout << "Students enrolled in " << code << ":\n";
    ifstream fin("enrollments.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ','); getline(iss, ccode, ',');
        if (trim(ccode) == code) {
            // Get student name
            ifstream sfin("students.txt");
            string sline;
            while (getline(sfin, sline)) {
                istringstream siss(sline);
                string id, name;
                getline(siss, id, ','); getline(siss, name, ',');
                if (trim(id) == trim(sid)) {
                    cout << id << " - " << name << endl;
                    break;
                }
            }
        }
    }
}
void editStudent() {
    cout << "Enter Student ID to edit: ";
    string id; getline(cin, id);
    if (!studentExists(id)) { cout << "Student not found.\n"; return; }
    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, name, email, age, program, pwd;
        getline(iss, sid, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(sid) == id) {
            cout << "Edit Name (" << name << "): "; string n; getline(cin, n); if (!n.empty()) name = n;
            cout << "Edit Email (" << email << "): "; string e; getline(cin, e); if (!e.empty()) email = e;
            cout << "Edit Age (" << age << "): "; string a; getline(cin, a); if (!a.empty()) age = a;
            cout << "Edit Program (" << program << "): "; string p; getline(cin, p); if (!p.empty()) program = p;
            fout << sid << "," << name << "," << email << "," << age << "," << program << "," << pwd << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    if (edited) {
        Logger::getInstance()->log("Admin edited student " + id);
        cout << "Student updated.\n";
    }
}
void editCourse() {
    cout << "Enter Course Code to edit: ";
    string code; getline(cin, code);
    if (!courseExists(code)) { cout << "Course not found.\n"; return; }
    ifstream fin("courses.txt");
    ofstream fout("courses_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode, name, units;
        getline(iss, ccode, ','); getline(iss, name, ','); getline(iss, units, ',');
        if (trim(ccode) == code) {
            cout << "Edit Name (" << name << "): "; string n; getline(cin, n); if (!n.empty()) name = n;
            cout << "Edit Units (" << units << "): "; string u; getline(cin, u); if (!u.empty()) units = u;
            fout << ccode << "," << name << "," << units << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("courses.txt"); rename("courses_tmp.txt", "courses.txt");
    if (edited) {
        Logger::getInstance()->log("Admin edited course " + code);
        cout << "Course updated.\n";
    }
}
void deleteStudent() {
    cout << "Enter Student ID to delete: ";
    string id; getline(cin, id);
    if (!studentExists(id)) { cout << "Student not found.\n"; return; }
    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool deleted = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid;
        getline(iss, sid, ',');
        if (trim(sid) == id) {
            deleted = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    // Remove enrollments
    ifstream ein("enrollments.txt");
    ofstream eout("enrollments_tmp.txt");
    while (getline(ein, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ','); getline(iss, ccode, ',');
        if (trim(sid) != id) eout << sid << "," << ccode << endl;
    }
    ein.close(); eout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (deleted) {
        Logger::getInstance()->log("Admin deleted student " + id);
        cout << "Student deleted.\n";
    }
}
void deleteCourse() {
    cout << "Enter Course Code to delete: ";
    string code; getline(cin, code);
    if (!courseExists(code)) { cout << "Course not found.\n"; return; }
    ifstream fin("courses.txt");
    ofstream fout("courses_tmp.txt");
    string line;
    bool deleted = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode;
        getline(iss, ccode, ',');
        if (trim(ccode) == code) {
            deleted = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("courses.txt"); rename("courses_tmp.txt", "courses.txt");
    // Remove enrollments
    ifstream ein("enrollments.txt");
    ofstream eout("enrollments_tmp.txt");
    while (getline(ein, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ','); getline(iss, ccode, ',');
        if (trim(ccode) != code) eout << sid << "," << ccode << endl;
    }
    ein.close(); eout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (deleted) {
        Logger::getInstance()->log("Admin deleted course " + code);
        cout << "Course deleted.\n";
    }
}

// --- Student Features ---
void viewProfile(const string& id) {
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, name, email, age, program, pwd;
        getline(iss, sid, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(sid) == id) {
            cout << "\nID: " << sid << "\nName: " << name << "\nEmail: " << email
                 << "\nAge: " << age << "\nProgram: " << program << endl;
            return;
        }
    }
}
void enrollCourse(const string& sid) {
    cout << "Available courses:\n";
    ifstream fin("courses.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string code, name, units;
        getline(iss, code, ','); getline(iss, name, ','); getline(iss, units, ',');
        cout << code << " - " << name << " (" << units << " units)\n";
    }
    cout << "Enter Course Code to enroll: ";
    string code; getline(cin, code);
    if (!courseExists(code)) { cout << "Course not found.\n"; return; }
    if (isEnrolled(sid, code)) { cout << "Already enrolled in this course.\n"; return; }
    ofstream fout("enrollments.txt", ios::app);
    fout << sid << "," << code << endl;
    Logger::getInstance()->log("Student " + sid + " enrolled in " + code);
    cout << "Enrolled in course.\n";
}
void viewEnrolledCourses(const string& sid) {
    cout << "Enrolled courses:\n";
    ifstream fin("enrollments.txt");
    string line;
    bool found = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, code;
        getline(iss, id, ','); getline(iss, code, ',');
        if (trim(id) == sid) {
            // Get course name
            ifstream cfin("courses.txt");
            string cline;
            while (getline(cfin, cline)) {
                istringstream ciss(cline);
                string ccode, cname, units;
                getline(ciss, ccode, ','); getline(ciss, cname, ','); getline(ciss, units, ',');
                if (trim(ccode) == trim(code)) {
                    cout << ccode << " - " << cname << " (" << units << " units)\n";
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) cout << "None.\n";
}
void editProfile(const string& sid) {
    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, name, email, age, program, pwd;
        getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(id) == sid) {
            cout << "Edit Name (" << name << "): "; string n; getline(cin, n); if (!n.empty()) name = n;
            cout << "Edit Email (" << email << "): "; string e; getline(cin, e); if (!e.empty()) email = e;
            cout << "Edit Age (" << age << "): "; string a; getline(cin, a); if (!a.empty()) age = a;
            fout << id << "," << name << "," << email << "," << age << "," << program << "," << pwd << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    if (edited) {
        Logger::getInstance()->log("Student " + sid + " edited profile");
        cout << "Profile updated.\n";
    }
}
void dropCourse(const string& sid) {
    cout << "Enter Course Code to drop: ";
    string code; getline(cin, code);
    if (!isEnrolled(sid, code)) { cout << "Not enrolled in this course.\n"; return; }
    ifstream fin("enrollments.txt");
    ofstream fout("enrollments_tmp.txt");
    string line;
    bool dropped = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, ccode;
        getline(iss, id, ','); getline(iss, ccode, ',');
        if (trim(id) == sid && trim(ccode) == code) {
            dropped = true;
        } else {
            fout << id << "," << ccode << endl;
        }
    }
    fin.close(); fout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (dropped) {
        Logger::getInstance()->log("Student " + sid + " dropped course " + code);
        cout << "Dropped course.\n";
    }
}

// --- Admin Option Handler ---
bool Admin::handleOption(int opt) {
    switch (opt) {
        case 1: addStudent(); break;
        case 2: addCourse(); break;
        case 3: viewAllStudents(); break;
        case 4: viewAllCourses(); break;
        case 5: viewStudentsPerCourse(); break;
        case 6: editStudent(); break;
        case 7: editCourse(); break;
        case 8: deleteStudent(); break;
        case 9: deleteCourse(); break;
        case 10: Logger::getInstance()->log(getId() + " logged out"); return false;
        default: cout << "Invalid option.\n";
    }
    return true;
}

// --- Student Option Handler ---
bool Student::handleOption(int opt) {
    if (opt == 1) viewProfile(getId());
    else if (opt == 2) enrollCourse(getId());
    else if (opt == 3) viewEnrolledCourses(getId());
    else if (opt == 4) editProfile(getId());
    else if (opt == 5) dropCourse(getId());
    else if (opt == 6) { Logger::getInstance()->log(getId() + " logged out"); return false; }
    else cout << "Invalid option.\n";
    return true;
}

// --- Login ---
unique_ptr<User> login() {
    string username, password;
    cout << "Username (admin or student ID): ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    // Admin credentials (hardcoded for demo)
    if (username == "admin" && password == "admin123") {
        Logger::getInstance()->log("Admin logged in");
        return make_unique<Admin>("admin", "Administrator", "admin@school.edu", "admin123");
    }

    // Student credentials (from file)
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, name, email, age, program, pwd;
        getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(id) == username && trim(pwd) == password) {
            Logger::getInstance()->log("Student " + id + " logged in");
            return make_unique<Student>(id, name, email, pwd);
        }
    }
    throw runtime_error("Login failed: Invalid credentials.");
}

// --- Main ---
int main() {
    try {
        cout << "=== Student Management System ===\n";
        auto user = login();
        bool running = true;
        while (running) {
            user->menu();
            cout << "Select option: ";
            int opt;
            cin >> opt;
            cin.ignore();
            running = user->handleOption(opt);
        }
    } catch (const exception& ex) {
        cerr << ex.what() << endl;
        Logger::getInstance()->log(string("Login failed: ") + ex.what());
    }
    return 0;
}
