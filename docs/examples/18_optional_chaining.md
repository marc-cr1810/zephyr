# Zephyr Optional Chaining

This document explains Zephyr's optional chaining operators (`?.`, `?[]`), safe navigation, direct dictionary member access, chaining, and error prevention in nested data access.

---

## Overview

Optional chaining allows you to safely access properties, methods, or elements on objects, class instances, or dictionaries that might be `none`. This prevents runtime errors and simplifies code when working with deeply nested or optional data.

---

## Optional Property Access with ?. Operator

**Description:**  
Safely access object or dictionary properties, returning `none` if the base value is `none`.

```zephyr
user = none
name = user?.profile?.name  # Safe, returns none
```

---

## Optional Index Access with ?[] Operator

**Description:**  
Safely access list or dictionary elements, returning `none` if the base value is `none`.

```zephyr
data = none
val = data?["key"]  # Safe, returns none
```

---

## Safe Navigation Through Potentially None Values

**Description:**  
Navigate nested data structures without risk of runtime errors.

```zephyr
profile = {"name": "Alice"}
print(profile?.name)  # Output: Alice
profile = none
print(profile?.name)  # Output: none
```

---

## Direct Dictionary Member Access with Dot Notation

**Description:**  
Access dictionary values using dot notation for string keys.

```zephyr
info = {"version": 1.0, "author": "Marc"}
print(info.version)  # Output: 1.0
```

---

## Chaining Optional Operators for Complex Navigation

**Description:**  
Combine multiple optional operators for deep, safe data access.

```zephyr
user = none
result = user?.settings?["theme"]?.color  # Safe, returns none
```

---

## Error Prevention in Nested Data Access

**Description:**  
Prevent errors when accessing deeply nested or optional data.

```zephyr
users = none
try {
    color = users[0]["profile"]["theme"]["color"]
} catch e {
    print("Error:", e)
}
# With optional chaining:
color = users?[0]?["profile"]?["theme"]?.color  # Safe, returns none
```
