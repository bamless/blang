---
layout: default
title: Functions
nav_order: 8
description: "Functions"
parent: Documentation
permalink: docs/functions
---

# Functions
{: .no_toc }

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

Functions are a bundle of statements and expressions that are combined to perform a task, and that 
can be used repeatedly. They can accept input data to be operated on in the form of parameters, and
return some data to the caller via *return* statements.

In **J\*** functions are an implementation of *closures* and as such they support the concept of
[upvalues](functions#upvalues). Also, just like everything else in **J\***, they are first-class
values. This means that they can be stored in variables, passed in as arguments to other functions
and can even be returned as a result of a function call.

## Function definitions
The main way that **J\*** lets you create functions is through a *function definition* statement:
```jstar
fun printFoo()
    var foo = "Foo"
    print(foo)
end
```

In **J\*** a function definition is an executable statement. Its execution creates a new *function
object* (an object containing the executable code of the body) and binds it to the provided name in
the current scope:
<pre class='runnable-snippet'>
fun newFunction()
    print("This is a function!")
end

print(newFunction) // the function is bound to `newFunction` in the current scope!
</pre>
Note that the function definition does not execute the function body; this gets executed only when
the function is [called](functions#calling-functions).

Being a statement, a function definition can appear in every place a statement can, even inside 
other functions:
```jstar
fun function()
    // create a new function and bind its name to a variable 
    // named `nested` inside `function` scope
    fun nested()
        print("A nested function")
    end
    print("Outer function")
end
```

## Function literals

**J\*** provides another way of creating functions: *function literals*:
<pre class='runnable-snippet'>
var newFunction = fun()
    print("This is a function!")
end

print(newFunction)
</pre>

Function literals, differently from function definitions, are expressions, and as such can be used
in every context where an expression can, such as in a variable initializer as showed in the example
above.

They are often used to create so called "anonymous" functions: one-time use functions
that are not meant to be bound to a name, particularly useful for callbacks:
<pre class='runnable-snippet'>
fun callMe(fn)
    fn()
end

callMe(fun()
    print("I've been called!")
end)
</pre>

Function literals can also be used in conjuction with variables to achieve the same effect of a 
*function definition* statement:
<pre class='runnable-snippet'>
var newFunction
newFunction = fun()
    print("New function")
end
</pre>

In fact, when **J\*** sees a *function definition* statement, it desugars it (in other words, 
trasforms it) into the form above, making them equivalent. Nontheless, if the only thing you need to
do is create a function and bind it to a name, you should prefer using a *function declaration*
statement, as it is more natural to write and less verbose.

## Lambdas

It is not uncommon for function literals to be composed of a single return statement. Being such a
common use case, **J\*** provides a special syntax for creating such functions:
```jstar
var lambda = |x, y| => x + y // A lambda body is composed of a single expression
```

This is called a *lambda* function literal, and the compiler will desugar it into the following
function literal:
```jstar
var lambda = fun(x, y)
    return x + y
end
```

## Calling functions

As already mentioned, creating a function via function definitions or literals doesn't execute its
body. When you want to execute a function you must *call* it. This is done by using the *call
operator* `()`:
<pre class='runnable-snippet'>
fun someFunction()
    print("'someFunction' called")
end

someFunction() // Function call
</pre>

A function call is an *expression*, and as such can be used in every place an expression can.

## Function parameters

It would be pretty limiting if functions could only operate on the same data every time they're
called.  Fortunately, functions can specify input parameters that can be varied between function
calls:
<pre class='runnable-snippet'>
fun foo(a, b)
    print("Called foo with {0} and {1}" % (a, b))
end

foo("bar", 49)
foo(false, null)
</pre>

Function parameters in **J\*** are *positional*. This means that the arguments passed to the call
operator will be bound to the parameters based on their order.

Also, the number of arguments between the function definition and the function call must match,
passing too many or too few will result in an error:
<pre class='runnable-snippet'>
fun foo(a, b)
    print("Called foo with {0} and {1}" % (a, b))
end

foo("bar", 49, 50)
</pre>

## Default parameters

Sometimes, it can be useful to have default values for function parameters, so that some of them can
be left unspecified at the call site. This can be achieved using this syntax:
<pre class='runnable-snippet'>
fun foo(a, b="Default value")
    print("Calling with {0} and {1}" % (a, b))
end

foo("bar")     // If left unspecified, `b` will be bound to "Default value"
foo("bar", 49) // If instead a second argument is passed, it will take the place of the default one
</pre>

Default parameters *must* appear after all positional ones have been listed (if any). Specifying a
positional parameter after a default one will result in an error:
<pre class='runnable-snippet'>
fun foo(a="Default", b)
    print("Calling with {0} and {1}" % (a, b))
end
</pre>

Valid values for default paramters are: *strings*, *numbers*, *booleans* and *null*; the
*constant values*. If you try to use any other **J\*** value the compiler will scream at you:
<pre class='runnable-snippet'>
fun foo(a, b=[1, 2, 3])
    print("Calling with {0} and {1}" % (a, b))
end
</pre>

## Variadic functions

For some functions it can be useful to accept an unlimited number of arguments. These are called
*variadic* functions. In **J\*** a function is variadic if the last parameter is an *ellipsis*
(`...` token):
```jstar
fun variadic(a, b, ...)
    // Function body
end

// We can pass extra arguments in addition to `a` and `b`
variadic(1, 2, 3, 4, 5)
```

When calling a variadic function, any extra argument will be put in a tuple and passed to the
function via an hidden parameter called *args*:
<pre class='runnable-snippet'>
fun variadic(a, b, ...)
    print("Argument 1:", a)
    print("Argument 2:", b)
    for var e in args // `args` holds the extra arguments
        print("Variadic:", e)
    end
end

variadic(1, 2, 3, 4, 5)
</pre>

If instead no extra parameters are passed at the call site, `args` will be bound to the empty tuple:
<pre class='runnable-snippet'>
fun variadic(...)
    print(args)
end

variadic()
</pre>

Variadic functions can still use positional and default parameters, with the usual rules: positional
first, then default ones, and an ellipsis at the end to make it variadic:
<pre class='runnable-snippet'>
// Using all kinds of parameters in a function
fun all(a, b, c="Default", ...)
    print(a, b, c, args)
end

all(1, 2)                  // Calling by specifying only positional parameters
all(1, 2, "foo")           // Calling by specifying positional and default parameters
all(1, 2, "foo", false, 3) // Calling with extra parameters after positional and defaults
</pre>

## Unpacking call
Just like with variables, sometimes we wish to extract the elements of a tuple or a list and pass
them as arguments to a function call. This can be achieved via an *unpacking* function call:
<pre class='runnable-snippet'>
fun foo(a, b, c)
    print(a, b, c)
end

var unpackable = 1, 2, 3
foo(unpackable)... // Unpacking call
</pre>

An *unpacking* call is composed by a normal function call followed by an *ellipsis*. When an 
unpacking call is executed, **J\*** will try to unpack the last argument and bind its values to the
remaining parameters.

If the last argument of the call is not a list or a tuple, an error will be prduced:
<pre class='runnable-snippet'>
fun foo(a, b, c)
    print(a, b, c)
end

foo("not a tuple")...
</pre>

Also, if the number of elements in the list or tuple doesn't match the number of parameters that
haven't been yet specified, an error will be produced as well:
<pre class='runnable-snippet'>
fun foo(a, b, c)
    print(a, b, c)
end

// The first parameter is specified, this leaves only 2 that can be 
// bound by the unpacking call.
// But the provided tuple has 3 elements, so the call will fail.
// The same would happen even if the tuple has a low item count, such as 1
foo(1, (2, 3, 4))...
</pre>

## Keyword parameters

**TODO**

## Closures and upvalues

**TODO**
