---
layout: default
title: Syntax
nav_order: 2
description: "J* syntax"
parent: Documentation
permalink: /docs/syntax
---

# Syntax
{: .no_toc }

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

**J\*** has a syntax pretty similar to Lua, while borrowing some elements from Python and C.  
The main ways that the syntax diverges from the one of Lua are the absence of the `do` and `then`
keywords to start control statement bodies, the significance of newlines in the source code and the
addition of some keywords.

## Comments

In **J\*** comments starts after a `//` and run until the end of a line, just like in c:
```jstar
// This is a comment
// Another comment
```

## Newlines

As mentioned above, newlines are significant in **J\*** code. They are mainly used to separate 
statements:
```jstar
print("Statement 1") // \n
print("Statement 2")
```

Doing something like this will instead result in a parsing error:
<div class="runnable-snippet">
print("Statement 1") print("Statement 2")
</div>

If you **really** want to put two or more statements on a single line, you can separate them using 
the `;` token:
<div class="runnable-snippet">
print("Statement 1"); print("Statement 2")
</div>

Semicolons follwed by a newline are also accepted, making code like this possible, even though not
reccomended:
```jstar
print("Statement 1");
print("Statement 2");
```