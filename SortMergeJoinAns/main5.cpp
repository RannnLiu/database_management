/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  

#include <bits/stdc++.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

// copare function
bool compare_function_emp(Records a, Records b){
    return a.emp_record.eid < b.emp_record.eid;
}

bool compare_function_dept(Records a, Records b){
    return a.dept_record.managerid < b.dept_record.managerid;
}

//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs) 
void Sort_Buffer(string input, int pos, int size){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs
    string name = input;
    input += to_string(pos)+".csv";
    fstream output_file;
    output_file.open(input, ios::out);
    // cout << input << endl;
    if(name.compare("emp") == 0){
        sort(buffers, buffers+size, compare_function_emp);
        for(int i = 0; i < size; ++i){
            output_file << buffers[i].emp_record.eid << "," << buffers[i].emp_record.ename << "," << buffers[i].emp_record.age << "," << setprecision(7) << buffers[i].emp_record.salary << "\n";
        }
    }
    if(name.compare("dept") == 0){
        sort(buffers, buffers+size, compare_function_dept);
        for(int i = 0; i < size; ++i){
            output_file << buffers[i].dept_record.did << "," << buffers[i].dept_record.dname << "," << setprecision(7) << buffers[i].dept_record.budget << "," << buffers[i].dept_record.managerid << "\n";
        }
    }
    output_file.close();
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
void PrintJoin(vector<Records> e, vector<Records> d, fstream& joinout){
    cout << "The Join info: " << endl;
    for(int i = 0; i < e.size(); ++i){
        cout << d[i].dept_record.did << ", " << d[i].dept_record.dname << ", " << setprecision(7) << d[i].dept_record.budget << ", " << d[i].dept_record.managerid << ", " << e[i].emp_record.ename << ", " << e[i].emp_record.age << ", " << setprecision(7) << e[i].emp_record.salary << endl;
        joinout << d[i].dept_record.did << "," << d[i].dept_record.dname << "," << setprecision(7) << d[i].dept_record.budget << "," << d[i].dept_record.managerid << "," << e[i].emp_record.ename << "," << e[i].emp_record.age << "," << setprecision(7) << e[i].emp_record.salary << "\n";
    }
    joinout.close();
}

//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void Merge_Join_Runs(int emp_run, int dept_run, fstream& joinout){
    vector<Records> match_emp;
    vector<Records> match_dept;
    //cout << emp_run << ", " << dept_run << endl;
    for(int i = 0; i < emp_run; ++i){
        fstream empin;
        string emp_name = "emp"+to_string(i)+".csv";
        empin.open(emp_name, ios::in);
        vector<Records> emp_record;
        while(true){
            Records single_record = Grab_Emp_Record(empin);
            if(single_record.no_values == -1) break;
            emp_record.push_back(single_record);
        }
        //cout << "emp_size: " << "======> " << emp_record.size() << endl;
        int emp_line = 0, dept_line = 0;
        for(int j = 0; j < dept_run; ++j){
            fstream deptin;
            string dept_name = "dept"+to_string(j)+".csv";
            deptin.open(dept_name, ios::in);
            vector<Records> dept_record;
            while(true){
                Records single_record = Grab_Dept_Record(deptin);
                if(single_record.no_values == -1) break;
                dept_record.push_back(single_record);
            }
            //cout << "dept_size: " << "======> " << dept_record.size() << endl;
            while(emp_line < emp_record.size() && dept_line < dept_record.size()){
                // cout << emp_record[emp_line].emp_record.eid << ", " << dept_record[dept_line].dept_record.managerid << endl;
                if(emp_record[emp_line].emp_record.eid == dept_record[dept_line].dept_record.managerid){
                    // cout << "yes" << endl;
                    match_emp.push_back(emp_record[emp_line]);
                    match_dept.push_back(dept_record[dept_line]);
                    ++dept_line;
                    ++emp_line;
                }
                else if(emp_record[emp_line].emp_record.eid > dept_record[dept_line].dept_record.managerid){
                    ++dept_line;
                }
                else{
                    ++emp_line;
                }
            }
            dept_record.clear();
        }
        emp_record.clear();
    }
    sort(match_emp.begin(), match_emp.end(), compare_function_emp);
    sort(match_dept.begin(), match_dept.end(), compare_function_dept);
    //and store the Joined new tuples using PrintJoin()
    PrintJoin(match_emp, match_dept, joinout);
}

void clearn_mem(string input, int pos){
    string name;
    for(int i = 0; i < pos; ++i){
        name = input+to_string(i)+".csv";
        if(remove(name.c_str()) == 0){
            cout << "file removed" << endl;
        }
        else{
            cout << name << "file not removed" << endl;
        }
    }
}

int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);

   
    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);

    // flags check when relations are done being read
    bool flag_emp = true;
    bool flag_dept = true;
    int pos_emp = 0;
    int pos_dept = 0;

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    // create emp run
    while(flag_emp){
        Records single_record = Grab_Emp_Record(empin);
        int n = buffers->number_of_emp_records;
        if(single_record.no_values == -1){
            flag_emp = false;
            // store in csv
            Sort_Buffer("emp", pos_emp, n);
            ++pos_emp;
        }
        if(n+1 <= buffer_size){
            buffers[n] = single_record;
            buffers->number_of_emp_records = n+1;
        }
        else{
            buffers->number_of_emp_records = 0;
            // store in csv
            Sort_Buffer("emp", pos_emp, n);
            ++pos_emp;
        }
    }
    // create dept run
    while(flag_dept){
        Records single_record = Grab_Dept_Record(deptin);
        int n = buffers->number_of_dept_records;
        if(single_record.no_values == -1){
            flag_dept = false;
            // store in csv
            Sort_Buffer("dept", pos_dept, n);
            ++pos_dept;
        }
        if(n+1 <= buffer_size){
            buffers[n] = single_record;
            buffers->number_of_dept_records = n+1;
        }
        else{
            buffers->number_of_dept_records = 0;
            // store in csv
            Sort_Buffer("dept", pos_dept, n);
            ++pos_dept;
        }
    }
    empin.close();
    deptin.close();
    
    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations 
    Merge_Join_Runs(pos_emp, pos_dept, joinout);

    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv
    clearn_mem("emp", pos_emp);
    clearn_mem("dept", pos_dept);

    return 0;
}
