#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum Speciality { COMPUTER_SCIENCE, INFORMATICS, MATH_ECON, PHYS_INF, HANDICRAFT };
string specialityStr[] = { "Computer Science", "Informatics", "Math & Econ", "Phys & Inf", "Handicraft" };

union ThirdGrade {
    int programming;
    int numerical_methods;
    int pedagogy;
};

struct Student {
    char surname[51];
    int course;
    Speciality speciality;
    int physics;
    int math;
    ThirdGrade thirdGrade;
};

struct IndexRecord {
    int id;
    int course;
    double avg;
    string surname;
};

void create(const string filename);
void display(const string filename);
void createIndexFile(const string filename, vector<IndexRecord>& index);
void sortIndex(vector<IndexRecord>& index);
void displayIndexed(const string filename, const vector<IndexRecord>& index);
void binarySearch(const string filename, const vector<IndexRecord>& index);
double calculateAverage(const Student& s);

int main() {
    string filename;
    vector<IndexRecord> index;
    int menuItem;

    cout << "Enter filename: ";
    cin >> filename;

    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Create/Add records to file\n";
        cout << "2. Display file (table)\n";
        cout << "3. Create index array and display sorted\n";
        cout << "4. Binary search by index\n";
        cout << "0. Exit\n";
        cout << "Selection: ";
        cin >> menuItem;

        switch (menuItem) {
        case 1: create(filename); break;
        case 2: display(filename); break;
        case 3:
            createIndexFile(filename, index);
            sortIndex(index);
            displayIndexed(filename, index);
            break;
        case 4:
            if (index.empty()) cout << "Create index first (option 3)!\n";
            else binarySearch(filename, index);
            break;
        }
    } while (menuItem != 0);

    return 0;
}

void create(const string filename) {
    ofstream f(filename, ios::binary | ios::app);
    int n;
    cout << "How many students to add? "; cin >> n;

    for (int i = 0; i < n; i++) {
        Student s;
        cout << "Surname: "; cin >> s.surname;
        cout << "Course: "; cin >> s.course;
        int spec;
        cout << "Speciality (0-CS, 1-INF, 2-ME, 3-PI, 4-HC): "; cin >> spec;
        s.speciality = (Speciality)spec;
        cout << "Physics grade: "; cin >> s.physics;
        cout << "Math grade: "; cin >> s.math;

        if (s.speciality == COMPUTER_SCIENCE) {
            cout << "Programming grade: "; cin >> s.thirdGrade.programming;
        } else if (s.speciality == INFORMATICS) {
            cout << "Numerical methods grade: "; cin >> s.thirdGrade.numerical_methods;
        } else {
            cout << "Pedagogy grade: "; cin >> s.thirdGrade.pedagogy;
        }
        f.write((char*)&s, sizeof(Student));
    }
    f.close();
}

double calculateAverage(const Student& s) {
    int third = 0;
    if (s.speciality == COMPUTER_SCIENCE) third = s.thirdGrade.programming;
    else if (s.speciality == INFORMATICS) third = s.thirdGrade.numerical_methods;
    else third = s.thirdGrade.pedagogy;
    return (s.physics + s.math + third) / 3.0;
}

void printHeader() {
    cout << "========================================================================================================\n";
    cout << "| # | Surname       | Course | Speciality         | Phy | Mat | Prg | Num | Ped |\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";
}

void printStudent(int i, const Student& s) {
    cout << "|" << setw(3) << i + 1 << " |" << setw(13) << s.surname << " |" << setw(6) << s.course << " |" << setw(18) << specialityStr[s.speciality] << " |"
         << setw(4) << s.physics << " |" << setw(4) << s.math << " |";
    
    if (s.speciality == COMPUTER_SCIENCE) cout << setw(4) << s.thirdGrade.programming << " |" << setw(5) << " " << " |" << setw(4) << " " << " |\n";
    else if (s.speciality == INFORMATICS) cout << setw(4) << " " << " |" << setw(5) << s.thirdGrade.numerical_methods << " |" << setw(4) << " " << " |\n";
    else cout << setw(4) << " " << " |" << setw(5) << " " << " |" << setw(4) << s.thirdGrade.pedagogy << " |\n";
}

void display(const string filename) {
    ifstream f(filename, ios::binary);
    Student s;
    printHeader();
    int i = 0;
    while (f.read((char*)&s, sizeof(Student))) {
        printStudent(i++, s);
    }
    cout << "========================================================================================================\n";
    f.close();
}

void createIndexFile(const string filename, vector<IndexRecord>& index) {
    index.clear();
    ifstream f(filename, ios::binary);
    Student s;
    int id = 0;
    while (f.read((char*)&s, sizeof(Student))) {
        index.push_back({ id++, s.course, calculateAverage(s), s.surname });
    }
    f.close();
}

void sortIndex(vector<IndexRecord>& index) {
    sort(index.begin(), index.end(), [](const IndexRecord& a, const IndexRecord& b) {
        if (a.course != b.course) return a.course < b.course;
        if (a.avg != b.avg) return a.avg < b.avg;
        return a.surname < b.surname;
    });
}

void displayIndexed(const string filename, const vector<IndexRecord>& index) {
    ifstream f(filename, ios::binary);
    printHeader();
    for (size_t i = 0; i < index.size(); i++) {
        Student s;
        f.seekg(index[i].id * sizeof(Student));
        f.read((char*)&s, sizeof(Student));
        printStudent(i, s);
    }
    cout << "========================================================================================================\n";
    f.close();
}

void binarySearch(const string filename, const vector<IndexRecord>& index) {
    string searchSurname;
    int searchCourse;
    double searchAvg;
    cout << "Search criteria:\nSurname: "; cin >> searchSurname;
    cout << "Course: "; cin >> searchCourse;
    cout << "Avg grade: "; cin >> searchAvg;

    int left = 0, right = index.size() - 1;
    bool found = false;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (index[mid].course == searchCourse && abs(index[mid].avg - searchAvg) < 0.01 && index[mid].surname == searchSurname) {
            cout << "Student found at index position " << mid + 1 << ".\n";
            found = true;
            break;
        }
        if (index[mid].course < searchCourse || 
           (index[mid].course == searchCourse && index[mid].avg < searchAvg) ||
           (index[mid].course == searchCourse && index[mid].avg == searchAvg && index[mid].surname < searchSurname))
            left = mid + 1;
        else
            right = mid - 1;
    }

    if (!found) cout << "Student not found.\n";
}
