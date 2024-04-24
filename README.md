DNS cache class

## Interface
explicit DNSCache(size_t max_size); - initializes cache with max_size entries. If elements count exceeds max_size then older entries are deleted.
void update(const std::string& name, const std::string& ip); - adds or updates a dns entry
std::string resolve(const std::string& name); - gets ip string by dns name. If entry doesn't exist then empty string is returned

## Complexity
1. add, update - log(n)
2. resolve - amortize constant

## Requirements
1. C++17 compiler

## Caveats
To achieve fine-grained parallelism for resove function each hash table bucket has its own shared mutex.
This is not the best solution because mutexes are heavy. Imstead it would be better to have a mutex per thread but implementation in that case would be more complex 
