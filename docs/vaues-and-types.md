---
layout: default
title: Values and Types
nav_order: 2
description: "Values and Types"
parent: Documentation
permalink: /docs/values-and-types
---

# Values and Types
{: .no_toc }

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

**J\*** is a *dynamically* typed programming language. This means that variables do not have types,
instead values carry their own type with them. Keep in mind that **J\*** is *dynamically* typed but 
not *weakly* typed. On the contrary, it's actually a fairly *strongly* typed language, meaning that
automatic conversions between types are not performed. For example code like this

```jstar
"2" + 3
```

will fail with an error, instead of converting the string into a number (or viceversa), like some 
other scripting languages do.
