// WARNING: this is a file generated automatically by the build process from
// "/home/fabrizio/Workspace/c/jstar/jstar/src/core.jsr". Do not modify.
const char *core_jsr =
"class Number\n"
"    native new(n)\n"
"    native isInt()\n"
"    native __string__()\n"
"    native __hash__()\n"
"end\n"
"class Boolean\n"
"    native new(b)\n"
"    native __string__()\n"
"    native __hash__()\n"
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
"class Userdata end\n"
"class Iter\n"
"    fun forEach(func)\n"
"        for var e in this do func(e) end\n"
"    end\n"
"    fun enumerate() \n"
"        return IEnumerate(this)\n"
"    end\n"
"    fun map(func)\n"
"        return IMap(func, this)\n"
"    end\n"
"    fun filter(func) \n"
"        return IFilter(func, this)\n"
"    end\n"
"    fun zip(iterable)\n"
"        return IZip(this, iterable)\n"
"    end\n"
"end\n"
"class Sequence is Iter\n"
"    fun new()\n"
"        raise NotImplementedException()\n"
"    end\n"
"    fun contains(e)\n"
"        return this.indexOf(e) != -1\n"
"    end\n"
"    fun indexOf(e)\n"
"        var length = #this\n"
"        for var i = 0; i < length; i += 1 do\n"
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
"    fun count(e)\n"
"        var count = 0\n"
"        for var o in this do\n"
"            if o == e then \n"
"                count += 1\n"
"            end\n"
"        end\n"
"        return count\n"
"    end\n"
"    fun slice(from, to=null)\n"
"        if !to then to = #this end\n"
"        var res = []\n"
"        for var i = from; i < to; i += 1 do\n"
"            res.add(this[i])\n"
"        end\n"
"        return res\n"
"    end\n"
"    fun empty()\n"
"        return #this == 0\n"
"    end\n"
"    fun reverse()\n"
"        return IReverse(this)\n"
"    end\n"
"    fun __eq__(o)\n"
"        if type(o) != type(this) then\n"
"            return false \n"
"        end\n"
"        var length = #this\n"
"        if length != #o then\n"
"            return false\n"
"        end\n"
"        for var i = 0; i < length; i += 1 do\n"
"            if this[i] != o[i] then\n"
"                return false\n"
"            end\n"
"        end\n"
"        return true\n"
"    end\n"
"end\n"
"class String is Sequence\n"
"    native new(...)\n"
"    native slice(from, to=null)\n"
"    native charAt(idx)\n"
"    native startsWith(prefix, offset=0)\n"
"    native endsWith(suffix)\n"
"    native strip()\n"
"    native chomp()\n"
"    native join(iterable)\n"
"    native __eq__(o)\n"
"    native __len__()\n"
"    native __iter__(iter)\n"
"    native __next__(idx)\n"
"    native __string__()\n"
"    native __hash__()\n"
"end\n"
"class List is Sequence\n"
"    native new(n=0, init=null)\n"
"    native add(e)\n"
"    native insert(i, e)\n"
"    native removeAt(i)\n"
"    native slice(from, to=null)\n"
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
"        var changed = false\n"
"        for var e in iterable do\n"
"            this.insert(i, e)\n"
"            changed = true\n"
"        end\n"
"        return changed\n"
"    end\n"
"    fun remove(e)\n"
"        var length = #this\n"
"        for var i = 0; i < length; i += 1 do\n"
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
"    fun pop()\n"
"        return this.removeAt(#this - 1)\n"
"    end\n"
"    fun sort(comp=null)\n"
"        var temp = this.slice(0)\n"
"        var size = #this - 1\n"
"        for var m = 1; m <= size; m *= 2 do\n"
"            for var i = 0; i < size; i += 2 * m do\n"
"                var from, mid, to = i, i + m - 1, i + 2 * m - 1\n"
"                if to > size then to = size end\n"
"                this._merge(from, mid, to, comp, temp)\n"
"            end\n"
"        end\n"
"    end\n"
"    fun _merge(l, m, r, comp, temp)\n"
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
"    fun __string__()\n"
"        return \"[\" + \", \".join(this) + \"]\"\n"
"    end\n"
"end\n"
"class Tuple is Sequence\n"
"    native new(seq)\n"
"    native slice(from, to=null)\n"
"    native __len__()\n"
"    native __iter__(iter)\n"
"    native __next__(idx)\n"
"    native __hash__()\n"
"    fun __string__()\n"
"        return \"(\" + \", \".join(this) + \")\"\n"
"    end\n"
"end\n"
"class Table is Iter\n"
"    native delete(key)\n"
"    native clear()\n"
"    native contains(key)\n"
"    native keys()\n"
"    native values()\n"
"    native __get__(key)\n"
"    native __set__(key, val)\n"
"    native __len__()\n"
"    native __iter__(i)\n"
"    native __next__(i)\n"
"    native __string__()\n"
"end\n"
"class Enum\n"
"    native new(...)\n"
"    native value(name)\n"
"    native name(value)\n"
"end\n"
"fun assert(cond, msg=\"assertion failed\")\n"
"    if !cond then raise AssertException(msg) end\n"
"end\n"
"fun sorted(iterable, comp=null)\n"
"    var lst = toList(iterable)\n"
"    lst.sort(comp)\n"
"    return lst\n"
"end\n"
"fun reduce(iterable, fn)\n"
"    var acc, iter = null, iterable.__iter__(null)\n"
"    if !iter then return null end\n"
"    acc = iterable.__next__(iter) \n"
"    while iter = iterable.__iter__(iter) do\n"
"        acc = fn(acc, iterable.__next__(iter))\n"
"    end\n"
"    return acc\n"
"end\n"
"fun any(iterable, func)\n"
"    for var e in iterable do\n"
"        if func(e) then\n"
"            return true\n"
"        end\n"
"    end\n"
"    return false\n"
"end\n"
"fun all(iterable, func)\n"
"    for var e in iterable do\n"
"        if !func(e) then\n"
"            return false\n"
"        end\n"
"    end\n"
"    return true\n"
"end\n"
"fun toList(iterable)\n"
"    if iterable is Sequence then\n"
"        var len = #iterable\n"
"        var list = List(len)\n"
"        for var i = 0; i < len; i += 1 do\n"
"            list[i] = iterable[i]\n"
"        end\n"
"        return list\n"
"    end\n"
"    var list = []\n"
"    for var e in iterable do\n"
"        list.add(e)\n"
"    end\n"
"    return list\n"
"end\n"
"fun toTable(iterable)\n"
"    var table = {}\n"
"    if iterable is Table then\n"
"        for var k in iterable do\n"
"            table[k] = iterable[k]\n"
"        end\n"
"    else\n"
"        for var k, v in iterable do\n"
"            table[k] = v\n"
"        end\n"
"    end\n"
"    return table\n"
"end\n"
"native ascii(num)\n"
"native char(c)\n"
"native eval(source)\n"
"native exec(cmd)\n"
"native int(n)\n"
"native system(cmd)\n"
"native print(s, ...)\n"
"native type(o)\n"
"class IReverse is Sequence\n"
"    fun new(sequence)\n"
"        this._sequence = sequence\n"
"        this._size = #sequence\n"
"    end\n"
"    fun __get__(i)\n"
"        try\n"
"            return this._sequence[this._size - i - 1]\n"
"        except IndexOutOfBoundException e\n"
"            raise IndexOutOfBoundException(##i)\n"
"        end\n"
"    end\n"
"    fun __len__()\n"
"        return this._size\n"
"    end\n"
"    fun __iter__(i)\n"
"        if i == null then return 0 if this._size > 0 else false end\n"
"        return i + 1 if i < this._size - 1 else false\n"
"    end\n"
"    fun __next__(i)\n"
"        return this._sequence[this._size - i - 1]\n"
"    end\n"
"end\n"
"class IEnumerate is Iter\n"
"    fun new(iter, start=0)\n"
"        this._iter = iter\n"
"        this._idx = start\n"
"    end\n"
"    fun __iter__(i)\n"
"        return this._iter.__iter__(i)\n"
"    end\n"
"    fun __next__(i)\n"
"        var ret = this._idx, this._iter.__next__(i)\n"
"        this._idx += 1\n"
"        return ret\n"
"    end\n"
"end\n"
"class IFilter is Iter\n"
"    fun new(func, iter)\n"
"        this._func = func\n"
"        this._iter = iter\n"
"    end\n"
"    fun __iter__(i)\n"
"        var iter = i\n"
"        while iter = this._iter.__iter__(iter) do\n"
"            if this._func(this._iter.__next__(iter)) then\n"
"                return iter\n"
"            end\n"
"        end\n"
"        return false\n"
"    end\n"
"    fun __next__(i)\n"
"        return this._iter.__next__(i)\n"
"    end\n"
"end\n"
"class IMap is Iter\n"
"    fun new(func, iter)\n"
"        this._func = func\n"
"        this._iter = iter\n"
"    end\n"
"    fun __iter__(i)\n"
"        return this._iter.__iter__(i)\n"
"    end\n"
"    fun __next__(i)\n"
"        return this._func(this._iter.__next__(i))\n"
"    end\n"
"end\n"
"class IZip is Iter\n"
"    fun new(...)\n"
"        if #args == 0 then \n"
"            raise InvalidArgException(\"Expected at least one iterator\")\n"
"        end\n"
"        this._iters = args\n"
"        this._result = []\n"
"    end\n"
"    fun __iter__(tup)\n"
"        var i = 0\n"
"        for var iterator in this._iters do\n"
"            var res = iterator.__iter__(tup[i] if tup else null)\n"
"            if !res then return false end\n"
"            this._result.add(res)\n"
"            i += 1\n"
"        end\n"
"        var res = Tuple(this._result)\n"
"        this._result.clear()\n"
"        return res\n"
"    end\n"
"    fun __next__(tup)\n"
"        var i = 0\n"
"        for var iterator in this._iters do\n"
"            this._result.add(iterator.__next__(tup[i]))\n"
"            i += 1\n"
"        end\n"
"        var res = Tuple(this._result)\n"
"        this._result.clear()\n"
"        return res\n"
"    end\n"
"end\n"
"class Exception\n"
"    fun new(err=\"\")\n"
"        this._err = err\n"
"        this._stacktrace = null\n"
"    end\n"
"    fun err()\n"
"        return this._err\n"
"    end\n"
"    native printStacktrace()\n"
"end\n"
"class TypeException is Exception end\n"
"class NameException is Exception end\n"
"class FieldException is Exception end\n"
"class MethodException is Exception end\n"
"class ImportException is Exception end\n"
"class StackOverflowException is Exception end\n"
"class SyntaxException is Exception end\n"
"class InvalidArgException is Exception end\n"
"class IndexOutOfBoundException is Exception end\n"
"class AssertException is Exception end\n"
"class NotImplementedException is Exception end\n"
;