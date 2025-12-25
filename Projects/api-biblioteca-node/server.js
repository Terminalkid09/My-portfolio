const express = require('express');
const app = express();
const port = 3000;

// Middleware per leggere JSON nel body
app.use(express.json());


let libri = [
  { id: 1, titolo: "Il Nome della Rosa", autore: "Umberto Eco", anno: 1980 },
  { id: 2, titolo: "1984", autore: "George Orwell", anno: 1949 },
  { id: 3, titolo: "Harry Potter", autore: "J.K. Rowling", anno: 1997 }
];

// GET tutti i libri
app.get('/api/libri', (req, res) => {
  res.json(libri);
});

// GET un libro per ID
app.get('/api/libri/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const libro = libri.find(l => l.id === id);
  
  if (libro) {
    res.json(libro);
  } else {
    res.status(404).json({ errore: "Libro non trovato" });
  }
});

// POST aggiungere un nuovo libro
app.post('/api/libri', (req, res) => {
  const nuovoLibro = {
    id: libri.length > 0 ? Math.max(...libri.map(l => l.id)) + 1 : 1,
    titolo: req.body.titolo,
    autore: req.body.autore,
    anno: req.body.anno
  };
  
  libri.push(nuovoLibro);
  res.status(201).json(nuovoLibro);
});

// PUT modifica un libro
app.put('/api/libri/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const index = libri.findIndex(l => l.id === id);
  
  if (index !== -1) {
    libri[index] = { ...libri[index], ...req.body };
    res.json(libri[index]);
  } else {
    res.status(404).json({ errore: "Libro non trovato" });
  }
});

// DELETE elimina un libro
app.delete('/api/libri/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const index = libri.findIndex(l => l.id === id);
  
  if (index !== -1) {
    const libroRimosso = libri.splice(index, 1);
    res.json(libroRimosso[0]);
  } else {
    res.status(404).json({ errore: "Libro non trovato" });
  }
});

// Avvia il server
app.listen(port, () => {
  console.log(`🚀 API in esecuzione su http://localhost:${port}`);
  console.log(`Prova: http://localhost:${port}/api/libri`);
});