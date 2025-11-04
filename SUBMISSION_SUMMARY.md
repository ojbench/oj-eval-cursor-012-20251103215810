# Submission Summary - Problem 012 (LinkedHashMap)

## Result
✅ **PERFECT SCORE: 100/100 on First Submission**

## Submission Details
- **Submission ID**: 707179
- **Status**: Accepted
- **Score**: 100/100
- **Total Time**: 33.773 seconds
- **Peak Memory**: 438 MB
- **Attempts Used**: 1 out of 6 allowed

## Test Results
All 12 test groups passed:

| Group | Name | Score | Status | Time (ms) | Memory (MB) |
|-------|------|-------|--------|-----------|-------------|
| 1 | one | 10.0 | ✅ Accepted | 155 | 15.9 |
| 2 | one.memcheck | 10.0 | ✅ Accepted | 3,664 | 152.9 |
| 3 | two | 10.0 | ✅ Accepted | 191 | 4.6 |
| 4 | two.memcheck | 10.0 | ✅ Accepted | 10,339 | 438.2 |
| 5 | three | 10.0 | ✅ Accepted | 27 | 6.4 |
| 6 | three.memcheck | 10.0 | ✅ Accepted | 1,613 | 84.3 |
| 7 | four | 10.0 | ✅ Accepted | 19 | 5.0 |
| 8 | four.memcheck | 10.0 | ✅ Accepted | 1,300 | 60.1 |
| 9 | five | 5.0 | ✅ Accepted | 1,245 | 115.9 |
| 10 | five.memcheck | 5.0 | ✅ Accepted | 3,083 | 91.6 |
| 11 | six | 5.0 | ✅ Accepted | 518 | 4.1 |
| 12 | six.memcheck | 5.0 | ✅ Accepted | 11,619 | 210.1 |

## Implementation Highlights

### Data Structure Design
1. **Hash Table with Separate Chaining**
   - Dynamic bucket array with configurable initial size (16 buckets)
   - Load factor of 0.75 triggers automatic rehashing
   - Each bucket contains a linked list of nodes for collision handling

2. **Doubly-Linked List for Insertion Order**
   - Sentinel head and tail nodes for simplified edge case handling
   - All elements maintain insertion order through prev/next pointers
   - Iterators traverse this list, not the hash buckets

3. **Node Structure**
   - Contains value_type (pair<const Key, T>)
   - `next_in_bucket` pointer for hash collision chain
   - `prev` and `next` pointers for insertion order list

### Time Complexity
- **Insert**: O(1) expected (amortized with rehashing)
- **Lookup/Access**: O(1) expected
- **Erase**: O(1) expected
- **Iteration**: O(n) where n is the number of elements

### Memory Management
- Proper deep copy in copy constructor and assignment operator
- Complete cleanup in destructor
- No memory leaks detected in any test case
- All nodes and buckets properly deallocated

### Key Features
- Bidirectional iterators with full support for ++ and -- operations
- Iterator validation (throws invalid_iterator for invalid operations)
- Rehashing mechanism to maintain performance as size grows
- const and non-const versions of all accessor methods

## Git History
```
299286b Fix ACMOJ submission script and achieve 100/100 score
89d895a Implement LinkedHashMap data structure
61993de Initial commit: Problem 012 setup
```

## Local Testing
All provided test cases passed before submission:
- Test 1 (testone): ✅ Passed
- Test 3 (testtwo): ✅ Passed
- Test 5 (testthree): ✅ Passed
- Test 7 (testfour): ✅ Passed

## Notes
- Implementation follows C++ best practices
- Only allowed headers used: functional, cstddef, and provided utility/exceptions
- Code is clean, well-structured, and maintainable
- No optimizations needed - perfect score achieved on first try
