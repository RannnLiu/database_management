#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include <math.h>
using namespace std;

class Record {
public:
    int id, manager_id;
    string bio, name;

    Record(vector<string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class LinearHashIndex {

private:
    const int PAGE_SIZE = 4096;

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    int numBlocks; // n
    int i;
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to

    string fName;
    // Linear hash table
    // vector<vector<Record>> Linear_hash(2, vector<Record>());
    vector<vector<int>> linear_hash;

    // bit flip
    string bit_flip(string bit_value){
        string tmp = bit_value;
        tmp[bit_value.size() - i] = '0';
        return tmp;
    }

    int binary_to_int(string b){
        int sum = 0;
        for(int x = b.size()-1; x > -1; --x){
            sum += pow(2, b.size()-x-1) * (b[x]-'0');
        }
        return sum;
    }

    void print_table(vector<vector<int>> a){
        cout << endl;
        for(int x = 0; x < a.size(); ++x){
            cout << "tabel " << x << ":";
            for(int y = 0; y < a[x].size(); ++y){
                cout << a[x][y] << " ";
            }
            cout << endl;
        }
        cout << "------------------------------" << endl;
    }

    void re_build(vector<vector<int>>& table){
        vector<vector<int>> tmp_table;
        tmp_table = table;
        table.clear();
        for(int x = 0; x < tmp_table.size(); ++x){
            table.push_back({});
        }
        // print_table(tmp_table);
        
        for(int x = 0; x < tmp_table.size(); ++x){
            for(int y = 0; y < tmp_table[x].size(); ++y){
                //cout << x << ", " << y << endl;
                // transfer hash value to bit type
                string bit_value = bitset<16>(tmp_table[x][y]).to_string();

                // if bit bigger than n then flip bit
                string tmp_bit_value = bit_value.substr(bit_value.size()-i, bit_value.size());
                // cout << "tmp_bit_value" << tmp_bit_value << endl;
                if(binary_to_int(tmp_bit_value) >= numBlocks/2){
                    tmp_bit_value = bit_flip(bit_value);
                    tmp_bit_value = tmp_bit_value.substr(tmp_bit_value.size()-i, tmp_bit_value.size());
                }
                //cout << i << endl;
                //cout << "tmp_bit_value: " << tmp_bit_value << endl;
                // add hash value to blocks
                //cout  << "binary_to_int(tmp_bit_value): " << binary_to_int(tmp_bit_value) << endl;
                table[binary_to_int(tmp_bit_value)].push_back(tmp_table[x][y]);
            }
        }
        tmp_table.clear();
    }

    // Insert new record into index
    void insertRecord(Record record) {
        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
        }

        // number of blocks
        numBlocks = linear_hash.size()*2;
        //cout << "--------------------------" << endl;
        //cout << "Before: " << endl;
        //print_table(linear_hash);

        // Add record to the index in the correct block, creating overflow block if necessary
        // compute hash value (won't change it, jut used to compare and find position)
        int hash_value = record.id % (int)(pow(2.0, 16.0));

        // compute index number
        i = ceil(log2(numBlocks/2));

        // transfer hash value to bit type
        string bit_value = bitset<16>(hash_value).to_string();
        //cout << "numRecords:" << numRecords << endl;
        //cout << "numBlocks:" << numBlocks << endl;
        //cout << "bit_value: " << bit_value << endl;
        //cout << "hash value: " << hash_value << endl;

        // if bit bigger than n then flip bit
        string tmp_bit_value = bit_value.substr(bit_value.size()-i, bit_value.size());
        //cout << "tmp_bit_value: " << tmp_bit_value << endl;
        if(binary_to_int(tmp_bit_value) >= numBlocks/2){
            tmp_bit_value = bit_flip(bit_value);
            tmp_bit_value = tmp_bit_value.substr(tmp_bit_value.size()-i, tmp_bit_value.size());
        }

        // add hash value to blocks
        // linear_hash[binary_to_int(tmp_bit_value)].push_back(record);
        //cout << "binary_to_int(tmp_bit_value): " << binary_to_int(tmp_bit_value) << endl;
        linear_hash[binary_to_int(tmp_bit_value)].push_back(hash_value);
        ++numRecords;

        // Take neccessary steps if capacity is reached
        // if capacity is reached 70%, n+1 and  re-build hash table
        if(numRecords >= 0.7*numBlocks){
            linear_hash.push_back({});
            numBlocks = linear_hash.size()*2;
            i = ceil(log2(numBlocks/2));
            //cout << "numBlocks: " << numBlocks << endl;
            //cout << "i: " << i << endl;
            re_build(linear_hash);
        }

        // print linear hash table
        // cout << "--------------------------" << endl;
        // cout << "After: " << endl;
        // print_table(linear_hash);
    }

    // using hash table to reocrd data
    void save_table(vector<vector<int>> table, string csvFName){
        ifstream in_file;
        in_file.open(csvFName);

        vector<string> record_data;
        string line, field;

        fstream output_file;
        string filename = fName+".csv";
        output_file.open(filename, ios::out);
        while(getline(in_file, line, '\n')){
            stringstream ss(line);
            // id
            getline(ss, field, ',');
            record_data.push_back(field);
            // name
            getline(ss, field, ',');
            record_data.push_back(field);
            // bio
            getline(ss, field, ',');
            record_data.push_back(field);
            // manager_id
            getline(ss, field, ',');
            record_data.push_back(field);

            Record newRecord(record_data);

            // using id to find out where to save
            int hash_value = newRecord.id % (int)(pow(2.0, 16.0));
            string bit_value = bitset<16>(hash_value).to_string();
            string tmp_bit_value = bit_value.substr(bit_value.size()-i, bit_value.size());
            if(binary_to_int(tmp_bit_value) >= table.size()){
                tmp_bit_value = bit_flip(bit_value);
                tmp_bit_value = tmp_bit_value.substr(tmp_bit_value.size()-i, tmp_bit_value.size());
            }
            int pos;
            for(pos = 0; pos < table[binary_to_int(tmp_bit_value)].size(); ++pos){
                if(table[binary_to_int(tmp_bit_value)][pos] == hash_value) break;
            }
            //cout << pos << endl;
            // compute csv position (expect maximum block number is 3)
            int position = binary_to_int(tmp_bit_value)*3+pos;
            // cout << binary_to_int(tmp_bit_value) << ", " << pos << ": " << position << endl;
            output_file << position << "," << newRecord.id << "," << newRecord.name << "," << newRecord.bio << "," << newRecord.manager_id << "\n";
            record_data.clear();
        }
        output_file.close();
    }

public:
    LinearHashIndex(string indexFileName) {
        numBlocks = 0;
        i = 0;
        numRecords = 0;
        numBlocks = 0;
        fName = indexFileName;
        // Initialize index with first blocks (start with 2)
        for(int x = 0; x < 2; ++x){
            linear_hash.push_back({});
        }
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        ifstream in_file;
        in_file.open(csvFName);

        if(!in_file) cerr << "Cannot open file..." << endl;
        vector<string> record_data;
        string line, field;

        while(getline(in_file, line, '\n')){
            stringstream ss(line);
            // id
            getline(ss, field, ',');
            record_data.push_back(field);
            // name
            getline(ss, field, ',');
            record_data.push_back(field);
            // bio
            getline(ss, field, ',');
            record_data.push_back(field);
            // manager_id
            getline(ss, field, ',');
            record_data.push_back(field);

            Record newRecord(record_data);
            insertRecord(newRecord);

            record_data.clear();
        }
        save_table(linear_hash, csvFName);
        in_file.close();
        //print_table(linear_hash);
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        int hash_value = id % (int)(pow(2.0, 16.0));
        string bit_value = bitset<16>(hash_value).to_string();
        string tmp_bit_value = bit_value.substr(bit_value.size()-i, bit_value.size());
        if(binary_to_int(tmp_bit_value) >= numBlocks/2){
            tmp_bit_value = bit_flip(bit_value);
            tmp_bit_value = tmp_bit_value.substr(tmp_bit_value.size()-i, tmp_bit_value.size());
        }
        ifstream in_file;
        in_file.open(fName+".csv");

        vector<string> record_data;
        string line, field;
        int code;
        vector<string> nf;
        nf.push_back("404");
        nf.push_back("no_this");
        nf.push_back("data");
        nf.push_back("404");
        Record no_found(nf);

        while(getline(in_file, line, '\n')){
            stringstream ss(line);
            getline(ss, field, ',');
            code = stoi(field);
            int pos;
            for(pos = 0; pos < linear_hash[binary_to_int(tmp_bit_value)].size(); ++pos){
                if(linear_hash[binary_to_int(tmp_bit_value)][pos] == hash_value) break;
            }
            //cout << pos << endl;
            // compute csv position (expect maximum block number is 3)
            int position = binary_to_int(tmp_bit_value)*3+pos;
            if(code == position){
                // id
                getline(ss, field, ',');
                record_data.push_back(field);
                // name
                getline(ss, field, ',');
                record_data.push_back(field);
                // bio
                getline(ss, field, ',');
                record_data.push_back(field);
                // manager_id
                getline(ss, field, ',');
                record_data.push_back(field);

                Record tmp(record_data);
                record_data.clear();
                return tmp;
            }
        }
        return no_found;
    }
};
