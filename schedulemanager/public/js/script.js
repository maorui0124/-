// script.js
function showForm(formType) {
    const formContainer = document.getElementById('formContainer');
    switch (formType) {
        case 'login':
            formContainer.innerHTML = `
                <h2>Login</h2>
                <input type="text" id="loginUsername" placeholder="Username">
                <input type="password" id="loginPassword" placeholder="Password">
                <button onclick="login()">Submit</button>
            `;
            break;
        case 'register':
            formContainer.innerHTML = `
                <h2>Register</h2>
                <input type="text" id="registerUsername" placeholder="Username">
                <input type="password" id="registerPassword" placeholder="Password">
                <button onclick="register()">Submit</button>
            `;
            break;
        case 'addTask':
            formContainer.innerHTML = `
                <h2>Add Task</h2>
                <input type="text" id="taskName" placeholder="Task Name">
                <input type="text" id="startTime" placeholder="Start Time">
                <input type="text" id="priority" placeholder="Priority (0: LOW, 1: MEDIUM, 2: HIGH)">
                <input type="text" id="category" placeholder="Category (0: STUDY, 1: ENTERTAINMENT, 2: LIFE)">
                <input type="text" id="remindTime" placeholder="Remind Time (YYYY-MM-DD HH:MM:SS)">
                <button onclick="addTask()">Submit</button>
            `;
            break;
        case 'showTasks':
            formContainer.innerHTML = `
                <h2>Show Tasks</h2>
                <input type="text" id="taskDate" placeholder="Date (YYYY-MM-DD)">
                <button onclick="showTasks()">Submit</button>
            `;
            break;
        case 'deleteTask':
            formContainer.innerHTML = `
                <h2>Delete Task</h2>
                <input type="text" id="taskId" placeholder="Task ID">
                <button onclick="deleteTask()">Submit</button>
            `;
            break;
        default:
            formContainer.innerHTML = '';
    }
}

function login() {
    const username = document.getElementById('loginUsername').value;
    const password = document.getElementById('loginPassword').value;
    const command = 'login';
    const args = [username, password];

    executeCommand(command, args);
}

function register() {
    const username = document.getElementById('registerUsername').value;
    const password = document.getElementById('registerPassword').value;
    const command = 'register';
    const args = [username, password];

    executeCommand(command, args);
}

function addTask() {
    const taskName = document.getElementById('taskName').value;
    const startTime = document.getElementById('startTime').value;
    const priority = document.getElementById('priority').value || '1'; // 默认优先级为MEDIUM
    const category = document.getElementById('category').value || '2'; // 默认类别为LIFE
    const remindTime = document.getElementById('remindTime').value || startTime;
    const command = 'addtask';
    const args = ['username', 'password', taskName, startTime, priority, category, remindTime];

    executeCommand(command, args);
}

function showTasks() {
    const date = document.getElementById('taskDate').value;
    const command = 'showtask';
    const args = ['username', 'password', date];

    executeCommand(command, args);
}

function deleteTask() {
    const taskId = document.getElementById('taskId').value;
    const command = 'deltask';
    const args = ['username', 'password', taskId];

    executeCommand(command, args);
}

function executeCommand(command, args) {
    fetch('/executeCommand', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ command, args })
    })
    .then(response => response.text())
    .then(data => {
        alert(data);
    })
    .catch(error => console.error('Error:', error));
}
