//
//This program reads two files containing: (file 1)a list of cities and 
//(file 2) a list of safety indices. Then according to user input, the 
//program uses linear/binary search to determine how many safety indices
//are present in the cities. The result is printed to a user-specified
//file, and the run-time is printed on the command line
//
//By: Richard Qin ID: 917636088
//For ECS36C HW1
//Professor Porquet 
//
//first draft on: 4/7/2020
//  wrote city class and file reading code
//second draft on: 4/8/2020
//  wrote searching algorithms and error checks
//third draft on: 4/9/2020
//  wrote time measurement code
//
//References:
//  HW instruction slides: counter/switch statement/linear search/sort
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <cctype>
#include <chrono>

//represents a timer, records runtime
class Counter {
 public:
  Counter() { 
    start_ = std::chrono::high_resolution_clock::now(); 
    end_ = std::chrono::high_resolution_clock::now();
  }
  
  void resetCount() { 
    start_ = std::chrono::high_resolution_clock::now();
  }
  void endCount() {
    end_ = std::chrono::high_resolution_clock::now();
  }
  double getTimeElapsed() {
    return std::chrono::duration<double, std::micro>(end_ - start_).count();
  }
 private:
  std::chrono::high_resolution_clock::time_point start_;
  std::chrono::high_resolution_clock::time_point end_;
};

class City {
 public:
  City(double p, double c, double r) 
      : population_(p), case_num_(c), recoveries_(r) {
    safety_index_ = (int)(10000*(2.0*(1-case_num_/population_)
    +1/2.0*(recoveries_/case_num_)));
  }

  double population() { return population_; }
  double caseNum() { return case_num_; }
  double recoveries() { return recoveries_; }
  int calcSafetyIndex() { return safety_index_; }

  friend std::ostream& operator<<(std::ostream& stream, const City &c);
  friend bool operator< (const City& c1, const City& c2);

 private:
  double population_;
  double case_num_;
  double recoveries_;
  int safety_index_;
};

std::ostream& operator<<(std::ostream& stream, const City &c) {
  stream << "CITY: Population: " << c.population_ << " Cases: " << c.case_num_
  << " Recoveries: " << c.recoveries_ << " index: " << c.safety_index_;
  return stream;
}

bool operator<(const City& c1, const City& c2) {
  int c1_index = c1.safety_index_;
  int c2_index = c2.safety_index_;
  
  if (c1_index==c2_index) {
    return (c1.population_ < c2.population_);
  } else {
    return (c1_index < c2_index);
  }
}

int main(int argc, char *argv[]) {
  //confirms that arguments are valid
  if (argc!=4) {
    std::cerr << "Usage: " << argv[0] 
        << " <city_file.dat> <safety_file.dat> <result_file.dat>" 
        << std::endl;
    return 0;
  }

  std::ifstream city_file;
  std::ifstream safety_file;
  std::ofstream result_file(argv[3]);
  std::vector<City> cities;
  std::vector<int> safety_indices;
  
  city_file.open(argv[1]);
  safety_file.open(argv[2]);
  
  //A line of text from safety_indices file
  std::string line_s;
  
  //used to load city_file data into City objects
  if (city_file.is_open()) {
    int pop = 0, cases = 0, rec = 0;
    //adds cities until EOF is reached
    while (city_file >> pop >> cases >> rec){
      cities.push_back(*(new City(pop, cases, rec)));
    }
  } else {
    std::cerr << "Error: cannot open file " << argv[1] << std::endl;
  }

  //used to read and store the safety indices
  if (safety_file.is_open()) {
    while (getline(safety_file,line_s)) {
      safety_indices.push_back(stod(line_s));
    }
  } else {
    std::cerr << "Error: cannot open file " << argv[2] << std::endl;
  }
  

  //does user want to use linear or binary search?
  std::cout << "Choice of search method ([l]inear, [b]inary)?" << std::endl;
  char input; 
  bool valid_input = false;
  int count = 0;
  Counter ct;
  do {
    std::cin >> input;
    ct.resetCount();
    switch (std::tolower(input)) {
      case 'l': {
        //do linear search
        for (unsigned int i=0; i<safety_indices.size(); i++) {
          for (unsigned int j=0; j<cities.size(); j++) {
            if (safety_indices[i] == cities[j].calcSafetyIndex()) { 
              count++;
              break;
            }
          }
        }
        valid_input = true;
        break;
      } case 'b': {
        //do binary search
        std::sort(cities.begin(), cities.end());

        for (unsigned int i=0; i<safety_indices.size(); i++) {
          int mid_index;
          int upper_index = cities.size()-1, lower_index = 0;
          while (lower_index<=upper_index) {
            mid_index = lower_index + ((upper_index - lower_index)/2);
            int curr_safety = cities[mid_index].calcSafetyIndex();
            if (curr_safety==safety_indices[i]) {
              count++;
              break;
            } else if (curr_safety<safety_indices[i]) {
              lower_index = mid_index + 1;
            } else {
              upper_index = mid_index - 1;
            }
          }
        }         
        valid_input = true;
        break;
      } default: {
        std::cerr << "Incorrect choice" << std::endl;
      }
    }
  } while (!valid_input);

  ct.endCount();

  std::cout << "CPU time: " << ct.getTimeElapsed() << " microseconds"
      << std::endl;
  result_file << count << std::endl;
 
  
  return 0;
}
