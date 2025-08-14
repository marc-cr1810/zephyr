# Zephyr Built-in Functions

This document provides a detailed description of Zephyr's built-in functions, explaining their purpose, usefulness, and providing practical examples. These functions are integral to the language, offering core functionalities for input/output, type conversion, data structure manipulation, and asynchronous programming.

---

### `print(value1, value2, ...)`

*   **Description:** Prints the string representation of one or more values to the console. Values are separated by spaces, and a newline character is added at the end. It converts any Zephyr object to its string representation using its `to_string()` method.
*   **Usefulness:** Essential for debugging, displaying output to the user, and logging information during program execution. It provides a simple way to inspect the state of variables and the flow of a program.
*   **Examples:**
    ```zephyr
    print("Hello, World!")
    
    x = 10
    y = "Zephyr"
    print("Value of x:", x, "and y:", y)
    
    my_list = [1, 2, 3]
    print("List content:", my_list)
    ```

### `len(collection)`

*   **Description:** Returns the number of items in a string, list, or dictionary.
    *   For strings, it returns the number of characters.
    *   For lists, it returns the number of elements.
    *   For dictionaries, it returns the number of key-value pairs.
    *   Throws a runtime error if the argument is not a string, list, or dictionary.
*   **Usefulness:** Useful for determining the size of collections, iterating through them, and performing validation checks (e.g., checking if a list is empty).
*   **Examples:**
    ```zephyr
    my_string = "programming"
    print("Length of string:", len(my_string)) # Output: 11
    
    my_list = [10, 20, 30, 40]
    print("Number of elements in list:", len(my_list)) # Output: 4
    
    my_dict = {"a": 1, "b": 2, "c": 3}
    print("Number of key-value pairs in dictionary:", len(my_dict)) # Output: 3
    
    # len(123) # This would cause a runtime error
    ```

### `input(prompt)`

*   **Description:** Reads a single line of text from standard input. An optional `prompt` string can be displayed to the user before reading input. The function returns the read line as a string.
*   **Usefulness:** Enables interactive programs by allowing them to receive user input from the console.
*   **Examples:**
    ```zephyr
    name = input("Enter your name: ")
    print("Hello,", name)
    
    age_str = input("How old are you? ")
    age = int(age_str) # Convert input string to integer
    print("You are", age, "years old.")
    ```

### `str(value)`

*   **Description:** Converts any Zephyr value to its string representation. This is useful for explicit type conversion, especially when concatenating values of different types with strings.
*   **Usefulness:** Facilitates string manipulation and ensures that values are in the correct format for display or logging.
*   **Examples:**
    ```zephyr
    num = 123
    text = "The number is: " + str(num)
    print(text) # Output: The number is: 123
    
    is_active = true
    print("Status:", str(is_active)) # Output: Status: true
    
    my_list = [1, "hello", false]
    print("List as string:", str(my_list)) # Output: List as string: [1, "hello", false]
    ```

### `int(value)`

*   **Description:** Converts a given value to an integer.
    *   If the value is already an integer, it's returned as is.
    *   If it's a float, it's truncated to an integer (e.g., `3.9` becomes `3`).
    *   If it's a string, it attempts to parse the string as an integer. Throws a runtime error if the string is not a valid integer literal.
    *   If it's a boolean, `true` becomes `1` and `false` becomes `0`.
    *   Throws a runtime error for other types.
*   **Usefulness:** Essential for type conversion, especially when performing arithmetic operations on user input or data read as strings.
*   **Examples:**
    ```zephyr
    float_val = 3.14
    int_val = int(float_val)
    print("Float to int:", int_val) # Output: 3
    
    str_num = "456"
    parsed_int = int(str_num)
    print("String to int:", parsed_int) # Output: 456
    
    bool_true = true
    print("True to int:", int(bool_true)) # Output: 1
    
    # int("hello") # This would cause a runtime error
    ```

### `float(value)`

*   **Description:** Converts a given value to a floating-point number.
    *   If the value is already a float, it's returned as is.
    *   If it's an integer, it's converted to a float (e.g., `5` becomes `5.0`).
    *   If it's a string, it attempts to parse the string as a float. Throws a runtime error if the string is not a valid float literal.
    *   If it's a boolean, `true` becomes `1.0` and `false` becomes `0.0`.
    *   Throws a runtime error for other types.
*   **Usefulness:** Necessary for type conversion when precise decimal calculations are required, especially with user input or data from external sources.
*   **Examples:**
    ```zephyr
    int_val = 7
    float_val = float(int_val)
    print("Int to float:", float_val) # Output: 7.0
    
    str_float = "9.81"
    parsed_float = float(str_float)
    print("String to float:", parsed_float) # Output: 9.81
    
    bool_false = false
    print("False to float:", float(bool_false)) # Output: 0.0
    
    # float("abc") # This would cause a runtime error
    ```

### `append(list, item)`

*   **Description:** Adds an `item` to the end of the specified `list`. This function modifies the list in-place.
    *   Throws a runtime error if the first argument is not a list.
*   **Usefulness:** Dynamically extends lists, allowing for the collection of data during program execution.
*   **Examples:**
    ```zephyr
    my_list = [1, 2]
    append(my_list, 3)
    print(my_list) # Output: [1, 2, 3]
    
    data = ["apple", "banana"]
    new_fruit = "cherry"
    append(data, new_fruit)
    print(data) # Output: ["apple", "banana", "cherry"]
    ```

### `pop(list, index)`

*   **Description:** Removes and returns an item from a `list` at a given `index`.
    *   If `index` is not provided, it removes and returns the last item in the list.
    *   Supports negative indexing (e.g., `-1` refers to the last element).
    *   Throws a runtime error if the first argument is not a list, the list is empty, or the index is out of range.
*   **Usefulness:** Modifies lists by removing elements, often used in conjunction with stack-like or queue-like data structures.
*   **Examples:**
    ```zephyr
    my_list = ["a", "b", "c", "d"]
    
    last_item = pop(my_list)
    print("Popped last:", last_item) # Output: Popped last: d
    print("List after pop:", my_list) # Output: List after pop: ["a", "b", "c"]
    
    second_item = pop(my_list, 1)
    print("Popped second:", second_item) # Output: Popped second: b
    print("List after pop:", my_list) # Output: List after pop: ["a", "c"]
    
    # pop([]) # This would cause a runtime error (pop from empty list)
    # pop(my_list, 10) # This would cause a runtime error (index out of range)
    ```

### `keys(dictionary)`

*   **Description:** Returns a new list containing all the keys present in the given `dictionary`. The order of keys in the returned list is not guaranteed.
    *   Throws a runtime error if the argument is not a dictionary.
*   **Usefulness:** Allows iteration over dictionary keys, checking for key existence, or performing operations based on keys.
*   **Examples:**
    ```zephyr
    my_dict = {"name": "Alice", "age": 30, "city": "New York"}
    all_keys = keys(my_dict)
    print("Dictionary keys:", all_keys) # Output: Dictionary keys: ["name", "age", "city"] (order may vary)
    
    for k in keys(my_dict) {
        print("Key:", k, "Value:", my_dict[k])
    }
    ```

### `values(dictionary)`

*   **Description:** Returns a new list containing all the values present in the given `dictionary`. The order of values in the returned list is not guaranteed and corresponds to the order of keys returned by `keys()`.
    *   Throws a runtime error if the argument is not a dictionary.
*   **Usefulness:** Allows iteration over dictionary values or performing operations on all values without needing their corresponding keys.
*   **Examples:**
    ```zephyr
    my_dict = {"name": "Alice", "age": 30, "city": "New York"}
    all_values = values(my_dict)
    print("Dictionary values:", all_values) # Output: Dictionary values: ["Alice", 30, "New York"] (order may vary)
    
    total_age = 0
    for v in values(my_dict) {
        if type(v) == "int" {
            total_age += v
        }
    }
    print("Total age (if applicable):", total_age)
    ```

### `type(value)`

*   **Description:** Returns the name of the type of a given `value` as a string.
*   **Usefulness:** Crucial for runtime type checking, conditional logic based on type, and debugging.
*   **Examples:**
    ```zephyr
    print("Type of 10:", type(10)) # Output: int
    print("Type of 3.14:", type(3.14)) # Output: float
    print("Type of 'hello':", type("hello")) # Output: string
    print("Type of true:", type(true)) # Output: bool
    print("Type of [1, 2]:", type([1, 2])) # Output: list
    print("Type of {}:", type({})) # Output: dictionary
    
    func my_func() {}
    print("Type of my_func:", type(my_func)) # Output: function
    
    my_lambda = x -> x * 2
    print("Type of my_lambda:", type(my_lambda)) # Output: lambda
    ```

### `map(list, func)`

*   **Description:** Applies a `func` to every item in the `list` and returns a new list containing the results. The original list remains unchanged. The `func` must be a function, lambda, or built-in function that accepts one argument.
    *   Throws a runtime error if the first argument is not a list or the second argument is not a callable function.
*   **Usefulness:** A fundamental functional programming construct for transforming collections of data. It allows for concise and readable data transformations without explicit loops.
*   **Examples:**
    ```zephyr
    numbers = [1, 2, 3, 4]
    
    # Using a lambda to double each number
    doubled_numbers = map(numbers, x -> x * 2)
    print("Doubled numbers:", doubled_numbers) # Output: [2, 4, 6, 8]
    
    # Using a function to convert numbers to strings
    func to_string_func(num) {
        return "Number: " + str(num)
    }
    string_numbers = map(numbers, to_string_func)
    print("String numbers:", string_numbers) # Output: ["Number: 1", "Number: 2", "Number: 3", "Number: 4"]
    ```

### `filter(list, func)`

*   **Description:** Returns a new list containing only the items from the original `list` for which the `func` returns a "truthy" value. The original list remains unchanged. The `func` must be a function, lambda, or built-in function that accepts one argument and returns a boolean or a value that can be evaluated as truthy/falsy.
    *   Throws a runtime error if the first argument is not a list or the second argument is not a callable function.
*   **Usefulness:** Used for selecting a subset of elements from a collection based on a specific condition, promoting cleaner and more declarative code than traditional loops with `if` statements.
*   **Examples:**
    ```zephyr
    numbers = [1, 2, 3, 4, 5, 6]
    
    # Using a lambda to filter even numbers
    even_numbers = filter(numbers, x -> x % 2 == 0)
    print("Even numbers:", even_numbers) # Output: [2, 4, 6]
    
    # Using a function to filter strings longer than 5 characters
    words = ["apple", "banana", "cat", "dogfood"]
    func is_long_word(word) {
        return len(word) > 5
    }
    long_words = filter(words, is_long_word)
    print("Long words:", long_words) # Output: ["banana", "dogfood"]
    ```

### `reduce(list, func, initial)`

*   **Description:** Reduces a `list` to a single value by iteratively applying a `func` to an accumulator and each element.
    *   The `func` must be a function, lambda, or built-in function that accepts two arguments: the accumulator and the current element.
    *   An optional `initial` value can be provided for the accumulator. If `initial` is not provided, the first element of the list is used as the initial accumulator, and the function starts processing from the second element.
    *   Throws a runtime error if the list is empty and no `initial` value is provided, or if arguments are of incorrect types.
*   **Usefulness:** Powerful for aggregating data, performing calculations like sums, products, or finding minimum/maximum values across a collection.
*   **Examples:**
    ```zephyr
    numbers = [1, 2, 3, 4, 5]
    
    # Summing all numbers
    sum_func = (acc, current) -> acc + current
    total_sum = reduce(numbers, sum_func, 0) # Initial value 0
    print("Total sum:", total_sum) # Output: 15
    
    # Concatenating strings
    words = ["Hello", "World", "Zephyr"]
    concat_func = (acc, current) -> acc + " " + current
    sentence = reduce(words, concat_func, "") # Initial value empty string
    print("Sentence:", sentence) # Output: " Hello World Zephyr" (note leading space from initial empty string)
    
    # Finding the maximum number
    max_func = (acc, current) -> acc > current ? acc : current
    max_val = reduce(numbers, max_func) # No initial value, uses first element
    print("Max value:", max_val) # Output: 5
    ```

### `enumerate(list)`

*   **Description:** Returns a new list of (index, value) pairs for each item in the input `list`.
    *   Throws a runtime error if the argument is not a list.
*   **Usefulness:** Ideal for scenarios where you need both the index and the value of elements while iterating through a list, such as when modifying elements by index or displaying numbered lists.
*   **Examples:**
    ```zephyr
    fruits = ["apple", "banana", "cherry"]
    indexed_fruits = enumerate(fruits)
    print("Indexed fruits:", indexed_fruits) # Output: [[0, "apple"], [1, "banana"], [2, "cherry"]]
    
    for (index, fruit) in enumerate(fruits) {
        print("Fruit at index", index, "is", fruit)
    }
    ```

### `zip(list1, list2, ...)`

*   **Description:** Returns a new list of lists, where the i-th sublist contains the i-th element from each of the argument lists. The resulting list is truncated to the length of the shortest input list.
    *   Requires at least two list arguments.
    *   Throws a runtime error if any argument is not a list.
*   **Usefulness:** Combines elements from multiple lists into single iterables, useful for processing related data in parallel or creating key-value pairs from separate lists.
*   **Examples:**
    ```zephyr
    names = ["Alice", "Bob", "Charlie"]
    ages = [25, 30, 35]
    
    combined_data = zip(names, ages)
    print("Combined data:", combined_data) # Output: [["Alice", 25], ["Bob", 30], ["Charlie", 35]]
    
    scores = [90, 85, 92]
    all_info = zip(names, ages, scores)
    print("All info:", all_info) # Output: [["Alice", 25, 90], ["Bob", 30, 85], ["Charlie", 35, 92]]
    
    # Handling different lengths
    short_list = [1, 2]
    long_list = ["a", "b", "c"]
    zipped_short = zip(short_list, long_list)
    print("Zipped short:", zipped_short) # Output: [[1, "a"], [2, "b"]]
    ```

### `all(promises)`

*   **Description:** Takes a single argument, which must be a list containing only Promise objects. It waits for all the Promises in the list to resolve. Once all Promises have resolved, it returns a new list containing their resolved values, in the same order as the input Promises. If any of the Promises in the input list reject, the `all()` function will also reject with the reason of the first Promise that rejected.
    *   Throws a runtime error if the argument is not a list or if the list contains non-Promise objects.
*   **Usefulness:** Essential for managing concurrent asynchronous operations. It allows you to wait for multiple independent async tasks to complete before proceeding, which is common in scenarios like fetching multiple pieces of data from different sources.
*   **Examples:**
    ```zephyr
    async func fetch_data(id) {
        print("Fetching data for ID:", id)
        # Simulate async operation
        return "Data for " + str(id)
    }
    
    async func main() {
        promise1 = spawn fetch_data(1)
        promise2 = spawn fetch_data(2)
        promise3 = spawn fetch_data(3)
        
        all_results = await all([promise1, promise2, promise3])
        print("All data fetched:", all_results)
        # Output: All data fetched: ["Data for 1", "Data for 2", "Data for 3"]
    }
    
    await main()
    
    # Example with a rejecting promise (will cause main to throw an error)
    async func failing_fetch(id) {
        if id == 2 {
            throw "Failed to fetch data for ID 2"
        }
        return "Data for " + str(id)
    }
    
    async func main_with_error() {
        try {
            p1 = spawn failing_fetch(1)
            p2 = spawn failing_fetch(2)
            p3 = spawn failing_fetch(3)
            
            results = await all([p1, p2, p3])
            print("Should not reach here:", results)
        } catch e {
            print("Caught error from all():", e)
            # Output: Caught error from all(): Failed to fetch data for ID 2
        }
    }
    
    await main_with_error()
    ```