# TobyLang

TobyLang is a minimalist esolang that supports variable assignments, basic arithmetic, and print statements. 

This project was a test of my ability to implement parsing and evaluating expressions.

## Example Code

An example file is included - simply drag and drop to run it - and you can also type out commands line by line.

### Assigning a Value

```toby
t 5
```

This assigns the value `5` to the variable `t`.

### Printing a Variable

```toby
toby t
```

This prints the value of `t`. Since `t` was assigned `5`, the output is:

```
5
```

### Arithmetic Expressions

```toby
toby t + 4
```

This evaluates `t + 4`, where `t` is `5`. The result of the expression is `9`, and the output is:

```
9
```

The value of `t` is not modified.

### Multiple Assignments

```toby
t 5
b 10
toby t
toby b
```

- Assign `5` to `t`
- Assign `10` to `b`
- Print the value of `t` → `5`
- Print the value of `b` → `10`

Output:

```
5
10
```
