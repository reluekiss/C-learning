#include <assert.h>
// Taken from: https://en.wikipedia.org/wiki/Circular_buffer#Circular_buffer_mechanics
enum { N = 10000 };  // size of circular buffer

int buffer [N]; // note: only (N - 1) elements can be stored at a given time
int writeIndx = 0;
int readIndx  = 0;

int put (int item) 
{
  if ((writeIndx + 1) % N == readIndx)
  {
     // buffer is full, avoid overflow
     return 0;
  }
  buffer[writeIndx] = item;
  writeIndx = (writeIndx + 1) % N;
  return 1;
}

int get (int * value) 
{
  if (readIndx == writeIndx)
  {
     // buffer is empty
     return 0;
  }

  *value = buffer[readIndx];
  readIndx = (readIndx + 1) % N;
  return 1;
}


int main (void)
{
    int result, value;
    
    // Initially empty: get should fail.
    result = get(&value);
    assert(result == 0);
    
    // Fill the buffer with (N - 1) items.
    for (int i = 0; i < N - 1; i++) {
        result = put(i);
        assert(result == 1);
    }
    
    // Buffer full: next put should fail.
    result = put(100);
    assert(result == 0);
    
    // Retrieve all items and verify order.
    for (int i = 0; i < N - 1; i++) {
        result = get(&value);
        assert(result == 1);
        assert(value == i);
    }
    
    // Buffer empty: get should fail.
    result = get(&value);
    assert(result == 0);
    
    return 0;
}
