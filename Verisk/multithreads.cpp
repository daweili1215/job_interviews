#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using uchar = unsigned char;
std::mutex mtx;

/*
Parallel  Programming Exercise
Given an array of uchars, implement the multithreading application to determine the frequency of distinct
values in the array. From the function prototype below, accessing result[0] would give the frequency of the
value 0, result[1] would give the frequency of value 1, etc.  
dataCount: Length of inputData
result: Output result
CreateHistogram (uchar* inputData, int dataCount, uint* result){}
*/


const int RANGE = 256;
const int THREAD_LIMIT = 4;
void CreateHistogram(uchar* inputData, int dataCount /* Length of inputData */, uint* result /*Output result*/){
  // initialize result array
  for (int i = 0; i < RANGE; ++i)
    result[i] = 0;
  
  std::vector<std::thread> threads;
  auto chunk_length = dataCount / THREAD_LIMIT;
  
  auto thread_worker = [&](auto start, auto end) {
    uint32_t counts[RANGE] = {0};
    
    for (auto i = start; i < end; ++i)
      ++counts[inputData[i]];
    
    std::lock_guard<std::mutex> lock(mtx);
    for (auto i = 0; i < RANGE; ++i)
      result[i] += counts[i];
  };
  
  for (int i = 0; i < THREAD_LIMIT; ++i) {
    auto start = i * chunk_length;
    auto end = (i == THREAD_LIMIT - 1) ? dataCount : (i + 1) * chunk_length;
    threads.emplace_back(thread_worker, start, end);
  }
  
  for (auto& t : threads)
    t.join();
}

void print_content(uint32_t* counts) {
  for (int i = 0; i < RANGE; ++i) {
    if (counts[i] > 0) 
      cout << "counts of " << i << ": " << counts[i] << endl;
  }
}

int main() {
  uchar data[] = {0,1,2,3,4,5,4,3,2,1,100,100,255,255};
  int data_size = sizeof(data) / sizeof(uchar);
  uint32_t histogram[256] = {0};
  CreateHistogram(data, data_size, histogram);
  
  print_content(histogram);
  
  return 0;
}