const express = require('express');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');
const app = express();
const port = 3000;

// 配置静态文件目录
app.use(express.static('public'));

app.use(bodyParser.json());

let scheduleManagerProcess = null;

function startScheduleManager(callback) {
    if (!scheduleManagerProcess) {
        scheduleManagerProcess = spawn('./build/schedule_manager', ['run'], { stdio: ['pipe', 'pipe', 'pipe'] });

        scheduleManagerProcess.stdout.on('data', (data) => {
            console.log(`stdout: ${data}`);
        });

        scheduleManagerProcess.stderr.on('data', (data) => {
            console.error(`stderr: ${data}`);
        });

        scheduleManagerProcess.on('close', (code) => {
            console.log(`child process exited with code ${code}`);
            scheduleManagerProcess = null;
        });

        scheduleManagerProcess.on('spawn', callback);
    } else {
        callback();
    }
}

app.post('/executeCommand', (req, res) => {
    const { command, args } = req.body;

    // 打印接收到的命令和参数
    console.log(`Received command: ${command}`);
    console.log(`Received args: ${args}`);

    if (command === 'login') {
        startScheduleManager(() => {
            // 确保 scheduleManagerProcess 已经启动，然后执行 login 命令
            const loginCommand = `${args[0]}\n${args[1]}\n`;
            console.log(`Executing login command: ${loginCommand}`);  // 打印执行的命令

            scheduleManagerProcess.stdin.write(loginCommand);

            let output = '';
            scheduleManagerProcess.stdout.once('data', (data) => {
                output += data.toString();
                console.log(`Login response: ${output}`);  // 打印响应
                res.send(output);
            });
        });
    } else if (command === 'addtask') {
        const [username, password, taskName, startTime, priority, category, remindTime] = args;
        const execCommand = `addtask\n${taskName}\n${startTime}\n${priority}\n${category}\n${remindTime}\n`;
        console.log(`Executing addtask command: ${execCommand}`);  // 打印执行的命令

        scheduleManagerProcess.stdin.write(execCommand);

        let output = '';
        scheduleManagerProcess.stdout.once('data', (data) => {
            output += data.toString();
            console.log(`AddTask response: ${output}`);  // 打印响应
            res.send(output);
        });
    } else if (command === 'showtask') {
        const [username, password, date] = args;
        const execCommand = `showtask\n${date}\n`;
        console.log(`Executing showtask command: ${execCommand}`);  // 打印执行的命令

        scheduleManagerProcess.stdin.write(execCommand);

        let output = '';
        scheduleManagerProcess.stdout.once('data', (data) => {
            output += data.toString();
            console.log(`ShowTask response: ${output}`);  // 打印响应
            res.send(output);
        });
    } else if (command === 'deltask') {
        const [username, password, taskId] = args;
        const execCommand = `deltask\n${taskId}\n`;
        console.log(`Executing deltask command: ${execCommand}`);  // 打印执行的命令

        scheduleManagerProcess.stdin.write(execCommand);

        let output = '';
        scheduleManagerProcess.stdout.once('data', (data) => {
            output += data.toString();
            console.log(`DelTask response: ${output}`);  // 打印响应
            res.send(output);
        });
    } else if (command === 'register') {
        const [username, password] = args;
        const execCommand = `./build/schedule_manager register ${username} ${password}`;
        console.log(`Executing register command: ${execCommand}`);  // 打印执行的命令
    
        const registerProcess = spawn(execCommand, { shell: true });
    
        let output = '';
        registerProcess.stdout.on('data', (data) => {
            output += data.toString();
        });
    
        registerProcess.stderr.on('data', (data) => {
            console.error(`stderr: ${data}`);
        });
    
        registerProcess.on('close', (code) => {
            console.log(`Register process exited with code ${code}`);
            res.send(output);
        });
    } else {
        res.status(400).json({ error: 'Invalid command' });
    }
});

app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});
