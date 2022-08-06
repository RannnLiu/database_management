/* This is a skeleton code for two-pass multi-way sorting, you can make modifications as long as you meet 
   all question requirements*/  
//Currently this program will just load the records in the buffers (main memory) and print them when filled up.
//And continue this process untill the full Emp.csv is read. 

#include <bits/stdc++.h>
using namespace std;

//defines how many buffers are available in the Main Memory 
#define buffer_size 22

struct EmpRecord {
    int eid;
    string ename;
    int age;
    double salary;
};
EmpRecord buffers[buffer_size]; // this structure contains 22 buffers; available as your main memory.

// Grab a single block from the Emp.csv file, in theory if a block was larger than
// one tuple, this function would return an array of EmpRecords and the entire if
// and else statement would be wrapped in a loop for x times based on block size
EmpRecord Grab_Emp_Record(fstream &empin) {
    string line, word;
    EmpRecord  emp;
    // grab entire line
    if (getline(empin, line, '\n')) {
        // turn line into a stream
        stringstream s(line);
        // gets everything in stream up to comma
        getline(s, word,',');
        emp.eid = stoi(word);
        getline(s, word, ',');
        emp.ename = word;
        getline(s, word, ',');
        emp.age = stoi(word);
        getline(s, word, ',');
        emp.salary = stod(word);
        return emp;
    } else {
        emp.eid = -1;
        return emp;
    }
}

vector<EmpRecord> get_info(fstream &input, vector<EmpRecord>& res){
    string line, word;
    EmpRecord  emp;
    while(getline(input, line, '\n')){
        stringstream s(line);
        getline(s, word,',');
        emp.eid = stoi(word);
        getline(s, word, ',');
        emp.ename = word;
        getline(s, word, ',');
        emp.age = stoi(word);
        getline(s, word, ',');
        emp.salary = stod(word);
        res.push_back(emp);
    }
    return res;
}

void write_tmp_scv(int pos, int cur_size){
	fstream output_file;
	string filename = to_string(pos)+".csv";
	output_file.open(filename, ios::out);
	for (int i = 0; i < cur_size; i++)
	{
		output_file << fixed << buffers[i].eid << ',' << buffers[i].ename << ',' << buffers[i].age << ',' << setprecision(0) << buffers[i].salary << "\n";
	}
	output_file.close();
}

//Printing whatever is stored in the buffers of Main Memory 
//Can come in handy to see if you've sorted the records in your main memory properly.
void Print_Buffers(int cur_size) {
    for (int i = 0; i < cur_size; i++)
    {
        cout << i << " " << buffers[i].eid << " " << buffers[i].ename << " " << buffers[i].age << " " << buffers[i].salary << endl;
    }
}

//Sorting the buffers in main_memory based on 'eid' and storing the sorted records into a temporary file 
//You can change return type and arguments as you want. 
void Sort_in_Main_Memory(int size, int pos, vector<tuple<int, int, int>>& recorder){
    for(int i = 0; i < size; ++i){
        //int t = buffers[i].eid
        recorder.push_back(make_tuple(buffers[i].eid, pos, i));
    }
    return;
}

//You can use this function to merge your M-1 runs using the buffers in main memory and storing them in sorted file 'EmpSorted.csv'(Final Output File) 
//You can change return type and arguments as you want. 
void Merge_Runs_in_Main_Memory(vector<tuple<int, int, int>> recorder, fstream& output, int num){
    string input_file;
    fstream run[num];
    vector<EmpRecord> tmp;
    vector<vector<EmpRecord>> line;
    for(int i = 0; i < num; ++i){
		input_file = to_string(i)+".csv";
		run[i].open(input_file, ios::in);
        get_info(run[i], tmp);
        line.push_back(tmp);
        tmp.clear();
	}
    for(int i = 0; i < num ; ++i){
        run[i].close();
    }
    for(int i = 0; i < recorder.size(); ++i){
        line = {};
        int id = get<0>(recorder[i]);
        int p = get<1>(recorder[i]);
        int l = get<2>(recorder[i]);
        cout << id << ", " << p << ", " << l << endl;
        cout << line[p][l].eid << "," << line[p][l].ename << "," << line[p][l].age << "," << line[p][l].salary << endl;
        output << line[p][l].eid << "," << line[p][l].ename << "," << line[p][l].age << "," << line[p][l].salary << endl;
    }
    line.clear();
    output.close();
}

void clearn_mem(int pos){
	string input_file;
	for(int i = 0; i < pos; ++i){
		input_file = to_string(i)+".csv";
		if(remove(input_file.c_str()) == 0){
			cout << "file removed" << endl;
		}
		else{
			cout << "file not removed" << endl;
		}
	}
}

int main() {
  // open file streams to read and write
  fstream input_file;
  input_file.open("Emp.csv", ios::in);
 
  // flags check when relations are done being read
  bool flag = true;
  int cur_size = 0;
  int pos = 0;

  vector<tuple<int, int, int>> recorder;
  
  /*First Pass: The following loop will read each block put it into main_memory 
    and sort them then will put them into a temporary file for 2nd pass */
  while (flag) {
      // FOR BLOCK IN RELATION EMP
      // grabs a block
      EmpRecord single_EmpRecord = Grab_Emp_Record(input_file);
      // checks if filestream is empty
      if (single_EmpRecord.eid == -1) {
          flag = false;
          Sort_in_Main_Memory(cur_size, pos, recorder);
          write_tmp_scv(pos, cur_size);
          ++pos;
          //Print_Buffers(cur_size); // The main_memory is not filled up but there are some leftover data that needs to be sorted.
      }
      if(cur_size + 1 <= buffer_size){
          //Memory is not full store current record into buffers.
          buffers[cur_size] = single_EmpRecord ;
          cur_size += 1;
      }
      else{
          //memory is full now. Sort the blocks in Main Memory and Store it in a temporary file (runs)
          //cout << "Main Memory is full. Time to sort and store sorted blocks in a temporary file" << endl;
          //Print_Buffers(buffer_size);
          //SortMain("Attributes You Want");
          Sort_in_Main_Memory(buffer_size, pos, recorder);
          write_tmp_scv(pos, buffer_size);
          //Print_Buffers(buffer_size);
          //After sorting start again. Clearing memory and putting the current one into main memory.
          cur_size = 0;
          buffers[cur_size] = single_EmpRecord;
          cur_size += 1;
          ++pos;
      }
      
  }
  sort(recorder.begin(), recorder.end());
  /*for(int i; i < recorder.size(); ++i){
      int a = get<0>(recorder[i]);
      int b = get<1>(recorder[i]);
      int c = get<2>(recorder[i]);
      cout << a << ", " << b << ", " << c <<endl;
  }*/
  input_file.close();
  
  /* Implement 2nd Pass: Read the temporary sorted files and utilize main_memory to store sorted runs into the EmpSorted.csv*/

  //Uncomment when you are ready to store the sorted relation
  fstream sorted_file;  
  sorted_file.open("EmpSorted.csv", ios::out | ios::app);

  //Pseudocode
  bool flag_sorting_done = false;
  while(!flag_sorting_done){
      Merge_Runs_in_Main_Memory(recorder, sorted_file, pos);
      break;
  }
  clearn_mem(pos);
  
  //You can delete the temporary sorted files (runs) after you're done if you want. It's okay if you don't.

  return 0;
}
