// ==========================================
// CONEXÃO WEBSOCKET COM O C++
// ==========================================
const socket = new WebSocket('ws://localhost:8080/ws');

socket.onopen = () => {
    console.log('Conectado ao servidor C++');
};

socket.onmessage = (event) => {
    const estado = JSON.parse(event.data);
    atualizarInterface(estado);
};

socket.onerror = () => {
    mostrarModal('<h2 style="color:#ef4444">⚠ Servidor offline</h2><p>Verifique se o servidor C++ está rodando.</p>', 5000);
};

// ==========================================
// ESTADO LOCAL DE SELEÇÃO
// ==========================================
let cartaSelecionada = null; // { tipo: "coluna"|"descarte", indice: int, cartaIndice: int }

// ==========================================
// ATUALIZAR INTERFACE COM BASE NO ESTADO
// ==========================================
function atualizarInterface(estado) {
    // Pontuação
    document.getElementById('pontos').innerText   = estado.pontuacao || 0;
    document.getElementById('recorde').innerText  = estado.recorde   || 0;

    // Cava
    const cavaCount = estado.cava_tamanho || 0;
    document.getElementById('cava-count').innerText = cavaCount;
    const cavaVisual = document.getElementById('cava-visual');
    if (cavaCount === 0) {
        cavaVisual.outerHTML = '<div class="cava-vazia" id="cava-visual" onclick="comprarCarta()">↺</div>';
    } else {
        const el = document.getElementById('cava-visual');
        if (el) el.className = 'verso-grande';
    }

    // Descarte
    renderizarDescarte(estado.descarte);

    // Fundações
    renderizarFundacoes(estado.fundacoes);

    // Colunas
    renderizarColunas(estado.colunas, estado.cartas_escondidas);

    // Vitória
    if (estado.vitoria) {
        mostrarModal('<h2 style="color:#4ade80">🏆 PARABÉNS!</h2><p>Você completou o Paciência!</p>', 0);
    }
}

// ==========================================
// RENDERIZAR DESCARTE
// ==========================================
function renderizarDescarte(descarte) {
    const div = document.getElementById('descarte');
    div.innerHTML = '';

    if (!descarte || descarte.length === 0) {
        div.innerHTML = '<span class="placeholder">↩</span>';
        return;
    }

    const carta = descarte[descarte.length - 1];
    const el    = criarElementoCarta(carta, 'descarte');
    el.classList.add('carta-descarte');

    const selecionado = cartaSelecionada && cartaSelecionada.tipo === 'descarte';
    if (selecionado) el.classList.add('selecionada');

    el.onclick = () => selecionarDescarte();
    div.appendChild(el);
}

// ==========================================
// RENDERIZAR FUNDAÇÕES
// ==========================================
function renderizarFundacoes(fundacoes) {
    for (let i = 0; i < 4; i++) {
        const div = document.getElementById(`fundacao-${i}`);
        div.innerHTML = '';

        const simbolos = ['♠', '♥', '♦', '♣'];
        if (!fundacoes || !fundacoes[i] || fundacoes[i].length === 0) {
            div.innerHTML = `<span class="placeholder">${simbolos[i]}</span>`;
        } else {
            const carta = fundacoes[i][fundacoes[i].length - 1];
            const el    = criarElementoCarta(carta, 'fundacao');
            el.classList.add('carta-coluna', 'frente');
            el.style.position = 'relative';
            el.style.cursor   = 'default';
            div.appendChild(el);
        }

        div.onclick = () => moverParaDestino('fundacao', i);
    }
}

// ==========================================
// RENDERIZAR COLUNAS
// ==========================================
function renderizarColunas(colunas, cartasEscondidas) {
    for (let i = 0; i < 7; i++) {
        const div     = document.getElementById(`coluna-${i}`);
        div.innerHTML = '';

        const coluna    = colunas ? colunas[i] : [];
        const escondidas = cartasEscondidas ? cartasEscondidas[i] : 0;
        const offsetY   = 25; // pixels de deslocamento entre cartas

        coluna.forEach((carta, j) => {
            const el = document.createElement('div');
            el.classList.add('carta-coluna');
            el.style.top = `${j * offsetY}px`;

            if (j < escondidas) {
                // Carta virada para baixo
                el.classList.add('verso');
            } else {
                // Carta virada para cima
                const naipe   = obterNomeNaipe(carta.naipe);
                const simbolo = obterSimboloNaipe(carta.naipe);
                const valor   = traduzirValor(carta.valor);

                el.classList.add('frente', naipe);
                el.setAttribute('data-naipe-simbolo', simbolo);
                el.innerText = valor;

                const ehUltima = (j === coluna.length - 1);
                if (ehUltima) {
                    const selecionado = cartaSelecionada &&
                        cartaSelecionada.tipo === 'coluna' &&
                        cartaSelecionada.indice === i;
                    if (selecionado) el.classList.add('selecionada');
                    el.onclick = (e) => { 
    e.stopPropagation(); 
    if (cartaSelecionada && !(cartaSelecionada.tipo === 'coluna' && cartaSelecionada.indice === i)) {
        moverParaDestino('coluna', i);
    } else {
        selecionarColuna(i, j);
    }
};
                } else {
                    el.style.cursor = 'default';
                }
            }

            div.appendChild(el);
        });

        // Altura mínima da coluna
        div.style.minHeight = `${Math.max(110, coluna.length * offsetY + 105)}px`;
        div.onclick = () => moverParaDestino('coluna', i);
    }
}

// ==========================================
// SELEÇÃO E MOVIMENTO
// ==========================================
function selecionarDescarte() {
    if (cartaSelecionada && cartaSelecionada.tipo === 'descarte') {
        cartaSelecionada = null;
    } else {
        cartaSelecionada = { tipo: 'descarte', indice: 0 };
    }
    renderizarDescarte(window.estadoAtual?.descarte);
}

function selecionarColuna(colunaIdx, cartaIdx) {
    // Se já tem uma carta selecionada de outro lugar, tenta mover
    if (cartaSelecionada && !(cartaSelecionada.tipo === 'coluna' && cartaSelecionada.indice === colunaIdx)) {
        moverParaDestino('coluna', colunaIdx);
        return;
    }

    // Se clicou na mesma carta, deseleciona
    if (cartaSelecionada &&
        cartaSelecionada.tipo   === 'coluna' &&
        cartaSelecionada.indice === colunaIdx) {
        cartaSelecionada = null;
    } else {
        // Seleciona a carta
        cartaSelecionada = { tipo: 'coluna', indice: colunaIdx, cartaIdx };
    }
    renderizarColunas(window.estadoAtual?.colunas, window.estadoAtual?.cartas_escondidas);
}
function moverParaDestino(tipoDestino, indiceDestino) {
    if (!cartaSelecionada) return;

    socket.send(JSON.stringify({
        acao:           'MOVER',
        origem_tipo:    cartaSelecionada.tipo,
        origem_indice:  cartaSelecionada.indice,
        destino_tipo:   tipoDestino,
        destino_indice: indiceDestino
    }));

    cartaSelecionada = null;
}

function comprarCarta() {
    socket.send(JSON.stringify({ acao: 'COMPRAR_CARTA' }));
}

// ==========================================
// BOTÕES
// ==========================================
document.getElementById('btn-desfazer').onclick = () => {
    socket.send(JSON.stringify({ acao: 'DESFAZER' }));
    cartaSelecionada = null;
};

document.getElementById('btn-novo-jogo').onclick = () => {
    socket.send(JSON.stringify({ acao: 'NOVO_JOGO' }));
    cartaSelecionada = null;
};

// ==========================================
// AUXILIARES
// ==========================================
function criarElementoCarta(carta, tipo) {
    const el     = document.createElement('div');
    const naipe  = obterNomeNaipe(carta.naipe);
    const simbolo = obterSimboloNaipe(carta.naipe);
    const valor  = traduzirValor(carta.valor);

    el.classList.add(naipe);
    el.setAttribute('data-naipe-simbolo', simbolo);
    el.innerText = valor;
    return el;
}

function traduzirValor(valor) {
    const mapa = { 1: 'A', 11: 'J', 12: 'Q', 13: 'K' };
    return mapa[valor] || valor;
}

function obterNomeNaipe(naipe) {
    const mapa = { 0: 'ouros', 1: 'copas', 2: 'espadas', 3: 'paus' };
    return mapa[naipe] || naipe;
}

function obterSimboloNaipe(naipe) {
    const mapa = { 0: '♦', 1: '♥', 2: '♠', 3: '♣' };
    return mapa[naipe] || '';
}

function mostrarModal(htmlContent, tempoMs) {
    const modal = document.getElementById('modal-notificacao');
    const texto = document.getElementById('modal-texto');

    texto.innerHTML = htmlContent;
    modal.classList.remove('modal-oculto');

    if (tempoMs > 0) {
        if (window.modalTimer) clearTimeout(window.modalTimer);
        window.modalTimer = setTimeout(() => {
            modal.classList.add('modal-oculto');
        }, tempoMs);
    }
}

// Guarda estado atual para re-renderização parcial
socket.onmessage = (event) => {
    const estado = JSON.parse(event.data);
    window.estadoAtual = estado;
    atualizarInterface(estado);
};