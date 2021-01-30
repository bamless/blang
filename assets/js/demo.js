// Code that implements the logic of the demo page.
// Sets up the example buttons and the editor, as well as code execution.

window.addEventListener("load", () => {

// Demo elements and helper functions
let input = document.querySelector(".demo-editor");
let output = document.querySelector(".demo-output");
let errorLabel = document.querySelector(".demo-error-label");
let editor = CodeJar(input, withLineNumbers(Prism.highlightElement));

function setErrorLabel(message) {
    errorLabel.innerText = message;
    errorLabel.setAttribute('class', 'label label-red demo-error-label');
}

function clearErrorLabel() {
    errorLabel.innerText = '';
    errorLabel.setAttribute('class', 'demo-error-label');
}

function clearOutput() {
    output.innerText = "";
}

function addOutput(out) {
    output.innerText += out;
}

function addErrorOutput(err) {
    err = err.replace(/&/g, "&amp;").replace(/>/g, "&gt;").replace(/"/g, "&quot;")
                .replace(/'/g, "&#039;").replace(/</g, "&lt;");
    output.innerHTML += `<span class="demo-error">${err}</span>`;
}

function setEditorCode(source) {
    clearOutput();
    addOutput('[...]');
    clearErrorLabel();
    editor.updateCode(source);
}

// Button functionalities
let runButton = document.querySelector('#run');
runButton.addEventListener('click', () => {
    clearErrorLabel();
    clearOutput();

    let res = jstar_api.run(editor.toString());

    if(res == 1) {
        setErrorLabel('Syntax Error');
    } else if(res == 2) {
        setErrorLabel('Compilation Error');
    } else if(res == 3) {
        setErrorLabel('Runtime Error');
    }

    addOutput(jstar_api.out);
    addErrorOutput(jstar_api.err);
});

// Example buttons
let helloWorldButton = document.querySelector('#hello-world');
helloWorldButton.addEventListener('click', () => {
    setEditorCode(`print('Hello, World!')`);
});

let loopButton = document.querySelector('#loop');
loopButton.addEventListener('click', () => {
    setEditorCode(
`for var i = 0; i < 10; i += 1 do
	print('Iteration {0}' % i)
end`
    );
});

let quickSortButton = document.querySelector('#quick-sort');
quickSortButton.addEventListener('click', () => {
    setEditorCode(
`fun partition(list, low, high)
	var pivot = list[high]
	var i = low
	for var j = low; j < high; j += 1 do
		if list[j] <= pivot then
			list[i], list[j] = list[j], list[i]
			i += 1
		end
	end
	list[i], list[high] = list[high], list[i]
	return i
end

fun quickSort(list, low, high)
	if low < high then
		var p = partition(list, low, high)
		quickSort(list, low, p - 1)
		quickSort(list, p + 1, high)
	end
end

var list = [9, 1, 36, 37, 67, 45, 11, 27, 3, 5]
quickSort(list, 0, #list - 1)
print(list)`
    );
});

let regexButton = document.querySelector('#regex');
regexButton.addEventListener('click', () => {
    setEditorCode(
`import re

var message = '{lang} on {platform}!'
var formatted = re.gsub(message, '{(%a+)}', fun(arg)
	return { 
		'lang' : 'J*', 'platform' : 'the Web'
	}[arg]
end)

print(formatted)`
        );
    });

let classesButton = document.querySelector("#classes");
classesButton.addEventListener('click', () => {
    setEditorCode(
`class Person
    fun new(name, age)
        this.name = name
        this.age = age
    end

    fun getName()
        return this.name
    end

    fun showIncome()
        print("{0}'s income is unknown" % this.getName())
    end
end

class Employee is Person
    fun new(name, age, income)
        super(name, age)
        this.income = income
    end

    fun showIncome()
        print("{0}'s income is {1}$" % (super.getName(), this.income))
    end
end

var john = Person("John", 20)
var alice = Employee("Alice", 34, 2000)

john.showIncome()
alice.showIncome()`        
    );
});

});
