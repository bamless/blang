// WARNING: this is a file generated automatically by the build process from
// "/home/fabrizio/Workspace/c/blang/src/vm/core.jsr". Do not modify.
const char *core_jsr =
"class Number\n"
"    native __string__()\n"
"    native __hash__()\n"
"end\n"
"class Boolean\n"
"    native __string__()\n"
"end\n"
"class Null\n"
"    native __string__()\n"
"end\n"
"class Function\n"
"    native __string__()\n"
"end\n"
"class Module\n"
"    native __string__()\n"
"end\n"
"class StackTrace end\n"
"class String\n"
"    native substr(from, to)\n"
"    native join(iterable)\n"
"    native __eq__(o)\n"
"    native __len__()\n"
"    native __iter__(iter)\n"
"    native __next__(idx)\n"
"    native __string__()\n"
"    native __hash__()\n"
"end\n"
"class List\n"
"    native new(n=0, init=null)\n"
"    native add(e)\n"
"    native insert(i, e)\n"
"    native removeAt(i)\n"
"    native subList(from, to)\n"
"    native clear()\n"
"    native __len__()\n"
"    native __iter__(iter)\n"
"    native __next__(idx)\n"
"    fun addAll(iterable)\n"
"        var changed = false\n"
"        for var e in iterable do\n"
"            this.add(e)\n"
"            changed = true\n"
"        end\n"
"        return changed\n"
"    end\n"
"    fun insertAll(iterable, i=0)\n"
"        if i < 0 or i >= #this then\n"
"            raise IndexOutOfBoundException(str(i))\n"
"        end\n"
"        var changed = false\n"
"        for var e in iterable do\n"
"            this.insert(i, e)\n"
"            changed = true\n"
"        end\n"
"        return changed\n"
"    end\n"
"    fun remove(e)\n"
"        for var i = 0; i < #this; i += 1 do\n"
"            if e == this[i] then\n"
"                this.removeAt(i)\n"
"                return true\n"
"            end\n"
"        end\n"
"        return false\n"
"    end\n"
"    fun removeAll(iterable)\n"
"        var changed = false\n"
"        for var e in iterable do\n"
"            var r = this.remove(e)\n"
"            changed = changed or r\n"
"        end\n"
"        return changed\n"
"    end\n"
"    fun contains(e)\n"
"        return this.indexOf(e) != -1\n"
"    end\n"
"    fun indexOf(e)\n"
"        for var i = 0; i < #this; i += 1 do\n"
"            if this[i] == e then\n"
"                return i\n"
"            end\n"
"        end\n"
"        return -1\n"
"    end\n"
"    fun indexOfLast(e)\n"
"        for var i = #this - 1; i >= 0; i -= 1 do\n"
"            if this[i] == e then\n"
"                return i\n"
"            end\n"
"        end\n"
"        return -1\n"
"    end\n"
"    fun empty()\n"
"        return #this == 0\n"
"    end\n"
"    fun sort(comp=null)\n"
"        var size = #this - 1\n"
"        var temp = List(#this, fun(i) return this[i] end)\n"
"        for var m = 1; m <= size; m *= 2 do\n"
"            for var i = 0; i < size; i += 2 * m do\n"
"                var from, mid, to = i, i + m - 1, i + 2 * m - 1\n"
"                if to > size then to = size end\n"
"                this.__merge(temp, from, mid, to, comp)\n"
"            end\n"
"        end\n"
"    end\n"
"    fun __merge(temp, l, m, r, comp)\n"
"        var k, i, j = l, l, m + 1\n"
"        while i <= m and j <= r do\n"
"            if comp(this[i], this[j]) <= 0 if comp else this[i] <= this[j] then\n"
"                temp[k] = this[i]\n"
"                i += 1\n"
"            else\n"
"                temp[k] = this[j]\n"
"                j += 1\n"
"            end\n"
"            k += 1\n"
"        end\n"
"        \n"
"        var size = #this\n"
"        while i < size and i <= m do\n"
"            temp[k] = this[i]\n"
"            i += 1; k += 1\n"
"        end\n"
"        for var i = l; i <= r; i += 1 do\n"
"            this[i] = temp[i]\n"
"        end\n"
"    end\n"
"    fun foreach(pred)\n"
"        for var e in this do pred(e) end\n"
"    end\n"
"    fun __string__()\n"
"        return \"[\" + \", \".join(this) + \"]\"\n"
"    end\n"
"    fun __eq__(lst)\n"
"        if !(lst is List) then return false end\n"
"        var length = #this\n"
"        if length != #lst then\n"
"            return false\n"
"        end\n"
"        for var i = 0; i < length; i += 1 do\n"
"            if this[i] != lst[i] then\n"
"                return false\n"
"            end\n"
"        end\n"
"        return true\n"
"    end\n"
"end\n"
"class Tuple\n"
"    native new(seq)\n"
"    native sub(from, to)\n"
"    native __len__()\n"
"    native __iter__(iter)\n"
"    native __next__(idx)\n"
"    fun __string__()\n"
"        return \"(\" + \", \".join(this) + \")\"\n"
"    end\n"
"    fun __eq__(tup)\n"
"        if !(tup is Tuple) then return false end\n"
"        var length = #this\n"
"        if length != #tup then\n"
"            return false\n"
"        end\n"
"        for var i = 0; i < length; i += 1 do\n"
"            if this[i] != tup[i] then\n"
"                return false\n"
"            end\n"
"        end\n"
"        return true\n"
"    end\n"
"end\n"
"// Standard iterators\n"
"class iter\n"
"    fun new(indexable)\n"
"        this.indexable = indexable\n"
"    end\n"
"    fun __iter__(i)\n"
"        if i == null then\n"
"            return 0\n"
"        end\n"
"        if i < #this.indexable - 1 then\n"
"            return i + 1\n"
"        else \n"
"            return false\n"
"        end\n"
"    end\n"
"    fun __next__(i)\n"
"        return this.indexable[i]\n"
"    end\n"
"end\n"
"class reversed\n"
"    fun new(indexable)\n"
"        this.indexable = indexable\n"
"    end\n"
"    fun __iter__(i)\n"
"        if i == null then\n"
"            var idx = #this.indexable - 1\n"
"            return idx if idx >= 0 else false\n"
"        end\n"
"        if i > 0 then\n"
"            return i - 1\n"
"        else\n"
"            return false\n"
"        end\n"
"    end\n"
"    fun __next__(i)\n"
"        return this.indexable[i]\n"
"    end\n"
"end\n"
"class ipairs\n"
"    fun new(iter, start=0)\n"
"        this.iter = iter\n"
"        this.idx = start\n"
"    end\n"
"    fun __iter__(i)\n"
"        return this.iter.__iter__(i)\n"
"    end\n"
"    fun __next__(i)\n"
"        var ret = this.idx, this.iter.__next__(i)\n"
"        this.idx += 1\n"
"        return ret\n"
"    end\n"
"end\n"
"class izip\n"
"    fun new(...)\n"
"        if #args == 0 then \n"
"            raise TypeException(\"Must provide at least one iterator.\") \n"
"        end\n"
"        this.iters = args\n"
"        this.result = []\n"
"    end\n"
"    fun __iter__(tup)\n"
"        if tup == null then\n"
"            for var iterator in this.iters do\n"
"                this.result.add(iterator.__iter__(null))\n"
"            end\n"
"        else\n"
"            var i = 0\n"
"            for var iterator in this.iters do\n"
"                var res = iterator.__iter__(tup[i])\n"
"                if !res then return false end\n"
"                this.result.add(res)\n"
"                i += 1\n"
"            end\n"
"        end\n"
"        var res = Tuple(this.result)\n"
"        this.result.clear()\n"
"        return res\n"
"    end\n"
"    fun __next__(tup)\n"
"        var i, res = 0, []\n"
"        for var iterator in this.iters do\n"
"            res.add(iterator.__next__(tup[i]))\n"
"        end\n"
"        return Tuple(res)\n"
"    end\n"
"end\n"
"// Core functions\n"
"fun assert(cond, msg=\"assertion failed\")\n"
"    if !cond then \n"
"        raise AssertException(msg) \n"
"    end\n"
"end\n"
"fun sorted(iterable, comp=null)\n"
"    var lst = tolist(iterable)\n"
"    lst.sort(comp)\n"
"    return lst\n"
"end\n"
"fun tolist(iterable)\n"
"    var lst = []\n"
"    for var e in iterable do\n"
"        lst.add(e)\n"
"    end\n"
"    return lst\n"
"end\n"
"native type(o)\n"
"native ascii(char)\n"
"native char(num)\n"
"native eval(source)\n"
"native int(n)\n"
"native isInt(n)\n"
"native num(n)\n"
"native print(s, ...)\n"
"// Exceptions builtin classes\n"
"class Exception\n"
"    fun new(err=null)\n"
"        this.err = err\n"
"    end\n"
"end\n"
"class TypeException : Exception end\n"
"class NameException : Exception end\n"
"class FieldException : Exception end\n"
"class MethodException : Exception end\n"
"class ImportException : Exception end\n"
"class StackOverflowException : Exception end\n"
"class DivisionByZeroException : Exception end\n"
"class InvalidArgException : Exception end\n"
"class IndexOutOfBoundException : Exception end\n"
"class AssertException : Exception end\n"
;