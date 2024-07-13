const express = require('express');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const app = express();
const port = 3000;

// 配置静态文件目录
app.use(express.static('public'));

app.use(bodyParser.json());

app.post('/executeCommand', (req, res) => {
    const { command } = req.body;
    exec(`./schedule_manager ${command}`, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).json({ error: `exec error: ${error.message}` });
        }
        if (stderr) {
            console.error(`stderr: ${stderr}`);
            return res.status(500).json({ error: `stderr: ${stderr}` });
        }
        res.send(stdout);
    });
});

app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});
