# Compound Assignment Operators in Zephyr

Zephyr supports modern compound assignment operators for efficient variable manipulation, including increment/decrement, string repetition, and nested assignments.

---

## Basic Compound Assignment Operators

```zephyr
x = 10
x += 5      # x = x + 5 → 15
x -= 2      # x = x - 2 → 13
x *= 3      # x = x * 3 → 39
x /= 2      # x = x / 2 → 19.5
x %= 4      # x = x % 4 → 3.5
```

Works with both integers and floats.

---

## String Compound Assignment

```zephyr
text = "Hello"
text += " World"    # "Hello World"
text *= 2           # "Hello WorldHello World"
```

---

## Increment and Decrement Operators

Both prefix and postfix forms are supported.

```zephyr
count = 0
count++     # Postfix: use value, then increment (count = 1)
++count     # Prefix: increment, then use value (count = 2)
count--     # Postfix: use value, then decrement (count = 1)
--count     # Prefix: decrement, then use value (count = 0)
```

Works with floats:

```zephyr
f = 1.5
f++
++f
f--
--f
```

---

## Member Compound Assignment

```zephyr
class Score {
    func init() { this.value = 0 }
    func add(points) { this.value += points }
    func inc() { this.value++ }
    func dec() { --this.value }
}
s = Score()
s.add(10)
s.inc()
s.dec()
```

Inside class methods, `this.member++` and `this.member--` work as expected.

---

## Collection Element Compound Assignment

```zephyr
scores = [85, 92, 78]
scores[0] += 10      # [95, 92, 78]
scores[1] *= 1.1     # [95, 101.2, 78]
scores[2] -= 5       # [95, 101.2, 73]
scores[0] %= 100     # [95, 101.2, 73]

inventory = {"apples": 50, "oranges": 30}
inventory["apples"] += 20
inventory["oranges"] *= 2
inventory["apples"] %= 50
```

---

## Nested Structure Compound Assignment

```zephyr
data = {"users": [{"score": 100}, {"score": 85}]}
data["users"][0]["score"] += 50
data["users"][1]["score"] *= 1.2
data["users"][0]["score"] %= 1000
```

---

## Advanced Compound Assignment Patterns

### Compound Assignment with Dictionaries and Lists

```zephyr
# Dictionary value compound assignment
inventory = {"apples": 10, "bananas": 5}
inventory["apples"] += 3
inventory["bananas"] *= 2
inventory["apples"] %= 5

# List element compound assignment
nums = [1, 2, 3]
nums[1] += 10
nums[2] *= 4
nums[0] -= 1
```

### Compound Assignment with Nested Structures

```zephyr
team = {
    "members": [
        {"name": "Alice", "score": 10},
        {"name": "Bob", "score": 15}
    ]
}
team["members"][0]["score"] += 5
team["members"][1]["score"] *= 2
```

### Compound Assignment with Optional Chaining

```zephyr
profile = none
# Safe: does nothing if profile is none
profile?.score += 10
```

---

## Compound Assignment with Functions and Lambdas

```zephyr
add_bonus = x -> x + 10
score = 50
score += add_bonus(5)  # score = 65

# In a loop
total = 0
for i = 1; i <= 3; i++ {
    total += add_bonus(i)
}
```

---

## Compound Assignment in Classes and Methods

```zephyr
class Counter {
    func init() { this.value = 0 }
    func inc() { this.value++ }
    func add(n) { this.value += n }
    func dec() { --this.value }
}

c = Counter()
c.inc()
c.add(5)
c.dec()
```

---

## Compound Assignment with Explicit Typing

```zephyr
x : int = 10
x += 2
x -= 1

y : float = 5.5
y *= 2
y /= 3

s : string = "abc"
s *= 3
```

---

## Edge Cases and Error Handling

- Compound assignment with `none` values will error.
- String multiplication with zero/negative returns empty string.
- Postfix operators on members outside class methods may not parse; use `+= 1` as a workaround.
- Type mismatches (e.g., adding string to int) will throw errors.

```zephyr
try {
    val = none
    val += 1
} catch e {
    print("Error:", e)
}

try {
    s = "abc"
    s *= 0    # ""
    s *= -2   # ""
} catch e {
    print("Error:", e)
}
```

---

## Integration with Control Flow

```zephyr
sum = 0
for n in [1, 2, 3, 4] {
    sum += n
}
print(sum)  # 10

# With where clause
total = 0
for n in [1, 2, 3, 4, 5] where n % 2 == 0 {
    total += n
}
print(total)  # 6
```

---

## Integration with Async/Await

```zephyr
async func get_value(x) { return x * 2 }
result = 10
result += await get_value(5)
print(result)  # 20
```

---

## Integration with Switch Statements

```zephyr
x = 5
switch x {
    case 5:
        x += 10
        break
    case 10:
        x *= 2
}
print(x)  # 15
```

---

## Integration with Enhanced Loops

```zephyr
nums = [1, 2, 3, 4]
for i, n in enumerate(nums) {
    nums[i] *= 2
}
print(nums)  # [2, 4, 6, 8]
```

---

## Integration with Optional Chaining

```zephyr
user = none
user?.score += 10  # Safe, does nothing
user = {"score": 5}
user?.score += 10  # user["score"] = 15
```

---

## Integration with Explicit Typing

```zephyr
counter : int = 0
counter += 1
counter++
```

---

## Integration with Classes and Lambdas

```zephyr
class Player {
    func init() { this.score = 0 }
    func win(points) { this.score += points }
    func lose(points) { this.score -= points }
}

p = Player()
p.win(10)
p.lose(3)
```

---

## Integration with Data Structures

```zephyr
matrix = [[1,2],[3,4]]
matrix[0][1] += 5
matrix[1][0] *= 2
```

---

## Integration with Real-World Patterns

```zephyr
class ShoppingCart {
    func init() { this.items = {"apple": 1, "banana": 2} }
    func add_item(name, qty) { this.items[name] += qty }
    func remove_item(name, qty) { this.items[name] -= qty }
}

cart = ShoppingCart()
cart.add_item("apple", 3)
cart.remove_item("banana", 1)
print(cart.items)  # {"apple": 4, "banana": 1}
```

---

## Best Practices

- Prefer `obj.member += 1` for member increment outside methods.
- Use compound assignment for clarity and performance.
- Always handle possible errors in division/modulo.
- Use compound assignment in loops for concise accumulation.

---

## Summary Table

| Operator | Example         | Description                |
|----------|----------------|----------------------------|
| +=       | x += 5         | Addition assignment        |
| -=       | x -= 2         | Subtraction assignment     |
| *=       | x *= 3         | Multiplication assignment  |
| /=       | x /= 2         | Division assignment        |
| %=       | x %= 4         | Modulo assignment          |
| ++       | x++ / ++x      | Increment (post/prefix)    |
| --       | x-- / --x      | Decrement (post/prefix)    |

---

## Real-World Example

```zephyr
class Cart {
    func init() { this.items = {"apple": 2, "banana": 3} }
    func add_item(name, qty) { this.items[name] += qty }
    func remove_item(name, qty) { this.items[name] -= qty }
}

cart = Cart()
cart.add_item("apple", 5)
cart.remove_item("banana", 1)
print(cart.items)  # {"apple": 7, "banana": 2}
```
