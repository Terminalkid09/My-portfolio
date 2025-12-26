const express = require('express');
const app = express();
const port = 3000;

// Middleware to parse JSON bodies
app.use(express.json());

let books = [
  { id: 1, title: "The Name of the Rose", author: "Umberto Eco", year: 1980 },
  { id: 2, title: "1984", author: "George Orwell", year: 1949 },
  { id: 3, title: "Harry Potter and the Philosopher's Stone", author: "J.K. Rowling", year: 1997 }
];

// GET all books
app.get('/api/books', (req, res) => {
  res.json(books);
});

// GET a book by ID
app.get('/api/books/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const book = books.find(b => b.id === id);
  
  if (book) {
    res.json(book);
  } else {
    res.status(404).json({ error: "Book not found" });
  }
});

// POST add a new book
app.post('/api/books', (req, res) => {
  const newBook = {
    id: books.length > 0 ? Math.max(...books.map(b => b.id)) + 1 : 1,
    title: req.body.title,
    author: req.body.author,
    year: req.body.year
  };
  
  books.push(newBook);
  res.status(201).json(newBook);
});

// PUT update a book
app.put('/api/books/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const index = books.findIndex(b => b.id === id);
  
  if (index !== -1) {
    books[index] = { ...books[index], ...req.body };
    res.json(books[index]);
  } else {
    res.status(404).json({ error: "Book not found" });
  }
});

// DELETE delete a book
app.delete('/api/books/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const index = books.findIndex(b => b.id === id);
  
  if (index !== -1) {
    const deletedBook = books.splice(index, 1);
    res.json(deletedBook[0]);
  } else {
    res.status(404).json({ error: "Book not found" });
  }
});

app.listen(port, () => {
  console.log(`🚀 API running on http://localhost:${port}`);
  console.log(`Try: http://localhost:${port}/api/books`);
});