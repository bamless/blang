---
layout: default
title: Demo
nav_order: 2
description: "J* demo"
permalink: /demo
last_modified_date: 2020-04-27T17:54:08+0000
---

<script src="{{ 'assets/js/linenumbers.js' | absolute_url }}"></script>
<script src="{{ 'assets/js/demo.js' | absolute_url }}"></script>

Examples
{: .text-delta }
<button type="button" id="hello-world" class="btn btn-blue">Hello World</button>
<button type="button" id="loop" class="btn btn-blue">Loop</button>
<button type="button" id="quick-sort" class="btn btn-blue">Quick Sort</button>
<button type="button" id="regex" class="btn btn-blue">Regex</button>

---

Type your code here
{: .text-delta }
<div class="demo-style demo-editor language-jstar">print('Hello, World!')</div>

<p class="demo-error-label"></p>
<button type="button" id="run" class="btn btn-green">
    <i class="fas fa-play" style="margin-right: 0.3em"></i>Run
</button>
<pre class="demo-style demo-output">[...]</pre>