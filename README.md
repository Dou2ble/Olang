# The OttoC Compiler

## TODO
- murmur hash hashtable

## Examples

### Fibonacci sequence
```java
fn printFibonacci(limit) {
    var x = 0
    var y = 1
    var z = 0

    while lessThan(x limit) {
        printLine(string(x))

    	z = add(x y)	
	x = y
	y = z
    }
}
```

### Printing arrays
```java
fn printArray(array) {
    print("[")
    var i = 0
    while lessThan(i int(array)) {
	print(string(getIndex(array i)))

	if lessThan(i subtract(int(array) 1)) {
	     print(" ")
	}

	i = add(i 1)
    }
    printLine("]")
}
```

