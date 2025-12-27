//  Configuration / Storage keys 
const STORAGE = {
  PASS_HASH: 'journal_pass_hash_v1',
  NOTES: 'journal_notes_v1',
  HEARTS: 'journal_hearts_v1'
};


const IMAGES = [
  
  'https://images.unsplash.com/photo-1503023345310-bd7c1de61c7d?w=1400&q=60&auto=format&fit=crop',
  'https://images.unsplash.com/photo-1498050108023-c5249f4df085?w=1400&q=60&auto=format&fit=crop'
];
const PRIVATE_IMAGES = []; // private gallery images (optional)

//  Utilities
const $ = id => document.getElementById(id);
function saveJSON(key, value) { localStorage.setItem(key, JSON.stringify(value)); }
function loadJSON(key, fallback = null) {
  try { const d = localStorage.getItem(key); return d ? JSON.parse(d) : fallback; }
  catch (e) { return fallback; }
}
function elExists(id){ return !!$(id); }

// SHA-256 hashing helper (returns hex string)
async function hashString(str) {
  const enc = new TextEncoder();
  const data = enc.encode(str);
  const hashBuffer = await crypto.subtle.digest('SHA-256', data);
  const hashArray = Array.from(new Uint8Array(hashBuffer));
  return hashArray.map(b => b.toString(16).padStart(2,'0')).join('');
}

// App state 
let initialized = false;
let galleryIntervals = [];

//  Prepare UI on load 
function prepare() {
  // hide secret content for safety
  const secret = $('secret');
  if (secret) {
    secret.hidden = true;
    secret.setAttribute('aria-hidden','true');
    secret.classList.remove('active');

    // make interactive children unfocusable and disabled
    secret.querySelectorAll('a,button,input,textarea,select').forEach(el => {
      if (el.hasAttribute('tabindex')) el.dataset._savedTabindex = el.getAttribute('tabindex');
      else el.dataset._savedTabindex = '';
      el.setAttribute('tabindex', '-1');

      if (el.tagName.toLowerCase() === 'button' || el.tagName.toLowerCase() === 'input') {
        el.dataset._disabled = el.disabled ? '1' : '0';
        el.disabled = true;
      }
    });

    secret.style.pointerEvents = 'none';
  }

  // Determine whether passphrase exists
  const passHash = localStorage.getItem(STORAGE.PASS_HASH);
  if (!passHash) {
    // show setup, hide unlock
    if (elExists('setup-panel')) $('setup-panel').hidden = false;
    if (elExists('quiz')) $('quiz').hidden = true;
  } else {
    // show unlock, hide setup
    if (elExists('setup-panel')) $('setup-panel').hidden = true;
    if (elExists('quiz')) $('quiz').hidden = false;
  }

  attachHandlersForSetupAndUnlock();
}

//  Setup & unlock handlers 
function attachHandlersForSetupAndUnlock() {
  // Setup: set passphrase first time
  const setBtn = $('set-pass');
  if (setBtn) {
    setBtn.onclick = async () => {
      const p1 = String($('setup-pass').value || '');
      const p2 = String($('setup-pass-confirm').value || '');
      const feedback = $('setup-feedback');
      if (p1.length < 4) {
        if (feedback) feedback.textContent = 'Passphrase too short (min 4 chars).';
        return;
      }
      if (p1 !== p2) {
        if (feedback) feedback.textContent = 'Passphrases do not match.';
        return;
      }
      // hash and store
      try {
        const h = await hashString(p1);
        localStorage.setItem(STORAGE.PASS_HASH, h);
        if (feedback) feedback.textContent = 'Passphrase saved. Unlocking...';
        // show unlock screen
        $('setup-panel').hidden = true;
        if ($('quiz')) $('quiz').hidden = false;
        // prefill answer and auto-unlock for convenience
        if ($('answer')) $('answer').value = p1;
        setTimeout(() => attemptUnlock(), 400);
      } catch (e) {
        if (feedback) feedback.textContent = 'Error saving passphrase.';
        console.error(e);
      }
    };
  }

  // Unlock
  const unlockBtn = $('unlock');
  const answerInput = $('answer');
  if (unlockBtn) unlockBtn.onclick = () => attemptUnlock();
  if (answerInput) answerInput.onkeydown = (e) => { if (e.key === 'Enter') attemptUnlock(); };

  // Logout
  const logoutBtn = $('logout');
  if (logoutBtn) logoutBtn.onclick = () => {
    // hide secret, show unlock
    const secret = $('secret');
    if (secret) {
      secret.hidden = true;
      secret.setAttribute('aria-hidden','true');
      secret.classList.remove('active');
      secret.style.pointerEvents = 'none';
      secret.querySelectorAll('a,button,input,textarea,select').forEach(el => {
        if (el.dataset._savedTabindex !== undefined) {
          if (el.dataset._savedTabindex !== '') el.setAttribute('tabindex', el.dataset._savedTabindex);
          else el.removeAttribute('tabindex');
        } else el.removeAttribute('tabindex');

        if (el.dataset._disabled !== undefined) {
          el.disabled = el.dataset._disabled === '1';
          delete el.dataset._disabled;
        } else {
          if (el.tagName.toLowerCase() === 'button') el.disabled = true;
        }
      });
    }
    if ($('quiz')) $('quiz').hidden = false;
    if ($('setup-panel')) $('setup-panel').hidden = true;
    initialized = false;
    // clear gallery intervals
    galleryIntervals.forEach(i => clearInterval(i));
    galleryIntervals = [];
  };
}

// Unlock logic 
async function attemptUnlock() {
  const answerEl = $('answer');
  const feedbackEl = $('feedback');
  if (!answerEl || !feedbackEl) return;
  const pass = String(answerEl.value || '');
  if (!pass) { feedbackEl.textContent = 'Enter passphrase.'; return; }
  const stored = localStorage.getItem(STORAGE.PASS_HASH);
  if (!stored) { feedbackEl.textContent = 'No passphrase set. Please set one first.'; return; }

  try {
    const h = await hashString(pass);
    if (h === stored) {
      feedbackEl.textContent = 'Access granted — unlocking...';
      revealSecret();
    } else {
      feedbackEl.textContent = 'Incorrect passphrase.';
      answerEl.focus();
    }
  } catch (e) {
    feedbackEl.textContent = 'Error verifying passphrase.';
    console.error(e);
  }
}

function revealSecret() {
  createConfetti();
  setTimeout(() => {
    const quiz = $('quiz');
    if (quiz) quiz.hidden = true;
    const secret = $('secret');
    if (secret) {
      secret.hidden = false;
      secret.removeAttribute('aria-hidden');
      secret.classList.add('active');
      // restore interactive children
      secret.querySelectorAll('a,button,input,textarea,select').forEach(el => {
        if (el.dataset._savedTabindex !== undefined) {
          if (el.dataset._savedTabindex !== '') el.setAttribute('tabindex', el.dataset._savedTabindex);
          else el.removeAttribute('tabindex');
          delete el.dataset._savedTabindex;
        } else el.removeAttribute('tabindex');

        if (el.dataset._disabled !== undefined) {
          el.disabled = el.dataset._disabled === '1';
          delete el.dataset._disabled;
        } else {
          if (el.tagName.toLowerCase() === 'button') el.disabled = false;
        }
      });
      secret.style.pointerEvents = '';
    }
    initAll();
  }, 350);
}

// small confetti
function createConfetti() {
  const container = document.createElement('div');
  container.style.position = 'fixed';
  container.style.inset = 0;
  container.style.pointerEvents = 'none';
  container.style.zIndex = 9999;
  document.body.appendChild(container);

  const count = 16;
  for (let i = 0; i < count; i++) {
    const el = document.createElement('div');
    el.textContent = '•';
    el.style.position = 'absolute';
    el.style.left = (Math.random() * 90 + 5) + 'vw';
    el.style.top = '-6vh';
    el.style.opacity = 0.95;
    el.style.fontSize = (Math.random() * 16 + 10) + 'px';
    el.style.color = ['#6a1b9a', '#9b59b6', '#8e44ad'][Math.floor(Math.random() * 3)];
    el.style.transition = `transform ${2.2 + Math.random() * 1.2}s linear, opacity 1.2s linear`;
    container.appendChild(el);
    setTimeout(() => {
      el.style.transform = `translateY(${110 + Math.random() * 30}vh) rotate(${Math.random() * 360}deg)`;
      el.style.opacity = 0;
    }, 70 + Math.random() * 300);
    setTimeout(() => el.remove(), 3400);
  }
  setTimeout(() => container.remove(), 3600);
}

//  Initialize interactive features only once per unlock 
function initAll() {
  if (initialized) return;
  initialized = true;

  initGallery('slides', IMAGES, 'prev', 'next');
  initGallery('slides-private', PRIVATE_IMAGES, 'prev-private', 'next-private');
  initNotes();
  initIdeas();
  initHearts();

  const revealBtn = $('reveal-private');
  if (revealBtn) {
    revealBtn.onclick = () => {
      const pg = $('private-gallery');
      if (pg) {
        pg.hidden = false;
        pg.removeAttribute('aria-hidden');
        pg.classList.add('active');
      }
      revealBtn.disabled = true;
    };
  }
}

//  Gallery helper 
function initGallery(slidesId, images, prevId, nextId) {
  const slidesEl = $(slidesId);
  if (!slidesEl) return;
  slidesEl.innerHTML = '';

  const prevBtn = $(prevId);
  const nextBtn = $(nextId);

  if (!images || images.length === 0) {
    const slide = document.createElement('div');
    slide.className = 'slide';
    const placeholder = document.createElement('div');
    placeholder.className = 'slide-placeholder';
    placeholder.textContent = 'No images — replace in app.js';
    placeholder.style.padding = '48px';
    placeholder.style.color = '#9aa0a6';
    slide.appendChild(placeholder);
    slidesEl.appendChild(slide);
    if (prevBtn) { prevBtn.style.display = 'none'; prevBtn.setAttribute('aria-hidden','true'); }
    if (nextBtn) { nextBtn.style.display = 'none'; nextBtn.setAttribute('aria-hidden','true'); }
    return;
  }

  images.forEach(src => {
    const d = document.createElement('div');
    d.className = 'slide';
    const img = document.createElement('img');
    img.src = src;
    img.alt = 'Gallery image';
    img.loading = 'lazy';
    img.onerror = () => { img.src = 'https://via.placeholder.com/1200x800?text=Image'; };
    d.appendChild(img);
    slidesEl.appendChild(d);
  });

  let index = 0;
  const total = images.length;
  const update = () => { slidesEl.style.transform = `translateX(-${index * 100}%)`; };

  if (total > 1) {
    if (prevBtn) { prevBtn.style.display = ''; prevBtn.removeAttribute('aria-hidden'); }
    if (nextBtn) { nextBtn.style.display = ''; nextBtn.removeAttribute('aria-hidden'); }
  } else {
    if (prevBtn) { prevBtn.style.display = 'none'; prevBtn.setAttribute('aria-hidden','true'); }
    if (nextBtn) { nextBtn.style.display = 'none'; nextBtn.setAttribute('aria-hidden','true'); }
  }

  if (prevBtn) prevBtn.onclick = () => { index = (index - 1 + total) % total; update(); };
  if (nextBtn) nextBtn.onclick = () => { index = (index + 1) % total; update(); };

  const interval = setInterval(() => { index = (index + 1) % total; update(); }, 7000);
  galleryIntervals.push(interval);
  update();
}

//  Ideas 
const IDEAS = [
  'Write a short case study about a recent project.',
  'Sketch a layout for a minimal landing page.',
  'Document a debugging session and the solution you found.',
  'Create a 1-minute demo video idea for social.',
  'List three micro-interactions to improve UX.'
];
function initIdeas() {
  const btn = $('newIdea');
  const ideaEl = $('idea');
  if (!btn || !ideaEl) return;
  btn.onclick = () => {
    const i = Math.floor(Math.random() * IDEAS.length);
    ideaEl.textContent = IDEAS[i];
  };
  btn.click();
}

//  Hearts (reaction) 
function initHearts() {
  const countEl = $('count');
  let heartBtn = $('heart-btn');
  if (!heartBtn) heartBtn = document.querySelector('.heart');
  if (!countEl || !heartBtn) return;

  const stored = loadJSON(STORAGE.HEARTS, 0) || 0;
  let n = Number(stored);
  countEl.textContent = n;
  heartBtn.onclick = () => {
    n++;
    countEl.textContent = n;
    saveJSON(STORAGE.HEARTS, n);
    if (heartBtn.animate) heartBtn.animate([{ transform:'scale(1.15)'},{ transform:'scale(1)' }], { duration:200 });
  };
}

//  Notes (autosave) 
function initNotes() {
  const listEl = $('notes-list');
  const titleEl = $('note-title');
  const bodyEl = $('note-body');
  const statusEl = $('note-status');
  const newBtn = $('new-note');
  const deleteBtn = $('delete-note');
  if (!listEl || !titleEl || !bodyEl || !statusEl || !newBtn || !deleteBtn) {
    console.warn('Notes elements missing; notes disabled');
    return;
  }

  let notes = loadJSON(STORAGE.NOTES, []) || [];
  let activeId = notes.length ? notes[0].id : null;

  function renderList() {
    listEl.innerHTML = '';
    notes.slice().reverse().forEach(note => {
      const item = document.createElement('div');
      item.className = 'note-item';
      if (note.id === activeId) item.classList.add('active');
      item.dataset.id = note.id;
      item.innerHTML = `<div>${escapeHtml(note.title || 'Untitled')}</div><div class="muted small">${new Date(note.updated).toLocaleString()}</div>`;
      item.onclick = () => setActive(note.id);
      listEl.appendChild(item);
    });
  }

  function setActive(id) {
    activeId = id;
    const n = notes.find(x => x.id === id);
    if (n) {
      titleEl.value = n.title;
      bodyEl.value = n.body;
      statusEl.textContent = 'Saved';
    } else {
      titleEl.value = '';
      bodyEl.value = '';
      statusEl.textContent = 'No changes';
    }
    renderList();
  }

  function createNote() {
    const id = 'n_' + Date.now().toString(36) + Math.random().toString(36).slice(2, 8);
    const n = { id, title: '', body: '', updated: Date.now() };
    notes.push(n);
    saveNotes();
    setActive(id);
  }

  function deleteActive() {
    if (!activeId) return;
    if (!confirm('Delete this note? This action cannot be undone.')) return;
    notes = notes.filter(n => n.id !== activeId);
    saveNotes();
    activeId = notes.length ? notes[0].id : null;
    setActive(activeId);
  }

  function saveNotes() {
    saveJSON(STORAGE.NOTES, notes);
    renderList();
  }

  let timeout = null;
  function scheduleSave() {
    statusEl.textContent = 'Saving...';
    if (timeout) clearTimeout(timeout);
    timeout = setTimeout(() => {
      const n = notes.find(x => x.id === activeId);
      if (n) {
        n.title = String(titleEl.value || '');
        n.body = String(bodyEl.value || '');
        n.updated = Date.now();
        saveNotes();
        statusEl.textContent = 'Saved';
      } else {
        statusEl.textContent = 'No active note';
      }
    }, 600);
  }

  titleEl.addEventListener('input', scheduleSave);
  bodyEl.addEventListener('input', scheduleSave);
  titleEl.addEventListener('blur', scheduleSave);
  bodyEl.addEventListener('blur', scheduleSave);
  newBtn.onclick = createNote;
  deleteBtn.onclick = deleteActive;

  if (!notes.length) createNote();
  else {
    if (!activeId) activeId = notes[0].id;
    setActive(activeId);
  }
  renderList();
}

//  Helpers 
function escapeHtml(str) {
  return String(str).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

//  Start 
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', prepare);
} else {
  prepare();
}