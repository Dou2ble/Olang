# The OttoC Compiler

## Examples

### Fibonacci sequence
```java
fn fibonacci(limit) {
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

fn main() {
    fibonacci(3000000000000000000)
}
```
