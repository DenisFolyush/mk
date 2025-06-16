// rfid_access_backend/server.js
const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const bodyParser = require('body-parser');
const path = require('path');
const app = express();
const port = 3000;

// Middleware
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'public')));

// Ініціалізація бази даних
const db = new sqlite3.Database('./rfid_access.db', (err) => {
  if (err) return console.error(err.message);
  console.log('✅ Підключено до бази даних SQLite.');
});

// Створення таблиць
db.serialize(() => {
  db.run(`CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT UNIQUE,
    name TEXT
  )`);

  db.run(`CREATE TABLE IF NOT EXISTS logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT,
    result TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
  )`);
});

// Перевірка UID з ESP8266
app.post('/access-check', (req, res) => {
  const { uid } = req.body;
  if (!uid) return res.status(400).json({ error: 'UID missing' });

  db.get(`SELECT * FROM users WHERE uid = ?`, [uid], (err, row) => {
    if (err) return res.status(500).json({ error: 'DB error' });

    const access = !!row;
    const result = access ? 'granted' : 'denied';

    db.run(`INSERT INTO logs(uid, result) VALUES (?, ?)`, [uid, result]);

    res.json({ access });
  });
});

// Перегляд журналу доступу
app.get('/logs', (req, res) => {
  db.all(`SELECT * FROM logs ORDER BY timestamp DESC LIMIT 100`, [], (err, rows) => {
    if (err) return res.status(500).json({ error: 'DB error' });
    res.json(rows);
  });
});

// Перегляд користувачів
app.get('/users', (req, res) => {
  db.all(`SELECT * FROM users ORDER BY id DESC`, [], (err, rows) => {
    if (err) return res.status(500).json({ error: 'DB error' });
    res.json(rows);
  });
});

// Додавання UID
app.post('/add-uid', (req, res) => {
  const { uid, name } = req.body;
  if (!uid || !name) return res.status(400).json({ error: 'Missing fields' });

  db.run(`INSERT OR IGNORE INTO users(uid, name) VALUES (?, ?)`, [uid, name], function (err) {
    if (err) return res.status(500).json({ error: 'DB error' });
    res.json({ added: true, id: this.lastID });
  });
});

// Видалення UID
app.post('/delete-uid', (req, res) => {
  const { uid } = req.body;
  if (!uid) return res.status(400).json({ error: 'UID missing' });

  db.run(`DELETE FROM users WHERE uid = ?`, [uid], function (err) {
    if (err) return res.status(500).json({ error: 'DB error' });
    res.json({ deleted: this.changes > 0 });
  });
});

// Віддача HTML інтерфейсу
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.listen(port, () => {
  console.log(`🚀 Сервер запущено на http://localhost:${port}`);
});
