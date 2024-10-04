#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using uchar = unsigned char;

constexpr int RANGE = 256;
constexpr int THREAD_LIMIT = 4;
void CreateHistogram(uchar* inputData, int dataCount /* Length of inputData */, uint* result /*Output result*/){
  // initialize result array
  for (int i = 0; i < RANGE; ++i)
    result[i] = 0;
  
  auto chunk_length = dataCount / THREAD_LIMIT;

  uint32_t counts[THREAD_LIMIT][RANGE] = {0};
  
  auto thread_worker = [&](auto thread_id) {
    auto start = thread_id * chunk_length;
    auto end = (thread_id == THREAD_LIMIT - 1) ? dataCount : start + chunk_length;
    
    for (auto i = start; i < end; ++i)
      ++counts[thread_id][inputData[i]];
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < THREAD_LIMIT; ++i) {
    threads.emplace_back(thread_worker, i);
  }
  for (auto& t : threads)
    t.join();
  // sum over sub counts
  for (auto i = 0; i < RANGE; ++i) {
    for (auto j = 0; j < THREAD_LIMIT; ++j)
      result[i] += counts[j][i];
  }
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